inherit StuffCode;

int quaff_delay = 1;
int throw_delay = 1;

void create();
set( string var, mixed val );
mixed query( string var );
string short();
string long( string id );
string post_long();
int id( string str );
void init();
int do_quaff( string str );
int do_throw( string str );
varargs void quaff( object who, int thrown );
varargs void cursed_quaff( object who, int thrown );
varargs void blessed_quaff( object who, int thrown );
int try_quaff( object who );
int try_throw( object thrower, object target );
void quaff_signal( object who );
void throw_signal( object thrower, object target );


void
create()
{
  ::create();
  if( !query("value") )
    set( "value", 75 );
  if( !query("short") )
  {
    set( "short", "a potion of water" );
    set( "ids", ({ "a potion of water",
                   "potion of water",
                   "water potion",
                   "water" }) );
  }
  return;
}

set( string var, mixed val )
{
  switch( var )
  {
    case "quaff_delay":
      quaff_delay = val;
      return 1;
    case "throw_delay":
      throw_delay = val;
      return 1;
  }
  return ::set( var, val );
}

mixed
query( string var )
{
  switch( var )
  {
    case "quaff_delay":
      return quaff_delay;
    case "throw_delay":
      return throw_delay;
    case PotionP:
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
    out = sprintf( "%s %s potion",
                   ( IsVowel(id_label[0]) ? "an" : "a" ),
                   id_label );
  else
    out = "a potion";

  return out + post_short();
}

string
long( string id )
{
  string out;
  out = format( sprintf( "This small corked bottle contains a strange %s "
                "liquid.  Drinking it or throwing it at another player might "
                "have unpredictable effects.", id_label ) );
  return out + THISO->post_long();
}

string
post_long()
{
  return "\nPrinted on the underside of the bottle are the words, "
         "\"Stuff, Inc.\"";
}

// Special identify majicks.
int
id( string str )
{
  string lstr;
  if( str == THISO->short() )
    return 1;
  lstr = lower_case( str );
  if( lstr == "stuff" || lstr == "potion" || lstr == "a potion" )
    return 1;
  if( ( THISO->query_identified(THISP) & ID_ITEM ) && ::id(str) )
    return 1;
  if( ( lstr == sprintf( "%s %s potion",
                      ( IsVowel(id_label[0]) ? "an" : "a" ),
                      id_label ) ) ||
      ( lstr == sprintf( "%s potion", id_label ) ) )
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
  add_action( "do_quaff", "quaff" );
  add_action( "do_quaff", "drink" );
  add_action( "do_throw", "throw" );
  add_action( "do_throw", "toss" );
  return;
}

// This is the action function for drinking the potion.
int
do_quaff( string str )
{
  int i, j;
  object oldp, *inv;

  if( ENV(THISO) != THISP )
    return 0;
  if( !stringp(str) || str=="" )
  {
    notify_fail( sprintf( "%s what?\n", CAP( query_verb() ) ) );
    return 0;
  }
  if( present(str,THISP) != THISO )
  {
    notify_fail( sprintf( "You don't have anything like that to %s.\n",
                 query_verb() ) );
    return 0;
  }
  if( is_player( THISP ) && ( !THISP->query_digestion_rate() || 
      !THISP->drink_alco( ({ 0,0,0,10 }) ) ) )
  {
    notify_fail( sprintf( "You aren't able to %s that.\n",
                 query_verb() ) );
    return 0;
  }
  if( THISO->try_quaff(THISP) )
    return 1;

  THISP->add_combat_delay(quaff_delay);

  // All the set_this_player() craziness in the messages is to make sure
  // the item is properly identified for all the different people.
  tell_player( THISP, "You uncork the bottle and let the smooth liquid slide "
         "down your throat." );
  inv = all_inventory( ENV(THISP) );
  oldp = THISP;
  for( i=0, j=sizeof(inv) ; i<j ; i++ )
  {
    if( !living(inv[i]) || inv[i]==oldp )
      continue;
    set_this_player(inv[i]);
    printf( "%s drinks %s.\n", oldp->query_name(), short() );
    set_this_player(oldp);
  }

  if( THISO->query_cursed() )
    THISO->cursed_quaff( THISP, 0 );
  else if( THISO->query_blessed() )
    THISO->blessed_quaff( THISP, 0 );
  else
    THISO->quaff( THISP, 0 );

  THISO->quaff_signal( THISP );
  StuffServer->pull_item( THISO );

  return 1;
}

// This is the action function for throwing the potion.
int
do_throw( string str )
{
  mixed who;
  string name;
  int chance, i, j;
  object oldp, *inv;

  if( ENV(THISO) != THISP )
    return 0;
  if( !stringp(str) || str=="" )
  {
    notify_fail( sprintf( "%s what?\n", CAP( query_verb() ) ) );
    return 0;
  }
  if( sscanf( str, "%s at %s", str, who ) != 2 )
  {
    notify_fail( sprintf( "%s what at whom?\n", CAP( query_verb() ) ) );
    return 0;
  }
  if( present(str,THISP) != THISO )
  {
    notify_fail( sprintf( "You don't have anything like that to %s.\n",
                 query_verb() ) );
    return 0;
  }
  if( !objectp( who = present( who, ENV(THISP) ) ) )
  {
    notify_fail( "There's no one here by that name.\n" );
    return 0;
  }

  if( THISO->try_throw(THISP, who) )
    return 1;

  if( !living( who ) )
  {
    if( stringp( name = who->short() ) )
      printf( "You hurl the potion at %s!\nWhat a waste.\n", name );
    else
      write( "You hurl the potion into the air!\nWhat a waste.\n" );
    return 1;
  }

  // Standard combat checks.
  if( ( ENV(who)->query(NoPKP) && is_player(who) ) ||
      ENV(who)->query(NoCombatP) )
  {
    write( "Aggressive acts are not allowed here.\n" );
    return 1;
  }
  if( !THISP->valid_target( who ) )
  {
    write( "You can't attack that person.\n" );
    return 1;
  }

  THISP->add_combat_delay(throw_delay);

  // Find out their chances of hitting their target.
  chance = 200 +
           random( 100 + THISP->query_stat("dex") ) +
           THISP->query_skill("weapon throwing") * 3;
  chance -= random(2000);
  if( ( THISP->query_stealthy() || THISP->query_hidden() ) &&
      !(THISP->in_combat()) )
    chance = 1;

  // All the set_this_player() craziness in the messages is to make sure
  // the item is properly identified for all the different people.
  if( chance < 0 ) {
    inv = all_inventory( ENV(THISP) );
    oldp = THISP;
    for( i=0, j=sizeof(inv) ; i<j ; i++ )
    {
      if( !living(inv[i]) || inv[i]==oldp || inv[i]==who )
        continue;
      set_this_player(inv[i]);
      tell_player( inv[i], sprintf( "%s throws %s but it completely misses "
      "%s!", oldp->query_name(), short(), who->query_name() ) );
      set_this_player(oldp);
    }
    set_this_player(who);
    tell_player( who,
                 sprintf( "%s throws %s at you but completely misses!",
                 oldp->query_name(), short() ) );
    set_this_player(oldp);
    tell_player( THISP, sprintf( "You hurl the potion and it whizzes right "
                 "past %s!", who->query_name() ) );
  } else {
    inv = all_inventory( ENV(THISP) );
    oldp = THISP;
    for( i=0, j=sizeof(inv) ; i<j ; i++ )
    {
      if( !living(inv[i]) || inv[i]==oldp || inv[i]==who )
        continue;
      set_this_player(inv[i]);
      tell_player( inv[i], sprintf( "%s's %s flies through the air and "
               "shatters right on %s's head!", oldp->query_name(),
               strip_article(short()), who->query_name() ) );
      set_this_player(oldp);
    }
    set_this_player(who);
    tell_player( who,
                 sprintf( "%s's %s flies through the air and shatters "
                 "right on your head!", oldp->query_name(),
                 strip_article(short()) ) );
    set_this_player(oldp);
    tell_player( THISP,
                 sprintf( "Your %s flies through the air and shatters "
                 "right on %s's head!", strip_article(short()),
                 who->query_name() ) );

    who->take_damage( random( THISP->query_stat("str")/50 + 1 ),
                      member( who->query_hit_locations(), "head" ),
                      "blunt",
                      THISP,
                      THISO );
    if( THISO->query_cursed() )
      THISO->cursed_quaff( who, 1 );
    else if( THISO->query_blessed() )
      THISO->blessed_quaff( who, 1 );
    else
      THISO->quaff( who, 1 );
    THISO->throw_signal( THISP, who );
  }

  StuffServer->pull_item( THISO );

  return 1;
}

// Some scaffolding
int
try_quaff( object who )
{
  return 0;
}

int
try_throw( object thrower, object target )
{
  return 0;
}

void
quaff_signal( object who )
{
  return;
}

void
throw_signal( object thrower, object target )
{
  return;
}

varargs void
quaff( object who, int thrown )
{
  tell_object( who, "Nothing happens.\n" );
  return;
}

varargs void
cursed_quaff( object who, int thrown )
{
  return quaff( who, thrown );
}

varargs void
blessed_quaff( object who, int thrown )
{
  return quaff( who, thrown );
}
