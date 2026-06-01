//
// Created by franc on 21-05-2026.
//

#include "snapshot.h"
#include "matchmaker.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

/* =========================================================================
 * save_snapshot:
 * Guarda el estado actual de los jugadores en un archivo binario.
 * Se ejecuta al apagar el servidor (Graceful Shutdown).
 * ========================================================================= */
void save_snapshot(void) {

    // 1. Abrimos el archivo en modo "wb" (Write Binary).
    // Usamos binario para guardar la estructura tal cual esta en la memoria RAM, sin convertir a texto.
    FILE* f = fopen(sim_config.snapshot_path, "wb");
    if (!f) {
        printf("[Snapshot] Error: No se pudo crear el archivo %s\n", sim_config.snapshot_path);
        return;
    }

    // 2. Guardamos un "Metadato" o Cabecera: La cantidad de jugadores.
    // Esto servirá en el futuro para saber si el archivo de guardado es compatible con la configuracion actual.
    fwrite(&sim_config.n_players, sizeof(int), 1, f);

    // 3. Volcado de memoria masivo (Snapshot):
    // Escribimos el arreglo completo de jugadores en el disco en una sola operacion eficiente.
    fwrite(tournament.players, sizeof(Player), sim_config.n_players, f);

    fclose(f);
    printf("[Snapshot] Estado guardado exitosamente en %s\n", sim_config.snapshot_path);
}

/* =========================================================================
 * load_snapshot:
 * Carga el estado previo de los jugadores desde el archivo binario a la RAM.
 * Se ejecuta justo antes de crear los hilos en main.c.
 * ========================================================================= */
void load_snapshot(void) {

    // 1. Abrimos el archivo en modo "rb" (Read Binary).
    FILE* f = fopen(sim_config.snapshot_path, "rb");
    if (!f) {
        // Es normal que falle la primera vez que se ejecuta el programa, porque el archivo aun no existe.
        printf("[Snapshot] No se encontro archivo previo (%s). Iniciando torneo desde cero.\n", sim_config.snapshot_path);
        return;
    }

    // 2. Leemos la cabecera para verificar la compatibilidad.
    int saved_n_players = 0;
    fread(&saved_n_players, sizeof(int), 1, f);

    // Si el archivo viejo tenia 50 jugadores y ahora el .env dice 100, los datos son incompatibles.
    // Abortamos la carga para evitar una corrupcion de memoria (Segmentation Fault).
    if (saved_n_players != sim_config.n_players) {
        printf("[Snapshot] Advertencia: El archivo de guardado no coincide con N_PLAYERS actual. Ignorando snapshot.\n");
        fclose(f);
        return;
    }

    // 3. Carga masiva a la memoria:
    // Aplastamos los datos por defecto (los 1000 de ELO) con los datos reales del archivo binario.
    fread(tournament.players, sizeof(Player), sim_config.n_players, f);

    // 4. Sanitización de estados (Evitar hilos zombies):
    // Si el programa se apago abruptamente mientras un jugador estaba PLAYING,
    // debemos regresarlo a WAITING para que no se quede colgado esperando a un tablero vacio.
    for (int i = 0; i < sim_config.n_players; i++) {
        // Si el jugador ya se habia retirado (FINISHED), respetamos su decision.
        if (tournament.players[i].state != FINISHED) {
            tournament.players[i].state = WAITING;
        }
        // Reseteamos el ticket de prioridad temporal para que todos arranquen en igualdad de condiciones.
        tournament.players[i].wait_ticket = 0;
    }

    fclose(f);
    printf("[Snapshot] Estado restaurado exitosamente desde %s\n", sim_config.snapshot_path);
}