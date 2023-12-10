#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>


#define ROWS 20
#define COLS 11
#define TRUE 1
#define FALSE 0

char Table[ROWS][COLS] = {0};
int score = 0;
char GameOn = TRUE;
double timer = 500000; // half second
 

//---------------------------------------------------------------
typedef struct
{
  char **array;
  int width, row, col;
} Shape;
Shape current;

const Shape ShapesArray[7] = {
    {(char *[]){(char[]){0, 1, 1}, (char[]){1, 1, 0}, (char[]){0, 0, 0}}, 3},                               // S_shape
    {(char *[]){(char[]){1, 1, 0}, (char[]){0, 1, 1}, (char[]){0, 0, 0}}, 3},                               // Z_shape
    {(char *[]){(char[]){0, 1, 0}, (char[]){1, 1, 1}, (char[]){0, 0, 0}}, 3},                               // T_shape
    {(char *[]){(char[]){0, 0, 1}, (char[]){1, 1, 1}, (char[]){0, 0, 0}}, 3},                               // L_shape
    {(char *[]){(char[]){1, 0, 0}, (char[]){1, 1, 1}, (char[]){0, 0, 0}}, 3},                               // ML_shape
    {(char *[]){(char[]){1, 1}, (char[]){1, 1}}, 2},                                                        // SQ_shape
    {(char *[]){(char[]){0, 0, 0, 0}, (char[]){1, 1, 1, 1}, (char[]){0, 0, 0, 0}, (char[]){0, 0, 0, 0}}, 4} // R_shape
};

Shape CopyShape(Shape shape)
{
  Shape new_shape = shape;
  char **copyshape = shape.array;
  new_shape.array = (char **)malloc(new_shape.width * sizeof(char *));
  int i, j;
  for (i = 0; i < new_shape.width; i++)
  {
    new_shape.array[i] = (char *)malloc(new_shape.width * sizeof(char));
    for (j = 0; j < new_shape.width; j++)
    {
      new_shape.array[i][j] = copyshape[i][j];
    }
  }
  return new_shape;
}

void DeleteShape(Shape shape)
{
  int i;
  for (i = 0; i < shape.width; i++)
  {
    free(shape.array[i]);
  }
  free(shape.array);
}
int CheckPosition(Shape shape)
{ // Check the position of the copied shape
  char **array = shape.array;
  int i, j;

  // Check for out-of-bounds positions
  for (i = 0; i < shape.width; i++)
  {
    for (j = 0; j < shape.width; j++)
    {
      if (shape.col + j < 0 || shape.col + j >= COLS ||
          shape.row + i < 0 || shape.row + i >= ROWS)
      {
        if (array[i][j])
        { // But is it just a phantom?
          return FALSE;
        }
      }
    }
  }

  // Check if there are any collisions with the existing table
  for (i = 0; i < shape.width; i++)
  {
    for (j = 0; j < shape.width; j++)
    {
      if (Table[shape.row + i][shape.col + j] && array[i][j])
      {
        return FALSE;
      }
    }
  }

  // If no collisions, return TRUE
  return TRUE;
}
void GetNewShape()
{ // returns random shape
  Shape new_shape = CopyShape(ShapesArray[rand() % 7]);

  new_shape.col = rand() % (COLS - new_shape.width + 1);
  new_shape.row = 0;
  DeleteShape(current);
  current = new_shape;
  if (!CheckPosition(current))
  {
    GameOn = FALSE;
  }
}

void RotateShape(Shape shape)
{ // rotates clockwise
  Shape temp = CopyShape(shape);
  int i, j, k, width;
  width = shape.width;
  for (i = 0; i < width; i++)
  {
    for (j = 0, k = width - 1; j < width; j++, k--)
    {
      shape.array[i][j] = temp.array[k][i];
    }
  }
  DeleteShape(temp);
}

void WriteToTable()
{
  int i, j;
  for (i = 0; i < current.width; i++)
  {
    for (j = 0; j < current.width; j++)
    {
      if (current.array[i][j])
        Table[current.row + i][current.col + j] = current.array[i][j];
    }
  }
}

