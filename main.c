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
  gcc main.c -o main -lpthread -lrt

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
#define DEFAULT_READ_FILE  "data.txt"
#define DEFAULT_WRITE_FILE  ".txt"

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

/* This function allows the welcome messages to pbe printed and updated as necessary */
void Welcome();

/* --- Main Code --- */
int main(int argc, char const *argv[])
{

  int result;
  pthread_t tid1, tid2, tid3; //Thread ID
  pthread_attr_t attr;

  ThreadParams params;

  if(argc != 3)
  {
    printf("No User Defined inputs, resulting to default file names \n");
    strcpy(params.read_file, DEFAULT_READ_FILE);
    strcpy(params.write_file, DEFAULT_WRITE_FILE);
  }
  else
  {
    printf("User Defined Read File = %s \n", argv[1]);
    printf("User Defined Write File = %s \n", argv[2]);
    //Copy Text File Names from Arguements
    strcpy(params.read_file, argv[1]);
    strcpy(params.write_file, argv[2]);
  }



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
  Welcome();
  // Initialize Sempahores
  sem_init(&(params->sem_A_to_B), 0, 1);
  sem_init(&(params->sem_B_to_C), 0, 0);
  sem_init(&(params->sem_C_to_A), 0, 0);

  return;
}

void *ThreadA(void *params)
{
  
  //declare local variables
  ThreadParams *A_thread_params = (ThreadParams *)(params);
  char buffer[BUFFER_SIZE]; // temporary variable to store the text from the file

  //open read file
  printf("Opening your file to read\n");
  FILE *fptr; //File pointer for Read File
  
  if ((fptr = fopen(A_thread_params->read_file, "r")) == NULL) //open read file
  {
    perror("Error! opening file\n");
    // Program exits if file pointer returns NULL.
    exit(1);
  }
  printf("Reading from your file: %s\n",A_thread_params->read_file);

  while (fgets(buffer, sizeof(buffer), fptr) != NULL) //cycle through all lines of the file until complete
  {
    sem_wait(&(A_thread_params->sem_A_to_B)); //Thread must wait for semaphore to be sent/posted

    write(A_thread_params->pipeFile[1], buffer, BUFFER_SIZE); // the character line from the buffer to the pipe
    sem_post(&(A_thread_params->sem_B_to_C)); //Flag thread B semaphore to start
  }


  fclose(fptr); //Close File pointer
  printf("Thank you for using the clippy service, have a nice day!\n");
  exit(0); // Exit the program

}



void *ThreadB(void *params)
{
  //declare local variables
  ThreadParams *B_thread_params = (ThreadParams *)(params);

  while (!sem_wait(&(B_thread_params->sem_B_to_C))) //run when a line has been placed in the pipe
  {
    if (read(B_thread_params->pipeFile[0], B_thread_params->message, BUFFER_SIZE) < 0) // Read from the pipe
    {
      perror("Error reading from pipe\n");
      exit(1);
    }
    sem_post(&B_thread_params->sem_C_to_A); //signal to thread C to being
  }

}



void *ThreadC(void *params)
{
  //declare local variables
  ThreadParams *C_thread_params = (ThreadParams *)(params);
  // Open the file in which the content will be written to
  printf("Opening your write file\n");
  FILE* writeFile = fopen(C_thread_params->write_file, "w");
  if (!writeFile)
  {
    perror("Invalid File\n");
    exit(0);
  }
  
  printf("Your output file will now be printed\n\n");
  int content = 0; //Flag for end of header file
  while(!sem_wait(&(C_thread_params->sem_C_to_A)))
  {
    // Only write content if it's not apart of the header
    if (content)
    {
      printf("Printing line to file: %s\n",C_thread_params->message);
      fputs(C_thread_params->message, writeFile);
    }
    else if (strcmp(C_thread_params->message, END_OF_HEADER) == 0) // check if content is apart of the header
    {
      content = 1; //Flags end of header
    }

    sem_post(&C_thread_params->sem_A_to_B);
  }

  printf("Your file has been written with your data\n");
  fclose(writeFile); // Close FILE*
}

void Welcome()
{
  printf(" _________\n< welcome >\n _________\n \\\n  \\\n     __\n    /  \\\n    |  |\n    @  @\n    |  |\n    || |/\n    || ||\n    |\\_/|\n    \\___/\n\n\n");

  printf("Welcome to the RTOS file converter, my name is clippy and I are here to provide all your file copy needs\n\n\n");

  printf("This program will by default, take a file named in the input arguements and read it's content, it will then only print the content of the file to a user defined text file by default output.txt\n");

  printf("Please press the enter key to continue \n");
  while (getchar() != '\n');
}