//
// Created by franc on 21-05-2026.
//

#ifndef BOARD_H
#define BOARD_H

#include <pthread.h>

typedef struct {
    int id;
    int is_occupied;  
    int player1_id;
    int player2_id;

    pthread_t thread;
} Board;


void* board_routine(void* arg);

#endif // BOARD_H
