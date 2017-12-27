#include "human.h"

#include <stdio.h>
#include <stdbool.h>

size_t run_human(Schotten * game, uint time_budget_s) {
    while (true) {
        Move move = {0};
        print_game(game);
        printf("What is the index of card do you want to play (starting at 0)?");
        scanf("%u", &move.card_idx);
        printf("What is the index of milestone where you want to play this card(starting at 0)?");
        scanf("%u", &move.milestone_idx);
        int move_idx = find_move_idx(game, move);
        if(move_idx != -1) {
            return move_idx;
        } else {
            printf("Wrong move!\n");
        }
    }
}