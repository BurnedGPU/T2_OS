//
// Created by franc on 21-05-2026.
//
#ifndef PLAYER_H
#define PLAYER_H

#include <pthread.h>

// Definimos los posibles estados de un jugador
typedef enum {
    WAITING,    // Buscando oponente en la cola
    PLAYING,    // Jugando una partida
    FINISHED    // Decidio no reingresar y termina su ejecucion
} PlayerState;

typedef struct {
    int id;
    int elo;
    int wins;
    int losses;
    int draws;
    PlayerState state;

    int wait_ticket;

    // El hilo nativo de POSIX asociado a este jugador
    pthread_t thread;
} Player;

// Prototipo de la funcion que ejecutara el hilo del jugador
void* player_routine(void* arg);

#endif // PLAYER_H
