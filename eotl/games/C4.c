    ///////  CONNECT 4,  Coded by Toothpick,  22nd October 1993  ///////

inherit ObjectCode;

#define BOARD_ID "S-C4_BOARD"
#define TIME 3
#define GM "C4"
#define GAME "shift connect 4"
#define CGAME "Shift Connect Four"
#define PROPERTY "S-C4SetUpP"
#define MOVEHELP "\t  C4 <num>              " \
                "Place a token in column <num>.      \n" \
                "\t  C4 <row> <left|right> " \
                "Shift row <row> left or right.      \n"
#define BDW 4
#define BDH 4

#define SMTOP   "    ________ "
#define SMBOT   "    ~~~~~~~~ "
#define SMNOS   "     1 2 3 4 "
#define MMMID   "   |---|---|---|---|"
#define MMNOS   "     1   2   3   4  "
#define BMTOP   "     ____________________________ "
#define BMMID1  "    ||_____||_____||_____||_____||"
#define BMMID2  "    ||~~~~~||~~~~~||~~~~~||~~~~~||"
#define BMBOT   "     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ "
#define BMNOS   "        1      2      3      4    "
#define SETUP1  "Shift-Connect 4 setup...\n"
#define SETUP2  "Board size? (1-3): "
#define SETUP3  "Colour or mono? (c/m): "
#define SETUP4  "Would you like a beep when " + \
                        "the other player makes a move? (y/n): "
#define SETUP5 "Do you wish your games to be viewable by others? (y/n): "
#define BLUE   "[1;44m"
#define NORM   "[22;37;2m[0;0m"
#define FRED    "[1;31m"
#define FGREEN  "[1;32m"
#define FWHITE  "[1;37m"
#define RED    "[1;41m"
#define GREEN  "[1;42m"
#define BLACK  "[1;40m"
#define FLASH  "[?5h[?5l"
#define SCBOT  "[1;44m   [1;32m1 [1;31m2 [1;32m3 [1;31m4 " + \
                        " [1;40m[0m"
#define MCBOT  "[1;44m    [1;32m1   [1;31m2   [1;32m3   " + \
               "[1;31m4   [1;40m[0m"
#define LCBOT  "[1;44m    [1;32m1     [1;31m2     [1;32m3     " + \
               "[1;31m4   [1;40m[0m"

#define FindBoard( pl ) present( BOARD_ID, pl )
#define pos( i, j ) grid[ ( j ) * BDW + ( i ) ]
#define BIT bit( pos( j, BDH - i -1 ) )
#define WLD( w, l, d ) pad( "" + w, -4 ) + " - " + l + " - " + d
#define RECORD( str ) "/zone/null/games/games/" GM "/records/" + str
#define CNAME capitalize( THISP->query_real_name() )
#define THISV this_verb()
#define CTHISV capitalize( this_verb() )
#define Assign( a, b )  for( a=({ }), i=0; i<sizeof(b); i++ ) a += ({ b[i] })

varargs string pad(mixed str, int len, int padchar);

int t, ym, *lastgrid, *grid, beep, viewable, offered,
        clock, waiting, cant_take_back, *wins, *losses, *draws;
string *chs, *ochs, *watchers, pl2, rpl1, rpl2,
        mess, lastmove, lastlastmove, *games;

int query_ym()           {    return ym;           }
int query_type()         {    return t;            }
int query_viewable()     {    return viewable;     }
int *query_grid()        {    return grid;         }
string query_playing()   {    return pl2;          }
string *query_watchers() {    return watchers;     }
int query_offered()      {    return offered;      }

void set_type( int i )             {    t = i;               }
void set_ym( int i )               {    ym = i;              }
void set_mess( string str )        {    mess = str;          }
void set_playing( string str )     {    pl2 = str;           }
void set_lastmove( string str )    {    lastmove = str;      }
void set_cant_take_back( int i )   {    cant_take_back = i;  }
int  set_rpls( string r1, string r2 )  {  rpl1 = r1;  rpl2 = r2;  }

status alpha_sort( string s1, string s2 )  {  return ( s1 > s2 ? 1 : 0 );  }
status object_sort( object o1, object o2 )
   {  return ( o1->query_real_name() > o2->query_real_name() ? 1 : 0 );  }

int destruct_signal() { if( pl2 ) catch( THISO->resign() ); return 0; }

int *query_lastgrid() { return lastgrid + ({ }); }

object get_player( string str )
{
        str = lower_case( str );
        if( find_player( str ) ) return find_player( str );
        else write( CTHISV + ": " +
                capitalize( str ) + " is not logged on.\n" );
}

object get_board( object pl )
{
        if( FindBoard( pl ) ) return FindBoard( pl );
        else write( CTHISV + ": " + pl->query_name() +
                " has no board on which to play.\n" );
}

void stop_clock()
{
      if( find_call_out( "tick_tock" ) ) remove_call_out( "tick_tock" );
}

static void new_rec( string player2 )
{
        games  += ({ player2 });
        wins   += ({ 0 });
        losses += ({ 0 });
        draws  += ({ 0 });
}

static void load_record( string name )
{
        string *lines, w, l, d;
        int i;
        games  = ({ });
        wins   = ({ });
        losses = ({ });
        draws  = ({ });
        lines  = grab_file( RECORD( name ) );
        for( i=0; i<sizeof(lines); i++, name=0, w=0, l=0, d=0 )
        {
                sscanf( lines[i], "%s %d %d %d", name, w, l, d );
                if( !name ) continue;
                games  += ({ name });
                wins   += ({ w });
                losses += ({ l });
                draws  += ({ d });
        }
}

static void save_record( string name )
{
        int i;
        string *lines;
        lines = ({ });
        rm( RECORD( name ) );
        for( i=0; i<sizeof(games); i++ )
                lines += ({ games[i] + " " + wins[i] + " " +
                        losses[i] + " " + draws[i] + "\n" });
        lines = sort_array( lines, "alpha_sort", THISO );
        for( i=0; i<sizeof(lines); i++ )
                write_file( RECORD( name ), lines[i] );
}

