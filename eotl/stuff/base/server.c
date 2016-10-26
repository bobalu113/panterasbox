// stuff = ([ load_name : clone_count ;
//                           spawn_limit ;
//                           spawn_rate ;
//                           id_label
//         ])
// ids = ([ load_name : ([ player1_real_name : n, player2_real_name : n, ... ]),
//          object    : ([ player1_real_name : n, player2_real_name : n, ... ])
//       ])
// where n is one or more of ID_ITEM, ID_STATUS, ID_CHARGES
// shops = ([ prop_to_sell : ({ shop_ob_list, ... }) ])
// labels = ([ prop_to_label : ({ available_labels }) ])
mapping stuff, ids, shops, labels, clones;
string *spawn_queue;
object last_tried;
 
void create();
void heart_beat();
void reset();
static int try_spawn( string prog );
static object do_spawn( string prog );
object find_dest( object bp );
int register_shop( object shop, string prop );
int push_item( object item );
int pull_item( object item );
varargs string *query_stuff_list(string filter);
void do_id( object item, object who, int id_type );
int check_id( object item, object who );
string request_label( object item );
void push_log( object item, object targ );
void pull_log( object item );
mapping query_id_info();
mapping query_stuff_map();
mapping query_clones();


#define NO_STUFF_SHOPS
 
