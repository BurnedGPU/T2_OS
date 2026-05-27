
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "matchmaker.h"
#include "board.h"
#include "player.h"

int main(void) {
    printf("Iniciando motor cmatch...\n");

    // 1. Cargar la configuracion desde el archivo .env
    load_config(".env");

    // 2. Asignar memoria e inicializar Mutex/Variables de condicion
    // Esta funcion que armamos hace todos los malloc y los init
    init_matchmaker();

    printf("Creando %d hilos de tableros...\n", sim_config.k_boards);
    // 3. Crear los threads de los tableros activos
    for (int i = 0; i < sim_config.k_boards; i++) {
        // Lanzamos el hilo. Le pasamos su respectiva estructura de tablero como argumento
        if (pthread_create(&tournament.boards[i].thread, NULL, board_routine, &tournament.boards[i]) != 0) {
            perror("Error creando el thread del tablero");
            exit(EXIT_FAILURE);
        }
    }

    printf("Creando %d hilos de jugadores...\n", sim_config.n_players);
    // 4. Inicializar datos y crear los threads de los jugadores
    for (int i = 0; i < sim_config.n_players; i++) {
        tournament.players[i].id = i;
        tournament.players[i].elo = 1000; // Todos parten con 1000 de ELO
        tournament.players[i].wins = 0;
        tournament.players[i].losses = 0;
        tournament.players[i].draws = 0;
        tournament.players[i].state = WAITING;

        // Lanzamos el hilo. Le pasamos su respectiva estructura de jugador como argumento
        if (pthread_create(&tournament.players[i].thread, NULL, player_routine, &tournament.players[i]) != 0) {
            perror("Error creando el thread del jugador");
            exit(EXIT_FAILURE);
        }
    }

    printf("Todos los hilos estan en ejecucion. Simulacion corriendo...\n");

    // 5. El hilo principal (main) debe esperar a que todos los jugadores terminen
    // (pthread_join bloquea el main hasta que los threads finalicen su ciclo de vida)
    for (int i = 0; i < sim_config.n_players; i++) {
        pthread_join(tournament.players[i].thread, NULL);
    }

    // 6. Limpieza final
    destroy_matchmaker();
    printf("Simulacion finalizada correctamente.\n");

    return EXIT_SUCCESS;
}
