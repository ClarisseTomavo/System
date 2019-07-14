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
    union semun sem_arg;
    
    char *argp1[] = {"w", NULL, NULL};
    // clé pour le semaphore 
    key_t sem_key = ftok("semfile",75);
    // creation du semaphore
    int semid = semget(sem_key, 1, 0666|IPC_CREAT);

    // initialisation du semaphore à 1
    sem_arg.val = 1;
    if(semctl(semid, 0, SETVAL, sem_arg)==-1){
       perror("semctl");
       exit(1);
    }
    
    // generation de cle pour la memoire partagee
    key_t key = ftok("shmfile",65);

    int a = 0; 

    // Creation de la mémoire partagée
    int shmid = shmget(key,1024,0666|IPC_CREAT); 
    // la memoire partagee attachée à la variable str
    char *str = (char*) shmat(shmid,(void*)0,0); 
    // initialisation de la meoire partagee à 0 
    sprintf(str, "%d", a);

    //création du premier processus (pour lancer le process w)
    pid1 = fork();
    if(pid1 < 0){
          perror("Erreur de création du processus\n");
          exit(EXIT_FAILURE);
    }

    if(pid1 == 0){
        execv("./w", argp1);
    }

    else{
        // creation du second processus 
        char *argp2[] = {"Q1", NULL, NULL};
        pid2 = fork();
        sleep(7);
        if(pid2 < 0){
          perror("Erreur de création du second processus\n");
          pid1 = wait(&status);
          exit(EXIT_FAILURE);
        }

        if(pid2 == 0){
            execv("./w", argp2);
        }

        else{
		char *argp3[] = {"w", NULL, NULL};
                pid3 = fork();
		 if(pid3 < 0){
              		perror("Erreur de création du troisieme processus\n");
              		pid1 = wait(&status);
              		pid2 = wait(&status);
              		exit(EXIT_FAILURE);
            	}

            	if(pid3 == 0){
               		execv("./w", argp3);
            	}


		else{
		        // On attend la fin des deux processus
		        pid1 = wait(&status);
		        printf("Status de l'arret du fils %d %d\n", status, pid1);
		        pid2 = wait(&status);
		        printf("Status de l'arret du fils %d %d\n", status, pid2);
			
			pid3 = wait(&status);
		        printf("Status de l'arret du fils %d %d\n", status, pid3);

		        // lecture de la dernière valeur de la variable partagée
		        a = atoi(str);
		        printf("Valeur Finale de a = %d\n", a);
		        //on procede au détachement str de la mémoire partagée 
		        shmdt(str); 
		        // destruction de la mémoire 
		        shmctl(shmid,IPC_RMID,NULL);
		        
		        if(semctl(semid, 0, IPC_RMID, NULL) == -1){
		           perror("semctl");
		           exit(1);
		        	}	
		}
        }

        
    } 
}
