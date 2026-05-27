//
// Created by franc on 21-05-2026.
//

#ifndef MATCHMAKER_H
#define MATCHMAKER_H

#include <pthread.h>
#include "player.h"
#include "board.h"

// Estructura que agrupa todo el estado del torneo
typedef struct {
    Player *players;      // Arreglo dinamico de N jugadores
    Board *boards;        // Arreglo dinamico de K tableros

    // --- PRIMITIVAS DE SINCRONIZACION ---

    // Mutex principal: protege el acceso a la cola de espera y el estado de los tableros
    pthread_mutex_t match_mutex;

    // Mutex especifico para el ELO: previene condiciones de carrera al actualizar puntajes
    pthread_mutex_t elo_mutex;

    // Variable de condicion: permite a los jugadores "dormir" hasta que un tablero se libere
    // o hasta que llegue un oponente valido (minimiza el busy-waiting)
    pthread_cond_t state_changed;

    // --- ESTADO DEL SISTEMA ---
    int shutdown_flag;    // 1 si recibimos SIGINT, 0 si todo esta normal
    int active_players;   // Cantidad de jugadores que siguen vivos en el sistema
} MatchmakerState;

// Declaracion de la variable global del torneo
extern MatchmakerState tournament;

// Prototipos
void init_matchmaker();
void destroy_matchmaker();

#endif // MATCHMAKER_H
