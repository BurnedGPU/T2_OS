#define _DEFAULT_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "matchmaker.h"
#include "board.h"
#include "player.h"
#include "signal.h"
#include "snapshot.h"

void handle_sigint(int sig) {
    (void) sig;

    printf("\n[Sistema] Señal de apagado recibida. Iniciando Graceful Shutdown...\n");

    pthread_mutex_lock(&tournament.match_mutex);
    tournament.shutdown_flag = 1;

    pthread_cond_broadcast(&tournament.state_changed);
    pthread_mutex_unlock(&tournament.match_mutex);

}

int main(void) {
    printf("Iniciando motor cmatch...\n");

    // 1. Cargar la configuracion desde el archivo .env
    load_config(".env");

    // 2. Asignar memoria e inicializar Mutex/Variables de condicion
    // Esta funcion que armamos hace todos los malloc y los init
    init_matchmaker();

    load_snapshot();

    signal(SIGINT, handle_sigint);

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

    // Finalizador de partidas (gestion de graceful shutdown y otras formas de apagado)
    int stall_counter = 0;
    while (1) { //
        usleep(500000); // 500,000 microsegundos = 0.5 segundos

        pthread_mutex_lock(&tournament.match_mutex);

        // A. Apagado manual (Ctrl+C)
        if (tournament.shutdown_flag) {
            pthread_cond_broadcast(&tournament.state_changed);
            pthread_mutex_unlock(&tournament.match_mutex);
            break;
        }

        // B. Apagado natural (Falta de jugadores)
        if (tournament.active_players < 2) {
            printf("\n[Supervisor] Quedan menos de 2 jugadores (%d activos). Fin natural.\n", tournament.active_players);
            tournament.shutdown_flag = 1;
            pthread_cond_broadcast(&tournament.state_changed);
            pthread_mutex_unlock(&tournament.match_mutex);
            break;
        }

        // C. NUEVO: Detector de incompatibilidad de ELO
        int active_matches = 0;
        for (int i = 0; i < sim_config.k_boards; i++) {
            if (tournament.boards[i].is_occupied) {
                active_matches++;
            }
        }

        if (active_matches == 0) {
            stall_counter++;
            // Si pasan 4 ciclos (2 segundos totales) sin NINGUNA partida activa:
            if (stall_counter >= 4) {
                printf("\n[Supervisor] Torneo estancado. Los %d jugadores restantes son incompatibles por ELO. Auto-Apagado...\n", tournament.active_players);
                tournament.shutdown_flag = 1;
                pthread_cond_broadcast(&tournament.state_changed);
                pthread_mutex_unlock(&tournament.match_mutex);
                break;
            }
        } else {
            // Si hay al menos un tablero jugando, reiniciamos el contador
            stall_counter = 0;
        }

        pthread_mutex_unlock(&tournament.match_mutex);
    }

    for (int i = 0; i < sim_config.n_players; i++) {
        pthread_join(tournament.players[i].thread, NULL);
    }
    for (int i = 0; i < sim_config.k_boards; i++) {
        pthread_join(tournament.boards[i].thread, NULL);
    }

    // 6. Limpieza final
    printf("Simulacion finalizada correctamente.\n");

    save_snapshot();
    destroy_matchmaker();

    return EXIT_SUCCESS;
}
