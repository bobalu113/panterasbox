#include <hit_location.h>

int blessed, invoke_time;

int bless();
int uncurse();
int curse();
varargs int identify( object who, int id_type );
int query_identified( object who );
int destruct_signal( object dester );
int remove();
static object *find_owner();
static string strip_article( string str );
int try_invoke( object invoker );
void invoke_signal( object invoker );
void invoke( object invoker );
void blessed_invoke( object invoker );
void cursed_invoke( object invoker );
int check_timeout();

static int get_hand(object who);

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
  if( THISO->query( "id_default" ) )
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

  if( time() - THISO->query( "last_view" ) >= HOUR )
  {
    logf( IDLE_LOG, sprintf( "[%s] %s idle for %d.  %s idle for %d.\nOwner: "
      "%O\nEnv: %O\n\n",
      ( ctime() ),
      ( object_name( THISO ) ),
      ( time() - THISO->query( "last_view" ) ),
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

  if( ( time() - THISO->query( "last_view" ) ) > ( HOUR * 2 ) )
    StuffServer->pull_item(THISO);

  return;
}

// Catches destruction and tells the server to pull it first.
int
destruct_signal( object dester )
{
  if( THISO->query_worn() )
    "/secure/player/commands/remove"->remove_armor( ENV(THISO), THISO, 1 );
  else if( THISO->query_wielded() )
    "/secure/player/commands/unwield"->unwield_weapon( ENV(THISO), 1 );
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
  ienv = filter( env, #'interactive );
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

// This is the action function for invoking the artifact.
int
do_invoke( string str )
{
  object *inv, oldp;
  int i, j;
  string what, hand;
  mixed tmp;

  if( ENV(THISO) != THISP )
    return 0;

  if( !stringp(str) || str=="" )
  {
    notify_fail( "Invoke what?\n" );
    return 0;
  }

  if( !THISO->id(str) )
  {
    notify_fail( "Invoke what?\n" );
    return 0;
  }

  if( present(str,THISP) != THISO )
  {
    notify_fail( "Invoke what?\n" );
    return 0;
  }

  if( THISO->try_invoke( THISP ) )
    return 1;

  // All the set_this_player() craziness in the messages is to make sure
  // the item is properly identified for all the different people.
  inv = all_inventory( ENV(THISP) );
  oldp = THISP;

  hand = THISP->query_hand_name() || "hand";

  if( !THISO->query( "max_charges" ) || THISO->query( "charges" ) == 0 )
  {
    tell_player( THISP, "Nothing happens.\n" );
    THISO->identify( THISP, ID_CHARGES );
  }
  else if( check_timeout() )
  {
    tell_player( THISP,
                 sprintf( "The %s remains lifeless in your %s.\n",
                 strip_article(THISO->short()), hand ) );
  }
  else
  {
    for( i=0, j=sizeof(inv) ; i<j ; i++ )
    {
      if( !living(inv[i]) || inv[i]==oldp )
        continue;
      set_this_player(inv[i]);
      tell_player( inv[i], sprintf( "%s invokes %s %s.",
               oldp->query_name(), possessive(oldp),
               strip_article(THISO->short()) ) );
      set_this_player(oldp);
    }
    tell_player( THISP,
                 sprintf( "You invoke your %s.",
                 strip_article(THISO->short()) ) );

    if( THISO->query_cursed() )
    {
      THISO->cursed_invoke( THISP );
      tell_player( THISP,
                   sprintf( "The artifact flashes red and explodes right in "
                   "your %s!\n", hand ) );
      tell_room( ENV(THISP),
                 sprintf( "The artifact flashes red and explodes right in "
                 "%s's %s!\n", THISP->query_name(), hand ),
                 ({ THISP }) );
      THISP->take_damage( 50 + THISO->query("charges")*10, get_hand(THISP),
                 "explosive", THISP, 0 );

      if( THISO->query_worn() )
        "/secure/player/commands/remove"->remove_armor( THISP, THISO, 1 );
      else if( THISO->query_wielded() )
        "/secure/player/commands/unwield"->unwield_weapon( THISP, 1 );
      StuffServer->pull_item(THISO);
    }
    else if( THISO->query_blessed() )
      THISO->blessed_invoke( THISP );
    else
      THISO->invoke( THISP );
    if( THISO->query( "charges" ) > 0 )
      THISO->set( "charges", THISO->query( "charges" ) - 1 );

    invoke_time = time();
    THISO->invoke_signal( THISP );
  }
  return 1;
}

// This function returns 0 if enough time has passed since the
// artifact's last use to invoke it again.
int
check_timeout()
{
  if( (time() - invoke_time) >= THISO->query( "invoke_timeout" ) )
    return 0;
  else
    return 1;
}

// Some scaffolding
int
try_invoke( object invoker )
{
  return 0;
}

void
invoke_signal( object invoker )
{
  return;
}

void
invoke( object invoker )
{
  tell_object( invoker, "Nothing happens.\n" );
  return;
}

void
cursed_invoke( object invoker )
{
  return invoke( invoker );
}

void
blessed_invoke( object invoker )
{
  return invoke( invoker );
}

static int
get_hand(object who)
{
  mixed *loc;
  int i, j, x;

  loc = who->query_hit_locations();
  x = loc[0];
  loc = loc[1..];
  for( i=0, j=sizeof(loc) ; i<sizeof(loc) ; i+=x )
  {
    if( loc[i+AREA_HANDS_OFFSET] )
      return i+1;
  }
  return 0;
}
int recharge( int c )
{
    if( THISO->query( "max_charges" ) <= 0 )
        return 0;
    THISO->set( "charges", THISO->query( "max_charges" ) );
    return 1;
}