static void record_win( string player1, string player2 )
{
        int i;
        load_record( player1 );
        if( searcha( games, player2 ) == -1 ) new_rec( player2 );
        i = searcha( games, player2 );
        wins[i]++;
        save_record( player1 );
}

static void record_loss( string player1, string player2 )
{
        int i;
        load_record( player1 );
        if( searcha( games, player2 ) == -1 ) new_rec( player2 );
        i = searcha( games, player2 );
        losses[i]++;
        save_record( player1 );
}

static void record_draw( string player1, string player2 )
{
        int i;
        load_record( player1 );
        if( searcha( games, player2 ) == -1 ) new_rec( player2 );
        i = searcha( games, player2 );
        draws[i]++;
        save_record( player1 );
}

static void write_record( string name )
{
        int i, w, l, d;
        load_record( name );
        write( "\n" + pad( "=======[ " + capitalize( name ) +
                "'s " + CGAME + " Record (Wins-Losses-Draws) ]",
                78, 61 ) + "\n" );
        for( i=0; i<sizeof(games); i++ )
        {
                write( pad( " " + capitalize( games[i] ) + " (" + wins[i] +
                        "-" + losses[i] + "-" + draws[i] + ")", 26 ) );
                if( i%3==2 || i==sizeof(games)-1 ) write( "\n" );
                w += wins[i];
                l += losses[i];
                d += draws[i];
        }
        write( pad( "=======[ Total: " + w + "-" +
                l + "-" + d + " ]", 78, 61 ) + "\n" );
}

string wld( string name, string str )
{
        int i, w, l, d;
        load_record( name );
        if( str )
        {
                i = searcha( games, str );
                if( i == -1 ) return WLD( 0, 0, 0 );
                else return WLD( wins[i], losses[i], draws[i] );
        }
        for( i=0; i<sizeof(games); i++ )
        {
                w += wins[i];
                l += losses[i];
                d += draws[i];
        }
        return WLD( w, l, d );
}

int wholong()
{
        string out, str, whole, p2;
        object b, *all, *bds;
        int i, g;
        all = users();
        bds = ({ });
        whole = "";
        for( i=0; i<sizeof(all); i++ )
                if( FindBoard( all[i] ) ) bds += ({ all[i] });
        bds = sort_array( bds, "object_sort", THISO );
        for( i=0; i<sizeof(bds); i++ )
        {
                b = FindBoard( bds[i] );
                str = bds[i]->query_real_name();
                out = "";
                if( IsWizard( bds[i] ) )      out += "$";  else out += " ";
                if( searcha( chs, str )>=0 )  out += "<";  else out += " ";
                if( searcha( ochs, str )>=0 ) out += ">";  else out += " ";
                out += " " + pad( capitalize( str ), 14 );
                out += wld( str, 0 );
                if( p2 = b->query_playing() )
                {
                        g++;
                        out += "       " + pad( "vs " + capitalize( p2 ), 18 );
                        out += wld( str, p2 );
                }
                whole += out + "\n";
        }
        whole = "\n" + pad( "_________ " +
                CGAME + " Players: " + sizeof( bds ) +
                " total, " + g +
                " playing now ", 79, 95 ) + "\n" + pad( "", 79, 126 ) +
                "\n" + "===[ Name ]======[ Total: W-L-D ]====" +
                "[ Against ]======[ Games so far: W-L-D ]==\n" + whole +
                "===[ Notes: '<' challenged you, '>' challenged by you, " +
                "'$' wizhole. ]==========\n";
        THISP->more_string( whole );
        return 1;
}

int whoshort()
{
        string out, str, whole;
        object b, *all, *bds;
        int i, g;
        all = users();
        bds = ({ });
        whole = "";
        for( i=0; i<sizeof(all); i++ )
                if( FindBoard( all[i] ) ) bds += ({ all[i] });
        bds = sort_array( bds, "object_sort", THISO );
        for( i=0; i<sizeof(bds); i++ )
        {
                b = FindBoard( bds[i] );
                str = bds[i]->query_real_name();
                out = "";
                if( searcha( chs, str )>=0 )  out += "<";  else out += " ";
                if( searcha( ochs, str )>=0 ) out += ">";  else out += " ";
                if( b->query_playing() )   out += "*";  else out += " ";
                out += capitalize( str );
                whole = whole + pad( out, 15 );
                if( i%5==4 || i==sizeof(bds)-1 ) whole += "\n";
                else whole += " ";
                if( b->query_playing() ) g++;
        }
        whole = "\n" + pad( "=====[ " + CGAME +
                " players: " + sizeof( bds ) +
                " total, " + g + " playing now ]", 79, 61 ) + "\n" +
                whole + pad( "===[ Notes: '<' challenged you, '>' " +
                "challenged by you, '*' in game. ]", 79, 61 ) + "\n";
        THISP->more_string( whole );
        return 1;
}

int who( string str )
{
        if( str == "-s" ) return whoshort();
        return wholong();
}

int score( string str )
{
        if( !str ) str = PRNAME;
        str = lower_case( str );
        if( !exists( RECORD( str ) ) )
                if( str == PRNAME ) write( CTHISV +
                        ": You have no past games on record.\n" );
                else write( CTHISV + ": " +
                        capitalize( str ) + " has no past games " +
                        "on record.\n" );
        else write_record( str );
        return 1;
}

string *boards()
{
        string *bds, str;
        object *all, b;
        int i;
        all = users();
        bds = ({ });
        for( i=0; i<sizeof(all); i++ )
                if( b = FindBoard( all[i] ) )
                {
                        str = capitalize( all[i]->query_real_name() );
                        if( b->query_playing() ) bds += ({ "*" + str });
                        else bds += ({ str });
                }
        return sort_array( bds, "alpha_sort", THISO );
}

void save_setup()
{
        add_property( THISP, PROPERTY );
        set_property_value( THISP, PROPERTY, ({ t, beep, viewable }) );
}

int listlist( string str, string *list )
{
        string output;
        int i;
        output = "";
        if( !pointerp( list ) || !sizeof( list ) ) str += "No one.";
        else for( i=0; i<sizeof(list); i++ )
                output += capitalize( list[i] ) +
                        ( i<sizeof(list)-1 ? ", " : "." );
        write( strformat( str + output, "People " ) );
        write( "Total: " + sizeof( list ) + ".\n" );
        return 1;
}

