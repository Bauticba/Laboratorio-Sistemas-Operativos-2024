#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

#define NSEM 10  // numero de semaforos
#define TRUE 1 //para saber si esta abierto
#define FALSE 0

struct semaphore{
 struct spinlock lock; //lock para evitar condiciones de carrera
 int count; // contador del semáforo
 int open;
};

// arreglo de semaforos
struct semaphore sem[NSEM];

// inicializa el arreglo de semaforos
void 
sem_init ()
{
  for (int i = 0 ; i < NSEM ; i++)
  {
    initlock(&sem[i].lock, "sem_lock");
    sem[i].count = 0;
    sem[i].open = FALSE;
  }
}

//encuentra un semaforo libre y lo abre
int find_sem(int value)
{
  for (unsigned int i = 0; i < NSEM; i++){
    acquire(&sem[i].lock);
    if(sem[i].open == FALSE){
      sem[i].count = value;
      sem[i].open = TRUE;
      release(&sem[i].lock);
      return i;
    }
    release(&sem[i].lock);
  }
  printf("ERROR: no se pudo encontrar un semaforo disponible\n");
  return -1;
}

// Abre y/o inicializa el semáforo “sem” con un valor arbitrario “value”.
int 
sem_open(int sem_id, int value) 
{

  if (sem_id >= NSEM || sem_id < 0)
  {
    printf("ERROR: id fuera del rango\n");
    return 0;
  }

  acquire(&sem[sem_id].lock); 

  sem[sem_id].count = value;
  sem[sem_id].open = TRUE;

  release(&sem[sem_id].lock);

  return 1;
}

int sem_close(int sem_id)
{
  if (sem_id < 0 || sem_id>=NSEM)
  {
    printf("ERROR: id fuera del rango\n");
    return 0;
  }

  acquire(&sem[sem_id].lock); //bloqueo el sem asi otros procesos no lo modifican al mismo tiempo 
  sem[sem_id].open = FALSE; //lo marco como cerrado, ya no esta disponible
  release(&sem[sem_id].lock); //dejo libre a lock asi otros procesos pueden acceder

  return 1; //devuelvo el entero q nombra el semaforo asi lo pueden volver a utilizar  
}

int sem_up(int sem_id)
{
  if (sem_id < 0 || sem_id >= NSEM) 
  {
    printf("ERROR: id fuera del rango\n");
    return 0;  // Error: id fuera del rango.
  }
  if(sem[sem_id].open == FALSE){
    printf("ERROR: el semaforo con la id:%d esta cerrado\n", sem_id);
    return 0;
  }
  acquire(&sem[sem_id].lock);

  if (sem[sem_id].count == 0)
  {
    wakeup(&sem[sem_id]);
    sem[sem_id].count += 1;
  }
  else
  {
    sem[sem_id].count += 1;
  }
  
  release(&sem[sem_id].lock);
  return 1;
}

int sem_down(int sem_id){
  if (sem_id < 0 || sem_id >= NSEM) 
  {
  printf("ERROR: id fuera del rango\n");
  return 0;  // Error: id fuera del rango.
  }
  if(sem[sem_id].open == FALSE){
    printf("ERROR: el semaforo con la id:%d esta cerrado\n", sem_id);
    return 0;
  }

  acquire(&sem[sem_id].lock);

  while(sem[sem_id].count == 0){
    sleep(&sem[sem_id], &sem[sem_id].lock);
  }

  sem[sem_id].count--;
  release(&sem[sem_id].lock);

  return 1;
}