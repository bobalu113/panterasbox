#include "tpa.h"

inherit ObjectCode;

void multi_check();

int credits;
string *stations;

void
extra_create()
{
  credits = 0;
  stations = ({ });

  set( "gettable", 1 );
  set( "droppable", 1 );
  set( "id", ({ "card", "EotL TPA card", "TPA card" }) );
  set( "weight", 20 );
  set( "value", 1000 );
  set( "short", "an EotL TPA card" );
    
  return;
}

string
long( string id )
{
  object *online;
  
  string out = "";
  out += "This card enables you travel with one of the many Teleport "
         "Authority consoles around End of the Line.  Before you can "
         "travel to a destination on the TPA network, that destination "
         "must be added to your card by using the scanner on the console.  "
         "Most TPA travel isn't free, and you must purchase credits for "
         "your card at a TPA office for consoles that require payment.\n\n";
  out = format( out, THISP->query_page_width() );
  
  if( credits )
    out += "Available credits: " + credits + "\n";
  else
    out += "Available credits: None.\n";
  
  
  online = map( stations, (: TPAServer->query_arrival($1) :) );
  online -= ({ 0 });
  
  if( sizeof( online ) )
  {
    out += "Available stations:\n";
    out += sprintf( "  %-*#s\n\n", THISP->query_page_width()-2,
                    implode(map_objects(online, "query_arrival"), "\n") );
  }
  else
  {
    out += "No destination stations have been scanned onto the card.\n\n";
  }
  
  return out;  
}


void 
set_credits( int c )
{
  credits = 0;
  return;
}

void 
add_credits( int c )
{
  credits += c;
  return;
}

int 
query_credits()
{
  return credits;
}

void 
add_station( string s )
{
  if( member( stations, s ) == -1 )
    stations += ({ s });
  return;
}

void 
set_stations( string *s )
{
  stations = s;
  return;
}

void 
remove_station( string s )
{
  stations -= ({ s });
  return;
}

int 
query_station( string s )
{
  return ( member( stations, s ) != -1 );
}

string *
query_stations()
{
  return stations;
}

void
move_signal( object oldenv, object mover )
{
  multi_check();
  return;
}

void
multi_check()
{
  object *cards;
  cards = filter_array( all_inventory(ENV(THISO)), 
            (: program_name($1) == $2 :), program_name(THISO) );
  cards -= ({ THISO });
  if( !sizeof(cards) )
    return;
    
  foreach( object card : cards )
  {
    string *s = card->query_stations();
    s = s - stations;
    stations += s;
    card->set_stations(({}));
    int c = card->query_credits();
    credits = MAX(c, credits);
    card->set_credits(0);
  }
  if( living(ENV(THISO)) )
  {
    if( sizeof(cards) > 1 ) 
      tell_player(ENV(THISO), "A spark of static electricity arcs between "
      "this card and the other cards you are carrying.  The credits and "
      "stations on your other cards completely blank out.");
    else 
      tell_player(ENV(THISO), "A spark of static electricity arcs between "
      "this card and the other cards you are carrying.  The credits and "
      "stations on your other cards completely blank out.");
  }
  else
  {
    if( sizeof(cards) > 1 ) 
      tell_room(ENV(THISO), format("A spark of static electricity arcs "
      "between all the cards in the room.  The credits and stations on the "
      "other cards completely blank out.") );
    else 
      tell_room(ENV(THISO), format("A spark of static electricity arcs "
      "between both oft he cards in the room.  The credits and stations on "
      "the other card completely blank out.") );
  }
}
    
mixed
query( string var )
{
  if( var != "value" )
    return ::query(var);
  
  return CARD_COST + (credits*CREDIT_COST) + (sizeof(stations)*100);
}
