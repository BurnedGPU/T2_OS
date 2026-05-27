
#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int n_players;
    int k_boards;
    int k_elo;
    int max_elo_diff;
    int turn_delay_ms;
    float reenter_probability;
    char snapshot_path[256];
} Config;

extern Config sim_config;

void load_config(const char *filename);

#endif
