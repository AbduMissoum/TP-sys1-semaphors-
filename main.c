#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#define n  10 // nombre des processus
#define cpt  0   //pour acceded a cpt
#define v1  1  //pour acceded a v1

typedef struct info
{
    int pid;
    int rang;
    int val;
}info;

int main()
{
        int p, i, err;
    key_t clef;
    int t, cpt_v1_id,acces_v1_id;
    int mutex;
    int *cpt_v1;
    info * acces_v1;
  



//recuperation de la cle pour acceder a v1 et cpt
  clef = ftok("key_file", 1);   
  
  cpt_v1_id =shmget(clef,2*sizeof(int),IPC_CREAT|0666);//recuperation Id

  if(cpt_v1_id==-1) {printf("\n Erreur de creation de la mÃ©moire partagÃ©e"); exit(2);}

    cpt_v1= (int  *)shmat(cpt_v1_id,0,0);//attachement 

    //inialisation 
cpt_v1[cpt] = 0;
cpt_v1[v1] =0;
// //////////

// access_v1 recuperation de la cle
  clef = ftok("key_file", 2);   
  
  acces_v1_id =shmget(clef,n*sizeof(info),IPC_CREAT|0666);//recuperation id
  if(acces_v1_id==-1) {printf("\n Erreur de creation de la mÃ©moire partagÃ©e"); exit(2);}
    acces_v1= (info *)shmat(acces_v1_id,0,0);//attachemnt 
///////////////
 // mutex recuperation de la cle
  clef = ftok("key_file", 3);   
  mutex=semget(clef, 1, IPC_CREAT|0666);//mutex id
  if(mutex==-1) {printf("\n  Erreur de creation des sÃ©maphores"); exit(1);}
    semctl(mutex,0,SETVAL,1);      //inialisation 
    /////////////////////////////////////////////

  clef = ftok("key_file", 4);   
  t=semget(clef, n, IPC_CREAT|0666);
  if(t==-1) {printf("\n  Erreur de creation des sÃ©maphores"); exit(1);}
   

 semctl(t,n+2,SETALL,0);//inialisation t[i]=0 quelque soit i <n
 semctl(t,n-1,SETVAL,1);// t[n-1]=1;// la darniern semaphore 


 ////////////////////////
for(i=0;i<n;i++)//creation des fils
 {
p= fork();
if(p==0)
{
err =  execl("./Pgme_processus","Pgme_processus",NULL);  
     printf("Child process %d  erreur %d\n", i,err>>8);//test l'erreur 
     exit(2);
}

 }
 while(wait(NULL)!=-1);//wait 




//detachement et destruction des semaphores et les memoires partage 

  shmdt(acces_v1); 
  shmdt(cpt_v1); 
 shmctl(cpt_v1_id,IPC_RMID,0);  
  shmctl(acces_v1_id,IPC_RMID,0);  
 semctl(t, IPC_RMID, 0);
  semctl(mutex, IPC_RMID, 0);
}
