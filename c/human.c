#include "human.h"

#include <stdio.h>
#include <stdbool.h>

size_t run_human(Schotten * game, uint time_budget_s) {
    while (true) {
        Move move = {0};
        print_game(game);
        printf("What is the index of card do you want to play (starting at 0)?");
        while (scanf("%u", &move.card_idx) != 1) {
            while(getchar() != '\n');
            printf("Wrong input. What is the index of card do you want to play (starting at 0)?");
        }
        printf("What is the index of milestone where you want to play this card(starting at 0)?");
        while (scanf("%u", &move.milestone_idx) != 1) {
            while(getchar() != '\n');
            printf("Wrong input. What is the index of milestone where you want to play this card(starting at 0)?");
        }
        int move_idx = find_move_idx(game, move);
        if(move_idx != -1) {
            free(game);
            return move_idx;
        } else {
            printf("This move is not valid!\n");
        }
    }
}