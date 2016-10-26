// Stratego by Devo.  Look/feel/some code/etc... ripped off from Bughouse.
#include <acme.h>
 
static inherit AcmeAlloc;
static inherit AcmeArray;
static inherit AcmeBooleanPrompt;
 
#include AcmeAllocInc
#include AcmeArrayInc
#include AcmeBooleanPromptInc
#include <ansi.h>
#include <stratego.h>
 
#define Rules "/zone/null/games/games/stratego/strules"
 
mixed *board;
mapping captured;
string *move_hist;
object server, client;
object command_giver;
mixed game_status;
 
void
create()
{
  seteuid( getuid() );
  board = allocate( 10 );
  board[0] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[1] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[2] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[3] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[4] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[5] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[6] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[7] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[8] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[9] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[2][4] = ({ LAKE, NULL_OWNER });
  board[2][5] = ({ LAKE, NULL_OWNER });
  board[3][4] = ({ LAKE, NULL_OWNER });
  board[3][5] = ({ LAKE, NULL_OWNER });
  board[6][4] = ({ LAKE, NULL_OWNER });
  board[6][5] = ({ LAKE, NULL_OWNER });
  board[7][4] = ({ LAKE, NULL_OWNER });
  board[7][5] = ({ LAKE, NULL_OWNER });
  command_giver = 0;
  server = client = 0;
  captured = ([ server : ({ }) ]);
  move_hist = ({ });
  game_status = NOGAME;
  return;
}
 
status
drop()
{
  return 0;
}
 
status
get()
{
  return 1;
}
 
void
init()
{
  if( THISP != ENV(THISO) )
    return;
  add_action( "parse_cmd", "st" );
  add_action( "parse_cmd", "stratego" );
  add_action( "do_show", "show" );
  return;
}
 
status
id( string str )
{
  return ( str == "board" || str == "stratego board" || 
           str == "stratego" || str == "st" || str == "game" );
}
 
string
short()
{
  return "a stratego board";
}
 
string
long()
{
  return "This is a stratego board.  It is green.\n"
         "Type <st help> for more info.\n";
}
 
status
parse_cmd( string arg )
{
  string cmd;
  mixed tmp;
  status ret;
 
  if( !server )
    server = THISO;
 
  if( client && server && ( client == THISO ) )
    return server->parse_cmd( arg );
 
  if( !stringp(arg) || !sizeof( tmp = explode( arg, " " ) ) )
  {
    cmd = "view";
    arg = "";
  }
  else
  {
    cmd = tmp[0];
    arg = sizeof(tmp) > 1 ? implode( tmp[1..], " " ) : "";
  }
 
  if( previous_object() == client )
    command_giver = client;
  else if( previous_object() == server )
    command_giver = server;
  else
    command_giver = 0;
 
  switch( cmd )
  {
    case "rules"     : ret = do_rules(arg);     break;
    case "resign"    : ret = do_resign(arg);    break;
    case "mode"      : ret = do_mode(arg);      break;
    case "drop"      : ret = do_drop(arg);      break;
    case "swap"      : ret = do_swap(arg);      break;
    case "challenge" : ret = do_challenge(arg); break;
    case "start"     : ret = do_start(arg);     break;
    case "view"      : ret = do_view(arg);      break;
    case "help"      : ret = do_help(arg);      break;
    case "move"      : ret = do_move(arg);      break;
    case "strike"    : ret = do_strike(arg);    break;
    default          : ret = ( PreGame() ? 
                               do_swap(cmd+arg) : 
                               do_move(cmd+arg) );  
                       break;
  }
 
  command_giver = 0;
  return ret;
}
 
status
do_rules( string arg )
{
  if( !command_giver )
    return 0;
  THISP->more_string( read_file( Rules ) );
  return 1;
}
 
