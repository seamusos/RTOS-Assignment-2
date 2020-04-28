/***********************************************************************************/
//***********************************************************************************
//            *************NOTE**************
//                 RTOS UTS Assignment 2
//Group Members:
//  Seamus O'Sullivan     12045959
//  Alastair Bate         12585826

//***********************************************************************************
/***********************************************************************************/

/*
  To compile prog_1 ensure that gcc is installed and run the following command:
  gcc prog_1.c -o prog_1 -lpthread -lrt

*/
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/time.h>

/* --- Structs --- */

typedef struct ThreadParams
{
  int pipeFile[2];
  sem_t sem_read, sem_justify, sem_write;
  char message[255];
  pthread_mutex_t lock;

  char read_file[100], write_file[100];

} ThreadParams;

/* --- Prototypes --- */

/* Initializes data and utilities used in thread params */
void initializeData(ThreadParams *params);

/* This thread reads data from data.txt and writes each line to a pipe */
void *ThreadA(void *params);

/* This thread reads data from pipe used in ThreadA and writes it to a shared variable */
void *ThreadB(void *params);

/* This thread reads from shared variable and outputs non-header text to src.txt */
void *ThreadC(void *params);

/* --- Main Code --- */
int main(int argc, char const *argv[])
{
  struct timeval t1;
  gettimeofday(&t1, NULL); // Start Timer
  int fd[2];               //File descriptor for creating a pipe

  int result;
  pthread_t tid1, tid2, tid3; //Thread ID
  pthread_attr_t attr;
  ThreadParams params;

  //Define file names?

  //Write to Params
  params.pipeFile = fd;
  params.read_file = "data.txt";
  params.write_file = "output.txt";

  // Initialization
  initializeData(&params);
  pthread_attr_init(&attr);

  // Create pipe
  result = pipe(fd);
  if (result < 0)
  {
    perror("pipe error");
    exit(1);
  }

  // Create Threads
  if (pthread_create(&tid1, &attr, ThreadA, (void *)(&params)) != 0)
  {
    perror("Error creating threads: ");
    exit(-1);
  }

  if (pthread_create(&tid2, &attr, ThreadB, (void *)(&params)) != 0)
  {
    perror("Error creating threads: ");
    exit(-1);
  }
  if (pthread_create(&tid3, &attr, ThreadC, (void *)(&params)) != 0)
  {
    perror("Error creating threads: ");
    exit(-1);
  }
  //TODO: add your code



  // Wait on threads to finish
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  pthread_join(tid3, NULL);
  //TODO: add your code

  return 0;
}

void initializeData(ThreadParams *params)
{
  // Initialize Sempahores
  sem_init(&(params->sem_read), 0, 1);
  sem_init(&(params->sem_justify), 0, 1);
  sem_init(&(params->sem_write), 0, 1);

  //TODO: add your code
  params->pipeFile = 

  return;
}

void *ThreadA(void *params)
{
  //TODO: add your code
  char line[255];

  //Open File

  FILE* fptr = fopen(params->read_file,"r");

  //Read One Line

  fgets(line, sizeof(line), fptr); //Reads one line

  //Write to Pipe
  write(params->fd[1], line, strlen(line) + 1);

  fclose(fptr);

  printf("ThreadA\n");
}

void *ThreadB(void *params)
{
  ThreadParams *parameters = params;

  while(!sem_wait(&sem_justify))
  {
    read()
    sem_post(&sem_write);
  }

  close()

  printf("ThreadB\n");
}

void *ThreadC(void *params)
{
  ThreadParams *parameters = params;
  FILE* writeFile = fopen(parameters->filename, "w");
  if(!writeFile)
  {
      perror("Invalid File");
      exit(0);
  }

    



  printf("ThreadC\n");
}
