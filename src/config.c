//
// Created by franc on 21-05-2026.
//

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config sim_config;

void load_config(const char *filename) { // esta funcion "parsea" el .env para que sus datos puedan ser leidos
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error abriendo el archivo de configuracion .env");
        exit(EXIT_FAILURE);
    }

    char line[256];
    // Leer el archivo linea por linea
    while (fgets(line, sizeof(line), file)) {
        char key[128];
        char value[128];

        // Ignorar lineas vacias o comentarios
        if (line[0] == '\n' || line[0] == '#') continue;

        // Parsear el formato CLAVE=VALOR
        if (sscanf(line, "%127[^=]=%127s", key, value) == 2) {
            if (strcmp(key, "N_PLAYERS") == 0) {
                sim_config.n_players = atoi(value);
            } else if (strcmp(key, "K_BOARDS") == 0) {
                sim_config.k_boards = atoi(value);
            } else if (strcmp(key, "K_ELO") == 0) {
                sim_config.k_elo = atoi(value);
            } else if (strcmp(key, "MAX_ELO_DIFF") == 0) {
                sim_config.max_elo_diff = atoi(value);
            } else if (strcmp(key, "TURN_DELAY_MS") == 0) {
                sim_config.turn_delay_ms = atoi(value);
            } else if (strcmp(key, "REENTER_PROBABILITY") == 0) {
                sim_config.reenter_probability = atof(value);
            } else if (strcmp(key, "SNAPSHOT_PATH") == 0) {
                strncpy(sim_config.snapshot_path, value, sizeof(sim_config.snapshot_path) - 1);
                sim_config.snapshot_path[sizeof(sim_config.snapshot_path) - 1] = '\0';
            }
        }
    }
    fclose(file);
}