int challenges( string str )
{
               listlist( "who have challenged you: ", chs );
        return listlist( "who you have challenged: ", ochs );
}

int watchers( string str )
{
        object pl, b;
        if( !str ) str = PRNAME;
        if( !pl = get_player( str ) ) return 1;
        if( !b = get_board( pl ) ) return 1;
        return listlist( "watching your board: ", b->query_watchers() );
}

int gameboards( string str )
{
        return listlist( "with " + GAME + " boards " +
                "(\"*\" indicates a game in progress ):\n", boards() );
}

void extra_create()
{
        seteuid(getuid());
	set( "id", ({ "board", "C4", "shift c4 board", "shift c4", 
	              "shift-c4 board", "shift-c4", "C4 board",
	              BOARD_ID }) );
        set( "short", "a " + GAME + " playing board" );
	set( "long",
                "This is a classic two player game with a new twist.\n" +
                "Both players will need a board to play.\n" +
                "Type " + GM + "help for a list of commands, " +
                "and " + GM + "rules for the rules.\n" +
                "Mail all donations (and complaints and bugs) " +
                "to the Games Cabal.\n" );
        THISO->wipe_board();
        chs = ({ });
        ochs = ({ });
        watchers = ({ });
        rpl1 = "No one";
        rpl2 = "No one else";
        viewable = 1;
}

int help( string str )
{
        string str2;
        str = "\n    ///////  " + CGAME + ",  Coded by Toothpick, " +
  "22nd October 1993  ///////\n\n" +
  "\t  $help                This information.                   \n" +
  "\t  $rules               The game rules.                     \n" +
  "\t  $setup               Set your " + GAME + " preferences.  \n" +
  "\t  $challenge <name>    Challenge <name> to a game.         \n" +
  "\t  $challenge           Lists of current challenges.        \n" +
  "\t  $accept <name>       Accept <name>'s offer of a game.    \n" +
  "\t  $                    View the current state of the board.\n" +
        MOVEHELP +
  "\t  $takeback            Takes back the LAST move you made.  \n" +
  "\t  $clock               Gives opponent " + TIME +
          " mins to move or be  \n\t\t\t\t\t\t\t  disqualified. \n" +
  "\t  $resign              Resign from your current game.    \n\n" +
  "\t  $norecord            If both players type this, the game \n" +
                                  "\t\t\t\t\t\twon't be recorded.\n" +
  "\t  $score               Gives your " + GAME + " scores.     \n" +
  "\t  $score [name]        Gives scores of [name] in $.       \n" +
  "\t  $who                 " + CGAME + "player list and info.  \n" +
  "\t  $who -s              Shortened form of the $ player list.\n" +
  "\t  $boards              A simple list of people with a board.\n" +
  "\t  $games               A list of all games in progress.    \n" +
  "\t  $view <name>         Look at the board of <name>.        \n" +
  "\t  $watch <name>        Watch moves made on <name>'s board. \n" +
  "\t  $unwatch <name>      Stop watching <name>'s board.       \n" +
  "\t  $watchers <name>     See who's watching <name>'s board.  \n" +
  "\t  $watchers            See who's watching your board.  \n" +
  "\t  $dest                Destroy this " + GAME + " board.    \n\n" +
  "    /////////////////////////////////////////" +
  "///////////////////////////\n";
        while( sscanf( str, "%s$%s", str, str2 ) ) str += GM + str2;
        THISP->more_string( str );
        return 1;
}

void extra_init()
{
        mixed *set;
        if( THISP != ENV( THISO ) ) call_out( "dest", 1 );
        if( ENV( THISO ) && THISP == ENV( THISO ) )
                set = query_property_value( THISP, PROPERTY );
        if( set && pointerp( set ) && sizeof( set )>=3 )
        {
                if( set[0] < 1 ) t = 0;
                else if( set[0] > 4 ) t = 5;
                else t = set[0];
                beep = ( set[1] == 1 );
                viewable = ( set[2] == 1 );
        }
        add_action( "make_move", GM );
        add_action( "help", GM + "help" );
        add_action( "rules", GM + "rules" );
        add_action( "resign", GM + "resign" );
        add_action( "challenge", GM + "challenge" );
        add_action( "norecord", GM + "norecord" );
        add_action( "gameboards", GM + "boards" );
        add_action( "who", GM + "who" );
        add_action( "score", GM + "score" );
        add_action( "takeback", GM + "takeback" );
        add_action( "clock", GM + "clock" );
        add_action( "games", GM + "games" );
        add_action( "challenges", GM + "challenges" );
        add_action( "challengers", GM + "challengers" );
        add_action( "watch", GM + "watch" );
        add_action( "unwatch", GM + "unwatch" );
        add_action( "watchers", GM + "watchers" );
        add_action( "accept", GM + "accept" );
        add_action( "view", GM + "view" );
        add_action( "setup1", GM + "setup" );
        add_action( "dest", GM + "dest" );
}

void show_watchers( string str )
{
        int i;
        object pl, b;
        if( !viewable ) return;
        for( i=0; i<sizeof(watchers); i++ )
        {
                if( !pl = find_player( watchers[i] ) ) continue;
                if( !b = FindBoard( pl ) ) continue;
                tell_object( pl, THISO->draw_board( b->query_type() ) );
                if( str ) tell_object( pl, str );
        }
}

int challenge( string str )
{
        object pl, b;
        if( !str )
        {
                challenges( 0 );
                return 1;
        }
        if( pl2 )
        {
                write( "You're already playing another game.\n" );
                return 1;
        }
        str = lower_case( str );
        if( str == PRNAME )
        {
                write( CTHISV + ": You can't challenge yourself.\n" );
                return 1;
        }
        if( !pl = get_player( str ) ) return 1;
        if( !b = get_board( pl ) ) return 1;
        if( !b->receive_challenge( PRNAME ) )
        {
                write( "You have already challenged this player.\n" );
                return 1;
        }
        write( "You challenge " + pl->query_name() +
                " to a game of " + GAME + ".\n" );
        tell_object( pl, CNAME + " challenges you " +
                "to a game of " + GAME + ".\n" );
        if( str = b->query_playing() )
                write( "(* " + pl->query_name() + " is currently " +
                "playing against " + capitalize( str ) + " *)\n" );
        if( searcha( ochs, pl->query_real_name() ) == -1 )
                ochs += ({ pl->query_real_name() });
        return 1;
}

