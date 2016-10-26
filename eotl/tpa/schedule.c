#include "tpa.h"

inherit ObjectCode;

private string build_grid(); 
static string format_time(int time);
static void fix_prices( string arrival, int price, string departure );
int elemsort( mixed elem1, mixed elem2, mapping map );

void
extra_create()
{
  set( "gettable", 1 );
  set( "droppable", 1 );
  set( "id", ({ "schedule", "EotL TPA schedule", "TPA schedule" }) );
  set( "weight", 20 );
  set( "value", 10 );
  set( "short", "an EotL TPA schedule" );
  set( "long", "This small pamphlet lists all the stations on the EotL "
               "Teleport Authority network.  Printed on the front of "
               "the pamphlet is a large grid giving an overview of all "
               "the different connections available, and the prices of "
               "each.  The pages inside give a more detailed schedule "
               "for each station, noting departure times when applicable.\n\n"
               "     schedule :           view overview\n"
               "     schedule <station> : view station details\n" );
               
  return;
}

void
extra_init()
{
  add_action( "do_schedule", "schedule" );
  return;
}

int
do_schedule( string arg )
{
  object departure, room, *akeys;
  mapping arrivals, prices, reversemap;
  string out;
  int width;
  mixed tmp, *timetable;
  
  if( !stringp(arg) || !strlen(arg) )
  {
    write( build_grid() );
    return 1;
  }
  
  string larg = lower_case(arg);
  foreach( tmp, departure : TPAServer->query_departures() )
  {
    if( lower_case(tmp) == larg )
      break;
  }
  if( lower_case(tmp) != larg )
  {
    notify_fail(capitalize(arg) + " Station not found.\n");
    return 0;
  }
  
  room = ENV(departure);
  prices = departure->query_prices();
  timetable = departure->query_timetable();
  reversemap = mkmapping( m_indices(prices), map( m_indices(prices), 
               (: TPAServer->query_arrival($1) :) ) );
  akeys = m_values(reversemap);
  akeys -= ({ 0 });
  arrivals = mkmapping( akeys, 
                        akeys->query_schedule(), 
                        akeys->query_arrival() );
  akeys = filter_array( akeys, (: stringp($2[$1,0]) :), arrivals );
  akeys = sort_array(akeys, #'elemsort, arrivals ); 
  
  width = THISP->query_page_width();
  prices = copy(prices);
  walk_mapping( prices, #'fix_prices, departure->query_departure() );
  	                        
  out = sprintf("The %s Station is located at:\n\n%s\n%s\n"
                "This station departs to the following locations:\n"
                "  %-*#s\n\n",
                departure->query_departure(),
                room->query("short"), 
                ROOMDAEMON->format_long(room->query("day_long")), width-2,
                implode( map( akeys, (: sprintf("(%2s) %s: %s%s", 
                  $2[$1,0], $2[$1,1], 
                  ( $3[$2[$1,1]]>0 ? $3[$2[$1,1]] + " credit" : " Free" ),
                  ( $3[$2[$1,1]]>1 ? "s" : "" ) 
                ) :), arrivals, prices ), "\n" ) );
                

  if( timetable )
  {
    timetable = copy(timetable);
    if(timetable[<1][0] == 2400)
      timetable = ({ ({ 0, timetable[<1][1] }) }) + timetable[0..<2];
    
    if( timetable[0][0] != 0 )
      timetable = ({ ({ 0, timetable[<1][1]})  }) + timetable;
      
    out += format("The station console changes the destination automatically "
                  "according to the following timetable:\n");
    out += sprintf("  %-*#s\n", width-2, implode( map( timetable, 
                   (: sprintf("%6s| %2s", format_time($1[0]),
                      ( ( !member($2, $1[1]) || !objectp($2[$1[1]]) ) ? 
                        "--" :
                        $3[$2[$1[1]],0] ) ) :),
                      reversemap, arrivals ), "\n" ) );
  }
  else if( sizeof(departure->query_toggle()) > 1 )
    out += format("The destination can be switched manually on the station "
                  "console.\n");
  
  write(out);
  return 1;
}

private string
build_grid()
{
  mapping arrivals, departures;
  object *akeys, *dkeys;
  int dwidth, width, perpage;
  mixed tmp;
  string out;
  
  akeys = m_values( TPAServer->query_arrivals() );
  dkeys = m_values( TPAServer->query_departures() );
  akeys -= ({ 0 });
  dkeys -= ({ 0 });
  arrivals = mkmapping( akeys, 
                        akeys->query_schedule(),
                        akeys->query_arrival() );
  departures = mkmapping( dkeys,
                          dkeys->query_departure(),
                          dkeys->query_prices() );
  walk_mapping( departures, 
    (: $3 = copy($3); walk_mapping($3, #'fix_prices, $2); :) );
  
  akeys = filter_array( akeys, (: stringp($2[$1,0]) :), arrivals );
  akeys = sort_array( akeys, #'elemsort, arrivals );
  dkeys = sort_array( dkeys, #'elemsort, departures );
  
  dwidth = 0;
  width = THISP->query_page_width();
  foreach( tmp : dkeys )
  {
    int x;
    if( (x=strlen( departures[tmp,0] )) > dwidth )
      dwidth = x;
  }
  dwidth++;
  perpage = ( width - dwidth - 2 ) / 3;
  out = "";  

  // build legend
  out += sprintf("%|:*s\n\n  %-*#s\n\n%|:*s\n\n", 
           width, "Destination Station Legend", 
           width-2, implode( map( akeys, 
             (: sprintf("%2s: %s", $2[$1,0], $2[$1,1]) :), arrivals ), "\n" ),
           width, "Station Departure Prices" );


  // build grid
  tmp = ({ });  
  while( sizeof(akeys) )
  {
    tmp += ({ akeys[0..(perpage-1)] });
    akeys = akeys[perpage..];
  }
  akeys = tmp;
  
  foreach( int *page : akeys )
  {
    
    out += sprintf( "%*s%s\n", dwidth+2, "",
           implode( map_array( page, (: sprintf( "%:2s", $2[$1,0] ) :), 
                               arrivals ), " " ) );
    
    foreach( object row : dkeys )
    {
      out += sprintf( "%-*s|%s\n", dwidth, departures[row,0],
                      implode( map_array( 
                        page, 
                        (: member($3, $2[$1,1]) ? 
                           sprintf("%3d", $3[$2[$1,1]]) :
                           " --" :), 
                        arrivals, 
                        departures[row,1] ), "" ) );
    }
    out += "\n";
  }
  return out;
}

int
elemsort( mixed elem1, mixed elem2, mapping map )
{
  return map[elem1,0] > map[elem2,0];
}


int 
drop()
{
  call_out( "gobye", 0, THISP );
  return ::drop();
}
 
void 
gobye( object sploder )
{
  tell_room( ENV(THISO), "A gust of wind picks up the schedule and "
    "blows it away.\n");
  destruct(THISO);
}
 
static string
format_time(int time)
{
	string out;
	if( time == 0 )
	  return "12:00a";
	if( time == 1200 )
	  return "12:00p";
	  
	out = sprintf( "%d:%02d%s", ((time/100)%12)||12, (time%100), 
	      (time >= 12) ? "p" : "a" );
	
	return out;
}


static void
fix_prices( string arrival, int price, string departure )
{  
	int i = TPAServer->get_real_price( departure, arrival );
	if( i >= 0 )
	  price = i;
	return;
}
