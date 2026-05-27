//
// Created by franc on 21-05-2026.
//

#include "player.h"
#include "config.h"
#include "matchmaker.h"
#include <stdio.h>

void* player_routine(void* arg) {
    // Transformamos el argumento genérico de vuelta a un puntero de Jugador
    Player* me = (Player*)arg;

    static int global_ticket = 0;

    while (1) {

        pthread_mutex_lock(&tournament.match_mutex);

        if (tournament.shutdown_flag) {
            pthread_mutex_unlock(&tournament.match_mutex);
            break;
        }

        me->state = WAITING;
        me->wait_ticket = global_ticket++;

        int opponent_id = -1;
        int board_id = -1;

        

    }

    // Aquí irá el ciclo while para buscar partidas

    return NULL;
}