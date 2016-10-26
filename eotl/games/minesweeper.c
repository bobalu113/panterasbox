/* minesweeper.c -- a little minesweeper game for the mud.
 *
 * originally written by...   ????
 *
 * modified a bit by David (Him)@EotL, circa 2003-02-28
 *  (made things more polite, to player, coder, and game driver alike)
 *  (via improvement of messages; more standard formatting and the
 *  addition of comments; and taming down of recursion, respectively.)
 */

#include <ansi.h>
#define MAX_MINE_PERCENT 93
#define MAX_HEIGHT 24
#define MAX_WIDTH 30
#define MIN_MINE 10
#define MIN_HEIGHT 10
#define MIN_WIDTH 10
#define BLANK 0
#define FLAGGED -1
#define MARKED -2
#define NOMINES MAXINT

void	create();
void	init();
string	short();
string	long();
status	id(string str);
int	get();
int	drop();
status	do_view(string unused_action_arg);
status	do_mark(string str);
status	do_uncover(string str);
status	do_start(string str);
void	get_width(string str);
void	get_height(string str, int width);
void	get_mines(string str, int width, int height);
void	verify_end(string str, string action);
void	init_game(int width, int height, int mines);
void	end_game();
int	check_win();
void	change_num(int width, int height);
mixed	add_pair(mixed pairs, int width, int height);

mixed	*grid, *worker;
int	flagged, allmines, gameover, time;

void create()
{
    time = 0;
    grid = ({ }); 
    worker = ({ });
    flagged = 0;
    flagged = 0;
    gameover = -1;
}

void init()
{          
    add_action("do_view", "msview");
    add_action("do_mark", "mark");
    add_action("do_uncover", "uncover");
    add_action("do_start", "start");
}

int get() { return 1; }

int	drop()
{
    say( sprintf("You hear an explosion as %s drops %s minesweeper board.\n" +
	"It's a pretty pathetic explosion, though.\n", PNAME, possessive(THISP)) );
    write("As you drop the minesweeper game, it explodes!  (Pathetically.)\n");
    destruct(THISO);
    return(1);
}

string short() { return "a minesweeper board"; }

string long()
{
    return "This is a minesweeper board.  It is grey.\n"
    "Valid commands are:\n"
    "  msview: This shows ya the board.\n\n"
    "  mark <x> <y>: This marks the square at (x,y).  Cycles through flagged,\n"
    "                that questionmark deal, and no mark at all.\n\n"
    "  uncover <x> <y>: This uncovers the square at (x,y).  Ya better hope it\n"
    "                   ain't a mine.\n\n"
    "  start: This starts a game.  You may optionally specify width,\n"
    "         height, and number of mines; if not, you will be prompted.\n";
}

status id(string str)
{
    return(member(({
	  "board",
	  "minesweeper",
	  "minesweeper board",
	  "mines",
	  short(),
	}), str) >= 0);
    return str == "board" || str == "minesweeper" || 
    str == "minesweeper board" || str == short();
}

status do_start(string str)
{ 
    int height, width, mines;
    if(sizeof(grid) && !gameover)
    {
	write("End current game [y/n]: ");
	input_to("verify_end", 0, str);
    }
    else if( !str || sscanf(str, "%d %d %d", width, height, mines) == 3 )
    {
	init_game(width, height, mines);
    }
    else
    {
	write("How wide is the field? ");
	input_to("get_width");
    }
    return 1;
}

void verify_end(string str, string action)
{
    if(lower_case(str[0..0])=="y")
    {
	create();
	do_start(action);
    }
    return;
}

void get_width(string str)
{
    int width;
    width = to_int(str);
    if(width < MIN_WIDTH)
    {
	write("Width must be at least " + to_string(MIN_WIDTH) + ".\n"
	  "New width? ");
	input_to("get_width");
    }
    else if(width>MAX_WIDTH)
    {
	write("Width must be no larger than than " + to_string(MAX_WIDTH) + ".\n"
	  "New width? ");
	input_to("get_width");
    }
    else
    {
	write("How high is the field? ");
	input_to("get_height", 0, width);
    }
    return;
}

