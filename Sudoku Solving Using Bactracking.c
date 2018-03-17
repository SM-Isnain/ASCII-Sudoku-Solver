const int DIM = 9;

static const int EMPTY = 0;
static const char BLANK = '-';
static const int MIN = 1;
static const int MAX = 9;

static const int SUCCESS = 0;
static const int ERROR = -1;
static const int ERASE_EMPTY_CELL = -2;
static const int ERASE_FILLED_CELL = -3;
static const int ERROR_NEXT_CELL = -4;

struct sudoku {
  int puzzle[DIM * DIM];
  int sol[DIM * DIM];
};

static bool cell_empty(const int board[], int row, int col);

// checkbox(s,r,c,n) takes in a cell with row number
// and column number r and c respectively and checks to see
// if the number n is allowed in the box containing the cell
// requires: s is not NULL
// time: O(1) as spos is a number between 0 and 80
// so each for-loop runs for a constant amount of time
bool checkbox(const struct sudoku* s, int r, int c, int n) {
  assert((1<=n)&&(n<=9));
  assert(s);
  int qnum = r/3;
  int cnum = c/3;
  int pos = (r*DIM) + c;
  int spos=((qnum*3)*DIM)+(cnum*3);
  for (int a=spos; a<=(spos+2); ++a) {
    if ((s->sol[a]==n)&&
        (a!=pos)) {
      return false;
    }
  }
  for (int b=(spos+9); b<=(spos+11); ++b) {
    if ((s->sol[b]==n)&&
        (b!=pos)) {
      return false;
    }
  }
  for (int c=(spos+18); c<=(spos+20); ++c) {
    if ((s->sol[c]==n)&&
        (c!=pos)) {
      return false;
    }
  }
  return true;
}

// inrow(s,r1,c1,n1) takes in a cell with row number and column
// number r1 and c1 respectively and returns true if n1 is
// allowed in the row in which the cell is contained and
// false otherwise
// requires: s is not NULL
// time: O(1) as srow is a number between 0 and 80
// so each for-loop runs for a constant amount of time
bool inrow(const struct sudoku* s, int r1, int c1, int n1) {
  assert(s);
  assert((1<=n1)&&(n1<=9));
  int pos = (r1*DIM)+c1;
  int srow= r1*DIM;
  for (int i=srow; i<=(srow+8); ++i) {
    if ((s->sol[i]==n1)&&
        (i!=pos)) {
      return false;
    }
  }
  return true;
}

// incol(s,r2,c2,n2) takes in a cell with row number and column
// number r2 and c2 respectively and returns true if n1 is
// allowed in the column in which the cell is contained and
// false otherwise
// requires: s is not NULL
// time: O(1) as c2 is a number between 0 and 80
// so each for-loop runs for a constant amount of time
bool incol(const struct sudoku* s, int r2, int c2, int n2) {
  assert(s);
  assert((MIN<=n2)&&(n2<=MAX));
  int pos = (r2*DIM)+c2;
  for (int i=c2; i<=(c2+72); i=i+9) {
    if ((s->sol[i]==n2)&&
        (i!=pos)) {
      return false;
    }
  }
  return true;
}

int fill_cell(struct sudoku *s, int row, int col, int num) {
  int numarr=(row*DIM)+col;
  if (!cell_empty(s->sol,row,col)) {
    return ERROR;
  }
  assert(s);
  if ((MIN<=num)&&(num<=MAX)) {
    if ((inrow(s,row,col,num))&&
        (incol(s,row,col,num))&&
        (checkbox(s,row,col,num))) {
      s->sol[numarr]=num;
      return SUCCESS;
    } else {
      return ERROR;
    }
  } else {
    return ERROR;
  }
}

