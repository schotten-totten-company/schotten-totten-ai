#include <time.h>

#include "game-manager.h"
#include "human.h"
#include "mcts.h"

int main() {
    srand(time(NULL)); 
    GameManager * game_manager = new_game_manager();
    register_strategy(game_manager, run_mcts_with_determinization);
    register_strategy(game_manager, run_mcts_with_determinization);
    run_game(game_manager, 1);
    free_game_manager(game_manager);
}