void accepted( string str )
{
        int i;
        i = searcha( ochs, str );
        if( i >= 0 ) ochs = deletea( ochs, i );
}

int receive_challenge( string str )
{
        if( searcha( chs, str ) >= 0 ) return 0;
        chs = chs + ({ str });
        return 1;
}

int accept( string str )
{
        int i;
        string nom;
        object pl, b;
        if( !str && sizeof( chs ) > 1 )
        {
                write( "Who's challenge do you wish to accept? " +
                        "(Syntax: " + THISV + " <name>)\n" );
                return 1;
        }
        if( !sizeof( chs ) )
        {
                write( CTHISV + ": No one has challenged you.\n" );
                return 1;
        }
        if( pl2 )
        {
                write( "You're already playing another game.\n" );
                return 1;
        }
        if( !str && sizeof( chs ) == 1 ) str = chs[0];
        str = lower_case( str );
        nom = capitalize( str );
        i = searcha( chs, str );
        if( i == -1 )
        {
                write( "You have received no challenge from " +
                        nom + ".\n" );
                return 1;
        }
        if( !pl = get_player( str ) ) return 1;
        if( !b = get_board( pl ) ) return 1;
        if( b->query_playing() )
        {
                write( nom + " is already playing another game.\n" );
                return 1;
        }
        chs = deletea( chs, i );
        tell_object( pl, "Your challenge is accepted by " +
                CNAME + ".\n" );
        write( "You accept " + nom + "'s challenge.\n" );
        ym = random( 2 );
        if( ym )
        {
                write( "You get the first move.\n" );
                tell_object( pl, CNAME + " gets the first move.\n" );
        }
        else
        {
                write( nom + " gets the first move.\n" );
                tell_object( pl, "You get the first move.\n" );
        }
        b->set_playing( PRNAME );
        b->set_ym( !ym );
        b->accepted( PRNAME );
        b->wipe_grid();
        THISO->wipe_board();
        b->set_grid( grid );
        rpl1 = CNAME;
        rpl2 = nom;
        b->set_rpls( rpl2, rpl1 );
        pl2 = str;
        i = searcha( chs, str );
        if( i >= 0 ) deletea( chs, i );
        return 1;
}

int resign( string str )
{
        int i;
        string nom;
        object pl, b;
        if( !pl2 )
        {
                write( "You're not playing a game.\n" );
                return 1;
        }
        write( "You resign your " + GAME + " game (with " + rpl2 + ").\n" );
        mess = CNAME + " resigned.";
        if( pl = find_player( pl2 ) )
        {
                tell_object( pl, CNAME + " has resigned in the face " +
                        "of your superior " + GAME + " skills.\n" );
                if( b = FindBoard( pl ) )
                {
                        b->set_playing( 0 );
                        b->set_mess( CNAME + " resigned." );
                        b->stop_clock();
                }
        }
        if( viewable || ( b && b->query_viewable() ) )
        if( !query_offered() || ( b && !b->query_offered() ) )
        {
                record_loss( PRNAME, pl2 );
                record_win( pl2, PRNAME );
        }
        pl2 = 0;
        stop_clock();
        return 1;
}

int set_grid( int *new_grid )
{
        int i;
        for( i=0; i<sizeof(new_grid); i++ )
        {
            if( i < (BDH*BDW) )
            {
                if( new_grid[i]==0 ) grid[i] = 0;
                if( new_grid[i]==1 ) grid[i] = 2;
                if( new_grid[i]==2 ) grid[i] = 1;
            }
            else
              grid[i] = new_grid[i];
        }
        waiting = 0;
        tell_object( ENV( THISO ), ( beep ? "" : "" ) );
}

void out_board( object pl, object b, string me, string them )
{
        write( THISO->draw_board( t ) + ( me ? me : "" ) );
        tell_object( pl, b->draw_board( b->query_type() ) +
                ( them ? them : "" ) );
        show_watchers( them );
        b->show_watchers( them );
}

int view( string str )
{
        object pl, b;
        if( !str )
        {
                write( "Who's board do you wish to view? " +
                        "(Syntax: " + THISV + " <name>)\n" );
                return 1;
        }
        str = lower_case( str );
        if( !pl = find_player( str ) )
        {
                write( "View: " + capitalize( str ) +
                        " not logged in.\n" );
                return 1;
        }
        if( !b = FindBoard( pl ) )
        {
                write( pl->query_name() +
                        " has no board to view.\n" );
                return 1;
        }
        if( !b->query_viewable() )
        {
                write( pl->query_name() + " doesn't wish you to see " +
                        possessive( pl ) + " game.\n" );
                return 1;
        }
        write( b->draw_board( t ) );
        return 1;
}

int dest( string str )
{
        write( "The " + GAME + " board falls to the ground and " +
                "dissolves.\n" );
        say( "The " + GAME + " board dissolves into nothingness.\n" );
        destruct( THISO );
        return 1;
}

int takeback( string str )
{
        object pl, b;
        int n, i, ok;
        if( !pl2 )
        {
                write( "You're not currently playing a game.\n" );
                return 1;
        }
        if( ym )
        {
                write( "You can only take back your own move.\n" );
                return 1;
        }
        if( !pl = find_player( pl2 ) )
        {
                write( capitalize( pl2 ) + " has left the game" +
                        "...   I guess that means you win?.\n" );
                pl2 = 0;
                return 1;
        }
        if( !b = FindBoard( pl ) )
        {
                write( capitalize( pl2 ) + " has no board anymore" +
                        "...   I guess that means you win?.\n" );
                pl2 = 0;
                return 1;
        }
        if( cant_take_back )
        {
                write( "You can't take back any further.\n" );
                return 1;
        }
        ym = 1;
        b->set_ym( 0 );
        grid = ({ });
        b->stop_clock();
        Assign( grid, lastgrid );
        b->set_grid( lastgrid );
        lastmove = lastlastmove;
        b->set_lastmove( lastlastmove );
        out_board( pl, b, 0, CNAME + " took back " +
                possessive( THISP ) + " last move\n" );
        cant_take_back = 1;
        b->set_cant_take_back( 1 );
        return 1;
}

