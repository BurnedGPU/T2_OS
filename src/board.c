//
// Created by franc on 21-05-2026.
//

#include "board.h"
#include <stdio.h>

void* board_routine(void* arg) {
    // Transformamos el argumento genérico de vuelta a un puntero de Tablero
    Board* b = (Board*)arg;

    printf("Hilo del tablero %d listo y esperando jugadores.\n", b->id);

    // Aquí irá el ciclo while para simular las partidas de tic-tac-toe

    return NULL;
}
