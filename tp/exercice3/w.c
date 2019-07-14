#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <sys/sem.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

#define ITER 100

union semun {
   int  val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INF
                              (Linux-specific) */
};

int main() 
{ 
  // on genere la clé de la mémoire partagée
  key_t key = ftok("shmfile",65); 
  int i;
  //int a = 0; // shared data
   /* Déclarations */
  int A[50][50]; /* matrice donnée   */
  int B[50][50]; /* matrice donnée   */
  int C[50][50]; /* matrice résultat */
  int N, M, P;   /* dimensions des matrices */
  int I, J, K;   /* indices courants */
  struct sembuf sb = {0, 0, 0}; 
  union semun sem_arg;

  // clé pour le semaphore 
  key_t sem_key = ftok("semfile",75);
  // on demande au system de nour créer le semaphore
  int semid = semget(sem_key, 1, 0666);
    

  // shmget returns an identifier in shmid 
  int shmid = shmget(key,1024,0666); 
  // shmat to attach to shared memory 
  char *str = (char*) shmat(shmid,(void*)0,0); 


  for(i=0; i < ITER; i++){
     // tentative de verrou du semaphore
     sb.sem_op = -1;
     if(semop(semid, &sb,1) == -1){
       perror("sempo");
       exit(1);
     }
     // lecture de a
     C = atoi(str);
     printf("Proc w %d %d\n", getpid(), a);
     
	// Operation
     
    
	printf("*** Matrice A ***\n");
	 printf("Nombre de lignes de   A (max.50) : ");
	 scanf("%d", &N );
	 printf("Nombre de colonnes de A (max.50) : ");
	 scanf("%d", &M );
	 for (I=0; I<N; I++)
	    for (J=0; J<M; J++)
		{
		 printf("Elément[%d][%d] : ",I,J);
		 scanf("%d", &A[I][J]);
		}
	 printf("*** Matrice B ***\n");
	 printf("Nombre de lignes de   B : %d\n", M);
	 printf("Nombre de colonnes de B (max.50) : ");
	 scanf("%d", &P );
	 for (I=0; I<M; I++)
	    for (J=0; J<P; J++)
		{
		 printf("Elément[%d][%d] : ",I,J);
		 scanf("%d", &B[I][J]);
		}
	 /* Affichage des matrices */
	 printf("Matrice donnée A :\n");
	 for (I=0; I<N; I++)
	    {
	     for (J=0; J<M; J++)
		  printf("%7d", A[I][J]);
	     printf("\n");
	    }
	 printf("Matrice donnée B :\n");
	 for (I=0; I<M; I++)
	    {
	     for (J=0; J<P; J++)
		  printf("%7d", B[I][J]);
	     printf("\n");
	    }
	 /* Affectation du résultat de la multiplication à C */
	 for (I=0; I<N; I++)
	     for (J=0; J<P; J++)
		 {
		  C[I][J]=0;
		  for (K=0; K<M; K++)
		       C[I][J] += A[I][K]*B[K][J];
		 }
	  /* Edition du résultat */
	 printf("Matrice résultat C :\n");
	 for (I=0; I<N; I++)
	 {
	   for (J=0; J<P; J++)
	      sprintf(str,"%7d", C[I][J]);
	  printf("\n");
	 }

     sleep(C);
     // ecriture de a
     //sprintf(str, "%d", a);
     // déverrouillage du semaphore
     sb.sem_op = 1;
     if(semop(semid, &sb,1) == -1){
       perror("sempo");
       exit(1);
     } 
  }	
  //detach from shared memory 
  shmdt(str); 
  return 0; 
} 