int watch( string str )
{
        object pl, b;
        if( !str )
        {
                write( "Who's board do you wish to watch? " +
                        "(Syntax: " GM "watch <name>)\n" );
                return 1;
        }
        str = lower_case( str );
        if( !pl = find_player( str ) )
        {
                write( "Watch: " + capitalize( str ) +
                        " not logged in.\n" );
                return 1;
        }
        if( !b = FindBoard( pl ) )
        {
                write( pl->query_name() +
                        " has no board to watch.\n" );
                return 1;
        }
        if( !b->query_viewable() )
        {
                write( pl->query_name() + " doesn't wish you to see " +
                        possessive( pl ) + " game.\n" );
                return 1;
        }
        write( b->add_watcher( PRNAME ) );
        return 1;
}

int unwatch( string str )
{
        object pl, b;
        if( !str )
        {
                write( "Who's board do you wish to stop watching? " +
                        "(Syntax: " GM "unwatch <name>)\n" );
                return 1;
        }
        str = lower_case( str );
        if( !pl = find_player( str ) )
        {
                write( "Unwatch: " + capitalize( str ) +
                        " not logged in.\n" );
                return 1;
        }
        if( !b = FindBoard( pl ) )
        {
                write( pl->query_name() +
                        " has no board to unwatch.\n" );
                return 1;
        }
        write( b->remove_watcher( PRNAME ) );
        return 1;
}

string add_watcher( string str )
{
        if( searcha( watchers, str ) >=0 )
                return "You're already watching this board.\n";
        watchers += ({ str });
                return "Ok, watching " + ENV( THISO )->query_name() + "\n";
}

string remove_watcher( string str )
{
        int i;
        i = searcha( watchers, str );
        if( i == -1 )
                return "You're not watching this board.\n";
        watchers = deletea( watchers, i );
        return "Ok, stopped watching " + ENV( THISO )->query_name() + "\n";
}

int games( string str )
{
        object *bds;
        int i, j;
        string whole;
        object p, b, p2, b2;
        bds = users();
        whole = "";
        for( i=0; i<sizeof(bds); i++ )
        {
                p = bds[i];
                if( b = FindBoard( p ) ) if( str = b->query_playing() )
                {
                        if( !p2 = find_player( str ) ) continue;
                        j = searcha( bds, p2 );
                        if( j == -1 ) continue;
                        bds = deletea( bds, j );
                        if( !b2 = FindBoard( p2 ) ) continue;
                        whole += "\t\t" +
                           ( b->query_viewable() ? "(o) " : "(x) " ) +
                           pad( capitalize( p->query_real_name() ), 13 ) +
                           " vs.  " +
                           ( b2->query_viewable() ? "(o) " : "(x) " ) +
                           capitalize( str ) + "\n";
                }
        }
        if( whole == "" )
                write( "No games in progress.\n" );
        else write( "\n\t  Games in progress: (o) viewable, (x) " +
                "non-viewable\n\t" + pad( "", 53, 126 ) + "\n" + whole );
        return 1;
}

int start_clock( string str )
{
        tell_object( ENV( THISO ), capitalize( pl2 ) + " has given you " +
                TIME + " minutes to make your move.\nIf you don't " +
                "make it in time, you will automatically resign.\n" );
        clock = TIME;
        call_out( "tick_tock", 60 );
}

void tick_tock()
{
        clock -= 1;
        if( !clock )
        {
                tell_object( ENV( THISO ), "You have been disqualified " +
                        "from your game with " + pl2 + " and are\nforced " +
                        "to resign.\n" );
                command( GM "resign", ENV( THISO ) );
                return;
        }
        tell_object( ENV( THISO ), "You have another " + clock +
                " minutes to make your move or be disqualified.\n" );
        call_out( "tick_tock", 60 );
}

int clock( string str )
{
        object pl, b;
        int n, i, ok;
        if( !pl2 )
        {
                write( "You're not currently playing a game.\n" );
                return 1;
        }
        if( ym )
        {
                write( "You can only start the clock on your " +
                        "oppenents move.\n" );
                return 1;
        }
        if( !pl = find_player( pl2 ) )
        {
                write( capitalize( pl2 ) + " has left the game" +
                        "...   I guess that means you win?.\n" );
                pl2 = 0;
                return 1;
        }
        if( !b = FindBoard( pl ) )
        {
                write( capitalize( pl2 ) + " has no board anymore" +
                        "...   I guess that means you win?.\n" );
                pl2 = 0;
                return 1;
        }
        if( waiting )
        {
                write( "The clock is already ticking.\n" );
                return 1;
        }
        write( "You give " + capitalize( pl2 ) + " " + TIME +
                " minutes to make " + possessive( pl ) + " move.\n" );
        waiting = 1;
        b->start_clock();
        return 1;
}

int norecord( string str )
{
        object pl, b;
        int n, i, ok;
        if( !pl2 )
        {
                write( "You're not currently playing a game.\n" );
                return 1;
        }
        if( !pl = find_player( pl2 ) )
        {
                write( capitalize( pl2 ) + " has left the game" +
                        "...   I guess that means you win?.\n" );
                pl2 = 0;
                return 1;
        }
        if( !b = FindBoard( pl ) )
        {
                write( capitalize( pl2 ) + " has no board anymore" +
                        "...   I guess that means you win?.\n" );
                pl2 = 0;
                return 1;
        }
        if( b->query_offered() )
        {
                write( "You agree not to have your " + GAME +
                        " game with " + rpl2 + " recorded.\n" );
                tell_object( pl, CNAME + " has accepted your offer " +
                        "not to record this game of " + GAME + ".\n" );
                offered = 1;
        }
        tell_object( pl, CNAME + " offers not to have this " + GAME +
                " game go on record.\nTo accept this offer, type " +
                THISV + ".\n" );
        write( "You offer not to keep records of this " + GAME +
                " game with " + capitalize( pl2 ) + "\n" );
        offered = 1;
        return 1;
}
add_game_channel()
{
    string *channels;

    channels = THISP->query_prop_value( ChannelP );
    if( !channels )
    {
        command( "channel game", THISP );
        return 1;
     }
    if( searcha( channels, "game" ) == -1 )
        command( "channel game", THISP );
}

