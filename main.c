/***********************************************************************************/
//***********************************************************************************
//            *************NOTE**************
// This is a template for the subject of RTOS in University of Technology Sydney(UTS)
// Please complete the code based on the assignment requirement.

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

/* --- Global Definitions --- */

#define END_OF_HEADER "end_header\n"
#define BUFFER_SIZE 255

/* --- Structs --- */

typedef struct ThreadParams
{
  int pipeFile[2];
  sem_t sem_A_to_B, sem_B_to_C, sem_C_to_A;
  char message[255];
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

  int result;
  pthread_t tid1, tid2, tid3; //Thread ID
  pthread_attr_t attr;

  ThreadParams params;

  strcpy(params.read_file, "data.txt");
  strcpy(params.write_file, "output.txt");

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

  // Wait on threads to finish
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  pthread_join(tid3, NULL);

  return 0;
}

void initializeData(ThreadParams *params)
{
  // Initialize Sempahores
  sem_init(&(params->sem_A_to_B), 0, 1);
  sem_init(&(params->sem_B_to_C), 0, 0);
  sem_init(&(params->sem_C_to_A), 0, 0);

  return;
}

void *ThreadA(void *params)
{
  /* note: Since the data_stract is declared as pointer. the A_thread_params->message */
  ThreadParams *A_thread_params = (ThreadParams *)(params);
  char buffer[BUFFER_SIZE];

  FILE *fptr; //File pointer for Read File
  
  if ((fptr = fopen(A_thread_params->read_file, "r")) == NULL)
  {
    printf("Error! opening file");
    // Program exits if file pointer returns NULL.
    exit(1);
  }
  printf("reading from the file: \n");

  while (fgets(buffer, sizeof(buffer), fptr) != NULL)
  {
    sem_wait(&(A_thread_params->sem_A_to_B));

    write(A_thread_params->pipeFile[1], buffer, BUFFER_SIZE);
    sem_post(&(A_thread_params->sem_B_to_C)); //Flag thread B semaphore
  }

  fclose(fptr); //Close File pointer
  printf("ThreadA\n");
  exit(0);

}



void *ThreadB(void *params)
{

  ThreadParams *B_thread_params = (ThreadParams *)(params);
  while (!sem_wait(&(B_thread_params->sem_B_to_C)))
  {
    read(B_thread_params->pipeFile[0], B_thread_params->message, BUFFER_SIZE); // Read from the pipe
    printf("%s",B_thread_params->message);
    sem_post(&B_thread_params->sem_C_to_A);
  }
  printf("I've exited the loop");

  printf("ThreadB\n");
}



void *ThreadC(void *params)
{
  ThreadParams *C_thread_params = (ThreadParams *)(params);
  // Open the file in which the content will be written to
  FILE* writeFile = fopen(C_thread_params->write_file, "w");
  if (!writeFile)
  {
    perror("Invalid File");
    exit(0);
  }

  int content = 0; //Flag for end of header file
  while(!sem_wait(&(C_thread_params->sem_C_to_A)))
  {
    printf("runningC line\n");
    // Only write content if it's not apart of the header
    if (content)
    {
      printf("printing to file\n");
      fputs(C_thread_params->message, writeFile);
    }
    else if (strcmp(C_thread_params->message, END_OF_HEADER) == 0) // check if content is apart of the header
    {
      content = 1; //Flags end of header
    }

    sem_post(&C_thread_params->sem_A_to_B);
  }

  fclose(writeFile); // Close FILE*
  printf("ThreadC\n");
}
