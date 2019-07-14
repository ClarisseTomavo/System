#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h> 
#include <sys/shm.h>  
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>


union semun {
   int  val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INF
                              (Linux-specific) */
};


int main(int argc, char *argv[])
{ 
    pid_t pid1, pid2, pid3;
    int status; 
    union semun sem_arg; // paramètre pour la configuration du semaphore
    // Argument du premier processus
    char *argp1[] = {"w", NULL, NULL};
    // clé pour le semaphore 
    key_t sem_key = ftok("semfile",75);
    // on demande au system de nous créer le semaphore
    int semid = semget(sem_key, 1, 0666|IPC_CREAT);
    int N=10, M=10, P=10;   /* dimensions des matrices */
    int I=0, J=0, K=0;   /* indices courants */

    // initialisation de la valeur du semaphore à 1
    sem_arg.val = 1;
    if(semctl(semid, 0, SETVAL, sem_arg)==-1){
       perror("semctl");
       exit(1);
    }
    
    
    key_t key = ftok("shmfile",65);   //cle pour la memoire partagée
    int A[10][10] = {0}; 
    int B[10][10] = {0}; 
    int C[10][10] = {0}; 

    //Creation de la memoire partagée 
    int shmid = shmget(key,1024,0666|IPC_CREAT); 
    // attachement de str à la mémoire partagée
    char *str = (char*) shmat(shmid,(void*)0,0); 
    
    //création du premier processus (pour lancer le process w)
    pid1 = fork();
    if(pid1 < 0){
          perror("Erreur de création du processus\n");
          exit(EXIT_FAILURE);
    }

    if(pid1 == 0){
        //execv("./w", argp1);
	printf("Fils\n");
	exit(1);
    }
    else
    {
        sleep(5);
        
		        pid1 = wait(&status);
		        printf("Status de l'arret du fils %d %d\n", status, pid1);
		        // on lit la dernière valeur de la variable partagée
			printf("*** Matrice A ***\n");
			 printf("Nombre de lignes de la matrice A, au plus 50 : ");
			 scanf("%d", &N );
			 printf("Nombre de colonnes de la matrice A, auplus 50 ");
			 scanf("%d", &M );
			 for (I=0; I<N; I++)
			    for (J=0; J<M; J++)
				{
				 printf("A[%d][%d] : ",I,J);
				 scanf("%d", &A[I][J]);
				}
			 printf(" Voici la deuxieme Matrice notée B \n");
			 printf("Nombre de lignes de la matrice B, au plus 50 : %d\n", M);
			 printf("Nombre de colonnes de B la matrice B, au plus 50 : ");
			 scanf("%d", &P );
			 for (I=0; I<M; I++)
			    for (J=0; J<P; J++)
				{
				 printf("B[%d][%d] : ",I,J);
				 scanf("%d", &B[I][J]);
				}
			 /* Affichage des matrices */
			 printf("Matrice A :\n");
			 for (I=0; I<N; I++)
			    {
			     for (J=0; J<M; J++)
				  printf("%7d", A[I][J]);
			     printf("\n");
			    }
			 printf("Matrice B :\n");
			 for (I=0; I<M; I++)
			    {
			     for (J=0; J<P; J++)
				  printf("%7d", B[I][J]);
			     printf("\n");
			    }
			 /* Ecriture du resultat de A*B dans C */
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
			      printf("%7d", C[I][J]);
			  printf("\n");
			 }
	        
		        
		        //le processus détache str de la mémoire partagée 
		        shmdt(str); 
		        // destruction de la mémoire partagée 
		        shmctl(shmid,IPC_RMID,NULL);
		        // destruction du semaphore
		        if(semctl(semid, 0, IPC_RMID, NULL) == -1){
		           perror("semctl");
		           exit(1);
		        	}	
		
        

        
    } 
}
