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
#define n 10    // nombre des processus
#define cpt 0    //pour acceded a cpt
#define v1  1 //pour acceded a v1


typedef struct info
{
    int pid;
    int rang;
    int val;
}info;




int main()
{
int i,j;
info element ;

    key_t clef;
    int t, cpt_v1_id,acces_v1_id;
    int mutex;
    int *cpt_v1;
    info * acces_v1;

     struct sembuf pmutex={0,-1,0};  //p(mutex)
 struct sembuf vmutex={0,1,0};//v(mutex)
  struct sembuf pt={0,-1,0};//on va changer apres le nombre de semaphore selon j 
 struct sembuf vt={0,1,0}; //on va changer apres le nombre de semaphore selon j 

    ///////////////////////////////////////////
    //recuperation la cle et l'attachement pour utiliser  cpt_v1
      clef = ftok("key_file", 1);   
  
  cpt_v1_id =shmget(clef,2*sizeof(int),IPC_CREAT|0666);
  if(cpt_v1_id==-1) {printf("\n Erreur de creation de la mÃ©moire partagÃ©e"); exit(2);}
    cpt_v1= (int  *)shmat(cpt_v1_id,0,0);
    ////////////////////////////////
    /////////////////////////////////
    //recuperation la cle et l'attachement pour utiliser  acces_v1
     clef = ftok("key_file", 2);   
  
  acces_v1_id =shmget(clef,n*sizeof(info),IPC_CREAT|0666);
  if(acces_v1_id==-1) {printf("\n Erreur de creation de la mÃ©moire partagÃ©e"); exit(2);}
    acces_v1= (info *)shmat(acces_v1_id,0,0);
    /////////////////////////////////////////
    /////////////////////////////////////////
    //recuperation la cle  pour utiliser  mutex
clef = ftok("key_file", 3);   
  mutex=semget(clef, 1, IPC_CREAT|0666);
  if(mutex==-1) {printf("\n  Erreur de creation des sÃ©maphores"); exit(1);}

    /////////////////////////////////////////
     /////////////////////////////////////////
         //recuperation la cle  pour utiliser  t
          clef = ftok("key_file", 4);   
  t=semget(clef, n, IPC_CREAT|0666);
  if(t==-1) {printf("\n  Erreur de creation des sÃ©maphores"); exit(1);}

    /////////////////////////////////////////


/////programmm 




semop(mutex,&pmutex,1);//p(mutex)
j=cpt_v1[cpt];
cpt_v1[cpt]++;
printf("\nProcesssu N %d   de pid = %d\n",j,getpid());
semop(mutex,&vmutex,1);//v(mutex)
pt.sem_num=j;//on affect le rang pour appliquer les operations P ,V sur le bon semaphore 
semop(t,&pt,1);// p(t[j])
cpt_v1[v1]+=j;
element.pid= getpid();
element.rang= j;
element.val = cpt_v1[v1];
acces_v1[j]=element;
vt.sem_num=(j-1+n)%n;
semop(t,&vt,1);//v(t[(j-1+n)%n])

if(j==0){
printf("\nles resultant d'acces a la variable v1\n");
for(i=n-1;i>=0;i--)
{
    printf("\naccess  N %d  par le processus = %d de rang %d v1= %d \n",n-i,acces_v1[i].pid,acces_v1[i].rang,acces_v1[i].val);
}
}


//detachement 
 shmdt(acces_v1); 
  shmdt(cpt_v1);


//suite <i>

return 0;




}
