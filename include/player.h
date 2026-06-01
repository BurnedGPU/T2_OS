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

    int wait_ticket; // ticket de espera para el emparejamiento en player.c

    pthread_t thread; // hilo asociado al jugador
} Player;

extern Player* player;

void* player_routine(void* arg); // funcion que ejecuta el hilo del jugador

#endif // PLAYER_H
