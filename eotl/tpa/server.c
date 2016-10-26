#include "tpa.h"

static void preload_stations();

mapping arrivals, departures;

void
create()
{
  arrivals = ([ ]);
  departures = ([ ]);
  
  preload_stations();
  
  return;
}

int
set_departure( string station, object console )
{
  if( departures[station] == console )
    return 1;
    
  if( member( departures, station ) && departures[station] )
    return 0;
  
  departures[station] = console;
  return 1;
}

int
remove_departure( string station )
{
  m_delete( departures, station );
  return 1;
}

object
query_departure( string station )
{
  return departures[station];
}

object *
query_departures()
{
  return copy( departures );
}

int
set_arrival( string station, object console )
{
  if( arrivals[station] == console )
    return 1;
    
  if( member( arrivals, station ) && arrivals[station] )
    return 0;
  
  arrivals[station] = console;
  return 1;
}

int
remove_arrival( string station )
{
  m_delete( arrivals, station );
  return 1;
}

object
query_arrival( string station )
{
  return arrivals[station];
}

object *
query_arrivals()
{
  return copy( arrivals );
}

static void
preload_stations()
{
  string *stations = TPAStations;
  
  foreach( string s : stations )
  {
    if( objectp(find_object(s)) )
      destruct(find_object(s));
    catch( load_object(s) );
  }
  return;
}

int
destruct_signal()
{
  return 1;
}

int
query_move_ok()
{
  return 1;
}

int
get_real_price( string departure, string arrival)
{
  mapping p;
  int r;
  
  if( !stringp(departure) || !stringp(arrival) )
    return -1;
    
  if( !member(departures, departure) || !member(arrivals, arrival) )
    return -1;
  
  if( !objectp(departures[departure]) || !objectp(arrivals[arrival]) )
    return -1;
  
  if( !mappingp( p=(departures[departure]->query_prices()) ) )
    return -1;
    
  if( !member( p, arrival ) )
    return -1;
  
  r = arrivals[arrival]->query_reserve();
  
  if( p[arrival] < 0 )
    return -1;
    
  if( p[arrival] < r )
    return r;
    
  return p[arrival];
}
  
