#include <time.h>

#include "game-manager.h"
#include "mcts.h"

int main() {
    srand(time(NULL)); 
    GameManager * game_manager = new_game_manager();
    register_strategy(game_manager, run_mcts);
    register_strategy(game_manager, run_mcts);
    run_game(game_manager, 1);
}
