#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define TIMEOUT 1.0

// Structure for Tub
typedef struct
{
    int id;
    int priority;
    int security_bit;
    int destination;
    int wait_bit;
    int request_bit;
} Tub;

// Pico module
typedef struct
{
    int id;
    Tub *short_belt;
    Tub *long_belt;
} PicoModule;

// Function to handle priority-based access control
void handle_access(PicoModule *picoL, PicoModule *picoN)
{
    if (picoL->short_belt != NULL && picoN->long_belt != NULL)
    {
        picoL->short_belt->request_bit = 1;
        picoN->long_belt->request_bit = 1;

        // Compare priorities
        if (picoL->short_belt->priority < picoN->long_belt->priority)
        {
            printf("Grant access to Tub %d from Short Belt \n", picoL->short_belt->id);
            picoN->long_belt->wait_bit = 1;
            picoN->long_belt->request_bit = 0;
            usleep(TIMEOUT * 1000000);
            printf("Tub %d moved to Long Belt\n", picoL->short_belt->id);
            picoL->short_belt = NULL;
        }
        else
        {
            printf("Grant access to Tub %d from neighbour Long Belt\n", picoN->long_belt->id);
            picoL->short_belt->wait_bit = 1;
            picoL->short_belt->request_bit = 0;
            usleep(TIMEOUT * 1000000);
            printf("Tub %d moved to Long Belt\n", picoN->long_belt->id);
            picoN->long_belt = NULL;
        }
    }
}

// Function to process Tub movement in IN Module
void process_Tub(PicoModule *picoL, PicoModule *picoN)
{
    printf("\nProcessing Tub in Pico %d\n", picoL->id);

    if (picoL->short_belt != NULL)
    {
        picoL->short_belt->request_bit = 1;
    }

    handle_access(picoL, picoN);

    if (picoL->short_belt != NULL && picoL->short_belt->wait_bit == 1)
    {
        if (picoN->long_belt == NULL) // No one else waiting
        {
            printf("No other requests, Tub %d can proceed\n", picoL->short_belt->id);
            picoL->short_belt->wait_bit = 0;
            usleep(TIMEOUT * 1000000);
            printf("Tub %d moved to Long Belt\n", picoL->short_belt->id);
            picoL->short_belt = NULL;
        }
        else
        {
            printf("Tub %d waiting, retrying in %.1f seconds...\n", picoL->short_belt->id, TIMEOUT);
            usleep(TIMEOUT * 1000000);
            printf("Rechecking access for Tub %d\n", picoL->short_belt->id);
            process_Tub(picoL, picoN);
        }
    }
}

// Structure for Request
typedef struct
{
    int tId;
    int tDeadline;
    int atDestination;
} Request;

// Function to send a request from a tub to access the long belt
void send_request(Tub *tub, PicoModule *pico)
{
    tub->request_bit = 1;

    Request request;
    request.tId = tub->id;
    request.tDeadline = tub->priority;
    request.atDestination = tub->destination;
}

int main()
{
    srand(time(NULL));

    Tub tub1 = {101, 3, 1, 2, 0, 0}; // Lower priority
    Tub tub2 = {103, 1, 1, 2, 0, 0}; // Highest priority

    PicoModule picoL = {1, &tub1, NULL}; // Short belt occupied
    PicoModule picoN = {2, NULL, &tub2}; // Long belt occupied

    process_Tub(&picoL, &picoN);

    return 0;
}
