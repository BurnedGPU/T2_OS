#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "matchmaker.h"
#include "player.h"
#include "config.h"
#include <unistd.h>
#include "elo.h"

void* board_routine(void* arg) {
    Board* b = (Board*)arg;     // Transformamos el argumento genérico de vuelta a un puntero de Tablero

    while (1) {
        pthread_mutex_lock(&tournament.match_mutex);

        while (!tournament.shutdown_flag && b->is_occupied == 0) {
            pthread_cond_wait(&tournament.state_changed, &tournament.match_mutex);
        }

        if (tournament.shutdown_flag && b->is_occupied == 0) { // si se levanta la bandera se cierra el negocio
            pthread_mutex_unlock(&tournament.match_mutex);
            break;
        }

        int p1_id = b->player1_id; // rescata a los dos jugadores y levanta el candado
        int p2_id = b->player2_id;

        pthread_mutex_unlock(&tournament.match_mutex);

        // Un juego de gato tiene entre 5 y 9 turnos
        int total_turnos = 5 + (rand() % 5);

        for (int i = 0; i < total_turnos; i++) {
            // Dormimos el hilo para simular el tiempo del turno
            // usleep requiere microsegundos, por lo que multiplicamos por 1000
            usleep(sim_config.turn_delay_ms * 1000);
        }

        // Generamos un resultado aleatorio: 0 = Empate, 1 = Gana P1, 2 = Gana P2
        int match_result = rand() % 3;

        pthread_mutex_lock(&tournament.elo_mutex);

        Player* p1 = &tournament.players[p1_id];
        Player* p2 = &tournament.players[p2_id];

        update_elo(p1, p2, match_result);

        pthread_mutex_unlock(&tournament.elo_mutex);
        
        // limpieza
        pthread_mutex_lock(&tournament.match_mutex);

        b->is_occupied = 0;
        b->player1_id = -1;
        b->player2_id = -1;

        //printf("[Tablero %d] Partida finalizada. Tablero liberado.\n", b->id);

        // Gritamos por el broadcast para despertar a los jugadores de esta partida (y que se vayan)
        // y para avisar a los que buscan partida que hay un tablero libre.
        pthread_cond_broadcast(&tournament.state_changed);
        pthread_mutex_unlock(&tournament.match_mutex);



    }

    return NULL;
}
