//
// Created by franc on 21-05-2026.
//

#include "matchmaker.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

MatchmakerState tournament;

void init_matchmaker() {
    // 1. Reservar memoria para los N jugadores y K tableros
    tournament.players = malloc(sim_config.n_players * sizeof(Player));
    tournament.boards = malloc(sim_config.k_boards * sizeof(Board));

    if (!tournament.players || !tournament.boards) {
        perror("Error al asignar memoria para el torneo");
        exit(EXIT_FAILURE);
    }

    // 2. Inicializar las variables de estado
    tournament.shutdown_flag = 0;
    tournament.active_players = sim_config.n_players;

    // 3. Inicializar las primitivas de sincronizacion (Mutex y Cond)
    pthread_mutex_init(&tournament.match_mutex, NULL);
    pthread_mutex_init(&tournament.elo_mutex, NULL);
    pthread_cond_init(&tournament.state_changed, NULL);

    // 4. Inicializacion basica de los tableros
    for (int i = 0; i < sim_config.k_boards; i++) {
        tournament.boards[i].id = i;
        tournament.boards[i].is_occupied = 0;
        tournament.boards[i].player1_id = -1;
        tournament.boards[i].player2_id = -1;
    }
}

void destroy_matchmaker() {
    // Limpiar los recursos al terminar (vital para el Graceful Shutdown)
    pthread_mutex_destroy(&tournament.match_mutex);
    pthread_mutex_destroy(&tournament.elo_mutex);
    pthread_cond_destroy(&tournament.state_changed);

    free(tournament.players);
    free(tournament.boards);
}