varargs string
pad(mixed str, int len, int padchar)
{
  string sp_format;
  str = to_string(str);
  if(len < 0) 
  {
    if( padchar ) 
    {
      sp_format = "%' '*.*s";
      sp_format[2] = padchar;
      return sprintf(sp_format, -len, -len, str);
    }
    return sprintf("%*.*s", -len, -len, str);
  }
  if( padchar ) 
  {
    sp_format = "%' '-*.*s%s";
    sp_format[2] = padchar;
    return sprintf(sp_format, len, len, str, "");
  }
  return sprintf("%-*.*s%s", len, len, str, "");
}

void wipe_board()
{
        int i;
        mess = 0;
        cant_take_back = 1;
        grid = ({ });
        for( i=0; i<BDH*BDW; i++ ) grid = grid + ({ 0 });
        grid += allocate(BDH);
}

int check_draw()
{
        int i, j, k, ok;
        for( i=0; i<BDH; i++ )
                for( j=0; j<BDW; j++ )
                        if( !pos( j, i ) ) return 0;
        return 1;
}

int check_hor()
{
        int i, j, k, ok;
        for( i=0; i<BDH; i++ )
        {
                for( j=0; j<BDW-3; j++ )
                {
                        ok=1;
                        for( k=0; k<4; k++ )
                                if( pos( j+k, i ) != 1 ) ok = 0;
                        if( ok ) return 1;
                }
        }
}

int check_vert()
{
        int i, j, k, ok;
        for( i=0; i<BDH-3; i++ )
        {
                for( j=0; j<BDW; j++ )
                {
                        ok=1;
                        for( k=0; k<4; k++ )
                                if( pos( j, i+k ) != 1 ) ok = 0;
                        if( ok ) return 1;
                }
        }
}

int check_diag_left()
{
        int i, j, k, ok;
        for( i=0; i<BDH-3; i++ )
        {
                for( j=0; j<BDW-3; j++ )
                {
                        ok=1;
                        for( k=0; k<4; k++ )
                                if( pos( j+k, i+k ) != 1 ) ok = 0;
                        if( ok ) return 1;
                }
        }
}

int check_diag_right()
{
        int i, j, k, ok;
        for( i=0; i<BDH-3; i++ )
        {
                for( j=0; j<BDW-3; j++ )
                {
                        ok=1;
                        for( k=0; k<4; k++ )
                                if( pos( BDW-j-k-1 , i+k ) != 1 ) ok = 0;
                        if( ok ) return 1;
                }
        }
}

int check_win()
{
        if( check_hor() ) return 1;
        if( check_vert() ) return 1;
        if( check_diag_left() ) return 1;
        if( check_diag_right() ) return 1;
        return 0;
}

int rules( string str )
{
        write(
    "The Rules for Shift-Connect 4...\n" 
    "   This game is real simple too.  Both players take turns playing\n" 
    "by either dropping a piece in a column (1-4) or shifting a row (a-d)\n" 
    "right or left.  Pieces dropped down columns are added to that\n" 
    "column, as far down the column as possible.  For instance, \"C4 3\"\n" 
    "drops a piece down column 3.  Instead of dropping a piece you may\n" 
    "also shift rows with a command like \"C4 c left\".  Rows may only be\n" 
    "shifted one column from their starting position, so that a row\n" 
    "been shifted left must be shifted right before it may be shifted\n" 
    "left again.  Pieces shifted off the end of the board are lost and\n" 
    "cannot be recovered.  You may not make any move which leaves the\n" 
    "board in the exact same state that it was before the last move.  For\n"
    "instance, if PlayerA shifts row C right and no pieces are made to\n" 
    "fall down a column or are shifted off the end, then PlayerB must\n" 
    "wait a whole turn before he can shift row C left.  Each of the\n" 
    "players has different colored pieces and the first player to get\n\n" 
    "FOUR of their color peices in a row, column or diagonal is the winner.\n"
        );
        return 1;
}

string bit( int x )
{
        if( t==0 )
        {
                if( x==1 ) return "()";
                if( x==2 ) return "::";
                return "  ";
        }
        if( t==1 || t==2 )
        {
                if( x==1 ) return "###";
                if( x==2 ) return ":::";
                return "   ";
        }
        if( t==3 )
        {
                if( x==1 ) return GREEN + "  ";
                if( x==2 ) return RED + "  ";
                return BLACK + "  ";
        }
        if( t==4 || t==5 )
        {
                if( x==1 ) return GREEN + "    ";
                if( x==2 ) return RED + "    ";
                return BLACK + "    ";
        }
}

string add_on( int i )
{
        if( i==1 ) return "  [" + bit( 1 ) + ( t>2 ? BLACK : "" ) + "] " + rpl1;
        if( i==2 ) return "  [" + bit( 2 ) + ( t>2 ? BLACK : "" ) + "] " + rpl2;
        if( i==4 )
        {
                if( mess ) return "   " + mess;
                else if( ym ) return "    " + rpl1 + "'s turn.";
                        return "    " + rpl2 + "'s turn.";
        }
        return "";
}

