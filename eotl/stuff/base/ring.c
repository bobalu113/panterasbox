
#include <acme.h>

inherit StuffCode;

inherit AcmeFile;
#include AcmeFileInc

int wear_delay = 1;
int remove_delay = 1;
string shadow_file = RingStuffShadow;
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
object ring_stuff_shadow();
void shadow_removed();


void
create()
{
  ::create();
  if( !query("value") )
    set( "value", 150 );
  if( !query("short") )
  {
    set( "short", "a ring of adornment" );
    set( "ids", ({ "a ring of adornment",
                   "ring of adornment",
                   "adornment ring",
                   "ring" }) );
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

mixed
query( string var )
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
    case RingP:
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
    out = sprintf( "%s %s ring", 
                   ( IsVowel(id_label[0]) ? "an" : "a" ),
                   id_label );
  else
    out = "a ring";

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
  out = format( sprintf( "The single piece of %s set in the middle of this "
                         "gold band has an unusual sparkle about it.  "
                         "Wearing the ring might have unpredictable effects.", 
                         id_label ) );
  return out + THISO->post_long();
}

string
post_long()
{
  return "\nEngraved on the inside of the ring are the words, "
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
  if( lstr == "stuff" || lstr == "ring" || lstr == "a ring" )
    return 1;
  if( ( THISO->query_identified(THISP) & ID_ITEM ) && ::id(str) )
    return 1;
  if( ( lstr == sprintf( "%s %s ring",
                      ( IsVowel(id_label[0]) ? "an" : "a" ),
                      id_label ) ) ||
      ( lstr == sprintf( "%s ring", id_label ) ) )
    return 1;
  return 0;
}

// Coding my on wear and remove commands because the ones in the
// player body were pissing me off.
void
init()
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

// This is the action function for wearing the ring.
int
do_wear( string str )
{
  object *wearing, oldp, *inv;
  string finger;
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
    write( "That ring's already being worn!\n" );
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
  wearing = THISP->all_ring_stuff() || ({ });
  wearing = filter_array( wearing, lambda( ({ 'x }),
    ({ #'?, 
       ({ #'call_other, 'x, "query_worn" }),
       1,
       ({ #',,
          ({ #'=, 'tmp, ({ #'call_other, 'x, "ring_stuff_shadow" }) }),
          ({ #'destruct, 'x }),
          ({ #'?, 'tmp, ({ #'call_other, 'tmp, "ring_dest" }) })
       })
    }) ) );
  if( sizeof( wearing ) >= THISP->total_hands() )
  {
    write( "You can't wear any more rings!\n" );
    return 1;
  }

    if(THISP->query_guild())
    {
        if(THISP->query_guild_ob()->wear_signal(THISO, THISP)) 
        { 
            return 1; 
        }
    }
	
	if( objectp( shadow ) )
    destruct(shadow);

  if( THISO->try_wear(THISP) )
    return 1;

  THISP->add_combat_delay(wear_delay);

  finger = THISP->query_hand_name();
  if( finger == "hand" )
    finger = "finger";

  if( !stringp(finger) || !strlen(finger) )
  {
    write( "You ain't got no hands!\n" );
    return 1;
  }

  if( shadow_file[0] != '/' )
    shadow_file = dirname( load_name(THISO) ) + shadow_file;
  if( !objectp( shadow = clone_object( expand_path(shadow_file) ) ) )
  {
    tell_player( THISP, "The ring is too big to fit snugly on your " +
                        finger + ", and slides right off." );
    return 1;
  }

  if( !funcall( symbol_function( "ring_init", shadow ), THISP, THISO ) )
  {
    tell_player( THISP, "The ring is too big to fit snugly on your " +
                        finger + ", and slides right off." );
    return 1;
  }

  tell_player( THISP, "You slide the ring snugly onto your " +finger+ "." );
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

// This is the action function for removing the ring.
int
do_remove( string str )
{  
  object wearing, oldp, *inv;
  string finger;
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

  if( !funcall( symbol_function( "ring_dest", shadow ) ) &&
      shadow && 
      funcall( symbol_function( "ring_stuff", shadow ) ) == THISO )
  {
    tell_player( THISP, "The ring's stuck and doesn't seem to want to "
                        "come off." );
    return 1;  
  }
  
  shadow = 0;

  finger = THISP->query_hand_name();
  if( finger == "hand" )
    finger = "finger";

  tell_player( THISP, "You slide the ring off your " +finger+ " and put it "
                      "back in your pocket." );
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
                 "The ring is cursed!  But, since you are a wizard the ring "
                 "slides off.\n");
        }
        else
        {
            tell_player( who, 
                "It won't come off.  The ring seems to be cursed!" );
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
  if( objectp(shadow) && 
      (member(ENV(THISO)->all_ring_stuff()||({}), THISO)!=-1) )
    return 1;
}

object
ring_stuff_shadow()
{
  return shadow;
}

void
shadow_removed()
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
    funcall( symbol_function( "ring_dest", shadow ) );
  if( wearer )
    THISO->remove_signal( wearer );
  return ::destruct_signal( what );
}

void
move_signal( object from, object to )
{
  if( objectp( shadow ) )
    funcall( symbol_function( "ring_dest", shadow ) );
  return;
}
int
drop() { return( THISO->query_worn() ); }



