
inherit ObjectCode;

int blessed, id_default, spawn_limit, spawn_rate, last_view;
string id_label, *prevent_spawn;

void create();
string post_short();
set( string var, mixed val );
mixed query( string var );
int bless();
int uncurse();
int curse();
varargs int identify( object who, int id_type );
int query_identified( object who );
int destruct_signal( object dester );
int remove();
static object *find_owner();
static string strip_article( string str );

#define CREATE_LOG StuffLogDir "create.log"

void
create()
{
  object *stack;

  spawn_limit = 1;
  spawn_rate = 0;
  last_view = time();

  // some objects get cursed/blessed automatically
  switch( random(100) )
  {
    case 0..19  : blessed = -1; break;
    case 20..29 : blessed = 1; break;
    default     : break;
  }

  ::create();

  // If the server can't register our item, give up right away.
  if( !StuffServer->push_item(THISO) )
    destruct(THISO);

  // For short descriptions later on.
  id_label = StuffServer->request_label(THISO);

  if( clonep() && pointerp( stack = caller_stack() ) &&
    load_name( stack[0] ) != StuffServer[0..<3] )
    logf( CREATE_LOG, sprintf( "[%s] %s cloned by:\n%O\n", ctime(),
      object_name( THISO ), caller_stack() ) );

  return;
}

// The post_short() addition in ObjectCode was fucking me up so I'll
// add it manually in the child modules.
string
short()
{
  if( THISI )
    last_view = time();
  return query("short");
}

// Just tells whether the item has been blessed, cursed, or is uncursed
// for people that have identified it.
string
post_short()
{
  if( !( query_identified(THISP) & ID_STATUS ) )
    return "";
  if( THISO->query_blessed() )
    return " [blessed]";
  else if( THISO->query_cursed() )
    return " [cursed]";
  else
    return " [uncursed]";
}

set( string var, mixed val )
{
  switch( var )
  {
    case "id_default":
      id_default = val ? 1 : 0;
      return 1;
    case "spawn_limit":
      spawn_limit = val;
      return 1;
    case "spawn_rate":
      spawn_rate = val;
      return 1;
    case "prevent_spawn":
      prevent_spawn = val;
    case CursedP:
      if( val )
        curse();
      else
        uncurse();
      return 1;
    case BlessedP:
      if( val )
      {
        bless();
        return 1;
      }
      else
        return 0;
  }
  return ::set( var, val );
}

mixed
query( string var )
{
  switch( var )
  {
    case "id_default":
      return id_default;
    case "spawn_limit":
      return spawn_limit;
    case "spawn_rate":
      return spawn_rate;
    case "prevent_spawn":
      return prevent_spawn;
    case StuffP:
      return 1;
    case NoStoreP:
      return 1;
    case BlessedP:
      return THISO->query_blessed();
    case CursedP:
      return THISO->query_cursed();
  }
  return ::query( var );
}

// Blesses an object, duh.
int
bless()
{
  if( blessed < 0 )
    blessed = 0;
  blessed++;
  return 1;
}

// Uncurses an object that's been cursed.
int
uncurse()
{
  if( blessed < 0 )
  {
    blessed = 0;
    return 1;
  }
  return 0;
}

// Unblesses an object that's been blessed.
int
unbless()
{
  if( blessed > 0 )
  {
    blessed = 0;
    return 1;
  }
  return 0;
}

// Curses an object, duh.
int
curse()
{
  if( blessed > 0 )
    blessed = 0;
  blessed--;
  return 1;
}

// Three accessor functions to see if something is blessed or cursed.
int
query_blessed()
{
  return blessed > 0;
}

int
query_cursed()
{
  return blessed < 0;
}

int
query_uncursed()
{
  return blessed == 0;
}

// Identifies this object for 'who'.  For further documentation see
// do_id() in server.c.
varargs int
identify( object who, int id_type )
{
  int base_id = query_identified(who);

  if( !id_type )
    id_type = ID_ALL;
  if( base_id & ID_STATUS )
    id_type |= ID_STATUS;
  if( base_id & ID_ITEM )
    id_type |= ID_ITEM;
  if( base_id & ID_CHARGES )
    id_type |= ID_CHARGES;
  return StuffServer->do_id( THISO, who, id_type );
}

