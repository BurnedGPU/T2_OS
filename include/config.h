
#ifndef CONFIG_H
#define CONFIG_H

typedef struct { // copia de las variables del .env
    int n_players;
    int k_boards;
    int k_elo;
    int max_elo_diff;
    int turn_delay_ms;
    float reenter_probability;
    char snapshot_path[256];
} Config;

extern Config sim_config; // sim_config se utiliza constantemente para rescatar cualquier valor del .env dentro de otras funciones

void load_config(const char *filename); // load_config se utiliza al principio del main()

#endif