string draw_board( int board_type )
{
        int i, j, temp;
        string s;
        s = "";
        temp = t;
        t = board_type;
        if( t==0 )
        {
                s = "\n" + SMTOP + "\n";
                for( i=0; i<BDH; i++ )
                {
                        s += sprintf( "%c ", (BDH-i-1) + 'A' );
                        if( grid[<(i+1)] < 0 )
                          s = s + "<|";
                        else
                          s = s + " |";                        
                        for( j=0; j<BDW; j++ ) s = s + BIT;
                        if( grid[<(i+1)] > 0 )
                          s = s + "|>" + add_on( i ) + "\n";
                        else
                          s = s + "| " + add_on( i ) + "\n";
                }
                s = s + SMBOT + "\n" + SMNOS + "\n";
        }
        if( t==1 )
        {
                for( i=0; i<BDH; i++ )
                {
                        s = s + "\n\t\t" + MMMID + "\n\t\t";
                        s += sprintf( "%c ", (BDH-i-1) + 'A' );                        
                        if( grid[<(i+1)] < 0 )
                          s += "<|";
                        else
                          s += " |";
                        for( j=0; j<BDW; j++ ) s = s + BIT + "|";
                        if( grid[<(i+1)] > 0 )
                          s += ">";
                        else
                          s += " ";
                        s = s + add_on( i );
                }
                s = s + "\n\t\t" + MMMID + "\n\t\t" + MMNOS + "\n";
        }
        if( t==2 )
        {
                s = "\n" + BMTOP + "\n" + BMMID2 + "\n";
                for( i=0; i<BDH; i++ )
                {
                        s += sprintf( "%c ", (BDH-i-1) + 'A' );                        
                        if( grid[<(i+1)] < 0 )
                          s = s + " <|| ";
                        else
                          s = s + "  || ";
                        for( j=0; j<BDW; j++ )
                        {
                                s = s + BIT + " || ";
                        }
                        if( grid[<(i+1)] > 0 )
                          s += ">";
                        else
                          s += " ";
                        s = s + "\n" + BMMID1 + add_on( i ) +
                                ( i!=BDH-1 ? "\n" + BMMID2 + "\n" :
                                "\n" + BMBOT + "\n" + BMNOS + "\n" );
                }
        }
        if( t==3 )
        {
                s = "\n  " + BLUE + pad( " ", 12 ) + BLACK + "\n";
                for( i=0; i<BDH; i++ )
                {
                        if( grid[<(i+1)] < 0 )
                          s = s + " <" + BLUE;
                        else
                          s = s + "  " + BLUE;
                        s += sprintf( " %s%c", 
                             ( i%2 ? FGREEN : FRED ), 
                             (BDH-i-1) + 'A' );
                        for( j=0; j<BDW; j++ ) s = s + BIT;
                        s = s + BLUE + "  " + NORM;
                        if( grid[<(i+1)] > 0 )
                          s += FWHITE ">";
                        else
                          s += " ";
                        s = s + BLACK + add_on( i ) + "\n";
                }
                s = s + "  " + SCBOT + "\n";
        }
        if( t==4 )
        {
                s = "\n  " + BLUE + pad( " ", 20 ) + BLACK + "\n";
                for( i=0; i<BDH; i++ )
                {
                        if( grid[<(i+1)] < 0 )
                          s = s + " <" + BLUE + "  ";
                        else
                          s = s + "  " + BLUE + "  ";
                        for( j=0; j<BDW; j++ ) s = s + BIT; 
                        s = s + BLUE + "  " + NORM;
                        if( grid[<(i+1)] > 0 )
                          s += FWHITE ">";
                        else
                          s += " ";
                        s = s + BLACK + add_on( i ) + "\n";
                        if( grid[<(i+1)] < 0 )
                          s = s + " <" + BLUE;
                        else
                          s = s + "  " + BLUE;
                        s += sprintf( " %s%c", 
                             ( i%2 ? FGREEN : FRED ), 
                             (BDH-i-1) + 'A' );
                        for( j=0; j<BDW; j++ ) s = s + BIT;
                        s = s + BLUE + "  " + NORM;
                        if( grid[<(i+1)] > 0 )
                          s += FWHITE ">";
                        else
                          s += " ";
                        s += BLACK + "\n";
                }
                s = s + "  " + MCBOT + "\n";
        }
        if( t==5 )
        {
                s = "\n  " + BLUE + pad( " ", 26 ) + BLACK + "\n";
                for( i=0; i<BDH; i++ )
                {
                        if( grid[<(i+1)] < 0 )
                          s = s + " <" + BLUE + "  ";
                        else
                          s = s + "  " + BLUE + "  ";
                        for( j=0; j<BDW; j++ ) s = s + BIT + BLUE + "  ";
                        s = s + BLACK;
                        if( grid[<(i+1)] > 0 )
                          s += FWHITE ">\n";
                        else
                          s += " \n";
                        if( grid[<(i+1)] < 0 )
                          s = s + " <" + BLUE;
                        else
                          s = s + "  " + BLUE;
                        s += sprintf( " %s%c", 
                             ( i%2 ? FGREEN : FRED ), 
                             (BDH-i-1) + 'A' );                     
                        for( j=0; j<BDW; j++ ) s = s + BIT + BLUE + "  ";
                        s = s + NORM;
                        if( grid[<(i+1)] > 0 )
                          s += FWHITE ">\n";
                        else
                          s += " \n";
                        if( i != BDH-1 ) s = s + "  " + BLUE + pad( " ", 26 ) +
                                BLACK + add_on( i ) + "\n";
                }
                s = s + "  " + LCBOT + "\n";
        }
        if( lastmove ) s += lastmove;
        t = temp;
        return s;
}

int setup1( string str )
{
        write( SETUP1 );
        THISO->setup2();
        return 1;
}

void setup2( string str )
{
        if( str != "1" && str != "2" && str != "3" )
        {
                write( SETUP2 );
                input_to( "setup2" );
                return;
        }
        t = atoi( str ) - 1;
        THISO->setup3();
}

void setup3( string str )
{
        if( str != "c" && str != "m" )
        {
                write( SETUP3 );
                input_to( "setup3" );
                return;
        }
        if( str == "c" ) t = t + 3;
        THISO->setup4();
}

void setup4( string str )
{
        if( str != "y" && str != "n" )
        {
                write( SETUP4 );
                input_to( "setup4" );
                return;
        }
        beep = ( str == "y" );
        THISO->setup5();
}

