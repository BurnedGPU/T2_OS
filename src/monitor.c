//
// Created by franc on 21-05-2026.
//

#include "monitor.h"
#include "matchmaker.h"
#include "config.h"
#include <stdio.h>
#include <pthread.h>

/* =========================================================================
 * player_stats:
 * Imprime las estadísticas detalladas de un jugador específico.
 * Utiliza un Mutex para evitar la "Lectura Sucia" (leer un ELO mientras
 * el hilo del tablero lo está modificando).
 * ========================================================================= */
void player_stats(int player_id) {
    // 1. Validar que el usuario no pida un jugador fuera de los límites
    if (player_id < 0 || player_id >= sim_config.n_players) {
        printf("[Monitor] Error: El jugador %d no existe.\n", player_id);
        return;
    }

    // 2. Bloquear el candado del ELO para tomar una "foto" segura de los datos
    pthread_mutex_lock(&tournament.elo_mutex);
    Player p = tournament.players[player_id]; // Copiamos la estructura localmente
    pthread_mutex_unlock(&tournament.elo_mutex);

    // 3. Imprimir los datos con seguridad
    printf("\n=== Estadisticas Jugador %d ===\n", p.id);
    printf("ELO Actual: %d\n", p.elo);
    printf("Historial : %d Victorias | %d Derrotas | %d Empates\n", p.wins, p.losses, p.draws);

    printf("Estado    : ");
    if (p.state == WAITING) printf("WAITING (Buscando partida)\n");
    else if (p.state == PLAYING) printf("PLAYING (en partida)\n");
    else printf("FINISHED (Retirado)\n");
    printf("===============================\n\n");
}

/* =========================================================================
 * current_matches:
 * Recorre todos los tableros y muestra cuáles están ocupados actualmente.
 * Requiere bloquear el match_mutex para que nadie inicie o termine una
 * partida mientras estamos imprimiendo la lista.
 * ========================================================================= */
void current_matches(void) {
    printf("\n=== Partidas Activas ===\n");
    int activas = 0;

    // Bloqueamos el estado global de las partidas
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
    pthread_mutex_unlock(&tournament.match_mutex); // Liberamos el candado inmediatamente

    if (activas == 0) {
        printf("\n=== No hubieron partidas en curso ===\n");
    }else {
        printf("Total de partidas en curso: %d / %d\n", activas, sim_config.k_boards);
    }
    printf("========================\n\n");
}

/* =========================================================================
 * match_status:
 * Revisa el estado de un tablero específico para saber si está libre o
 * quiénes están jugando en él.
 * ========================================================================= */
void match_status(int game_id) {
    if (game_id < 0 || game_id >= sim_config.k_boards) {
        printf("[Monitor] Error: El tablero %d no existe.\n", game_id);
        return;
    }

    // Tomamos una lectura segura del estado del tablero
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

/* =========================================================================
 * cli_routine:
 * Hilo independiente que se queda escuchando el teclado del usuario (stdin)
 * sin bloquear el resto de la simulación.
 * ========================================================================= */
void* cli_routine(void* arg) {
    (void)arg; // Evita warnings de compilador por variable sin usar
    char buffer[128];

    // Imprimimos el menu una vez al arrancar
    printf("\n======================================================\n");
    printf(" MONITOR DE JUEGO\n");
    printf(" Escribe un comando y presiona enter\n");
    printf("   m       -> Ver todas las partidas activas\n");
    printf("   p <id>  -> Ver estadisticas de un jugador\n");
    printf("   t <id>  -> Ver estado de un tablero\n");
    printf("   s       -> Detener simulacion\n");
    printf("======================================================\n\n");

    while (1) {
        // fgets pausa ESTE hilo esperando que el usuario escriba algo y presione Enter.
        // Los demas hilos (jugadores y tableros) siguen corriendo normalmente.
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {

            // Si el programa se apagó por inactividad, salimos del ciclo
            if (tournament.shutdown_flag) break;

            char cmd = '\0';
            int id = -1;

            // sscanf con " %c %d" lee inteligentemente ignorando espacios extra.
            // Retorna cuántos elementos logró leer exitosamente.
            int parsed = sscanf(buffer, " %c %d", &cmd, &id);

            if (parsed >= 1) {
                // Comando 'm': Mostrar partidas
                if (cmd == 'm' || cmd == 'M') {
                    current_matches();

                // Comando 'p': Mostrar jugador (exige ID)
                } else if (cmd == 'p' || cmd == 'P') {
                    if (parsed == 2) {
                        player_stats(id);
                    } else {
                        printf("[Monitor] Formato incorrecto. Escribe la letra 'p', un espacio y el numero (Ej: p 5)\n");
                    }

                // Comando 't': Mostrar tablero (exige ID)
                } else if (cmd == 't' || cmd == 'T') {
                    if (parsed == 2) {
                        match_status(id);
                    } else {
                        printf("[Monitor] Formato incorrecto. Escribe la letra 't', un espacio y el numero (Ej: t 2)\n");
                    }

                // Comando 's': Apagado de emergencia (Graceful Shutdown manual)
                } else if (cmd == 's' || cmd == 'S') {
                    printf("\n[Monitor] Comando de apagado manual recibido. Finalizando...\n");

                    // Notificamos a TODO el sistema que el torneo terminó
                    pthread_mutex_lock(&tournament.match_mutex);
                    tournament.shutdown_flag = 1;
                    pthread_cond_broadcast(&tournament.state_changed); // Despierta a todos los hilos dormidos
                    pthread_mutex_unlock(&tournament.match_mutex);
                    break;

                } else {
                    printf("[Monitor] Comando '%c' no reconocido. Usa m, p, t o s.\n", cmd);
                }
            }
        }
    }
    return NULL;
}