// Returns whether this object has been identified for 'who'.  For further
// documentation see check_id() in server.c.
int
query_identified( object who )
{
  int out;
  out = StuffServer->check_id( THISO, who );
  if( id_default )
    out |= ID_ITEM;
  return out;
}

#define IDLE_LOG StuffLogDir "idle.log"
#define HOUR 3600

// An object will expire itself if it hasn't been used after ~1-2 hours.
void
reset()
{
  object *owner;
  owner = find_owner();

  if( !clonep() )
    return;

  // pull Stuff from net-dead players (query_idle will be 0)
  if( pointerp(owner) && query_once_interactive(owner[<1]) &&
    !interactive(owner[<1]) )
  {
     logf( IDLE_LOG, sprintf( "[%s] %s idle for %d.\nOwner (%s) net-dead.\n\n",
       ctime(),
       object_name( THISO ),
       time() - last_view,
       owner[<1]->query_real_name() ) );
     StuffServer->pull_item( THISO );
     return;
  }

  if( time() - last_view >= HOUR )
  {
    logf( IDLE_LOG, sprintf( "[%s] %s idle for %d.  %s idle for %d.\nOwner: "
      "%O\nEnv: %O\n\n",
      ( ctime() ),
      ( object_name( THISO ) ),
      ( time() - last_view ),
      ( pointerp( owner ) ? to_string( owner[<1] ) :
      objectp( ENV( THISO ) ) ? to_string( ENV( THISO ) ) : "No ENV" ),
      ( pointerp( owner ) ? efun::query_idle( owner[<1] ) :
      ( objectp( ENV( THISO ) ) && interactive( ENV( THISO ) ) ) ?
      efun::query_idle( ENV( THISO ) ) : 0 ),
      ( owner ),
      ( all_environment( THISO ) ) ) );
  }

  if( pointerp(owner) && ( efun::query_idle(owner[<1]) > HOUR ) )
  {
//      if( !random( 4 ) )  If they idle for over an hour, screw em.  Whitewolf
          StuffServer->pull_item(THISO);
        return;
  }

  if( ( time() - last_view ) > ( HOUR * 2 ) )
    StuffServer->pull_item(THISO);

  return;
}

// Catches destruction and tells the server to pull it first.
int
destruct_signal( object dester )
{
  StuffServer->pull_item( THISO );
  return 0;
}

// The server calls this every reset to see if it can safely dest the
// object after it's been pulled from the game.  Return 0 to prevent
// destruction.
int
remove()
{
  if( ENV(THISO) != FINDO(StuffServer) )
    return 0;
  if( member( heart_beat_info(), THISO ) != -1 )
    return 0;
  if( member( map( call_out_info(), #'[, 0 ), THISO ) != -1 )
    return 0;
  return 1;
}

// utility function to find the interactive object holding THISO
// returns array of environments ala all_environment, with last
// element being the first containing interactive
static object *
find_owner()
{
  object *env, *ienv;
  int i;
  env = all_environment(THISO);
  if( !pointerp(env) )
    return 0;
  //ienv = filter( env, #'interactive );  
  ienv = filter( env, #'query_once_interactive );
  if( sizeof(ienv) > 0 )
  {
    i = member( env, ienv[0] );
    return env[0..i];
  }
  return 0;
}

// Here's a little helper function to remove the article at the beginning
// of the short description.
static string
strip_article(string str)
{
  int len;
  len = strlen(str);
  if( len>2 && str[0..1]=="a " )
    return str[2..];
  else if( len>3 && str[0..2]=="an " )
    return str[3..];
  else if( len>4 && str[0..3]=="the " )
    return str[4..];
  else
    return str;
}

// This function is called routinely by the StuffServer to ensure that
// inactive items still have reset() invoked by the driver.
nomask int
touch()
{
  return 1;
}
