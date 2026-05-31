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
            //printf("[Jugador %d] Buscando oponente (ELO %d; Ticket: %d) ...\n", me->id, me->elo, me->wait_ticket);
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

                    //printf("[Matchmaker] Partida armada: J%d vs J%d en Tablero %d\n", me->id, opponent->id, selected->id);

                    // Despertamos al tablero y al oponente
                    pthread_cond_broadcast(&tournament.state_changed);
                    break;
                }
            }
        }

        // seccion 3
        int is_playing_now = 0;

        if (selected_board_id != -1) {
            // yo arme la partida
            is_playing_now = 1;
            pthread_mutex_unlock(&tournament.match_mutex);
        }else {
            // yo no arme la partida asi que espero a que me inviten
            pthread_cond_wait(&tournament.state_changed, &tournament.match_mutex);

            if (me->state == PLAYING) {
                is_playing_now = 1;
            }
            pthread_mutex_unlock(&tournament.match_mutex);
        }

        // seccion 4
        if (is_playing_now) {
            pthread_mutex_lock(&tournament.match_mutex);
            int iam_on_board = 1;

            // ciclo anti spurious-wakeups
            while (iam_on_board == 1 && !tournament.shutdown_flag) {
                iam_on_board = 0;
                for (int i=0; i < sim_config.k_boards; i++) {
                    if (tournament.boards[i].player1_id == me->id || tournament.boards[i].player2_id == me->id) {
                        iam_on_board = 1;
                        break;
                    }
                }
                if (iam_on_board == 1) {
                    pthread_cond_wait(&tournament.state_changed, &tournament.match_mutex);
                }
            }
            pthread_mutex_unlock(&tournament.match_mutex);

            if (tournament.shutdown_flag) {
                break;
            }

            float chance = (float)rand() / (float)RAND_MAX;

            if (chance <= sim_config.reenter_probability) {
                //printf("continua el juego");

            } else {
                //printf("jugador se retira");
                me->state = FINISHED;

                pthread_mutex_lock(&tournament.match_mutex);
                tournament.active_players--;
                pthread_mutex_unlock(&tournament.match_mutex);
                break;
            }


        }

    }

    return NULL;
}
