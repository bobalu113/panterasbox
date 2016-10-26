
/*  Inheritable StuffCode Altar Module

** API **
int try_worship( object w )         // return 1 to allow membership
void add_worshipper( object w )     // background, adds player to ranks
void post_worship( object w )       // stub, also called when player logs in
                                    // This is where you set up whatever kind
                                    // of tracking obj/shad to change favor
int evaluate_worshipper( object w, int verbose ) // adjust favor, or give
                                                 // verbal indication of favor
int query_favor( object w )         // return favor rating
void punish_worshipper( object w )  // stub, called at reset() if favor <-20
void bless_worshipper( object w )   // stub, called at reset() if favor >20
int add_favor( object w, int amt )  // returns new favor rating

int remove_worshipper( object w )   // return 1 on success of abandonment
void post_abandon( object w )       // stub, put any divine retribution here
void remove_worshipper_name( string name )    // for auto-removal of a name,
                                          // player need not be logged in,
                                          // nor will they suffer retribution
*/

inherit ObjectCode;

int id_default, spawn_limit, spawn_rate, last_view;
string id_label, god_name;
mapping worshippers;
void create();
string post_short();
set( string var, mixed val );
mixed query( string var );
varargs int identify( object who, int id_type );
int query_identified( object who );
int destruct_signal( object dester );
int remove();
static string strip_article( string str );

#define ALTAR_LOG StuffLogDir "altar.log"
#define BANNED_GUILDS ({ "crusader", "dustman", "red disciple", "mageslayer" })

void
create()
{
  object *stack;
  int temp;

  spawn_limit = 1;
  spawn_rate = 0;
  last_view = time();
  worshippers = ([]);

  ::create();

/*  Add this in when Altars really go online
  // If the server can't register our item, give up right away.
  if( !StuffServer->push_item(THISO) )
    destruct(THISO);

  // For short descriptions later on.
  id_label = StuffServer->request_label(THISO);

  if( clonep() && pointerp( stack = caller_stack() ) &&
    load_name( stack[0] ) != StuffServer[0..<3] )
    logf( ALTAR_LOG, sprintf( "[%s] %s cloned by:\n%O\n", ctime(),
      object_name( THISO ), caller_stack() ) );
*/
//  Remove the next 2 lines when Altars really go online
  temp = random(sizeof(LABELS[NonStandardP]));
  id_label = LABELS[NonStandardP][temp];
  if( clonep() )
    NOTIFIER->add_notify();
  return;
}

string
short()
{
 string out;

  if( THISI )
    last_view = time();

  if( strlen( id_label ) )
    out = sprintf( "%s %s shrine",
                   ( IsVowel(id_label[0]) ? "an" : "a" ),
                   id_label );
  else
    out = "a shrine";

  return out + post_short();

}

// Just tells what god the altar belongs to,
// for people that have identified it.
string
post_short()
{
  string out = "";

  if( THISO->query_identified(THISP) & ID_ITEM )
  {  
    if( member( worshippers, THISP->query_real_name() ) )
      out += sprintf( " [%s, your god]", query( "god_name" ) );
    else out += sprintf( " [%s]", query( "god_name" ) );
  }
  return out;
}

string
post_long()
{
  if( THISP )
    THISO->identify( THISP, ID_ALL );
  return sprintf( "\nYou could 'worship' here if you desire to %s "
         "%s.\nEngraved at the base of the altar are "
         "the words \"Stuff, Inc.\"\n", 
         THISP->query("altar") == THISO ? "sense your standing with" :
         "become a follower of",
         query( "god_name" ) );
}

int
id( string str )
{
  string lstr;
  if( str == THISO->short() )
    return 1;
  lstr = lower_case( str );
  if( lstr == "stuff" || lstr == "altar" || lstr == "shrine" ||
      lstr == query("god_name") || lstr == lower_case( query("god_name") ) )
    return 1;
  if( ( THISO->query_identified(THISP) & ID_ITEM ) && ::id(str) )
    return 1;
  if( ( lstr == sprintf( "%s %s shrine",
                      ( IsVowel(id_label[0]) ? "an" : "a" ),
                      id_label ) ) ||
        lstr == sprintf( "%s altar", id_label ) ||
        lstr == sprintf( "%s shrine", id_label ) )
    return 1;
  return 0;
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
    case "god_name":
      god_name = val;
      return 1;
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
      return ({ ShopP, LivingP, ContainerP });
    case "god_name":
      return god_name;
    case "gettable":
      return 0;
    case StuffP:
      return 1;
    case NoStoreP:
      return 1;
    case NonStandardP:
      return 1;
    case "worshippers":
      return copy( worshippers );
  }
  return ::query( var );
}

