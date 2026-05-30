//
// Created by franc on 21-05-2026.
//

#include "snapshot.h"
#include "matchmaker.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

void save_snapshot(void) {

    FILE* f = fopen(sim_config.snapshot_path, "wb");
    if (!f) {
        printf("[Snapshot] Error: No se pudo crear el archivo %s\n", sim_config.snapshot_path);
        return;
    }

    fwrite(&sim_config.n_players, sizeof(int), 1, f);
    fwrite(tournament.players, sizeof(Player), sim_config.n_players, f);

    fclose(f);
    printf("[Snapshot] Estado guardado exitosamente en %s\n", sim_config.snapshot_path);
}

void load_snapshot(void) {
    FILE* f = fopen(sim_config.snapshot_path, "rb");
    if (!f) {
        printf("[Snapshot] No se encontro archivo previo (%s). Iniciando torneo desde cero.\n", sim_config.snapshot_path);
        return;
    }

    int saved_n_players = 0;
    fread(&saved_n_players, sizeof(int), 1, f);

    if (saved_n_players != sim_config.n_players) {
        printf("[Snapshot] Advertencia: El archivo de guardado no coincide con N_PLAYERS actual. Ignorando snapshot.\n");
        fclose(f);
        return;
    }

    fread(tournament.players, sizeof(Player), sim_config.n_players, f);

    for (int i = 0; i < sim_config.n_players; i++) {
        if (tournament.players[i].state != FINISHED) {
            tournament.players[i].state = WAITING;
        }
        tournament.players[i].wait_ticket = 0;
    }

    fclose(f);
    printf("[Snapshot] Estado restaurado exitosamente desde %s\n", sim_config.snapshot_path);
}