void get_height(string str, int width)
{
    int height;
    height=to_int(str);
    if(height < MIN_HEIGHT)
    {
	write("Height must be at least " + to_string(MIN_HEIGHT) + ".\n"
	  "New height? ");
	input_to("get_height", 0, width);
    }
    else if(height>MAX_HEIGHT)
    {
	write("Height must be no larger than " + to_string(MAX_HEIGHT) + ".\n"
	  "New height? ");
	input_to("get_height", 0, width);
    }
    else
    {
	write("How many mines? ");
	input_to("get_mines", 0, width, height);
    }
    return;
}


void get_mines(string str, int width, int height)
{
    int mines;
    mines = to_int(str);
    if(mines < MIN_MINE)
    {
	write("You must have at least " + to_string(MIN_MINE) + " mines.\n"
	  "New number? ");
	input_to("get_mines", 0, width, height);
    }
    else if( (float)mines / (height*width) > 
      MAX_MINE_PERCENT/100.0 )
    {
	write("You may only have " + to_string(MAX_MINE_PERCENT) + "% of your "
	  "field be mines.\nNew number? ");
	input_to("get_mines", 0, width, height);
    }
    else
	init_game(width, height, mines);
    return;
}

void init_game(int width, int height, int mines)
{
    int x, y;          
    allmines = mines;
    if(width<MIN_WIDTH)
    {
	write("Width must be at least " + to_string(MIN_WIDTH) + ".\n"
	  "New width? ");
	input_to("get_width");
    }
    else if(width>MAX_WIDTH)
    {
	write("Width must be no greater than " + to_string(MAX_WIDTH) + ".\n"
	  "New width? ");
	input_to("get_width");
    }
    else if(height < MIN_HEIGHT )
    {
	write("Height must be at least " + to_string(MIN_HEIGHT) + ".\n"
	  "New height? ");
	input_to("get_height", 0, width);
    }
    else if(height>MAX_HEIGHT)
    {
	write("Height must be no greater than " + to_string(MAX_HEIGHT) + ".\n"
	  "New height? ");
	input_to("get_height", 0, width);
    }
    else if(mines<MIN_MINE)
    {
	write("You must have at least " + to_string(MIN_MINE) + " mines.\n"
	  "New number? ");
	input_to("get_mines", 0, width, height);
    }
    else if( (float)mines / (height*width) > 
      MAX_MINE_PERCENT/100.0 )
    {
	write("You may only have " + to_string(MAX_MINE_PERCENT) + "% of your "
	  "field be mines.\nNew number? ");
	input_to("get_mines", 0, width, height);
    }
    else
    {
	grid = map_array(allocate(width), lambda(({'x, 'y}), 
	    ({ #'=, 'x, ({ #'allocate, 'y }) })), height);
	worker = map_array(allocate(width), lambda(({'x, 'y}),
	    ({ #'=, 'x, ({ #'allocate, 'y }) })), height); 
	flagged = 0;
	allmines = mines;
	gameover = 0;
	time = time();
	while(mines--)
	{
	    do
	    { 
		x = random(width);
		y = random(height);
	    }
	    while(grid[x][y]);
	    grid[x][y]++;
	}
	return;
    }
}

status do_mark(string str)
{
    int height, width, val;
    notify_fail("Usage: mark <height> <width>\n");
    if( !str ) return 0;
    if(sscanf(str, "%d %d", width, height)!=2) return 0;
    if( width <= 0 || height <= 0 ||
      width>sizeof(worker) || height>sizeof(worker[width-1]))
    {
	write("That is not a valid square.\n");
	return 1;
    }
    height--;
    width--;
    val=worker[width][height]; 
    if(val>0)
    {                
	write("You've already uncovered that square!\n");
	return 1;
    }
    if(val==BLANK)
    {
	worker[width][height] = FLAGGED;
	flagged++;
	if(flagged==allmines)
	    if(check_win())
		end_game();
	return 1;
    }
    else if(val==FLAGGED)
    {
	worker[width][height] = MARKED;
	flagged--;
    }
    else if(val==MARKED) worker[width][height] = BLANK;
    return 1;
}

status do_uncover(string str)
{
    int height, width, val;
    notify_fail("Usage: uncover <height> <width>\n");
    if( !str ) return 0;
    if(sscanf(str, "%d %d", width, height)!=2) return 0;
    if( width <= 0 || height <= 0 ||
      width>sizeof(worker) || height>sizeof(worker[width-1]))
    {
	write("That is not a valid square.\n");
	return 1;
    }
    height--;
    width--;
    if(val=worker[width][height]>0)
    {
	write("You've already uncovered that square!\n");
	return 1;
    }
    if(grid[width][height])
    {
	write("You lose!\n");
	gameover = 1; 
	return 1;
    }
    change_num(width, height);

    /* they may have won by uncovering the last of the non-bombs: */
    if(check_win())
	end_game();

    do_view("unused");
    return 1;
}

void change_num(int width, int height)
{
    int mines;
    mixed pairs = ({ ({ width, height }) });

    while(sizeof(pairs))
    {
	/* pull off the data from the first pair... */
	width = pairs[0][0];
	height = pairs[0][1];
	/* and now get rid of it, since we're about to handle it... */
	pairs = pairs[1..];
	/* and don't forget to init this each time through: */
	mines = 0;

	if( width<sizeof(grid)-1 &&
	  height<sizeof(grid[width])-1 &&
	  grid[width+1][height+1] ) mines++;

	if( width<sizeof(grid)-1 &&
	  grid[width+1][height] ) mines++;

	if( height<sizeof(grid[width])-1 &&
	  grid[width][height+1] ) mines++;

	if( width && height && grid[width-1][height-1] ) mines++;
	if( width && grid[width-1][height] ) mines++;
	if( height && grid[width][height-1] ) mines++;
	if( width<sizeof(grid)-1 && height && grid[width+1][height-1] ) mines++;    
	if( width && height<sizeof(grid[width])-1 && 
	  grid[width-1][height+1] ) mines++;

	if(mines)
	{
	    worker[width][height] = mines;
	}
	else
	{
	    /* mark this square clear: */
	    worker[width][height] = NOMINES;

	    /* and now check the adjacent squares for blanks... */

	    /* above, right */
	    if( width<sizeof(worker)-1 &&
	      height<sizeof(worker[width])-1 &&
	      worker[width+1][height+1] == BLANK)
		pairs = add_pair(pairs, width+1, height+1);

	    /* level, right */
	    if( width<sizeof(worker)-1 &&
	      worker[width+1][height] == BLANK )
		pairs = add_pair(pairs, width+1, height);

	    /* above, even */
	    if( height<sizeof(worker[width])-1 && 
	      worker[width][height+1] == BLANK )
		pairs = add_pair(pairs, width, height+1);

	    /* below, left */
	    if( width && height &&
	      worker[width-1][height-1] == BLANK )
		pairs = add_pair(pairs, width-1, height-1);

	    /* level, left */
	    if( width && worker[width-1][height] == BLANK ) 
		pairs = add_pair(pairs, width-1, height);

	    /* even, below */
	    if( height && worker[width][height-1] == BLANK ) 
		pairs = add_pair(pairs, width, height-1);

	    if( width<sizeof(worker)-1 && height &&
	      worker[width+1][height-1] == BLANK )
		pairs = add_pair(pairs, width+1, height-1);

	    if( width && height<sizeof(worker[width])-1 &&
	      worker[width-1][height+1] == BLANK )
		pairs = add_pair(pairs, width-1, height+1);
	}
    }
    return;
}

mixed add_pair(mixed pairs, int width, int height)
{
    foreach(mixed pair : pairs)
    {
	if(pair[0] == width && pair[1] == height)
	    return(pairs);
    }

    /* if we fell through, it wasn't found, so add it: */
    pairs += ({ ({ width, height }) });

    return(pairs);
}

void end_game()
{ 
    printf("Yay, you win.  You %s %d mines in %d seconds.\n",
      (flagged == allmines ? "flagged" : "uncovered all but"),
      allmines, time()-time);
    gameover = 1;
}

int check_win()
{
    int x, y, i, j, unmarked, nonbomb;
    unmarked = nonbomb = 0;
    i = sizeof(grid);  
    for(x=0;x<i;x++)
    {
	j = sizeof(grid[x]);
	for(y=0;y<j;y++)
	{
	    /* tally bombs which haven't been marked: */
	    if(grid[x][y] && worker[x][y] != FLAGGED)
		unmarked++;
	    /* but also tally what's missing, because... */
	    if(!grid[x][y] && worker[x][y] == BLANK)
		nonbomb++;
	}
    } 
    /* ... if there are no more non-bomb squares left covered, then
     * that's a valid way to win...  One can also win by having marked
     * all of the actual bombs.
     * Return value is 1 for win, 0 for not win.
     * (which is true if either nothing still blank is a non-bomb,
     * or no bombs are unmarked)
     */
    return(!nonbomb || !unmarked);
}

status do_view(string unused_action_arg)
{
    int i, j, w, h, ansi;
    string out;

    ansi = THISP->query_ansi();
    if(gameover==-1)
    { 
	write("There is not currently a game running.\n");
	return 1;
    }
    w = sizeof(worker);
    if(w)
	h = sizeof(worker[0]);

    out = "   ";
    for(i=1;i<=w;i++)
    {
	/* print the tens place; use "greenbar" (see below) */
	out += ((ansi && (((i - 1) / 2) % 2) == 1 ? CYAN : "") +
	  (i >= 10 ? i / 10 : " ") +
	  (ansi && (((i - 1) / 2) % 2) == 1 ? NORM : ""));
    }
    out += "\n   ";
    for(i=1;i<=w;i++)
    {
	/* print ones place; as above for greenbar */
	out += ((ansi && (((i - 1) / 2) % 2) == 1 ? CYAN : "") +
	  i % 10 +
	  (ansi && (((i - 1) / 2) % 2) == 1 ? NORM : ""));
    }
    out += "\n";
    out += "  +";
    for(i=0;i<w;i++)
	out += "-";
    out += "+\n";
    for(j=0;j<h;j++)
    {
	out += sprintf("%2d", j+1);
	out += "|";
	for(i=0;i<w;i++)
	{
	    if(gameover==-1) out += " ";
	    else if(gameover==1 && grid[i][j])              
		out += ansi ? BOLD_RED BLINK "!" NORM : "!";   
	    else
	    {         
		switch(worker[i][j])
		{
		    /* note: the funky extra stuff in BLANK and NOMINES is to get a "greenbar"
		     * type effect (though in this case I'm not using green, because there isn't
		     * really a pale enough one in the ANSI colors... */
		case BLANK   : out += ansi && ((i / 2) % 2) == 1 ? CYAN "#" NORM : "#"; break;
		case FLAGGED : out += ansi ? BOLD_RED "*" NORM : "*"; break;
		case MARKED  : out += ansi ? BLUE "?" NORM : "?"; break;
		case NOMINES : out += ansi && ((i / 2) % 2) == 1 ? CYAN "_" NORM : "_"; break;
		case 1 : out += ansi ? BOLD_BLUE "1" NORM : "1"; break;
		case 2 : out += ansi ? GREEN "2" NORM : "2"; break;
		case 3 : out += ansi ? RED "3" NORM : "3"; break;
		case 4 : out += ansi ? YELLOW "4" NORM : "4"; break;
		case 5 : out += ansi ? CYAN "5" NORM : "5"; break;
		case 6 : out += ansi ? BOLD_GREEN "6" NORM : "6"; break;
		case 7 : out += ansi ? MAGENTA "7" NORM : "7"; break;
		case 8 : out += ansi ? BOLD_YELLOW "8" NORM : "8"; break;
		default :
		    write("Error in grid array.  This is a bug; please report it.\n");
		}
	    }
	}
	out += "|\n";
    }
    out += "  +";
    for(i=0;i<w;i++)
	out += "-";
    out += "+\n";
    out += sprintf("  Mines Left: %d  Time: %d\n", allmines-flagged, time()-time);
    write(out);
    return 1;
}

query_grid() { return grid; }

query_worker() { return worker; }
