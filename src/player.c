//
// Created by franc on 21-05-2026.
//

#include "player.h"
#include "config.h"
#include "matchmaker.h"
#include <stdio.h>
#include <stdlib.h>

void* player_routine(void* arg) {
    Player* me = (Player*)arg; // Transformamos el argumento genérico de vuelta a un puntero de Jugador

    static int global_ticket = 0; // Variable copia del "wait ticket" declarado en player.h

    while (1) {

        pthread_mutex_lock(&tournament.match_mutex); // bloquear seccion critica del match con mutex

        if (tournament.shutdown_flag) { // verifica si la bandera de cierre SIGINT fue levantada
            pthread_mutex_unlock(&tournament.match_mutex); // si se cumple la condicion se apaga todo
            break;
        }

        if (me->state != WAITING) { // manda al jugador a esperar si estaba en otro estado
            me->state = WAITING;
            me->wait_ticket = global_ticket++; // el jugador saca un numero y espera
            //printf("[Jugador %d] Buscando oponente (ELO %d; Ticket: %d) ...\n", me->id, me->elo, me->wait_ticket);
        };

        int best_opponent_id = -1; // en esta variable se guarda el mejor oponente posible
        int best_ticket = -1; // aqui se guarda el mejor ticket para el emparejamiento
        int selected_board_id = -1; // se guarda el id del tablero seleccionado

        // BUSQUEDA DE OPONENTE

        for (int i=0; i < sim_config.n_players; i++) { // bucle for que busca al mejor oponente para el actual jugador "me"
            Player* candidate = &tournament.players[i]; //variable que guarda al posible candidato actual

            if (candidate->id != me->id && candidate->state == WAITING) { // verifica que el oponente y el jugador 1 no sean la misma persona
                int elo_diff = abs(candidate->elo - me->elo); //

                if (elo_diff <= sim_config.max_elo_diff) { // verifica que la diferencia de elo no sea mayor al limite del .env

                    if (best_opponent_id == -1 || candidate->wait_ticket < best_ticket) { // verifica que el oponente sea el que haya esperado mas
                        best_opponent_id = candidate->id;
                        best_ticket = candidate->wait_ticket;
                    }
                }
            }
        }

        // BUSQUEDA DE TABLERO

        if (best_opponent_id != -1) { // verifica que ya se haya encontrado oponente
            for (int i=0; i < sim_config.k_boards; i++) {
                Board* selected = &tournament.boards[i];
                if (selected->is_occupied == 0) {
                    selected_board_id = selected->id;
                    selected->is_occupied = 1;
                    selected->player1_id = me->id;
                    selected->player2_id = best_opponent_id; // agrega los datos al tablero seleccionado

                    Player* opponent = &tournament.players[best_opponent_id]; // guarda en una variable el id del oponente
                    me->state = PLAYING;
                    opponent->state = PLAYING;

                    //printf("[Matchmaker] Partida armada: J%d vs J%d en Tablero %d\n", me->id, opponent->id, selected->id);

                    // Despertamos al tablero y al oponente
                    pthread_cond_broadcast(&tournament.state_changed);
                    break;
                }
            }
        }

        // ADMINISTRACION DE SINCRONIZACION

        int is_playing_now = 0;

        if (selected_board_id != -1) { // El jugador "me" fue el que armo la partida
            is_playing_now = 1;
            pthread_mutex_unlock(&tournament.match_mutex);

        }else { // Este jugador "me" no armo la partida asi que espera

            pthread_cond_wait(&tournament.state_changed, &tournament.match_mutex);

            if (me->state == PLAYING) {
                is_playing_now = 1;
            }
            pthread_mutex_unlock(&tournament.match_mutex);
        }

        // GESTION DE SPURIOUS WAKEUPS Y POSIBILIDAD DE REJUGAR

        if (is_playing_now) {
            pthread_mutex_lock(&tournament.match_mutex);
            int iam_on_board = 1;

            // ciclo anti spurious-wakeups
            while (iam_on_board == 1 && !tournament.shutdown_flag) {
                iam_on_board = 0; // 1. Asumo que ya NO estoy en el tablero

                for (int i=0; i < sim_config.k_boards; i++) { // 2. Reviso todos los tableros para confirmar
                    if (tournament.boards[i].player1_id == me->id || tournament.boards[i].player2_id == me->id) {
                        iam_on_board = 1;
                        break;
                    }
                }
                if (iam_on_board == 1) { // 3. Si sigo en el tablero, me voy a dormir
                    pthread_cond_wait(&tournament.state_changed, &tournament.match_mutex);
                }
            }
            pthread_mutex_unlock(&tournament.match_mutex);

            if (tournament.shutdown_flag) { // verifica que no hayan presionado ctrl+c mientras estaba en partida
                break;
            }

            // PROBABILIDAD DE REINGRESAR

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
