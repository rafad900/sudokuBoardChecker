#include <stdio.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

/* These are the only two global variables allowed in your program */
static int verbose = 0;
static int use_fork = 0;

struct Data {
	char (*board)[9];
	int row;
	int col;
	int x;
	int y;
	int invalidRow;
	int invalidCol;
	int invalidCel;
};

// This is a simple function to parse the --fork argument.
// It also supports --verbose, -v
void parse_args(int argc, char *argv[])
{
    int c;
    while (1)
    {
        static struct option long_options[] =
        {
            {"verbose", no_argument,       0, 'v'},
            {"fork",    no_argument,       0, 'f'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long (argc, argv, "vf", long_options, &option_index);
        if (c == -1) break;

        switch (c)
        {
            case 'f':
                use_fork = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                exit(1);
        }
    }
}

// This is the function to read in the board
void read_board(char b[][9]) {
	char c;
	int x = 0, y = 0;
	scanf("%c", &c);
	while(1) {
        if(isdigit(c)) {
            if ((int)c < 0 || (int)c > 9) {
                printf("The file has out of range inputs\n");
                exit(1);
            }
			b[x][y] = c;
			y++;
			if (y == 9) {
				y = 0;
				x++;
			}
			if (x == 9) 
				break;
		}
		scanf("%c", &c);
	}
}

// Checks the rows of the board
void* checkrow (void* row) {
	struct Data* d = (struct Data *)row;
	int v [9] = {0,0,0,0,0,0,0,0,0};
	for (int i = 0; i < 9; i++) {
		int pos = d->board[d->row][i]-49;
		if (!v[pos])
			v[pos] = 1;
		else d->invalidRow = 1;	
	}
	return NULL;
}

// Checks the columns of the board
void* checkcolumn (void* column) {
	struct Data* d = (struct Data *)column;
	int v [9] = {0,0,0,0,0,0,0,0,0};
	for (int i = 0; i < 9; i++) {
		int pos = d->board[i][d->col]-49;
		if (!v[pos]) 
			v[pos] = 1;
		else d->invalidCol = 1;
	}	
	return NULL;
}

// Checks the cells of the board
void* checkcell (void* cell) {
	struct Data* d = (struct Data *)cell;
	int v [9] = {0,0,0,0,0,0,0,0,0};
	for (int x=d->x; x < d->x+3; x++) {
		for (int y=d->y; y < d->y+3; y++) {
			int pos = d->board[x][y]-49;
			if (!v[pos]) 
				v[pos] = 1;
			else 
                d->invalidCel = 1;
		}
	}
	return NULL;
}

// Gets everything running
int main(int argc, char *argv[])
{
    parse_args(argc, argv);

	// Create the variables, read in the board, set the pointer
	// create threads and structs
	char b[9][9];
	read_board( b );
	char (*p)[9] = b;
	pthread_t children[27];
	struct Data d[27];
	int _x = 0, _y = 0;
	printf("\n\n");

	// Just some output
    if (verbose && use_fork) {
        printf("We are forking child processes as workers.\n");
    } else if (verbose) {
        printf("We are using worker threads.\n");
    }

	// This is where the checking begins
	if (use_fork) {
		// CHECK SUDOKU WITH FORKS
		int pids [27];
		int parent = 1;
		int error = -1;
		for (int r = 0; r < 27 && parent; r++) {
			d[r].board = p, d[r].row = 0, d[r].col=0, d[r].x=0, d[r].y=0, d[r].invalidRow=0, d[r].invalidCol=0, d[r].invalidCel=0;
			if (r >= 0 && r < 9) {
				d[r].row = r;
				parent = fork();
				if (parent == 0) {
					checkrow(&d[r]);
					if (d[r].invalidRow == 1)
						exit(1);
					else 
						exit(0);
				}
			} else if (r >= 9 && r < 18) {
				d[r].col = r-9;
				parent = fork();
				if (parent == 0) {
					checkcolumn(&d[r]);
					if (d[r].invalidCol == 1)
						exit(2);
					else 
						exit(0);
				}
			} else {
				d[r].x = _x, d[r].y = _y;
				parent = fork();
				if (parent == 0) {
					checkcell(&d[r]);
					if (d[r].invalidCel == 1)
						exit(3);
					else 
						exit(0);
				}
				_y+=3;
				if (_y == 9) {
					_y = 0;
					_x+=3;
				}
			}
			pids[r] = parent;
		}

		for (int c = 0; c < 27; c++) {
			int id = wait(&error);
			for (int i = 0; i < 27; i++) {
				if (id == pids[i]) {
					if (error/255 == 1) { d[i].invalidRow = 1; }
					if (error/255 == 2) { d[i].invalidCol = 1; }
					if (error/255 == 3) { d[i].invalidCel = 1; }
				}
			}
		}
		// END OF FORKS
	} else {
		// CHECK SUDOKU WITH THREADS
		for (int r = 0; r < 27; r++) {
			d[r].board = p, d[r].row = 0, d[r].col=0, d[r].x=0, d[r].y=0, d[r].invalidRow=0, d[r].invalidCol=0, d[r].invalidCel=0;
			if (r >= 0 && r < 9) {
				d[r].row = r;
				if (pthread_create( &children[r], NULL, checkrow, &d[r])) {
					printf("Error running row thread\n");
					exit(1);
				}
			} else if (r >= 9 && r < 18) {
				d[r].col = r-9;
				if (pthread_create( &children[r], NULL, checkcolumn, &d[r])) {
					printf("Error running column thread\n");
					exit(1);
				}
			} else {
				d[r].x = _x, d[r].y = _y;
				if (pthread_create( &children[r], NULL, checkcell, &d[r])) {
					printf("Error running cell thread\n");
					exit(1);
				}
				_y+=3;
				if (_y == 9) {
					_y = 0;
					_x+=3;
				}
			}
		}
		// Join all the threads
		for (int z = 0; z < 27; z++) {
			if (pthread_join( children[z], NULL)) {
				printf("Error joining thread\n");
				exit(1);
			}
		}
		// END OF THREADS
	}


	// Print the valid or invalid messages for row,column, or cell
	int invalid_board = 0;
	for (int a = 0; a < 27; a++) {
		if (d[a].invalidRow) {
			printf("Row %i doesn't have the required values.\n", d[a].row + 1);
			invalid_board = 1;
		}
		if (d[a].invalidCol) {
			printf("Column %i doens't have the required values.\n", d[a].col + 1);
			invalid_board = 1;
		}
		if (d[a].invalidCel) {
			const char * xpos;
			const char * ypos;
			if (d[a].x == 0) { xpos = "top"; }
			if (d[a].x == 3) { xpos = "middle"; }
			if (d[a].x == 6) { xpos = "bottom"; }
			if (d[a].y == 0) { ypos = "left"; }
			if (d[a].y == 3) { ypos = "middle"; }
			if (d[a].y == 6) { ypos = "right"; }
			printf("Cell %s %s doesn't have the required values.\n", xpos, ypos);
			invalid_board = 1;
		}
	}
	// Print valid or invalid message for entire board
	if (invalid_board) 
		printf("The input is not a valid sudoku.\n\n");
	else 
		printf("The input is a valid sudoku.\n\n");
	
	return 0;
}