// validc(s,r3,c3,n3) takes in the row number and column
// number corresponding to the position of a cell and returns
// true if n3 is allowed at that particular row and that column
// such that it does not violate any of the row and box constraints.
// Otherwise it returns false
// requires: s is not NULL
// time: O(1) as all operations in this function is 0(1) and
// each function call is to functions with O(1) runtime
bool validc(const struct sudoku* s, int r3, int c3, int n3) {
  assert(s);
  if ((MIN<=n3)&&(n3<=MAX)) {
    if ((inrow(s,r3,c3,n3))&&
        (incol(s,r3,c3,n3))&&
        (checkbox(s,r3,c3,n3))) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void choices_cell(const struct sudoku *s, int row, int col, 
                  int choices[], int *num_choices) {
  assert(s);
  assert(choices);
  assert(num_choices);
  *num_choices=0;
  for (int j=0; j<9; ++j) {
    choices[j]=0;
  }
  if (!cell_empty(s->sol,row,col)) {
    return;
  }
  int cin = 0;
  for (int i=1; i<10; ++i) {
    if (validc(s,row,col,i)) {
      choices[cin]=i;
      *num_choices+=1;
      ++cin;
    }
  }
}

bool solved_puzzle(const struct sudoku *s) {
  for (int i=0; i<81; ++i) {
    if ((s->sol[i])==EMPTY) {
      return false;
    }
  }
  for (int r=0; r<9; ++r) {
    for (int c=0; c<9; ++c) {
      int numarr = (r*9)+c;
      if (validc(s,r,c,s->sol[numarr])==false) {
        return false;
      }
    }
  }
  return true;
}


bool solve(struct sudoku *s) {
  assert(s);
  if (solved_puzzle(s)) {
    return true;
  }
  int pos=0;
  while ((pos<(DIM*DIM))&&(s->sol[pos]!=EMPTY)) {
    ++pos;
  }
  if (pos==(DIM*DIM)) {
    return false;
  }
  int pref[9]={0};
  int rnum=pos/9;
  int cnum=pos%9;
  int num=0;
  int* numpref=&num;
  choices_cell(s,rnum,cnum,pref,numpref);
  if (*numpref==0) {
    return false;
  }
  for (int i=0; (i<(*numpref)); ++i) {
    s->sol[pos]=pref[i];
    if (solve(s)) {
      return true;
    }
  }
  s->sol[pos]=EMPTY;
  return false;
}

// read_sudoku() reads in a Sudoku puzzle from input and returns a 
// pointer to a new sudoku structure
// effects: reads from input
// allocates memory (client must call sudoku_destroy)
struct sudoku *read_sudoku(void) {
  struct sudoku *s = malloc(sizeof(struct sudoku));
  char c = 0;
  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
      scanf(" %c", &c);
      if (c == BLANK) {
        s->puzzle[row * DIM + col] = 0;
      } else {
        s->puzzle[row * DIM + col] = c - '0';
      }
    }
  }

  // copy puzzle to solution
  reset_sol(s);

  return s;
}

// sudoku_destroy(s) frees all memory for s
// effects: s is no longer valid
void sudoku_destroy(struct sudoku *s) {
  assert(s);
  free(s);
}

// print_sol(s) prints the current solution
// effects: produces output
void print_sol(const struct sudoku *s) {
  assert(s);

  printf("\n");
  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
      int num = s->sol[row * DIM + col];
      if (num == EMPTY) {
        printf("%c", BLANK);
      } else {
        printf("%d", num);
      }  
    }
    printf("\n");
  }
  printf("\n");
}

// reset_sol(s) resets the solution to be the initial puzzle.
// effects: mutates *s
void reset_sol(struct sudoku *s) {
  assert(s);

  for (int row = 0; row < DIM; ++row) {
    for (int col = 0; col < DIM; ++col) {
      s->sol[row * DIM + col] = s->puzzle[row * DIM + col];
    }
  }  
}


// cell_empty(board, row, col) returns true
// if cell (row,col) is empty on board.
// requires: board is a valid sudoku puzzle.
static bool cell_empty(const int board[], int row, int col) {
  assert(board);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);

  return board[row * DIM + col] == EMPTY;
}

