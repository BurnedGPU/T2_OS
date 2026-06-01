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

    pthread_t thread; // hilo asociado al tablero
} Board;

extern Board* board;

void* board_routine(void* arg); // funcion del ciclo de vida del tablero

#endif // BOARD_H
