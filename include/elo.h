//
// Created by franc on 21-05-2026.
//

#ifndef ELO_H
#define ELO_H

typedef struct {
int K;
int S;
int E;
int new_elo;

} Elo;

void* elo_calculator(void* arg);

#endif