// Identifies this object for 'who'.  For further documentation see
// do_id() in server.c.
varargs int
identify( object who, int id_type )
{
  int base_id = query_identified(who);

  return StuffServer->do_id( THISO, who, ID_ALL );
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

#define HOUR 3600
// Altars should stick around longer than other Stuff
void
reset()
{
  int x;
  if( !clonep() )
    return;

  if( ( time() - last_view ) > ( HOUR * 96 ) )  // 4 days
  {
    StuffServer->pull_item(THISO);
    return;
  }

  if( !sizeof( worshippers ) )
    return;

  foreach( string name : m_indices( worshippers ) )
    if( FINDP(name) && interactive( FINDP(name) ) )
    { 
       x = THISO->evaluate_worshipper( FINDP(name) );
       if( x > 20 )
         THISO->bless_worshipper( FINDP(name) );
       else if( x < -20 )
         THISO->punish_worshipper( FINDP(name) );
    }

  return;
}

int
destruct_signal( object dester )
{
  foreach( string name : m_indices( worshippers ) )
    if( FINDP(name) )
        tell_object( FINDP(name), sprintf( "%s evaporates in a puff "
          "of logic!\n", THISO->query("god_name") ) );
  StuffServer->pull_item(THISO);
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
   
void
init()
{
   ::init();
   //  stupid combat causing players to flee before init()
   if( !THISP ||
      ( !present( THISO, ENV(THISP) && !present( THISO, THISP ) ) ) )
    return;

  add_action( "do_worship", "worship" );
  return;
}

int do_worship( string arg )
{
  object ob, who;
  string gname;
  gname = THISO->query("god_name");
  ob = THISP->query( "altar" );
  who = THISP;

  notify_fail( "Worship whom?\n" );
  if( arg && !THISO->id( arg ) && arg != upper_case( gname ) ) 
      return 0;

  if( !arg && ob == THISO )
      arg = gname;

  if( !arg )
  {
      tell_object( who, sprintf( "%sAre you sure you want to join this "
          "religion?\nIf so, 'worship %s'.\n", THISO->query("long"), 
               gname ) );
      return 1;
  }
   
  if( member( BANNED_GUILDS, who->query_guild() ) > -1 )
  {
      tell_object( who, "You are already worshipping the god of your "
          "guild.\n" );
      return 1;
  }

  if( !ob )
  {
      if( THISO->try_worship( who ) )
          THISO->add_worshipper( who );
      return 1;
  }

  if( ob == THISO )
  {
      THISO->evaluate_worshipper( who, 1 );
      return 1;
  }

  if( arg != upper_case( gname ) )
  {
      tell_object( who, sprintf( "You are already a worshipper of %s!\n"
          "If you really want to switch to %s, you can 'worship %s'.\n",
               ob->query("god_name") || "another diety", gname, 
                   upper_case( gname ) ));
      return 1;
  }

  if( ob->remove_worshipper( who ) && THISO->try_worship( who ) )
      THISO->add_worshipper( who );
  return 1;
}

void add_worshipper( object w )
{ 
  if( member( worshippers, THISP->query_real_name() ) == 0 )
    worshippers += ([ THISP->query_real_name() : 1 ]);
  tell_object( w, sprintf( "%s accepts your worship!\n", 
    THISO->query( "god_name" ) ) );
  THISP->set( "altar", THISO );
  THISO->post_worship( THISP );
}

//  Return 1 on success, return 0 to disallow leaving
int remove_worshipper( object w )
{
  if( objectp(w) )
  {
    if( member( worshippers, w->query_real_name() ) > 0 )
    {
      worshippers -= ([ w->query_real_name() ]);
      THISO->post_abandon( w );
      w->set( "altar", 0 );
      return 1;
    }
  }
  return 0;
}

void remove_worshipper_name( string name )
{
  worshippers -= ([ name ]);
}

int try_worship( object w )
{
  return 1;
}

void post_worship( object w )
{
  return;
}

//  Called every reset, slowly moves toward 0
//  If verbose, give worshipper a clue of their god's happiness
int evaluate_worshipper( object w, int verbose )
{
    int x;
    if( member( worshippers, w->query_real_name() ) == 0 )
      return 0;
    x = worshippers[ w->query_real_name() ];
    if( verbose )
    {
      tell_object( w, sprintf( "%s is %s you.\n",
        THISO->query("god_name"),
        x > 100 ? "supremely pleased with" :
        x > 50 ? "pleased with" :
        x > 20 ? "satisfied with" :
        x >= 0 ? "aware of" :
        x > -5 ? "slightly miffed at" :
        x > -20 ? "disgruntled with" : "angry at" ) );
        return x;
    }
    if( abs(x) < 10 && x != 0 )  
      x += -x/x;     // go 1 step closer to 0
    else             // or
      x -= x/10;     // go 10% closer to 0
    worshippers[ w->query_real_name() ] = x; 
    return x;
}

void punish_worshipper( object w )
{
    return;
}

void bless_worshipper( object w )
{
    return;
}

int add_favor( object w, int amt )
{
    string name;

    if( !w ) return 0;
    name = w->query_real_name();

    if( !name || member( worshippers, name ) == 0 )
      return 0;

    worshippers[name] += amt;
    return worshippers[name];
}

mixed query_worshippers()
{
    return copy( worshippers );
}

notify_logon( object who, status recon, string name, string level, string ip )
{
    if( !clonep() )
        return;

    if( member( worshippers, who->query_real_name() ) == 1 && !recon )
    {
        THISO->post_worship( who );
        who->set( "altar", THISO );
        THISO->evaluate_worshipper( who, 1 );
    }
}

int query_favor( object w )
{
    string name;

    if( !w ) return 0;
    name = w->query_real_name();

    if( !name || member( worshippers, name ) == 0 )
      return 0;

    return worshippers[name];
}
