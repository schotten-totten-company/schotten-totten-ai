#ifndef _H_GAME_MANAGER
#define _H_GAME_MANAGER

#include <stdint.h>
#include "schotten-game.h"

typedef size_t (*Strategy)(Schotten *, uint);

typedef struct {
    Schotten * game;
    Strategy player1;
    Strategy player2;
} GameManager;

GameManager * new_game_manager();
void register_strategy(GameManager * game_manager, Strategy strategy);
void run_game(GameManager * game_manager, uint time_budget_s);


#endif //_H_GAME_MANAGER