void Halleluyah_Baby()
{ // checks lines
  int i, j, sum, count = 0;
  for (i = 0; i < ROWS; i++)
  {
    sum = 0;
    for (j = 0; j < COLS; j++)
    {
      sum += Table[i][j];
    }
    if (sum == COLS)
    {
      count++;
      int l, k;
      for (k = i; k >= 1; k--)
        for (l = 0; l < COLS; l++)
          Table[k][l] = Table[k - 1][l];
      for (l = 0; l < COLS; l++)
        Table[k][l] = 0;
    }
  }
  timer -= 1000;
  score += 100 * count;
}
void PrintTable()
{
  init_pair(2, COLOR_CYAN, COLOR_CYAN);
  char Buffer[ROWS][COLS] = {}; // Initialize Buffer array with all '0'
  int i, j;

  // Copy the current shape's data into the Buffer array
  for (i = 0; i < current.width; i++)
  {
    for (j = 0; j < current.width; j++)
    {
      if (current.array[i][j])
      {
        Buffer[current.row + i][current.col + j] = current.array[i][j];
      }
    }
  }

  // Clear the screen using the `clear()` function
  clear();
  refresh(); // Refresh the screen before printing the table

  // Iterate through the rows and columns of the table and print the corresponding elements from the Buffer array
  for (i = 0; i < ROWS; i++)
  {
    for (j = 0; j < COLS; j++)
    {
      if (Table[i][j] + Buffer[i][j])
      {                        // If the cell is occupied
        attron(COLOR_PAIR(2)); // Set the color to red for the falling shapes
        printw("O");
        attroff(COLOR_PAIR(2)); // Reset the color to the default
      }
      else
      {
        attron(COLOR_PAIR(5)); // Set the color to white for the field
        printw(".");
        attroff(COLOR_PAIR(5)); // Reset the color to the default
      }
      printw(" ");
    }
    printw("\n"); // Print a newline character after each row
  }

  // Print the current score
  printw("Score: %d\n\n", score);
  refresh(); // Refresh the screen after printing the score
}

void ManipulateCurrent(int action)
{
  Shape temp = CopyShape(current); // Create a temporary shape copy

  switch (action)
  {
  case 's':     // Move down
    temp.row++; // Move the temporary shape down
    if (CheckPosition(temp))
    {                // Check if the position is valid
      current.row++; // Move the current shape down
    }
    else
    {                    // If the position is invalid
      WriteToTable();    // Write the temporary shape to the table
      Halleluyah_Baby(); // Check full lines after putting down the shape
      GetNewShape();     // Get a new shape
    }
    break;

  case 'd':     // Move right
    temp.col++; // Move the temporary shape right
    if (CheckPosition(temp))
    {                // Check if the position is valid
      current.col++; // Move the current shape right
    }
    break;

  case 'a':     // Move left
    temp.col--; // Move the temporary shape left
    if (CheckPosition(temp))
    {                // Check if the position is valid
      current.col--; // Move the current shape left
    }
    break;

  case 'w':            // Rotate left
    RotateShape(temp); // Rotate the temporary shape
    if (CheckPosition(temp))
    {                       // Check if the rotated position is valid
      RotateShape(current); // Rotate the current shape
    }
    break;
  }

  DeleteShape(temp); // Delete the temporary shape
  PrintTable();      // Re-print the table with the updated positions
}

//--------------------------------------------------------

