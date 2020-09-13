#include <sys/wait.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <string.h>
#include "../headers/semaphores.h"
#include "../headers/shared_memory.h"

//Declaration of our 2 dynamic binary n-size semaphore's array.
unsigned int *mutexes1,*mutexes2; 

int main (int argc, char* argv[])
{
    //Declaration of significant integers.
    int pos,n,M,i,pid,status;
    //Seed the random system.
    srand(time(0)); 
    key_t key;
    struct timeval curr_time;
    long int average_delay,delay,all_delays=0;

    //Error checking.
    if(argc != 3)
    {
        printf("Error! You have to insert 2 arguments! ( ./feeder n M )\n");
        return -1;
    }
    else
    {
        n = atoi(argv[1]);
        M = atoi(argv[2]);
        if(n<=0)
        {
            printf("Error! You have to insert positive n!\n");
            return -2;
        }
        if(M<=3000)
        {
            printf("Error! M must be greater than 3000!\n");
            return -3;
        }
    }
    
    //Define array with M positions.
    int* array = (int* )malloc(M*sizeof(int));
   
    // Process to fill array with random numbers (0-100)
    // printf("Feeder's array: ");
    for(i=0;i<M;i++)    
    {
        array[i]=(rand()%100); 
        // printf("%d ",array[i]);
    }
    printf("\n");

    //Producing a random key for shared memory
    if ((key = ftok("source/feeder.c", 'R')) == -1) 
    {                
        printf("Error! Cannot create key!\n");
        exit(1);
    }

    //Creating shared memory...and getting access into it(made up from a single Entry).
    int shm_id = shmcreate(key,sizeof(Entry));
    Entry* shm_ptr = shmattach(shm_id);

    //For single Entry initialize member number with zero.
    shm_ptr->number = 0;

    //Allocation of memory for our 2 binary arrays.
    unsigned int* mutexes1 = (unsigned int* )malloc(n*sizeof(unsigned int));
    unsigned int* mutexes2 = (unsigned int* )malloc(n*sizeof(unsigned int));

    //Binary semaphores take their sem_id's.
    for(int j=0;j<n;j++)
    {
        mutexes1[j]= sem_create((key_t)rand()%10000000);
        mutexes2[j]= sem_create((key_t)rand()%10000000);
    }

    //Creating n Ci processes.
    for(i=0;i<n;i++)  
    {                     
        pid = fork();
        pos=i;
        if(pid==-1)
        {
            printf("Function fork() failed!\n");
            return -1;
        }
        else if(pid==0) //If it is a child, don't fork()
            break;
    }
    
    //Case 0: It's a child.
    if(pid==0)
    {
        //Define array with M positions.
        int* process_array = (int* )malloc(M*sizeof(int));

        for(int i=0;i<M;i++)
        {
            //Block processes with semaphore being down.
            if(!(sem_down(mutexes1[pos]))) exit(1);
            
            //Process Ci is getting each integer from array.
            process_array[i]=shm_ptr->number;
            //Store current time.
            gettimeofday(&curr_time,NULL);
            //Calculate delay for each Ci process.
            delay = ((curr_time.tv_sec-shm_ptr->timestamp.tv_sec)*1000000L) + (curr_time.tv_usec-shm_ptr->timestamp.tv_usec);
            //Add current delay to all delays in order to have an average delay in the end.
            all_delays+=delay;

            //Unblock processes with semaphore being up.
            if(!(sem_up(mutexes2[pos])))    exit(1);
        }

        //Print average delay into terminal.
        average_delay=all_delays/M;
        printf("Average delay of process %d:%ldμs\n",getpid(),average_delay);

        //Process to create filename of each Ci process.
        char filename[50],pid[20];
        sprintf(pid, "%d", getpid());
        strcpy(filename,"./files/file");
        strcat(filename,pid);
        
        //Attempt to open file.
        FILE* fPtr = fopen(filename, "w");
        
        if(fPtr == NULL)
        {
            printf("Unable to create file.\n");
            exit(EXIT_FAILURE);
        }

        //Write into file process array and average delay.
        fprintf(fPtr,"Array of process %d is: ",getpid());
        for(i=0;i<M;i++)    fprintf(fPtr,"%d ", process_array[i]);
        fprintf(fPtr,"\n\nAverage delay of process %d:%ldμs\n",getpid(),average_delay);

        //Close file.
        fclose(fPtr);

        //Each process deallocate memory for process_array.
        free(process_array);
        //Processes exiting...
        exit(1);
    }
    //Case 1: Father.
    else
    {
        for(i=0;i<M;i++)
        {    
            //Put integer inside shared memory.
            shm_ptr->number=array[i];
            //Write current time into timestamp of shared memory.
            gettimeofday(&shm_ptr->timestamp,NULL);

            for(int j=0;j<n;j++)
            {
                //Unblock processes with semaphore being up.
                if(!(sem_up(mutexes1[j])))    exit(1);
            }     
            
            if(i!=(M-1))
            {
                for(int j=0;j<n;j++)
                {
                    //Block processes with semaphore being down.
                    if(!(sem_down(mutexes2[j])))    exit(1);
                } 
            }
        }  
        //Father wait until all process are done.
        while(wait(&status) > 0);
    }
    
    //Detachment of shared memory.
    shmdetach(shm_ptr);
    shmdelete(shm_id);

    //Delete semaphores.
    for(int j=0;j<n;j++)
    {
        sem_del(mutexes1[j]);
        sem_del(mutexes2[j]);
    }

    //Deallocation of memory.
    free(mutexes1);
    free(mutexes2);
    free(array);
}