
#include <acme.h>

inherit StuffCode;

inherit AcmeFile;
#include AcmeFileInc

int wear_delay = 1;
int remove_delay = 1;
string shadow_file = AmuletStuffShadow;
object shadow;

void create();
set( string var, mixed val );
mixed query( string var );
string short();
string long( string id );
string post_long();
int id( string str );
void init();
int do_wear( string str );
int do_remove( string str );
int query_worn();
int destruct_signal( object what );
void move_signal( object from, object to );
int drop();
object amulet_stuff_shadow();
void shadow_removed();


void create()
{
  ::create();
  if( !query("value") )
    set( "value", 100 );
  if( !query("short") )
  {
    set( "short", "an amulet of adornment" );
    set( "ids", ({ "an amulet of adornment",
                   "amulet of adornment",
                   "adornment amulet",
                   "adornment" }) );
  }
  return;
}
set( string var, mixed val )
{
  switch( var )
  {
    case "shadow_file":
      shadow_file = val;
      return 1;
    case "wear_delay":
      wear_delay = val;
      return 1;
    case "remove_delay":
      remove_delay = val;
      return 1;
  }
  return ::set( var, val );
}

mixed query( string var )
{
  switch( var )
  {
    case "shadow_file":
      return shadow_file;
    case "shadow":
      return shadow;
    case "wear_delay":
      return wear_delay;
    case "remove_delay":
      return remove_delay;
    case AmuletP:
      return 1;
    case WearableP:
      return 1;
    case "worn":
      return THISO->query_worn();
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
    out = sprintf( "%s %s amulet", 
                   ( IsVowel(id_label[0]) ? "an" : "a" ),
                   id_label );
  else
    out = "an amulet";

  return out + post_short();
}

string
post_short()
{
  return ( THISO->query_worn() ? ::post_short() + " (worn)" : ::post_short() );
}

string
long( string id )
{
  string out;
  out = format( sprintf( "A small hole has been hand-molded into the top of "
                         "this charm, and a modest-looking chain has been "
                         "laced through it.  Wearing the amulet might have "
                         "unpredictable effects.", id_label ) );
  return out + THISO->post_long();
}

string post_long()
{
  return "\nEtched into the back of the amulet are the words, \"Stuff, Inc.\"";
}

// Special identify majicks.
int
id( string str )
{
  string lstr;
  if( str == THISO->short() )
    return 1;
  lstr = lower_case( str );
  if( lstr == "stuff" || lstr == "amulet" || lstr == "an amulet" )
    return 1;
  if( ( THISO->query_identified(THISP) & ID_ITEM ) && ::id(str) )
    return 1;
  if( ( lstr == sprintf( "%s %s amulet", 
                      ( IsVowel(id_label[0]) ? "an" : "a" ),
                      id_label ) ) ||
      ( lstr == sprintf( "%s amulet", id_label ) ) )
    return 1;
  return 0;
}

// Coding my on wear and remove commands because the ones in the
// player body were pissing me off.
void init()
{
  ::init();
  //  stupid combat causing players to flee before init() 
  if( !THISP || 
      ( !present( THISO, ENV(THISP) && !present( THISO, THISP ) ) ) )
    return;
  add_action( "do_wear", "wear" );
  add_action( "do_remove", "remove" );
  return;
}

// This is the action function for wearing the amulet.
int
do_wear( string str )
{
  object wearing, oldp, *inv;
  int i, j;

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

  if( THISO->query_worn() )
  {
    write( "That amulet's already being worn!\n" );
    return 1;
  }
  if( THISP->query( NoWearArmorP ) )
  {
    write( "You cannot wear any armor!\n" );
    return 1;
  }
  if( THISP->query_race_ob()->query_no_wear( THISO, THISP ) )
    return 1;
  if( THISP->query_no_wear( THISO ) )
    return 1;

   if(THISP->query_guild())
    {
        if(THISP->query_guild_ob()->wear_signal(THISO, THISP)) 
        { 
            return 1; 
        }
    }
  if( objectp( wearing = THISP->amulet_stuff() ) )
  {
    if( wearing->query_worn() )
    {
      write( "You're already wearing an amulet!\n" );
      return 1;
    }
    else
    {
      object tmp;
      destruct(wearing);
      if( tmp = THISP->amulet_stuff_shadow() )
        tmp->amulet_dest();
    }
  }
  if( objectp( shadow ) )
    destruct(shadow);

  if( THISO->try_wear(THISP) )
    return 1;

  THISP->add_combat_delay(wear_delay);

  if( shadow_file[0] != '/' )
    shadow_file = dirname( load_name(THISO) ) + shadow_file;
  if( !objectp( shadow = clone_object( expand_path(shadow_file) ) ) )
  {
    tell_player( THISP, "You attempt to tie the amulet around your neck, "
                         "but the clasp seems to be broken." );
    return 1;
  }

  if( !shadow->amulet_init( THISP, THISO ) )
  {
    tell_player( THISP, "You attempt to tie the amulet around your neck, "
                        "but the clasp seems to be broken." );
    return 1;
  }

  tell_player( THISP, "You reach around your the back of your head and tie "
                      "the amulet loosely around your neck." );
  inv = all_inventory( ENV(THISP) );
  oldp = THISP;
  for( i=0, j=sizeof(inv) ; i<j ; i++ )
  {
    if( !living(inv[i]) || inv[i]==oldp )
      continue;
    set_this_player(inv[i]);
    printf( "%s puts on %s.\n", oldp->query_name(), short() );
    set_this_player(oldp);
  }

  THISO->wear_signal( THISP );

  return 1;
}

// This is the action function for removing the amulet.
int
do_remove( string str )
{  
  object wearing, oldp, *inv;
  int i, j;

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

  if( !THISO->query_worn() )
  {
    write( "You're not even wearing that!\n" );
    return 1;
  }

  if( THISO->try_remove(THISP) )
    return 1;

  THISP->add_combat_delay(remove_delay);

  if( !shadow->amulet_dest() && shadow && (shadow->amulet_stuff()==THISO) )
  {
    tell_player( THISP, "The clasp holding the two ends of the chain "
                        "together seems to be stuck." );
    return 1;     }
  
  shadow = 0;

  tell_player( THISP, "You reach around your the back of your head and untie "
                      "the amulet." );
  inv = all_inventory( ENV(THISP) );
  oldp = THISP;
  for( i=0, j=sizeof(inv) ; i<j ; i++ )
  {
    if( !living(inv[i]) || inv[i]==oldp )
      continue;
    set_this_player(inv[i]);
    printf( "%s takes off %s.\n", oldp->query_name(), short() );
    set_this_player(oldp);
  }

  THISO->remove_signal( THISP );

  return 1;
}

// Some scaffolding
int 
try_wear( object who )
{
  return 0;
}

int 
try_remove( object who )
{
    if(THISO->query_cursed() )
    {
        if(who->query_wizard())
        {
            tell_object(who,
                 "The amulet is cursed!  But, since you are a wizard the amulet "
                 "slides off.\n");
        }
        else
        {
            tell_player( who, 
                "It won't come off.  The amulet seems to be cursed!" );
            return 1;
        }

    }
    return 0;
}

void 
wear_signal( object who )
{
  return;
}

void 
remove_signal( object who )
{
  return;
}

int
query_worn()
{
  if( objectp(shadow) && (ENV(THISO)->amulet_stuff()==THISO) )
    return 1;
}

object
amulet_stuff_shadow()
{
  return shadow;
}

void shadow_removed()
{
  shadow = 0;
  return;
}

int
destruct_signal( object what )
{
  object wearer;
  if( THISO->query_worn() )
    wearer = ENV(THISO);
  if( objectp( shadow ) )
    shadow->amulet_dest();
  if( wearer )
    THISO->remove_signal( wearer );
  return ::destruct_signal( what );
}

void
move_signal( object from, object to )
{
  if( objectp( shadow ) )
    shadow->amulet_dest();
  return;
}

int
drop() { return( THISO->query_worn() ); }