void setup5( string str )
{
        if( str != "y" && str != "n" )
        {
                write( SETUP5 );
                input_to( "setup5" );
                return;
        }
        viewable = ( str == "y" );
        save_setup();
        write( "Setup completed and saved.\n" );
}

int make_move( string str )
{
        object pl, b;
        int n, i, j, ok;
        string me, them;

        if( !str )
        {
                write( draw_board( t ) );
                return 1;
        }
        if( !pl2 )
        {
                write( "You're not currently playing a game.\n" );
                return 1;
        }
        if( !ym )
        {
                write( "It's not your move.\n" );
                return 1;
        }
        if( !pl = find_player( pl2 ) )
        {
                write( capitalize( pl2 ) + " has left the game" +
                        "...   I guess that means you win?.\n" );
                pl2 = 0;
                return 1;
        }
        if( !b = FindBoard( pl ) )
        {
                write( capitalize( pl2 ) + " has no board anymore" +
                        "...   I guess that means you win?.\n" );
                pl2 = 0;
                return 1;
        }
        n = atoi( str );
        if( !n )
        {
          string dir;
          int row;
          mixed tmp;
          if( ( sscanf( str, "%s %s", tmp, dir ) == 2 ) &&
              ( strlen(tmp) == 1 ) &&
              ( tmp = lower_case(tmp) ) &&
              ( dir = lower_case(dir) ) &&
              ( ( dir == "right" ) || ( dir == "left" ) ) &&
              ( ( row = tmp[0] - 'a' ) >= 0 ) &&
              ( row < BDH ) &&
              ( tmp = ( ( dir == "right" ) ? 1 : -1 ) ) &&
              ( abs( grid[<(BDH-row)] + tmp ) <= 1 ) )
	  {
            int *newgrid, *checkgrid;
            
            Assign( newgrid, grid );
            checkgrid = b->query_lastgrid();
            
            newgrid[<(BDH-row)] += tmp;
            i = row*BDW;
            if( tmp < 0 )
              newgrid[i..(i+(BDW-1))] = newgrid[i+1..(i+(BDW-1))] + ({ 0 });
            else if( tmp > 0 )
              newgrid[i..(i+(BDW-1))] = ({ 0 }) + newgrid[i..(i+(BDW-2))];
            for( i=0 ; i<(BDH*BDW) ; i++ )
            {
              if( !newgrid[i] && ( i < (BDW*(BDH-1)) ) )
              {
              	for( j=i+BDW ; j < (BDW*BDH) ; j+=BDW )
              	{
                  if( newgrid[j] )
                  {
                    newgrid[i] = newgrid[j];
                    newgrid[j] = 0;
                    break;
                  }
                }
              }
            }
            
            for( i=sizeof(newgrid)-1 ; i>=0 ; i-- )
            {
                if( i < (BDH*BDW) ) {
         	if( checkgrid[i] == 2 )
            	  checkgrid[i] = 1;
            	else if( checkgrid[i] == 1 )
            	  checkgrid[i] = 2;
            	}
            	if(!sizeof(checkgrid) || (newgrid[i] != checkgrid[i]))
            	{
                    stop_clock();
                    Assign( lastgrid, grid );
                    Assign( grid, newgrid );

                    ym = 0;
                    b->set_grid( grid );
                    lastlastmove = lastmove;
                    lastmove = sprintf("%s went (shift %c %s)\n", 
                                       CNAME, 'A'+row, dir);
                    b->set_lastmove( lastmove );
                    break;
                }
            }
            if( i<0 )
            {
                write( "Illegal move...\n" );
                write( draw_board( t ) );
                return 1;
            }
          }
          else
          {
            write( "Illegal move...\n" );
            write( draw_board( t ) );
            return 1;
          }
        }
        else
        {
          n--;
          if( n < 0 || n > BDW-1 || pos( n, BDH - 1 ) )
          {
                  write( "Illegal move...\n" );
                  write( draw_board( t ) );
                  return 1;
          }
          stop_clock();
          Assign( lastgrid, grid );
          ok = 0;
          for( i=0; i<BDH; i++ )
          {
                  if( !ok && !pos( n, i ) )
                  {
                          pos( n, i ) = 1;
                          ok = 1;
                  }
          }
          ym = 0;
          b->set_grid( grid );
          lastlastmove = lastmove;
          lastmove = CNAME + " went (column " + ( n + 1 ) + ")\n";
          b->set_lastmove( lastmove );
        }
        if( check_win() )
        {
            if( b->check_win() )
            {
                b->set_playing( 0 );
                b->set_mess( "Wow, it's a draw." );
                mess = "Wow, it's a draw.";
                me = "--------> A DRAW HAS OCCURRED <--------\n";
                them = "--------> A DRAW HAS OCCURRED <--------\n";
                if( !query_offered() || ( b && !b->query_offered() ) )
                {
                        record_draw( pl2, PRNAME );
                        record_draw( PRNAME, pl2 );
                }
                pl2 = 0;
            }
            else
            {
                b->set_playing( 0 );
                b->set_mess( CNAME + " won." );
                mess = CNAME + " won.";
                me = "--------> YOU  WIN <--------\n";
                if( t>2 ) for( i=0; i<10; i++ ) me += FLASH;
                them = "--------> " + CNAME + " has won <--------\n";
                if( !query_offered() || ( b && !b->query_offered() ) )
                {
                        record_loss( pl2, PRNAME );
                        record_win( PRNAME, pl2 );
                        pl2 = 0;
                }
            }
        }
        else if( b->check_win() )
        {
                b->set_playing( 0 );
                b->set_mess( rpl2 + " won." );
                mess = rpl2 + " won.";
                them = "--------> YOU  WIN <--------\n";
                if( t>2 ) for( i=0; i<10; i++ ) them += FLASH;
                me = "--------> " + rpl2 + " has won <--------\n";
                if( !query_offered() || ( b && !b->query_offered() ) )
                {
                        record_loss( PRNAME, pl2 );
                        record_win( pl2, PRNAME );
                        pl2 = 0;
                }
        }        	
        else b->set_ym( 1 );
        set_ym( 0 );
        cant_take_back = 0;
        out_board( pl, b, me, them );
        return 1;
}
