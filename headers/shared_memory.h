#pragma once
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <stdbool.h> 


//Shared memory storages a struct.
typedef struct data
{ 
    int number;
    struct timeval timestamp;
} Entry;

//Create shared memory and connect
int shmcreate(key_t, int );
//Deleting shared memory segment
int shmdelete(int);

//Getting a pointer to the segment
Entry* shmattach(int );
//Detaching segment
int shmdetach(Entry* );
