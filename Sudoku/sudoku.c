
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* structure for passing data to threads */
typedef struct
{ int row;
  int column; }
parameters;
/* The hard-coded sudoku grid. */
int grid[9][9] =
{ {6,5,3,1,2,8,7,9,4},
  {1,7,4,3,5,9,6,8,2},
  {9,2,8,4,6,7,5,3,1},
  {2,8,6,5,1,4,3,7,9},
  {3,9,1,7,8,2,4,5,6},
  {5,4,7,6,9,3,2,1,8},
  {8,6,5,2,3,1,9,4,7},
  {4,1,2,9,7,5,8,6,3},
  {7,3,9,8,4,6,1,2,5}};
//pthread_t row_tid;
//pthread_t col_tid;
pthread_t workers[11];
pthread_attr_t attr;
void *row(void *param);
void *col(void *param);
void *subgrid(void *param);
/* The result shared by all threads. */
int valid;
int sum;
static int SUM = 45;

int main(void)
{
  valid = 1; /* Assumed valid. Change it if not valid. */
  pthread_attr_init(&attr);
  /* Check each sub-grid. */
  int tnum = 0;
  for (int i = 0; i < 9; i = i + 3)
  {
    for (int j = 0; j < 9; j = j + 3)
    {
      parameters *data = (parameters *) malloc(sizeof(parameters));
      data->row = i;
      data->column = j;
      pthread_create(&workers[tnum], &attr, subgrid, data);
      free(data);
      tnum++;
    }
  }
  /* Check rows and columns. */
  pthread_create(&workers[9], &attr, row, NULL);
  pthread_create(&workers[10], &attr, col, NULL);
  /* Join the threads. */
  for (int i = 0; i < 11; i++) pthread_join(workers[i], NULL);
  /* Print the result. */
  for (int i = 0; i < 9; i++)
  {
    for (int j = 0; j < 9; j++)
    {
      printf("%d ", grid[i][j]);
    }
    printf("\n");
  }
  if (valid == 1) printf("The inputs are valid.\n");
  else printf("The inputs are not valid. Please check it again.\n");

  return 0;
}

/* Function for checking rows. */
void *row(void *param)
{
  for (int i = 0; i < 9; i++)
  {
    sum = 0;
    for (int j = 0; j < 9; j++)
    {
      int current = grid[i][j];
      sum = sum + current;
    }
    if (sum != SUM) valid = 0;
  }
  pthread_exit(0);
}

/* Function for checking columns. */
void *col(void *param)
{
  for (int j = 0; j < 9; j++)
  {
    sum = 0;
    for (int i = 0; i < 9; i++)
    {
      int current = grid[i][j];
      sum = sum + current;
    }
    if (sum != SUM) valid = 0;
  }
  pthread_exit(0);
}

/* Function for checking sub-grids. */
void *subgrid(void *param)
{
  int startrow = ((parameters *)param)->row;
  int startcol = ((parameters *)param)->column;
  sum = 0;
  for (int i = startrow; i < startrow + 3; i++)
  {
    for (int j = startcol; j < startcol + 3; j++)
    {
      int current = grid[i][j];
      sum = sum + current;
    }
  }
  if (sum != SUM) valid = 0;
  pthread_exit(0);
}
