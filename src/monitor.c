//
// Created by franc on 21-05-2026.
//

#include "monitor.h"
#include "matchmaker.h"
#include "config.h"
#include <stdio.h>
#include <pthread.h>

void player_stats(int player_id) {
    if (player_id < 0 || player_id >= sim_config.n_players) {
        printf("[Monitor] Error: El jugador %d no existe.\n", player_id);
        return;
    }

    pthread_mutex_lock(&tournament.elo_mutex);
    Player p = tournament.players[player_id];
    pthread_mutex_unlock(&tournament.elo_mutex);

    printf("\n=== Estadisticas Jugador %d ===\n", p.id);
    printf("ELO Actual: %d\n", p.elo);
    printf("Historial : %d Victorias | %d Derrotas | %d Empates\n", p.wins, p.losses, p.draws);

    printf("Estado    : ");
    if (p.state == WAITING) printf("WAITING (Buscando partida)\n");
    else if (p.state == PLAYING) printf("PLAYING (en partida)\n");
    else printf("FINISHED (Retirado)\n");
    printf("===============================\n\n");
}

void current_matches(void) {
    printf("\n=== Partidas Activas ===\n");
    int activas = 0;
    pthread_mutex_lock(&tournament.match_mutex);

    for (int i = 0; i < sim_config.k_boards; i++) {
        if (tournament.boards[i].is_occupied) {
            printf("Tablero %d: J%d vs J%d\n",
                   tournament.boards[i].id,
                   tournament.boards[i].player1_id,
                   tournament.boards[i].player2_id);
            activas++;
            }
        }
    pthread_mutex_unlock(&tournament.match_mutex);

    if (activas == 0) {
        printf("\n=== No hubieron partidas en curso ===\n");
    }else {
        printf("Total de partidas en curso: %d / %d\n", activas, sim_config.k_boards);
    }
    printf("========================\n\n");
}

void match_status(int game_id) {
    if (game_id < 0 || game_id >= sim_config.k_boards) {
        printf("[Monitor] Error: El tablero %d no existe.\n", game_id);
        return;
    }

    pthread_mutex_lock(&tournament.match_mutex);
    Board b = tournament.boards[game_id];
    pthread_mutex_unlock(&tournament.match_mutex);

    printf("\n=== Estado Tablero %d ===\n", b.id);
    if (b.is_occupied) {
        printf("Estado        : OCUPADO\n");
        printf("Enfrentamiento: J%d vs J%d\n", b.player1_id, b.player2_id);
        printf("Jugadas       : Partida de Tic-Tac-Toe en progreso...\n");
    }else {
        printf("Estado        : LIBRE\n");
        printf("El tablero esta esperando jugadores.\n");
    }
    printf("=========================\n\n");

}