void
create()
{
  int i, j, x, y;
  string *files, *dirs, prog;
  object ob;
  mixed tmp;
 
  // Init the database
  seteuid( getuid() );
  dirs = LOAD_DIRS;
  stuff = ([ ]);
  ids = ([ ]);
  shops = ([ ]);
  clones = ([ ]);
  labels = LABELS;
  spawn_queue = ({ });
 
  // Load all objects in LOAD_DIRS
  for( i=0, j=sizeof(dirs) ; i<j ; i++ )
  {
    if( dirs[i][<1] != '/' )
      dirs[i] += "/";
    files = get_dir( dirs[i] );
    for( x=0, y=sizeof(files) ; x<y ; x++ )
    {
      if( strlen(files[x]) < 2 || files[x][<2..<1] != ".c" )
        continue;
      // call_out() to prevent lag
      call_out( #'load_object, i*x, dirs[i]+files[x] );
    }
  }
  // Check to allow testing of the server in another path without having the 
  // test server also distributing StuffCode.
  if( load_name( THISO ) == StuffServer[0..<3] )
    set_heart_beat(1);
  return;
}
 
void
heart_beat()
{
  mixed tmp;
 
  // spread things out a little bit
  if( time() % 3 )
    return;
  if( random(100) <= ((set_next_reset(0)-300)/15) )
    return;
 
  // attempt to spawn any programs that need spawning
  if( sizeof( spawn_queue ) )
  {
    tmp = spawn_queue[0];
    spawn_queue = spawn_queue[1..];
    if( try_spawn(tmp) )
    {
        mixed err = catch( tmp = do_spawn(tmp) );
 
        if( err )
            pull_item(tmp);
        else if( objectp( tmp ) )
            tmp->spawn_signal();
    }
  }
 
  return;
}

#define HB_LOG StuffLogDir "hb.log"
 
int
heart_beat_lost( string err, string prg, string curobj, int line )
{
  logf( HB_LOG, sprintf( "[%s]\nErr: %O\nPrg: %O\nCurobj: %O\nLine: %O\n\n",
    ctime(), err, prg, curobj, line ) );
  return 1;
}
 
void
reset()
{
  // add all objects in database to spawn_queue
  spawn_queue += m_indices( stuff );
 
  // try cleaning up old items
  filter( filter_objects( all_inventory( THISO ), "remove" ),
                #'destruct );

  // touch all the clones out there to make sure they reset()
  clones -= ([ 0 ]);
  clones = filter_indices( clones, #'call_other, "touch" );

  return;
}

#define CLONE_LOG StuffLogDir "clone.log"
 
// Returns true if the server is allowed to spawn a new clone of 'prog'
static int
try_spawn( string prog )
{
  int chance, cl_num;

  cl_num = sizeof( clones( prog ) );

  if( cl_num != stuff[prog,0] )
  {
    logf( CLONE_LOG, sprintf( "[%s] %s had %d clones and %d registered.\n",
      ctime(), prog, cl_num, stuff[prog,0] ) );
    stuff[prog,0] = cl_num;
  }
  if( stuff[prog,0] > stuff[prog,1] )
  {
    logf( CLONE_LOG, sprintf( "[%s] %s had %d clones on a max of %d.\n",
      ctime(), prog, stuff[prog,0], stuff[prog,1] ) );
    return 0;
  }
  if( stuff[prog,0] == stuff[prog,1] )
    return 0;

  chance = ( stuff[prog,0] * 100 ) / stuff[prog,1];  // fail chance
  chance = 100-chance;                               // success chance
  chance += stuff[prog,2];                           // modify it
  return random(100) < chance;
}
 
// Finds a home for a new clone of 'prog' and puts it there.  Returns
// newly cloned object or 0 for failure.
static object
do_spawn( string prog )
{
  object bp, dest, out;
 
  // get the blueprint
  if( !objectp( bp = load_object( prog ) ) )
  {
      return 0;
  }
 
  // couldn't find an home, we'll try again later
  if( !objectp( dest = find_dest( bp ) ) )
  {
    spawn_queue += ({ prog });
    return 0;
  }
 
  // clone it up and move it out
  if( !objectp( out = clone_object( prog ) ) )
    return 0;
  push_log( out, dest );
  if( move_object( out, dest ) )
    return out;
  return 0;
}
 
// Does the actual finding of a destination location for a given
// blueprint object.  Return value is an object pointer to the
// destination found, if any.
object
find_dest( object bp )
{
  object dest;
  string name, prop, *prop_list, *prevent;
  int i, j, isize;
  closure check_prevent;

  check_prevent = lambda( ({'x, 'y}),
    ({ #'&&, ({ #'objectp, 'y }), ({ #'call_other, 'y, "query", 'x }) }) );
 
#ifndef NO_STUFF_SHOPS
  // find out what type of item we're spawning
  prop_list = m_indices( shops );
  for( i=0, j=sizeof(prop_list) ; i<j ; i++ )
  {
    if( bp->query(prop_list[i]) )
    {
      prop = prop_list[i];
      break;
    }
  }
 
  // find out if any of the shops want to sell the new item
  if( prop )
  {
    shops[prop] -= ({ 0 });
    for( i=0, j=sizeof(shops[prop]) ; i<j ; i++ )
    {
      if( !objectp( ENV(shops[prop][i]) ) )
        continue;
 
      // Make sure it can spawn to shops
      if( stringp( prevent = bp->query("prevent_spawn") ) )
        prevent = ({ prevent });
      if( !sizeof( prevent ) )
        prevent = ({ });
      if( sizeof( filter( prevent, check_prevent, shops[prop][i] ) ) )
        continue;
 
      // Try and prevent hoarding
      isize = sizeof( filter_objects( deep_inventory(
              ENV(shops[prop][i]) ), "query", prop ) );
      if( random(100) < (100-(isize*(100/(sizeof(stuff)/5)))) )
        dest = shops[prop][i];
    }
  }
#endif
 
  // now try rooms and monsters
  while( !objectp(dest) )
  {
    // prevent a runtime error
    if( get_eval_cost() < 100000 )
      break;
    // get new object to try
    last_tried = debug_info( 2, last_tried );
    if( !objectp( last_tried ) )
      return 0;
    name = file_name( last_tried );
 
    if( name[0..5] != "/zone/" )          // only cabal objects
      continue;
    if( last_tried->query(NoStuffP) )     // they don't like stuff :(
      continue;
    if( strstr( name, "attic" ) != -1 )   // offline shit
      continue;
    if( strstr( name, "offline" ) != -1 ) // more offline shit
      continue;
    if( strstr( name, "OLD" ) != -1 )     // more offline shit (DORF)
      continue;
    if( strstr( name, "bak" ) != -1 ) 
      continue;
    if( strstr( name, "backup" ) != -1 )
      continue;
 
    // Make sure it can spawn to the object
    if( stringp( prevent = bp->query("prevent_spawn") ) )
      prevent = ({ prevent });
    if( !sizeof( prevent ) )
      prevent = ({ });
    if( sizeof( filter( prevent, check_prevent, last_tried ) ) )
      continue;
 
    // take every 10th monster
    if( living(last_tried) &&
        !query_once_interactive(last_tried) &&
        !random(10) )
      dest = last_tried;
    // take every 20th room
    else if( last_tried->query(RoomP) && !random(8) )
      dest = last_tried;
  }
 
  return dest;
}
 
// Registers a shop object so that it will have the opportunity to be
// stocked with whatever stuff gets spawned.  'props' is an optional
// arg that specifies which kind(s) of items will be stocked.
int
register_shop( object shop, mixed props )
{
#ifndef NO_STUFF_SHOPS
  int i, j, out;
  if( !objectp( shop ) )
    return 0;
  if( !props )
    props = m_indices(LABELS);
  else if( stringp( props ) )
    props = ({ props });
  else if( !pointerp( props ) )
    return 0;
  out = 0;
  for( i=0, j=sizeof(props) ; i<j ; i++ )
  {
    if( !stringp( props[i] ) )
      continue;
    if( !member( shops, props[i] ) )
      shops[props[i]] = ({ });
    if( member( shops[props[i]], shop ) != -1 )
      continue;
    shops[props[i]] += ({ shop });
    out++;
  }
  // add all objects in database to spawn_queue
  spawn_queue += m_indices( stuff );
  return out;
#else
  return 0;
#endif
}
 
// Registers a new item in the stuff mapping.  Returns true for success
// and false for failure.
int
push_item( object item )
{
  object ob;
  string prog, label, prop, *prop_list;
  int i, j;
 
  if( !objectp( item ) || !item->query(StuffP) )
    return 0;
 
  prog = load_name( item );
 
  // clones should already be in the database, just increment the counter
  if( clonep( item ) )
  {
    if( !member( stuff, prog ) )
      return 0;
    stuff[prog,0]++;
    clones += ([ item ]);
  }
  else
  {
    // reloaded blueprint, update data
    if( member( stuff, prog ) )
    {
      stuff[prog,1] = item->query("spawn_limit");
      stuff[prog,2] = item->query("spawn_rate");
    }
    // fresh blueprint, create new entry
    else
    {
      prop_list = m_indices( labels );
      for( i=0, j=sizeof(prop_list) ; i<j ; i++ )
      {
        if( item->query(prop_list[i]) )
        {
          prop = prop_list[i];
          break;
        }
      }
      if( !prop )
      {
        push_log( item, 0 );
        return 1;
      }
      j = sizeof( labels[prop] );
      if( !j )
        return 0;
      i = random( j );
      label = labels[prop][i];
      labels[prop] -= ({ label });
 
     stuff += ([ prog : sizeof( clones( prog ) ) ;
                        item->query("spawn_limit") ;
                        item->query("spawn_rate") ;
                        label
              ]);
    }
 
    // This is to keep outside stuff from spawning.
    if( prog[0..15] != "/zone/null/stuff" )
    {
      stuff[prog,1] = 0;
      stuff[prog,2] = -100;
      push_log( item, 0 );
      return 1;
    }
 
    // try some new spawns
    for( i=stuff[prog,1]/2 ; i>0 ; i-- )
    {
      // the call_out mixes stuff up a bit more
      closure cl;
      cl = lambda( ({'x}), ({ #'+=, ({ #'spawn_queue }), 'x }) );
      call_out( cl, i, ({ prog }) );
    }
  }
  return 1;
}
 
// Pulls an item out of the game, and decrements its counter.  Basically
// we just wanna hold onto the object until all the call_out()s are done
// and heart_beat() is turned off.  Move it into the server where players
// can't get to it, and try desting it next reset().  Also queues the
// item to get respawned elsewhere on the mud.  Returns true if the item
// was successfully pulled out of the game.
int
pull_item( object item )
{
  string prog;
  if( !objectp( item ) || !item->query(StuffP) )
    return 0;
  if( ENV(item) == THISO )
    return 0;
  prog = load_name( item );
  if( !member( stuff, prog ) )
    return 0;
  if( !clonep( item ) )
    return 0;
  if( !move_object( item, THISO ) )
    return 0;
  stuff[prog,0]--;
  clones -= ([ item ]);
  call_out( lambda( 0, ({ #'+=, ({ #'spawn_queue }), quote(({prog})) }) ),
            set_next_reset(0)/2 );
  pull_log( item );
  return 1;
}
 
// This returns a list of all the stuff in the database optionally
// filtered through a specified property.
varargs string *
query_stuff_list( string prop )
{
  string *out;
  out = m_indices( stuff );
  if( prop )
    out = filter( out, lambda( ({ 'x }),
          ({ #'call_other, 'x, "query", prop }) ) );
  return out;
}
 
// Identify 'item' for 'who'.  Stores both program name and object for
// different types of identity.  id_type can set if just program
// identification is done, or object identification, or both.
void
do_id( object item, object who, int id_type )
{
  string prog, prname;
  if( !objectp( item ) || !item->query(StuffP) )
    return;
  if( !objectp( who ) || !query_once_interactive( who ) )
    return;
  prog = load_name(item);
  prname = who->query_real_name();
  if( !member( ids, prog ) )
    ids[prog] = ([ ]);
  if( !member( ids, item ) )
    ids[item] = ([ ]);
 
  ids[prog][prname] = 0;
  ids[item][prname] = 0;
 
  if( id_type & ID_ITEM )
    ids[prog][prname] |= ID_ITEM;
  if( id_type & ID_STATUS )
    ids[item][prname] |= ID_STATUS;
  if( id_type & ID_CHARGES )
    ids[item][prname] |= ID_CHARGES;
 
  return;
}
 
// Return whether or not 'item' has been identified for 'who'.  Checks
// both program and object identification, and returns a bitmask
// telling if the object has been identified in one or both ways, or 0
// if it hasn't been identified at all.
int
check_id( object item, object who )
{
  string prog, prname;
  int out;
 
  if( !objectp( who ) )
    return 0;
 
  prog = load_name(item);
  prname = who->query_real_name();
  if( member( ids, prog ) &&
      member( ids[prog], prname ) )
  {
    if( ids[prog][prname] & ID_ITEM )
      out |= ID_ITEM;
  }
  if( member( ids, item ) &&
      member( ids[item], prname ) )
  {
    if( ids[item][prname] & ID_STATUS )
      out |= ID_STATUS;
    if( ids[item][prname] & ID_CHARGES )
      out |= ID_CHARGES;
   }
  who->stuff_id_check( &out, item );
 
  return out;
}
 
// Returns the label assigned to the object's program group so that
// it can be used in the short description.
string
request_label( object item )
{
  string prog, out;
  int i, j;
 
  if( !objectp( item ) || !item->query(StuffP) )
    return 0;
  prog = load_name( item );
 
  if( member( stuff, prog ) )
    return stuff[prog,3];
  else
    return 0;
}
 
// Once we pull an item, we don't want it going back out again.
int
query_leave_ok( object item, object dest, object mover )
{
  return 1;
}
 
// Disallow shadowing
int
query_prevent_shadow( object shadower )
{
  return 1;
}
 
// Some temporary logging to monitor stuff usage in its infancy.
#define PUSH_LOG StuffLogDir "push.log"
#define PULL_LOG StuffLogDir "pull.log"
#define DEBUG_LOGGING 1  //  I don't think we need this right now -- Dorf
                         //  I disagree, something is wrong.  -- Whitewolf
 
void
push_log( object item, object targ )
{
#ifdef DEBUG_LOGGING
  if( targ )
    logf( PUSH_LOG, sprintf( "[%s] %O pushed to %O.\n",
      ctime(), item, targ ) );
  else                                                   // [ DORF ]
    logf( PUSH_LOG, sprintf( "[%s] %O not pushed.\n",
      ctime(), item ) );
#endif
  return;
}
 
void
pull_log( object item )
{
#ifdef DEBUG_LOGGING
  if( THISP )
    logf( PULL_LOG, sprintf( "[%s] %s pulled %O by \"%s\"\n",
      ctime(), PRNAME, item, query_command()||"" ) );
  else
    logf( PULL_LOG, sprintf( "[%s] %O was pulled by no one.\n",
      ctime(), item ) );
#endif
  return;
} 

mapping
datacheck()
{
  int i, j;
  string *progs;
  mapping out;

  out = ([ ]);
  progs = m_indices(stuff);
  for( i=0, j=sizeof(progs) ; i < j ; i++ )
  {
    object *guys = clones(progs[i], 2);
    guys = filter( guys, (: ENV($1) != THISO :) );
    if( sizeof(guys) != stuff[progs[i],0] )
      out += ([ progs[i] : sizeof(guys); stuff[progs[i],0] ]);
  }

  return out;
}

mapping
query_id_info()
{
  return deep_copy( ids );
}

mapping query_stuff_map()
{
  return( copy( stuff ) );
}

mapping query_clones()
{
  return( copy( clones ) );
}