status
do_resign( string arg )
{
  if( !command_giver )
    return 0;
  if( !InGame() )
  {
    notify_fail( "The game hasn't even started yet!\n" );
    return 0;
  }
  boolean_prompt( "Are you sure you want to resign?", 
                  lambda( 0, 
                    ({ #',, 
                       ({ #'tell_object, command_giver, "You resign.\n" }),
                       ({ #'tell_object, 
                          OtherBoard( command_giver ),
                          ({ #'sprintf, 
                             "%s resigns!\n",
                             ({ #'capitalize, 
                                ({ #'call_other, 
                                   command_giver, 
                                   "query_real_name"
                                })
                             })
                          })
                       }),
                       ({ #'end_game, OtherBoard( command_giver ) }), 
                       1
                    }) ),
                  lambda( 0, 
                    ({ #',,
                       ({ #'tell_object, command_giver, "Good!\n" }),
                       1
                    }) ),
                  0,
                  DEFAULT_NO
                );
  return 1;
}
 
status
do_mode( string arg )
{
}
 
status
do_drop( string arg )
{
  string piece;
  int row;
 
  if( !command_giver )
    return 0;
  if( !PreGame() )
  {
    notify_fail( "You can't drop pieces now!\n" );
    return 0;
  }
  if( arg == "random" )
    return drop_random();
  if( sscanf( arg, "%s %s", piece, arg ) < 2 )
  {
    notify_fail( "st drop <piece> <square>\n" );
    return 0;
  }
  piece = CAP( piece );
  if( searcha( captured[ command_giver ], piece ) == -1 )
  {
    write( "You don't have one of those to drop.\n" );
    return 1;
  }
 
  arg = munge_square( arg );
  switch( query_square( arg )[CONTENTS] )
  {
    case NULL_SQUARE :
      write( "That isn't a valid square.\n" );
      return 1;
      break;
 
    case LAKE :
      write( "That square is in the middle of a lake!\n" );
      return 1;
      break;
 
    case EMPTY :
      break;
 
    default :
      write( "That square is already occupied.\n" );
      return 1;
      break;
  }
 
  sscanf( arg, "%~s%D", row );
  if( ( ( command_giver == server ) && ( row < 4 ) ) ||
      ( ( command_giver == client ) && ( row > 6 ) ) )
    write( "That square is in enemy territory.\n" );
  else if( ( row == 4 ) || ( row == 5 ) )
    write( "That square is in the neutral zone.\n" );
  else
  {
    set_square( arg, piece, command_giver );
    captured[ command_giver ][ searcha( captured[ command_giver ], piece ) ] =
      NULL_PIECE;
    captured[ command_giver] -= ({ NULL_PIECE });
    write( "You place a piece on the board.\n" );
  }
  return 1;
}
 
status
drop_random()
{
  string *pieces, tmp;
  int i, j, end;
  pieces = captured[command_giver];
  if( !pointerp( pieces ) || !sizeof( pieces ) )
  {
    write( "You have no pieces to drop!\n" );
    return 1;
  }
  end = sizeof( pieces );
  for( i=end-1; i > 0; i-- )
  {
    j = random(i+1);
    tmp = pieces[i];
    pieces[i] = pieces[j];
    pieces[j] = tmp;
  }
  if( command_giver == server )
    end = 6;
  else
    end = 0;
  for( i = 9 ; i >= 0 ; i-- )
    for( j = end+3 ; j >= end ; j-- )
      if( board[i][j][CONTENTS] == EMPTY )
      {
        do_drop( sprintf( "%s %c%d", pieces[0], i+'a', j ) );
        pieces = deletea( pieces, 0 );
      }
  if( !sizeof( pieces ) )
    write( "All pieces dropped.\n" );
  else
    write( "Error!  Not all pieces dropped!\n" );
  return 1;
}

status
do_swap( string arg )
{
  int srow, erow;
  mixed *s, *e;
  string start, end;
  mixed tmp;
 
  if( !command_giver )
    return 0;
  if( !PreGame() )
  {
    notify_fail( "You can't swap pieces now!\n" );
    return 0;
  }
  arg = munge_square( arg );
  tmp = explode( arg, " " );
  if( sizeof( tmp ) != 2 )
  {
    notify_fail( "Usage: st strike <start><end>\n" );
    return 0;
  }
  start = tmp[0];
  end = tmp[1];
  s = query_square( start );
  e = query_square( end );
 
  if( s[CONTENTS] == NULL_SQUARE ||
      e[CONTENTS] == NULL_SQUARE )
  {
    write( "That isn't a valid square.\n" );
    return 1;
  }
  else if( s[CONTENTS] == LAKE || 
           e[CONTENTS] == LAKE )
  {
    write( "You can't swap squares with a lake!\n" );
    return 1;
  }
  if( s[OWNER] == OtherBoard( command_giver ) ||
      e[OWNER] == OtherBoard( command_giver ) )
  {
    write( "You can't swap with an opponents piece.\n" );
    return 1;
  }
  if( s[CONTENTS] == e[CONTENTS] )
  {
    write( "Those two squares have the same contents.\n" );
    return 1;
  }
 
  sscanf( start, "%~s%D", srow );
  sscanf( end, "%~s%D", erow );
  if( ( ( command_giver == server ) && ( srow < 4 ) ) ||
      ( ( command_giver == client ) && ( srow > 6 ) ) ||
      ( ( command_giver == server ) && ( erow < 4 ) ) ||
      ( ( command_giver == client ) && ( erow > 6 ) ) )
    write( "That square is in enemy territory.\n" );
  else if( ( srow == 4 ) || ( srow == 5 ) || ( erow == 4 ) || ( erow == 5 ) )
    write( "That square is in the neutral zone.\n" );
  else
  {
    set_square( start, e[CONTENTS], e[OWNER] );
    set_square( end, s[CONTENTS], s[OWNER] );
    if( s[CONTENTS] == EMPTY )
      printf( "You move the %s in square %s into empty square %s.\n",
        RankName( e[CONTENTS] ),
        end,
        start
      );
    else if( e[CONTENTS] == EMPTY )
      printf( "You move the %s in square %s into empty square %s.\n",
        RankName( s[CONTENTS] ),
        start,
        end
      );
    else
      printf( "You swap the %s in square %s with the %s in square %s.\n",
        RankName( s[CONTENTS] ), 
        start,
        RankName( e[CONTENTS] ),
        end
      );
  }
  return 1;
}
 
status
do_challenge( string arg )
{
  object who, lboard;
  if( !command_giver )
    return 0;
  if( !stringp(arg) || !( who = FINDP( arg ) ) )
  {
    notify_fail( "There's no person on mud with that name to challenge.\n" );
    return 0;
  }
  if( who == BoardOwner( command_giver ) )
  {
    write( "You can't challenge yourself, dumbass.\n" );
    return 1;
  }
  if( !( lboard = present_path( load_name(THISO), who ) ) )
  {
    printf( "%s doesn't have a board.\n", CAP( who->query_real_name() ) );
    return 1;
  }
 
  if( PreGame() || InGame() )
  {
    write( "You are already playing a game.\n" );
    return 1;
  }
  if( lboard->query_status() == PREGAME ||
      lboard->query_status() == INGAME )
  {
    printf( "%s is already playing a game.\n", 
      CAP( who->query_real_name() ) );
    return 1;
  }
 
  if( command_giver == client )
  {
    client && client->break_connection();
    server && server->break_connection();
    return command_giver->relay_challege( arg );
  }
  else
  {
    client && client->break_connection();
    server && server->break_connection();
  }
 
  if( lboard->query_client() == THISO )
  {
    tell_player( THISP, "You accept the challenge.\n" );
    tell_player( who, sprintf( 
      "%s accepts your challenge.\n",
      CPRNAME ) );
    client = THISO;
    server = lboard;
    server->new_game();
  }
  else
  {
    tell_player( THISP, sprintf( 
      "You challenge %s to a game of Stratego.\n", 
      CAP( who->query_real_name() ) ) );
    tell_player( who, sprintf( 
      "%s challenges you to a game of Stratego.\n",
      CPRNAME ) );
    server = THISO;
    client = lboard;
  }
  return 1;
}
 
status
relay_challenge( string arg )
{
  if( load_name( previous_object() ) != program_name( THISO ) )
    return 0;
  server = THISO;
  client = 0;
  command_giver = server;
  return do_challenge( arg );
}
 
status
do_start( string arg )
{
  if( !PreGame() )
  {
    write( "You can't start the game now.\n" );
    return 1;
  }
  if( sizeof( captured[command_giver] ) )
  {
    write( "You still have pieces to drop.\n" );
    return 1;
  }
  if( ( ( game_status == CLIENT_READY ) &&
        ( command_giver == client ) ) ||
      ( ( game_status == SERVER_READY ) &&
        ( command_giver == server ) ) )
  {
    write( "You've already signaled that you are ready to start.\n" );
    return 1;
  }
  tell_object( BoardOwner( command_giver ), 
    "You signal that you are ready to begin play.\n" 
  );
  tell_object( BoardOwner( OtherBoard( command_giver ) ), sprintf( 
    "%s signals that %s is ready to begin play.\n",
    CAP( BoardOwner( command_giver )->query_real_name() ),
    subjective( BoardOwner( command_giver ) )
  ) );
  if( ( game_status == CLIENT_READY ) || ( game_status == SERVER_READY ) )
  {
    tell_object( BoardOwner( command_giver ), 
      "The game may now begin.\n" 
    );
    tell_object( BoardOwner( OtherBoard( command_giver ) ),
      "The game may now begin.\n" 
    );
    game_status = CLIENT_TURN;
  }
  else
    game_status = ( command_giver == client ? CLIENT_READY : SERVER_READY );
  return 1;
}
 
status
do_view( string arg )
{
  if( game_status == NOGAME )
  {
    notify_fail( "You haven't started a game yet!\n" );
    return 0;
  }
  if( !stringp( arg ) || !strlen(arg) || id(arg) )
  {
    write( view_screen() );
    return 1;
  }
  else
  {
    notify_fail( "Usage: st view\n" );
    return 0;
  }
}
 
status
do_show( string arg )
{
    string what, whom;
    object guy;
 
  if( !server )
    server = THISO;
 
  if( client && server && ( client == THISO ) )
    return server->do_show( arg );
 
  if( previous_object() == client )
    command_giver = client;
  else if( previous_object() == server )
    command_giver = server;
  else
    command_giver = 0;
 
  if( !stringp( arg ) ||
      ( ( sscanf( arg, "%s to %s", what, whom ) != 2 ) &&
        ( sscanf( arg, "%s %s", whom, what ) != 2 ) ) )
  {
    notify_fail("Show whom what? -or- Show what to whom?\n");
    return 0;
  }
  if( !id(what) )
    return 0;
  if( !guy = FINDP(whom) )
  {
    write( "There is no one with that name to show your board to.\n" );
    return 1;
  }
  printf( "You show %s %s.\n", guy->query_name(), THISO->short() );
  tell_object( guy, sprintf( "%s shows you %s.\n", PNAME, THISO->short() ) );
  tell_object( guy, view_screen() );
  command_giver = 0;
  return 1;
}
 
string
view_screen()
{
  string  cuser,    ouser,
          ccolor,   ocolor,
         *ccapt,   *ocapt;
  string *lboard,  *lhist,   stat,
          xaxis,    yaxis;
  string out;
  int i, moveno, size;
 
  switch( game_status )
  {
    case CLIENT_TURN :
      stat = sprintf( "It's %s's move.",
        CAP( BoardOwner( client )->query_real_name() ) );
      break;
    case SERVER_TURN :
      stat = sprintf( "It's %s's move.",
        CAP( BoardOwner( server )->query_real_name() ) );
      break;
    case CLIENT_WIN :
      stat = sprintf( "%s wins!",
        CAP( BoardOwner( client )->query_real_name() ) );
      break;
    case SERVER_WIN :
      stat = sprintf( "%s wins!",
        CAP( BoardOwner( server )->query_real_name() ) );
      break;
    case CLIENT_READY :
      stat = sprintf( "%s is ready to play.",
        CAP( BoardOwner( client )->query_real_name() ) ); 
      break;
    case SERVER_READY :
      stat = sprintf( "%s is ready to play.",
        CAP( BoardOwner( server )->query_real_name() ) ); 
      break;
    case POSTGAME :
      stat = "The game is over.";
      break;
    case PREGAME :
      stat = "The game has not yet started.";
      break;
    default :
      stat = "There is no game in progress.";
      break;
  }
 
  lhist = ialloc( 4, "" );
  size = sizeof( move_hist );
  if( size <= sizeof(lhist) * 2 )
    i = 0;
  else
  {
    i = size - ( sizeof(lhist) * 2 );
    if( size % 2 )
      i++;
  }
  for( moveno = 0 ; i < size ; i+=2, moveno++ )
  {
    lhist[moveno] = sprintf( "%-:4d%-:10s%s",
      i/2,
      checked_item( move_hist, i )   || "...",
      checked_item( move_hist, i+1 ) || "..."
    );
  }
 
  ocolor = SERVER_COLOR;
  ccolor = CLIENT_COLOR;
  ouser = CAP( BoardOwner( server )->query_real_name() );
  cuser = CAP( BoardOwner( client )->query_real_name() );
  yaxis = "9876543210";
  xaxis = "a b c d e f g h i j";
  ocapt = get_capt( captured[server] );
  ccapt = get_capt( captured[client] );
 
  if( command_giver == server )
  {
    swapem( &ocolor, &ccolor );
    swapem( &ouser, &cuser );
    swapem( &ocapt, &ccapt );
    xaxis = reverse_string( xaxis );
    yaxis = reverse_string( yaxis );
  }
 
  if( PreGame() )
    ocapt = ({ "", "" });
 
  lboard = draw_board();
 
  out = sprintf( 
 
    "\r"
    "%-:3s%s%:3s     %s\n"
    "\n"
    "%-:3c%s%:3c     %s: %s\n"
    "%-:3c%s%:3c     Captured: %s\n"
    "%-:3c%s%:3c               %s\n"
    "%-:3c%s%:3c     %s\n"
    "%-:3c%s%:3c     %s\n"
    "%-:3c%s%:3c     %s\n"
    "%-:3c%s%:3c     %s\n"
    "%-:3c%s%:3c               %s\n"
    "%-:3c%s%:3c     Captured: %s\n"
    "%-:3c%s%:3c     %s: %s\n"
    "\n"
    "%-:3s%s%:3s\n",
 
    "",       xaxis,     "",       stat,
    yaxis[0], lboard[0], yaxis[0], ocolor,   ouser,
    yaxis[1], lboard[1], yaxis[1], ocapt[0],
    yaxis[2], lboard[2], yaxis[2], ocapt[1],
    yaxis[3], lboard[3], yaxis[3], lhist[0],
    yaxis[4], lboard[4], yaxis[4], lhist[1],
    yaxis[5], lboard[5], yaxis[5], lhist[2],
    yaxis[6], lboard[6], yaxis[6], lhist[3],
    yaxis[7], lboard[7], yaxis[7], ccapt[1],
    yaxis[8], lboard[8], yaxis[8], ccapt[0], 
    yaxis[9], lboard[9], yaxis[9], ccolor,   cuser, 
    "",       xaxis,     ""
  );
 
  return out;
}
 
status
do_help( string arg )
{
  if( !stringp(arg) || !strlen(arg) )
  {
    printf( "Current available help topics are:\n  %-1#s\n",
            implode( m_indices( HELP ), "\n" ) );
    return 1;
  }
  if( !member( HELP, arg ) )
  {
    printf( "There is no help available on \"%s.\"\n", arg );
    return 1;
  }
  THISP->more_string( HELP[arg] );
  return 1;
}
 
status
do_move( string arg )
{
  string start, end;
  mixed *s, *e;
  mixed tmp;
  string sab, eab;
  int sor, eor;
 
  if( !command_giver )
    return 0;
  if( !InGame() )
  {
    notify_fail( "The game hasn't started yet!\n" );
    return 0;
  }
  if( !StatusCheck( command_giver ) )
  {
    notify_fail( "It isn't your turn.\n" );
    return 0;
  }
  arg = munge_square( arg );
  tmp = explode( arg, " " );
  if( sizeof( tmp ) != 2 )
  {
    notify_fail( "Usage: st move <start><end>\n" );
    return 0;
  }
  start = tmp[0];
  end = tmp[1];
  s = query_square( start );
  e = query_square( end );
 
  // All this shit checks to see it the move is allowed.
  // I should probably put it in a subroutine, but I suck.
  if( ( sizeof( move_hist ) >= 4 ) && 
      ( arg == munge_square( move_hist[<4] ) ) )
  {
    write( format( "Pieces may not be moved back and forth between the "
                   "same two squares in three consecutive turns." ) );
    return 1;
  }
  if( s[CONTENTS] == NULL_SQUARE )
  {
    write( "Bad start square.\n" );
    return 1;
  }
  if( e[CONTENTS] == NULL_SQUARE )
  {
    write( "Bad end square.\n" );
    return 1;
  }
  sscanf( start, "%s%D", sab, sor );
  sscanf( end, "%s%D", eab, eor );
  if( s[OWNER] == OtherBoard( command_giver ) )
  {
    write( "You can't move your opponent's piece.\n" );
    return 1;
  }
  else if( s[OWNER] != command_giver )
  {
    write( "There's no piece there to move.\n" );
    return 1;
  }
  if( !IsMoveable( s[CONTENTS] ) )
  {
    write( "You can't move that piece.\n" );
    return 1;
  }
  if( e[OWNER] == OtherBoard( command_giver ) )
  {
    return do_strike( arg );
  }
  if( e[CONTENTS] != EMPTY )
  {
    write( "You may only move into an unoccupied square.\n" );
    return 1;
  }
  if( ( sab != eab ) && ( sor != eor ) )
  {
    write( "You make only move foward, back, left, or right.\n" );
    return 1;
  }
  if( s[CONTENTS] != SCOUT )
  {
    if( ( ABS( sab[0] - eab[0] ) > 1 ) ||
        ( ABS( sor - eor )       > 1 ) )
    {
      write( "You may only move one square at a time.\n" );
      return 1;
    }
  }
  else
  {
    for( tmp = MIN( sab[0], eab[0] ) + 1 ; tmp < MAX( sab[0], eab[0] ) ; tmp++ )
 
      if( query_square( sprintf( "%c%d", tmp, sor ) )[CONTENTS] != EMPTY )
      {
        write( "Scouts may only move through empty squares.\n" );
        return 1;
      }
    for( tmp = MIN( sor, eor ) + 1 ; tmp < MAX( sor, eor ) ; tmp++ )
      if( query_square( sab + to_string(tmp) )[CONTENTS] != EMPTY )
      {
        write( "Scouts may only move through empty squares.\n" );
        return 1;
      }
  }
 
  // The move is allowed.
  tell_player( BoardOwner( command_giver ), sprintf( 
    "You move your %s from square %s to square %s.\n",
    RankName( s[CONTENTS] ), 
    start,
    end
  ) );
  tell_player( BoardOwner( OtherBoard( command_giver ) ), sprintf( 
    "%s moves the piece in square %s to square %s.\n", 
    CAP( BoardOwner( command_giver )->query_real_name() ),
    start,
    end
  ) );
  set_square( start, EMPTY, NULL_OWNER );
  set_square( end, s[CONTENTS], s[OWNER] );
  move_hist += ({ start+end });
  game_status = ( game_status == CLIENT_TURN ? 
                    SERVER_TURN :
                    CLIENT_TURN );
  return 1;
}
 
status
do_strike( string arg )
{
  string start, end;
  mixed *s, *e;
  string sab, eab;
  int sor, eor;
  mixed tmp;
  object winner;
 
  if( !command_giver )
    return 0;
  if( !InGame() )
  {
    notify_fail( "The game hasn't started yet!\n" );
    return 0;
  }
  if( !StatusCheck( command_giver ) )
  {
    notify_fail( "It isn't your turn.\n" );
    return 0;
  }
  arg = munge_square( arg );
  tmp = explode( arg, " " );
  if( sizeof( tmp ) != 2 )
  {
    notify_fail( "Usage: st strike <start><end>\n" );
    return 0;
  }
  start = tmp[0];
  end = tmp[1];
  s = query_square( start );
  e = query_square( end );
 
  // All this shit checks to see it the strike is allowed.
  // I should probably put it in a subroutine, but I suck.
  if( s[CONTENTS] == NULL_SQUARE )
  {
    write( "Bad start square.\n" );
    return 1;
  }
  if( e[CONTENTS] == NULL_SQUARE )
  {
    write( "Bad end square.\n" );
    return 1;
  }
  sscanf( start, "%s%D", sab, sor );
  sscanf( end, "%s%D", eab, eor );
  if( s[OWNER] == OtherBoard( command_giver ) )
  {
    write( "You can't strike with your opponent's piece.\n" );
    return 1;
  }
  else if( s[OWNER] != command_giver )
  {
    write( "There's no piece there to strike with.\n" );
    return 1;
  }
  if( !IsMoveable( s[CONTENTS] ) )
  {
    write( "You can't attack someone with that piece.\n" );
    return 1;
  }
  if( e[OWNER] == command_giver )
  {
    write( "You can't strike your own piece.\n" );
    return 1;
  }
  else if( e[OWNER] != OtherBoard( command_giver ) )
  {
    write( "There's nothing there to strike.\n" );
    return 1;
  }
  if( ( ( sab != eab ) && ( sor != eor ) ) ||
      ( ( ABS( sab[0] - eab[0] ) > 1 ) ||
        ( ABS( sor - eor )       > 1 ) )  )
  {
    write( "You may only strike adjacent pieces.\n" );
    return 1;
  }
 
  // The strike is allowed.
  tell_player( BoardOwner( command_giver ), sprintf( 
    "Your %s in square %s strikes %s's %s in square %s.\n",
    RankName( s[CONTENTS] ), 
    start,
    CAP( BoardOwner( OtherBoard( command_giver ) )->query_real_name() ),
    RankName( e[CONTENTS] ),
    end
  ) );
  tell_player( BoardOwner( OtherBoard( command_giver ) ), sprintf( 
    "%s's %s in square %s strikes your %s in square %s.\n",
    CAP( BoardOwner( command_giver )->query_real_name() ),
    RankName( s[CONTENTS] ),
    start,
    RankName( e[CONTENTS] ),
    end
  ) );
 
  if( e[CONTENTS] == FLAG )
  {
    winner = command_giver;
    captured[ winner ] += ({ e[CONTENTS] });
    set_square( end, s[CONTENTS], winner );
    set_square( start, EMPTY, NULL_OWNER );
    end_game( command_giver );
    return 1;
  }
  else if( e[CONTENTS] == BOMB )
  {
    if( s[CONTENTS] == MINER )
      winner = command_giver;
    else
      winner = OtherBoard( command_giver );
  }
  else if( e[CONTENTS] == MARSHAL &&
           s[CONTENTS] == SPY )
    winner = command_giver;
  else
  {
    if( rank_order( e[CONTENTS] ) > rank_order( s[CONTENTS] ) )
      winner = OtherBoard( command_giver );
    else if( rank_order( e[CONTENTS] ) < rank_order( s[CONTENTS] ) )
      winner = command_giver;
    else
      winner = 0;
  }
  
  if( winner == command_giver )
  {
    captured[ winner ] += ({ e[CONTENTS] });
    set_square( end, s[CONTENTS], winner );
    set_square( start, EMPTY, NULL_OWNER );
  }
  else if( winner == OtherBoard( command_giver ) )
  {
    captured[ winner ] += ({ s[CONTENTS] });
    if( IsMoveable( e[CONTENTS] ) )
    {
      set_square( start, e[CONTENTS], winner );
      set_square( end, EMPTY, NULL_OWNER );
    }
    else
      set_square( start, EMPTY, NULL_OWNER );
  }
  else
  {
    captured[ command_giver ] += ({ e[CONTENTS] });
    captured[ OtherBoard( command_giver ) ] += ({ s[CONTENTS] });
    set_square( start, EMPTY, NULL_OWNER );
    set_square( end, EMPTY, NULL_OWNER );
  }
  move_hist += ({ start+end });
  game_status = ( game_status == CLIENT_TURN ? 
                    SERVER_TURN :
                    CLIENT_TURN );
  return 1;
}
 
void
new_game()
{
  captured = allocate_mapping(2);
  captured[client] = ({ MARSHAL, GENERAL, COLONEL, COLONEL, MAJOR, 
                        MAJOR, MAJOR, CAPTAIN, CAPTAIN, CAPTAIN, 
                        CAPTAIN, LIEUTENANT, LIEUTENANT, LIEUTENANT,
                        LIEUTENANT, SERGEANT, SERGEANT, SERGEANT, 
                        SERGEANT, MINER, MINER, MINER, MINER, MINER,
                        SCOUT, SCOUT, SCOUT, SCOUT, SCOUT, SCOUT, 
                        SCOUT, SCOUT, SPY, BOMB, BOMB, BOMB, BOMB, 
                        BOMB, BOMB, FLAG });
  captured[server] = captured[client] + ({ });
 
  board = allocate( 10 );
  board[0] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[1] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[2] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[3] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[4] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[5] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[6] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[7] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[8] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[9] = ialloc( 10, quote( ({ EMPTY, NULL_OWNER }) ) );
  board[2][4] = ({ LAKE, NULL_OWNER });
  board[2][5] = ({ LAKE, NULL_OWNER });
  board[3][4] = ({ LAKE, NULL_OWNER });
  board[3][5] = ({ LAKE, NULL_OWNER });
  board[6][4] = ({ LAKE, NULL_OWNER });
  board[6][5] = ({ LAKE, NULL_OWNER });
  board[7][4] = ({ LAKE, NULL_OWNER });
  board[7][5] = ({ LAKE, NULL_OWNER });
 
  move_hist = ({ });
  game_status = PREGAME;
 
  return;
}
 
static void
end_game(object winner)
{
  tell_player( BoardOwner( winner ), "You win!\n" );
  tell_player( BoardOwner( OtherBoard( winner ) ), "You lose!\n" );
  game_status = ( winner == server ? SERVER_WIN : CLIENT_WIN );
  return;
}
 
int
query_status()
{
  if( ( game_status == CLIENT_TURN ) ||
      ( game_status == SERVER_TURN ) )
    return INGAME;
  else if( ( game_status == PREGAME ) || 
           ( game_status == CLIENT_READY ) ||
           ( game_status == SERVER_READY ) )
    return PREGAME;
  else if( ( game_status == POSTGAME ) ||
           ( game_status == CLIENT_WIN ) ||
           ( game_status == SERVER_WIN ) )
    return POSTGAME;
  else
    return game_status;
}
 
object
query_server()
{
  return server;
}
 
object
query_client()
{
  return client;
}
 
status
set_server( object ob )
{
  object prev;
  prev = previous_object();
  if( ( prev != THISO  ) &&
      ( prev != server ) &&
      ( prev != client ) )
    return 0;
  server = ob;
  return 1;
}
 
status
set_client( object ob )
{
  object prev;
  prev = previous_object();
  if( ( prev != THISO  ) &&
      ( prev != server ) &&
      ( prev != client ) )
    return 0;
  client = ob;
  return 1;
}
 
void
break_connection()
{
  object prev;
  prev = previous_object();
  if( ( prev != THISO  ) &&
      ( prev != server ) &&
      ( prev != client ) )
    return 0;
  if( !server || !client )
  {
    server = THISO;
    client = 0;
    return;
  }
  if( client->query_server() == server )
  {
    server->set_server( server );
    server->set_client( 0 );
    client->set_server( client );
    client->set_client( 0 );
    return;
  }
  THISO->set_server( THISO );
  THISP->set_client( 0 );
  return;
}
 
string
munge_square( string square )
{
  string *alpha;
  int *num;
  int i, j;
  square = lower_case( square );
  alpha = ({ });
  num = ({ });
  do
  {
    alpha += ({ 0 });
    num += ({ 0 });
  }
  while( sscanf( square || "", 
                 "%.0t%1s%.0t%D%.0t%s", 
                 alpha[<1], 
                 num[<1], 
                 square ) >= 2 );
  square = "";
  for( i = 0 , j = sizeof( alpha ) ; i < j ; i++ )
  {
    if( !stringp(alpha[i]) || !strlen(alpha[i]) )
      continue;
    square += sprintf( "%s%d ", alpha[i], num[i] );
  }
  return square[0..<2];
}
 
static status
set_square( string square, string contents, object owner )
{
  int ab, or;
  if( strlen( square ) < 2 )
    return 0;
  ab = square[0] - 'a';
  if( ab < 0 || ab > sizeof(board)-1 )
    return 0;
  or = to_int( square[1..] );
  if( !or && ( square[1] != '0' ) )
    return 0;
  if( or < 0 || or > sizeof(board[0])-1 )
    return 0;
  board[ab][or] = allocate(2);
  board[ab][or][CONTENTS] = contents;
  board[ab][or][OWNER] = owner;
  return 1;
}
 
static mixed
query_square( string square )
{
  int ab, or;
  if( strlen( square ) < 2 )
    return ({ NULL_SQUARE, NULL_OWNER });
  ab = square[0] - 'a';
  if( ab < 0 || ab > sizeof(board)-1 )
    return ({ NULL_SQUARE, NULL_OWNER });
  or = to_int( square[1..] );
  if( !or && ( square[1] != '0' ) )
    return ({ NULL_SQUARE, NULL_OWNER });
  if( or < 0 || or > sizeof(board[0])-1 )
    return ({ NULL_SQUARE, NULL_OWNER });
  return board[ab][or];
}
 
string
reverse_string( string str )
{
  int i, size, middle;
  mixed temp;
  size = strlen( str );
  middle = size / 2;
  for( i = 0; i < middle; i++ )
  {
    temp                = str[i];
    str[i]              = str[ size - i - 1 ];
    str[ size - i - 1 ] = temp;
  }
  return str;
}
 
int
rank_order( string piece )
{
  return searcha( RANK_ORDER, piece );
}
 
void 
swapem( mixed val1, mixed val2 )
{
  mixed tmp;
  tmp = val1;
  val1 = val2;
  val2 = tmp;
  return;
}
 
static string *
get_capt( string *pieces )
{
  mixed out;
  string last;
  int i, j;
  if( !sizeof(pieces) )
    return ({ "", "" });
  pieces = sort_array( pieces, lambda( ({ 'x, 'y }),
    ({ #'>,
       ({ #'rank_order, 'x }),
       ({ #'rank_order, 'y })
    })
  ) );
  last = pieces[0];
  out = "";
  for( i = 0 , j = sizeof( pieces ) ; i < j ; i++ )
  {
    if( pieces[i] != last )
    {
      out += " ";
      last = pieces[i];
    }
    out += pieces[i];
  }
  out = format( out, 26 );
  out = explode( out, "\n" );
  if( sizeof(out) < 1 )
    out += ({ "", "" });
  else if( sizeof(out) < 2 )
    out += ({ "" });
  else if( sizeof(out) > 2 )
    out = out[0..1];
  return out;
}
 
static string *
draw_board()
{
  string *out;
  int x,   y,   count,
      row, col;
  status ansi;
  ansi = THISP->query_ansi();
  out = ialloc( sizeof(board), "" );
  col = sizeof(board)-1;
  row = sizeof(board[0])-1;
 
  if( command_giver == client )
  {
    for( y = row, count = 0 ; y >= 0 ; y--, count++ )
    {
      for( x = 0 ; x <= col ; x++ )
      {
        if( board[x][y][OWNER] == OtherBoard( command_giver ) )
          out[count] += ( ansi ? Color( board[x][y][OWNER] ) : "" ) +
                        ( PostGame() ? board[x][y][CONTENTS] : UNKNOWN ) +
                        ( ansi ? NORM : "" );
        else 
          out[count] += ( ansi ? Color( board[x][y][OWNER] ) : "" ) +
                        board[x][y][CONTENTS] + 
                        ( ansi ? NORM : "" );
        out[count] += " ";
      }
      out[count][<1] = 0;
    }
  }
  else if( command_giver == server )
  {
    for( y = 0, count = 0 ; y <= row ; y++, count++ )
    {
      for( x = col ; x >= 0 ; x-- )
      {
        if( board[x][y][OWNER] == OtherBoard( command_giver ) )
          out[count] += ( ansi ? Color( board[x][y][OWNER] ) : "" ) +
                        ( PostGame() ? board[x][y][CONTENTS] : UNKNOWN ) +
                        ( ansi ? NORM : "" );
        else 
          out[count] += ( ansi ? Color( board[x][y][OWNER] ) : "" ) +
                        board[x][y][CONTENTS] + 
                        ( ansi ? NORM : "" );
        out[count] += " ";
      }
      out[count][<1] = 0;
    }
  }
  else
  {
    for( y = row, count = 0 ; y >= 0 ; y--, count++ )
    {
      for( x = 0 ; x <= col ; x++ )
      {
        if( board[x][y][OWNER] != NULL_OWNER )
          out[count] += ( ansi ? Color( board[x][y][OWNER] ) : "" ) +
                        ( PostGame() ? board[x][y][CONTENTS] : UNKNOWN ) +
                        ( ansi ? NORM : "" );
        else
          out[count] += ( ansi ? Color( board[x][y][OWNER] ) : "" ) +
                        board[x][y][CONTENTS] + 
                        ( ansi ? NORM : "" );
        out[count] += " ";
      }
      out[count][<1] = 0;
    }
  }
  return out;
}
 
string
query_real_name()
{
  return "";
}

query_gettable() { return 1; }
