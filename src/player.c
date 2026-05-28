//
// Created by franc on 21-05-2026.
//

#include "player.h"
#include "config.h"
#include "matchmaker.h"
#include <stdio.h>
#include <stdlib.h>

void* player_routine(void* arg) {
    // Transformamos el argumento genérico de vuelta a un puntero de Jugador
    Player* me = (Player*)arg;

    static int global_ticket = 0;

    while (1) {

        pthread_mutex_lock(&tournament.match_mutex); // bloquear seccion critica del match con mutex

        if (tournament.shutdown_flag) { // verifica si la bandera de cierre SIGINT fue levantada
            pthread_mutex_unlock(&tournament.match_mutex); // si se cumple la condicion todo se rompe
            break;
        }

        if (me->state != WAITING) {
            me->state = WAITING;
            me->wait_ticket = global_ticket++;
            printf("[Jugador %d] Buscando oponente (ELO %d; Ticket: %d) ...\n", me->id, me->elo, me->wait_ticket);
        };

        int best_opponent_id = -1;
        int best_ticket = -1;
        int selected_board_id = -1;

        for (int i=0; i < sim_config.n_players; i++) {
            Player* candidate = &tournament.players[i];

            if (candidate->id != me->id && candidate->state == WAITING) {
                int elo_diff = abs(candidate->elo - me->elo);

                if (elo_diff <= sim_config.max_elo_diff) {

                    if (best_opponent_id == -1 || candidate->wait_ticket < best_ticket) {
                        best_opponent_id = candidate->id;
                        best_ticket = candidate->wait_ticket;
                    }
                }
            }
        }

        if (best_opponent_id != -1) {
            for (int i=0; i < sim_config.k_boards; i++) {
                Board* selected = &tournament.boards[i];
                if (selected->is_occupied == 0) {
                    selected_board_id = selected->id;
                    selected->is_occupied = 1;
                    selected->player1_id = me->id;
                    selected->player2_id = best_opponent_id;

                    Player* opponent = &tournament.players[best_opponent_id];
                    me->state = PLAYING;
                    opponent->state = PLAYING;

                    printf("[Matchmaker] Partida armada: J%d vs J%d en Tablero %d\n", me->id, opponent->id, selected->id);

                    // Despertamos al tablero y al oponente
                    pthread_cond_broadcast(&tournament.state_changed);
                    break;
                }
            }
        }

        if (selected_board_id != -1) {
             pthread_mutex_unlock(&tournament.match_mutex);



        }else {
            pthread_cond_wait(&tournament.state_changed, &tournament.match_mutex);

            if (me->state == PLAYING) {
                pthread_mutex_unlock(&tournament.match_mutex);


            }else {
                pthread_mutex_unlock(&tournament.match_mutex);
            }
        }

    }

    // Aquí irá el ciclo while para buscar partidas

    return NULL;
}
