# Project2
## Description
### 1. Get global PIDs
- Write a new system call `get_process_zero_session_group(unsigned int *, int)` to get the global PIDs of all processes in the same login session of process 0.
- If the number of processes in this login session is larger than the value of the second parameter, such as SIZE, you only need to store the PIDs of the first SIZE process.
- The return value is either 0 or a positive value.
  - 0 means that an error occurs.
  - A positive value means the number of processes in this login session.
- The first argument is used to store the result of this system call.
- Test program
```
#define  SIZE    100
  :
  unsigned int     results[SIZE];
  int              j,k;
  :
  k= get_process_zero_session_group(results, SIZE);
  
  if(k)
  {
    printf("What follows are the PIDs of the processes that are in the same login sesson of process 0\n");         
    for(j=0; j<k && j<SIZE; j++)
      printf("[%d] %u ",j,results[j]);
      
    printf("\n");
  }
  else
    printf("There is an error when executing this system call.\n");
```
### 2. Get local PIDs
- Write a system call `get_process_session_group(unsigned int *, int)` to get the local PIDs of all processes in the same login session as the caller process.
- If the number of processes in this login session is larger than the value of the second parameter, such as SIZE, you only need to store the PIDs of the first SIZE process.
- The return value is either 0 or a positive value.
  - 0 means that an error occurs.
  - A positive value means the number of processes in this login session.
- The first argument is used to store the result of this system call.
- Test Program:
```
#define  SIZE    100
  :
  unsigned int     results[SIZE];
  int              j,k;
  :
  k= get_process_session_group(results, SIZE);
  
  if(k)
  {
    printf("What follows are the local PIDs of the processes that are in the same login sesson this process.\n");         
    for(j=0; j<k && j<SIZE; j++)
      printf("[%d] %u ",j,results[j]);
      
    printf("\n");
  }
  else
    printf("There is an error when executing this system call.\n");
```