// erase_cell(s, row, col) tries to set the value of the cell (row,col)
// to be zero.  It returns 0 if successful and a negative integer otherwise.
// requires: row and col are valid indices
// effects:  mutates *s
int erase_cell(struct sudoku *s, int row, int col) {
  assert(s);
  assert(0 <= row && row <= DIM - 1);
  assert(0 <= col && col <= DIM - 1);

  if (cell_empty(s->sol, row, col)) {
    return ERASE_EMPTY_CELL;
  }
  if (!cell_empty(s->puzzle, row, col)) {
    return ERASE_FILLED_CELL;
  }
  s->sol[row * DIM + col] = EMPTY;
  return SUCCESS;
}

// next_cell(s) finds a cell that has only one possible value.  If such
// a cell exists, it mutates *row and *col to be the row and column 
// indices of the cell.  If no such cell exists, it returns a negative
// integer.
// requires: row and col are valid pointers.
// effects:  mutates *row and *col
int next_cell(const struct sudoku *s, int *row, int *col) {
  assert(s);
  assert(row);
  assert(col);

  int choices[DIM];
  int num_choices = 0;
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      if (!cell_empty(s->sol, i, j)) continue;
      choices_cell(s, i, j, choices, &num_choices);
      if (num_choices == 1) {
        *row = i;
        *col = j;
        return SUCCESS;
      }
    }
  }
  return ERROR_NEXT_CELL;
}

int main(void) {
  trace_off();
  printf("Welcome to ASCII sudoku!\n");

  struct sudoku *s = read_sudoku();
  print_sol(s);

  while (1) {
    // stop if puzzle is solved.
    if (solved_puzzle(s)) {
      printf("Congratulations!  You solved the puzzle!\n");
      break;
    }

    int cmd = read_symbol();
    if (cmd == lookup_symbol("bye")) {
      printf("Goodbye!\n");
      break;
    } else if (cmd == lookup_symbol("reset")) {
      reset_sol(s);
    } else if (cmd == lookup_symbol("print")) {
      print_sol(s);
    } else if (cmd == lookup_symbol("fill")) {
      int row = 0;
      int col = 0;
      int num = 0;
      scanf("%d", &row);
      scanf("%d", &col);
      scanf("%d", &num);

      int rv = fill_cell(s, row, col, num);
      if (rv < 0) {
        printf("Error putting %d in the cell (%d,%d)\n", num, row, col);
      } else {
        printf("Successfully put %d in the cell (%d, %d)\n", num, row, col);
      }
    } else if (cmd == lookup_symbol("erase")) {
      int row = 0;
      int col = 0;
      scanf("%d", &row);
      scanf("%d", &col);
      int rv = erase_cell(s, row, col);
      if (rv < 0) {
        printf("Error erasing cell (%d,%d)\n", row, col);
      } else { 
        printf("The cell (%d,%d) is now empty.\n", row, col);
      }
    } else if (cmd == lookup_symbol("choices")) {
      int row = 0;
      scanf("%d", &row);
      int col = 0;
      scanf("%d", &col);

      int choices[DIM];
      int num_choices = 0;
      choices_cell(s, row, col, choices, &num_choices);
      if (num_choices == 0) {
        printf("No possible value for cell (%d,%d)\n", row, col);
      } else {
        printf("Cell (%d,%d) could have values ", row, col);
        print_array(choices, num_choices);
      }
    } else if (cmd == lookup_symbol("hint")) {
      int row = -1;
      int col = -1;
      int rv = next_cell(s, &row, &col);
      if (rv < 0) {
        printf("Could not find a cell with only one possible value\n");
      } else {
        printf("Try filling cell (%d,%d)\n", row, col);
      }
    } else if (cmd == lookup_symbol("solve")) {
      if (solve(s)) {
        print_sol(s);
      } else {
        printf("Could not solve the puzzle using search and backtracking :(\n");
      }
    } else if (cmd == INVALID_SYMBOL) {
      printf("EOF Reached\n");
      break;
    } else {
      printf("Invalid Command: ");
      print_symbol(cmd);
      printf("\n");
      break;
    }
  }

  sudoku_destroy(s);
}
