
#include "elo.h"
#include <math.h>
#include "config.h"

void update_elo(Player* p1, Player* p2, int match_result) {

    float K = (float)sim_config.k_elo;
    float elo_a = (float)p1->elo;
    float elo_b = (float)p2->elo;

    float exponent_p1 = (elo_b - elo_a) / 400.0f;
    float expected_p1 = 1.0f / (1.0f + powf(10.0f, exponent_p1));

    float exponent_p2 = (elo_a - elo_b) / 400.0f;
    float expected_p2 = 1.0f / (1.0f + powf(10.0f, exponent_p2));

    if (match_result == 0) { // Empate
        p1->elo = (int)(elo_a + K * (0.5f - expected_p1));
        p2->elo = (int)(elo_b + K * (0.5f - expected_p2));
        p1->draws++;
        p2->draws++;
        
    } else if (match_result == 1) { // Gana P1
        p1->elo = (int)(elo_a + K * (1.0f - expected_p1));
        p2->elo = (int)(elo_b + K * (0.0f - expected_p2));
        p1->wins++;
        p2->losses++;
        
    } else if (match_result == 2) { // Gana P2
        p1->elo = (int)(elo_a + K * (0.0f - expected_p1)); 
        p2->elo = (int)(elo_b + K * (1.0f - expected_p2));
        p1->losses++;
        p2->wins++;
    }
}