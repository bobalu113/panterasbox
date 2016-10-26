#include <hit_location.h>

inherit StuffCode;

int zap_timeout = 3*60;
int zap_delay = 1;
int charges = -1;
int max_charges = 6;
int recharged = 0;
int try_pull = 0;
private int zap_time = 0;


void create();
set( string var, mixed val );
mixed query( string var );
string short();
string long( string id );
string post_long();
int id( string str );
void init();
int do_zap( string str );
static object zap_target( string arg, object who );
void cursed_zap( object target );
void zap( object target );
void blessed_zap( object target );
int check_timeout();
int try_zap( object zapper, object target );
void zap_signal( object zapper, object target );
static int get_hand( object who );


void
create()
{
  ::create();
  if( !query("value") )
    set( "value", 75 );
  if( charges < 0 )
    charges = random(max_charges/2) + (max_charges/2) + 1;
  if( !query("short") )
  {
    set( "short", "a wand of pointing" );
    set( "ids", ({ "a wand of pointing",
                   "wand of pointing",
                   "pointing wand",
                   "pointing" }) );
  }
  return;
}

set( string var, mixed val )
{
  switch( var )
  {

    case "zap_delay":
      zap_delay = val;
      return 1;
    case "zap_timeout":
      zap_timeout = val;
      return 1;
    case "charges":
      charges = val;
      if(charges > max_charges)
        max_charges = charges;
      return 1;
    case "max_charges":
      max_charges = val;
      if(max_charges < charges)
        charges = max_charges;
      return 1;
    case "recharged":
      return 0;
  }
  return ::set( var, val );
}

mixed
query( string var )
{
  switch( var )
  {
    case "zap_delay":
      return zap_delay;
    case "zap_timeout":
      return zap_timeout;
    case "charges":
      return charges;
    case "max_charges":
      return max_charges;
    case "recharged":
      return recharged;
    case WandP:
      return 1;
  }
  return ::query( var );
}

// Special identify majicks.
string
short()
{
  string out;

  if( THISI )
    last_view = time();

  if( THISO->query_identified(THISP) & ID_ITEM )
    out = ::short();
  else if( strlen( id_label ) )
    out = sprintf( "%s %s wand",
                   ( IsVowel(id_label[0]) ? "an" : "a" ),
                   id_label );
  else
    out = "a wand";

  return out + post_short();
}

// Overload post_short() to add charges info
string
post_short()
{
  string out;

  out = ::post_short();
  if( THISO->query_identified(THISP) & ID_CHARGES )
    out += sprintf(" (%d:%d)", charges, recharged );
  return out;
}

string
long( string id )
{
  string out, hand, timeout;
 
  hand = THISP->query_hand_name() || "hand";

  out = format( sprintf( "This %s rod is about 12 inches in length and is "
                "rounded at one end.  The base feels slightly %s in "
                "your %s.\n"
                "Zapping the wand might have unpredictable effects.",
                id_label, ( check_timeout() ? "cold" : "warm" ), hand ) );
  return out + THISO->post_long();
}

string
post_long()
{
  return "\nScratched into the base of the wand are the words, "
         "\"Stuff, Inc.\"";
}

// Special identify majicks.
int
id( string str )
{
  if( str == THISO->short() )
    return 1;
  str = lower_case( str );
  if( str == "stuff" || str == "wand" || str == "a wand" )
    return 1;
  if( ( THISO->query_identified(THISP) & ID_ITEM ) && ::id(str) )
    return 1;
  if( ( str == sprintf( "%s %s wand",
                      ( IsVowel(id_label[0]) ? "an" : "a" ),
                      id_label ) ) ||
      ( str == sprintf( "%s wand", id_label ) ) )
    return 1;
  return 0;
}

void
init()
{
  ::init();
  //  stupid combat causing players to flee before init() 
  if( !THISP || 
      ( !present( THISO, ENV(THISP) && !present( THISO, THISP ) ) ) )
    return;
  add_action( "do_zap", "zap" );
  return;
}

