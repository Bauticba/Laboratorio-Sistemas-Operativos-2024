#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void
pingpong(int rally)
{
  if (rally <= 0)
  {
    printf("ERROR: El numero de rounds tiene que ser mayor a 1\n");
    exit(1);
  }
  int sem_0 = find_sem(0);
  int sem_1 = find_sem(1);
  if (sem_0 == -1 || sem_1 == -1)
  {
    printf("ERROR: se produjo un error al abrir el semaforo\n");
    exit(1);
  }
  int pid = fork();
  if(pid < 0){
    printf("ERROR: se produjo un error al hacer fork\n");
    sem_close(1);
    exit(1);
  }
  for (int i = 0 ; i< rally; i++)
  {
    
    if(pid == 0){
      sem_down(sem_0);
      printf("        pong\n");
      sem_up(sem_1);
       
    
    } else {
      
      sem_down(sem_1);
      printf( "ping\n");
      sem_up(sem_0);
    
    }

  }
  if(pid == 0){
    sem_close(sem_0);
  }else{
    wait(0);
    sem_close(sem_1);
  }
 
  
}

int
main(int argc, char *argv[])
{
  int rally;

  if(argc <= 1){
    printf("ERROR: pingpong sin argumentos\n");
    exit(0);
  }
  rally = atoi(argv[1]);
  pingpong(rally);
  exit(0);
}
