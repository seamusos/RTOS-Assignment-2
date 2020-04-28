/***********************************************************************************/
//***********************************************************************************
//            *************NOTE**************
// This is a template for the subject of RTOS in University of Technology Sydney(UTS)
// Please complete the code based on the assignment requirement.

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
  sem_t sem_A_to_B, sem_B_to_A, sem_C_to_A;
  char message[255];

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

  int result;
  pthread_t tid1, tid2, tid3; //Thread ID
  pthread_attr_t attr;

  ThreadParams params;

  // Initialization
  initializeData(&params);
  pthread_attr_init(&attr);

  // Create pipe
  result = pipe(params.pipeFile);
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
  sem_init(&(params->sem_A_to_B), 0, 1);

  //TODO: add your code

  return;
}

void *ThreadA(void *params)
{
  //TODO: add your code

  /* note: Since the data_stract is declared as pointer. the A_thread_params->message */
  ThreadParams *A_thread_params = (ThreadParams *)(params);

  FILE *fptr;
  char file_name[100] = "data.txt";
  int sig;
  char check[12] = "end_header\n";

  if ((fptr = fopen(file_name, "r")) == NULL)
  {
    printf("Error! opening file");
    // Program exits if file pointer returns NULL.
    exit(1);
  }

  printf("reading from the file: \n");

  sig = 0;

  while(fgets(A_thread_params->message, sizeof(A_thread_params->message), fptr) != NULL)
  {

    if(sig == 1)
    {
      write(A_thread_params->pipeFile[1], A_thread_params->message, 1);
    }

    //Read until end of header
    if((sig == 0) && strcmp(A_thread_params->message, check) == 0)
    {
      sig = 1; //Flags end of header
    }
  }

  fclose(fptr);

  printf("ThreadA\n");
}

void *ThreadB(void *params)
{
  //TODO: add your code

  ThreadParams *B_thread_params = (ThreadParams *)(params);
  printf("ThreadB\n");
}

void *ThreadC(void *params)
{
  //TODO: add your code
  ThreadParams *C_thread_params = (ThreadParams *)(params);

  printf("ThreadC\n");
}