// This is the action function for zapping the wand.
int
do_zap( string str )
{
  object target, *inv, oldp;
  int i, j;
  string what, tname, hand;
  mixed tmp;

  if( ENV(THISO) != THISP )
    return 0;
  if( !stringp(str) || str=="" )
  {
    notify_fail( sprintf( "%s what?\n", CAP( query_verb() ) ) );
    return 0;
  }
  if( sscanf( str, "%s with %s", what, str ) != 2 )
    notify_fail( sprintf( "%s what with what?\n", CAP( query_verb() ) ) );
  if( sscanf( str, "%s at %s", str, what ) != 2 )
    notify_fail( sprintf( "%s what at what?\n", CAP( query_verb() ) ) );
  if( !stringp(what) || !strlen(what) )
    return 0;
  if( present(str,THISP) != THISO )
  {
    notify_fail(sprintf("You don't have anything like that with which to %s.\n",
                 query_verb() ) );
    return 0;
  }

  if( !objectp(target = zap_target( what, THISP )) )
  {
    notify_fail( "There isn't anything like that here.\n" );
    return 0;
  }

  if( target == THISO )
  {
    write( "The wand can't zap itself!\n" );
    return 1;
  }

  if( THISO->try_zap(THISP, target) )
    return 1;

  // Standard combat checks.
  if( target != THISP )
  {
    tmp = ({target}) +
          ( all_environment(target) || ({ }) ) -
          ({THISP});
    if( sizeof( filter( tmp, #'living ) ) )
    {
      // Zapper doing something to someone other than himself
      if( ( tmp[<1]->query(NoPKP) && is_player(target) ) ||
      tmp[<1]->query(NoCombatP) )
      {
        write( "Aggressive acts are not allowed here.\n" );
        return 1;
      }
    }

    if( living(target) &&  !THISP->valid_target( target ) )
    {
      write( "You can't attack that person.\n" );
      return 1;
    }
  }

  THISP->add_combat_delay(zap_delay);

  // All the set_this_player() craziness in the messages is to make sure
  // the item is properly identified for all the different people.
  inv = all_inventory( ENV(THISP) );
  oldp = THISP;
  if( living(target) )
    tname = target->query_name();
  else
    tname = target->short();
  if( !stringp(tname) || !strlen(tname) )
    tname = what;

  hand = THISP->query_hand_name() || "hand";

  if( charges <= 0 ) 
  {
    try_pull = 0;
    tell_player( THISP, "Nothing happens.\n" );
  }
  else if( check_timeout() )
  {
    tell_player( THISP,
                 sprintf( "The %s shakes violently in your %s.\n",
                 strip_article(short()), hand ) );
  }
  else
  {
    for( i=0, j=sizeof(inv) ; i<j ; i++ )
    {
      if( !living(inv[i]) || inv[i]==oldp || inv[i]==target )
        continue;
      set_this_player(inv[i]);
      tell_player( inv[i], sprintf( "%s zaps %s with %s %s.",
               oldp->query_name(), tname, possessive(oldp),
               strip_article(short()) ) );
      set_this_player(oldp);
    }
    set_this_player(target);
    if( living(target) )
      tell_player( target,
                   sprintf( "%s zaps you with %s %s!",
                   oldp->query_name(), possessive(oldp),
                   strip_article(short()) ) );
    set_this_player(oldp);
    tell_player( THISP,
                 sprintf( "You zap %s with your %s.",
                 tname, strip_article(short()) ) );

    if( THISO->query_cursed() )
    {
      tell_player( THISP,
                   sprintf( "The wand flashes red and explodes right in "
                   "your %s!\n", hand ) );
      tell_room( ENV(THISP),
                 sprintf( "The wand flashes red and explodes right in "
                 "%s's %s!\n", THISP->query_name(), hand ),
                 ({ THISP }) );
      THISP->take_damage( charges*10, get_hand(THISP), "explosive", THISP, 0 );
      THISO->cursed_zap( target );
      StuffServer->pull_item(THISO);
    }
    else if( THISO->query_blessed() )
      THISO->blessed_zap( target );
    else
      THISO->zap( target );
    charges--;

    zap_time = time();
    THISO->zap_signal( THISP, target );
  }
  return 1;
}

// This function returns 0 if enough time has passed since the 
// wand's last use to zap it again.
int
check_timeout()
{
  if( (time() - zap_time) >= zap_timeout )
    return 0;
  else
    return 1;
}

// Some scaffolding
int
try_zap( object zapper, object target )
{
  return 0;
}

void
zap_signal( object zapper, object target )
{
  return;
}

void
zap( object target )
{
  tell_object( THISP, "Nothing happens.\n" );
  if( living(target) )
    tell_object( target, "Nothing happens.\n" );
  return;
}

void
cursed_zap( object target )
{
  return;
}

void
blessed_zap( object target )
{
  return zap( target );
}

static object
zap_target( string arg, object who )
{
  object out;
  out = present( arg, ENV(who) );
  if( !objectp(out) )
    out = present( arg, who );
  return out;
}

int
recharge( int c )
{
  try_pull = 0;
  if( !c )
    charges = max_charges;
  else
    charges += c;
  recharged++;
  return 1;
}

void
reset()
{
  object *owner;

  ::reset();
  if( (ENV(THISO) != FINDO(StuffServer)) && (charges <= 0) )
  {
    // special handling for wands with zero charges
    try_pull++;
    owner = find_owner();

    if( pointerp(owner) && sizeof(owner) )
    {
      object who = owner[<1];
      // if being carried, pull on third reset
      if( sizeof(owner) >= 1 )
      {
        // being carried, not in a container
        if(try_pull >= 3)
        {
          tell_player( who, sprintf("You hear a loud pop as your %s "
            "vanishes into thin air!", strip_article(short())) );
          StuffServer->pull_item(THISO);
        }
        else
        {
          tell_player( who, sprintf("Your %s vibrates for a moment.",
            strip_article(short())) );
        }
      }
      else
      {
        object cont = owner[<2];
        // being carried in a container
        if(try_pull >= 3)
        {
          tell_player( who, sprintf("You hear a muffled pop come from "
            "inside your %s.", strip_article(cont->short())) );
          StuffServer->pull_item(THISO);
        }
        else
        {
          tell_player( who, sprintf("Your %s vibrates for a moment.",
            strip_article(cont->short())) );
        }
      }
    }
    else if( try_pull >= 2 )
    {
      // otherwise, pull on second reset
      StuffServer->pull_item(THISO);
    }
  }
  return;
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
