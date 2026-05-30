//
// Created by franc on 21-05-2026.
//

#ifndef ELO_H
#define ELO_H

#include "player.h"

// match_result: 0 = Empate, 1 = Gana p1, 2 = Gana p2
void update_elo(Player* p1, Player* p2, int match_result);

#endif
