#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/* mutex declarations, a global variable */
pthread_mutex_t mutex_lock; /* protect the global variable waiting_student */

/* semaphore declarations, global variables */
sem_t students_sem; /* ta waits for a student to show up, student notifies ta his/her arrival*/
sem_t ta_sem; /* student waits for ta to help, ta notifies student he/she is ready to help*/

/* the number of waiting students, a global variable */
int waiting_students;

/* the maximum time (in seconds) to sleep */
#define MAX_SLEEP_TIME 3

/* number of potential students */
#define NUM_OF_STUDENTS 4

#define NUM_OF_HELPS 2

/* number of available seats */
#define NUM_OF_SEATS 2

typedef struct
{ int i; }
parameters;

/*
 * For this assignment, I used dispatch semaphores first because I use Mac.
 * I tested it, took the screenshots.
 * And I changed the codes into unnamed semaphores, borrowed a computer which
 *   has ubuntu in it, tested the program again. It ran fine, but I didn't took
 *   screenshots in that computer, because it was borrowed to me.
 */

int main(void) {
  pthread_t ta;
  pthread_t students[NUM_OF_STUDENTS];
  pthread_attr_t attr;
  void *sleepingTA(void *param);
  void *student(void *param);

  pthread_attr_init(&attr);
  sem_init(&students_sem, 0, 0);
  sem_init(&ta_sem, 0, 1);
  pthread_mutex_init(&mutex_lock, NULL);
  pthread_create(&ta, &attr, sleepingTA, NULL);
  for (int i = 0; i < NUM_OF_STUDENTS; i++) {
    parameters *param = (parameters *) malloc(sizeof(parameters));
    param->i = i;
    pthread_create(&students[i], &attr, student, param);
  }
  for (int i = 0; i < NUM_OF_STUDENTS; i++) {
    pthread_join(students[i], NULL);
  }
  pthread_cancel(ta);
  sem_destroy(&students_sem);
  sem_destroy(&ta_sem);
  pthread_mutex_destroy(&mutex_lock);

  return 0;
}

void *sleepingTA(void *param) {
  /* It won't terminate by itself. */
  int true = 1;
  while (true == 1) {
  sem_wait(&students_sem); /* Waiting for a student comes. */
  pthread_mutex_lock(&mutex_lock);
  waiting_students--;
  pthread_mutex_unlock(&mutex_lock);
  unsigned int seed = 100;
  int time = (rand_r(&seed) % MAX_SLEEP_TIME) + 1;
  printf("Helping a student for %i seconds, # of waiting students = %i\n", time, waiting_students);
  sleep(time);
  sem_post(&ta_sem); /* Notify the students that TA is available now. */
  }
  return NULL;
}

void *student(void *param) {
  int num = ((parameters *)param)->i;
  int help_count = 0;
  unsigned int seed = 1000;
  int time = (rand_r(&seed) % MAX_SLEEP_TIME) + 1;
  printf("        Student %i programming for %i seconds\n", num, time);
  sleep(time);
  while (help_count < NUM_OF_HELPS) {
    pthread_mutex_lock(&mutex_lock);
    if (waiting_students < NUM_OF_SEATS) {
      waiting_students++;
      printf("                Student %i takes a seat, # of waiting students = %i\n", num, waiting_students);
      pthread_mutex_unlock(&mutex_lock);
      sem_wait(&ta_sem); /* Waiting for the TA to be available. */
      sem_post(&students_sem); /* Notify the TA that a student comes. */
      printf("Student %i receiving help\n", num);
      help_count++;
      time = (rand_r(&seed) % MAX_SLEEP_TIME) + 1;
      printf("        Student %i programming for %i seconds\n", num, time);
      sleep(time);
    }
    else {
      pthread_mutex_unlock(&mutex_lock);
      printf("                        Student %i will try later\n", num);
      time = (rand_r(&seed) % MAX_SLEEP_TIME) + 1;
      printf("        Student %i programming for %i seconds\n", num, time);
      sleep(time);
    }
  }
  pthread_exit(0);
}
