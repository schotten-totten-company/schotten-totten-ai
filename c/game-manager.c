#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "game-manager.h"

GameManager * new_game_manager() {
    GameManager * manager = (GameManager *)calloc(1, sizeof(GameManager));
    if(!manager) {
        printf("Not enough memory!\n");
        exit(-1);
    }
    manager->game = new_game();
    return manager;
}

void register_strategy(GameManager * game_manager, Strategy strategy){
    if(game_manager->player1 == NULL) {
        game_manager->player1 = strategy;
    } else {
        assert(game_manager->player2 == NULL);
        game_manager->player2 = strategy;
    }
}

void run_game(GameManager * game_manager, uint time_budget_s) {
    assert(game_manager->player1 != NULL && game_manager->player2 != NULL);
    while(true) {
        // print_game(game_manager->game);

        size_t move_idx;
        Schotten * new_game = clone_game(game_manager->game);
        if(game_manager->game->player == TOP) {
            move_idx = game_manager->player1(new_game, time_budget_s);
        } else {
            assert(game_manager->game->player == BOTTOM);
            move_idx = game_manager->player2(new_game, time_budget_s);
        }
        // free(new_game);

        Player winner = apply_move(game_manager->game, move_idx);
        if(winner == TOP) {
            printf("### GAME ENDED ###\n");
            print_game(game_manager->game);
            printf("### TOP WIN ###\n");
            break;
        }
        if(winner == BOTTOM) {
            printf("### GAME ENDED ###\n");
            print_game(game_manager->game);
            printf("### BOTTOM WIN ###\n");
            break;   
        }
        printf("\n\n@@@@@@@@@@@@@ New Turn @@@@@@@@@@@@@@@\n\n");

    }
}