int main(int argc, char *argv[])
{
  char startGame;
  scanf("%c", &startGame);

  char line[1000];
  int c = 0;
  // int points = 0;
  struct timeval before, after;
  FILE *file = fopen("score.txt", "r+");
  initscr();
  noecho();
  cbreak();

  if (has_colors() == FALSE)
  {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }

  start_color();                         // Инициализация цветов
  init_pair(1, COLOR_CYAN, COLOR_BLACK); // Инициализация пары цветов
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(4, COLOR_RED, COLOR_BLACK);

  printw("******************************************************************************\n");
  printw("                                            \n");
  printw("                                            \n");
  attron(COLOR_PAIR(1)); // Включить голубой цвет
  printw("                  _____   ____   _____   ____    ___   ____ \n");
  printw("                 (_   _) (  __) (_   _) (   ,)  (   ) /  __)\n");
  printw("                   ) (    ) __)   ) (    )   \\   ) (  \\__  \\ \n");
  printw("                  (___)  (____)  (___)  (__)\__) (___) (____/ \n");
  attroff(COLOR_PAIR(1)); // Выключить голубой цвет
  printw("                                            \n");
  printw("                                            \n");
  printw("                               Press ");
  attron(COLOR_PAIR(1)); // Включить голубой цвет
  printw("'E' ");
  attroff(COLOR_PAIR(1)); // Выключить голубой цвет
  printw("to ");
  attron(COLOR_PAIR(1));
  printw("start             \n");
  attroff(COLOR_PAIR(1));
  printw("                                            \n");
  printw("                                   Info ");
  attron(COLOR_PAIR(1)); // Включить голубой цвет
  printw("'I'");
  attroff(COLOR_PAIR(1)); // Выключить голубой цве
  printw("                                            \n");
  printw("                                            \n");
  printw("                                     EXIT          \n");
  printw("                                            \n");
  printw("                                            \n");
  printw("                                            \n");
  printw("******************************************************************************\n");
  int hc = getch();
  if (hc == 'I' || hc == 'i')
  {
    clear();
    printw("******************************************************************************\n");
    printw("                                            \n");
    printw("                                            \n");
    attron(COLOR_PAIR(1));
    printw("                              WELCOME TO TETRIS GAME\n");
    printw("                               by Anastasiia Rybalka                        \n");
    attroff(COLOR_PAIR(1));
    printw("                                            \n");
    printw("            In Tetris, players complete lines by moving differently\n");
    printw("              shaped pieces, which descend onto the playing field\n");
    printw("             completed lines disappear and grant the player points \n");
    printw("           the player can proceed to fill the vacated spaces.Game ends \n");
    printw("           when the uncleared lines reach the top of the playing field\n");
    printw("                                            \n");
    attron(COLOR_PAIR(1));
    printw("                                GAME CONTROL RULES                    \n");
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(1));
    printw("                                   W");
    attroff(COLOR_PAIR(1));
    printw(" - rotate \n");
    attron(COLOR_PAIR(1));
    printw("                                   S");
    attroff(COLOR_PAIR(1));
    printw(" - sprint              \n");
    attron(COLOR_PAIR(1));
    printw("                                   A");
    attroff(COLOR_PAIR(1));
    printw(" - step on left          \n");
    attron(COLOR_PAIR(1));
    printw("                                   D");
    attroff(COLOR_PAIR(1));
    printw(" - step on right       \n");
    printw("                                            \n");
    printw("                                                              \n");
    printw("                                 Press ");
    attron(COLOR_PAIR(1)); // Включить голубой цвет
    printw("'E' ");
    attroff(COLOR_PAIR(1)); // Выключить голубой цвет
    printw("to ");
    attron(COLOR_PAIR(1));
    printw("start             \n");
    attroff(COLOR_PAIR(1));
    printw("******************************************************************************\n");
  }
  int ch = getch(); // Ждем ввода от пользователя
  if (ch == 'E' || ch == 'e')
  {
    clear();
    printw("******************************************************************************\n");
    printw("                                            \n");
    printw("                                            \n");
    attron(COLOR_PAIR(1)); // Включить голубой цвет
    printw("                  _____   ____   _____   ____    ___   ____ \n");
    printw("                 (_   _) (  __) (_   _) (   ,)  (   ) /  __)\n");
    printw("                   ) (    ) __)   ) (    )   \\   ) (  \\__  \\ \n");
    printw("                  (___)  (____)  (___)  (__)\__) (___) (____/ \n");
    attroff(COLOR_PAIR(1)); // Выключить голубой цвет
    printw("                                            \n");
    attron(COLOR_PAIR(1));
    printw("                         Choose the game difficulty:             \n");
    attroff(COLOR_PAIR(1));
    printw("                                            \n");
    printw("                                 1. Easy                \n");
    printw("                                            \n");
    printw("                                 2. Medium              \n");
    printw("                                            \n");
    printw("                                 3. HARD                \n");
    printw("                                            \n");
    printw("                                            \n");
    printw("                                            \n");
    printw("******************************************************************************\n");
    refresh();
    int hc = getch();
    if (hc == 'B' || hc == 'b')
    {
      clear();

      printw("******************************************************************************\n");
      printw("                                            \n");
      printw("                                            \n");
      attron(COLOR_PAIR(1)); // Включить голубой цвет
      printw("                  _____   ____   _____   ____    ___   ____ \n");
      printw("                 (_   _) (  __) (_   _) (   ,)  (   ) /  __)\n");
      printw("                   ) (    ) __)   ) (    )   \\   ) (  \\__  \\ \n");
      printw("                  (___)  (____)  (___)  (__)\__) (___) (____/ \n");
      attroff(COLOR_PAIR(1)); // Выключить голубой цвет
      printw("                                            \n");
      printw("                                            \n");
      printw("                               Press ");
      attron(COLOR_PAIR(1)); // Включить голубой цвет
      printw("'E' ");
      attroff(COLOR_PAIR(1)); // Выключить голубой цвет
      printw("to ");
      attron(COLOR_PAIR(1));
      printw("start             \n");
      attroff(COLOR_PAIR(1));
      printw("                                            \n");
      printw("                                   Info ");
      attron(COLOR_PAIR(1)); // Включить голубой цвет
      printw("'I'");
      attroff(COLOR_PAIR(1)); // Выключить голубой цве
      printw("                                            \n");
      printw("                                            \n");
      printw("                                     EXIT          \n");
      printw("                                            \n");
      printw("                                            \n");
      printw("                                            \n");
      printw("******************************************************************************\n");
      int ch = getch(); // Ждем ввода от пользователя
      if (ch == 'E' || ch == 'e')
      {
        printw("******************************************************************************\n");
        printw("                                            \n");
        printw("                                            \n");
        attron(COLOR_PAIR(1)); // Включить голубой цвет
        printw("                  _____   ____   _____   ____    ___   ____ \n");
        printw("                 (_   _) (  __) (_   _) (   ,)  (   ) /  __)\n");
        printw("                   ) (    ) __)   ) (    )   \\   ) (  \\__  \\ \n");
        printw("                  (___)  (____)  (___)  (__)\__) (___) (____/ \n");
        attroff(COLOR_PAIR(1)); // Выключить голубой цвет
        printw("                                            \n");
        attron(COLOR_PAIR(1));
        printw("                         Choose the game difficulty:             \n");
        attroff(COLOR_PAIR(1));
        printw("                                            \n");
        printw("                                 1. Easy                \n");
        printw("                                            \n");
        printw("                                 2. Medium              \n");
        printw("                                            \n");
        printw("                                 3. HARD                \n");

        printw("                                            \n");
        printw("                                            \n");
        printw("                                            \n");
        printw("******************************************************************************\n");
        refresh();
        clear();
      }
    }
    while (1)
    {
      ch = getch(); // Получить выбор пользователя
      if (ch == '1' || ch == '2' || ch == '3')
      {
        int difficulty = getch(); // Получить выбор пользователя
        switch (difficulty)
        {
        case '1':
          getch();
          srand(time(0));
          score = 0;
          gettimeofday(&before, NULL);
          nodelay(stdscr, TRUE);
          GetNewShape();
          PrintTable();
          while (GameOn)
          {
            if ((c = getch()) != ERR)
            {
              ManipulateCurrent(c);
            }
            gettimeofday(&after, NULL);
            if (((double)after.tv_sec * 1000000 + (double)after.tv_usec) - ((double)before.tv_sec * 1000000 + (double)before.tv_usec) > timer)
            { // time difference in microsec accuracy
              before = after;
              ManipulateCurrent('s');
            }
          }
          printw("\nGame over\n");
          DeleteShape(current);
          break;
        case '2':
          getch();
          srand(time(0));
          score = 0;
          timer = 200000;
          gettimeofday(&before, NULL);
          nodelay(stdscr, TRUE);
          GetNewShape();
          PrintTable();
          while (GameOn)
          {
            if ((c = getch()) != ERR)
            {
              ManipulateCurrent(c);
            }
            gettimeofday(&after, NULL);
            if (((double)after.tv_sec * 1000000 + (double)after.tv_usec) - ((double)before.tv_sec * 1000000 + (double)before.tv_usec) > timer)
            { // time difference in microsec accuracy
              before = after;
              ManipulateCurrent('s');
            }
          }
          printw("\nGame over\n");
          DeleteShape(current);
        case '3':
          getch();
          srand(time(0));
          score = 0;
          timer = 75000;
          gettimeofday(&before, NULL);
          nodelay(stdscr, TRUE);
          GetNewShape();
          PrintTable();
          while (GameOn)
          {
            if ((c = getch()) != ERR)
            {
              ManipulateCurrent(c);
            }
            gettimeofday(&after, NULL);
            if (((double)after.tv_sec * 1000000 + (double)after.tv_usec) - ((double)before.tv_sec * 1000000 + (double)before.tv_usec) > timer)
            { // time difference in microsec accuracy
              before = after;
              ManipulateCurrent('s');
            }
          }
          printw("\nGame over\n");
          DeleteShape(current);

        default:
          printw("Let's GOOOO!");
          break;
        }
      }
    }
  }
  return 0;
}