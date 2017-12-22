#ifndef _H_SCHOTTEN_GAME
#define _H_SCHOTTEN_GAME

#include <stdint.h>
#include <stdlib.h>

#define NB_MILESTONES 9
#define NB_CARDS_IN_SIDE 3
#define NB_SLOT_IN_MILESTONE 7
#define NB_CARDS_IN_HAND 6
#define NB_CARDS_ON_BOARD (NB_MILESTONES*NB_SLOT_IN_MILESTONE)
#define DECK_SIZE (NB_CARDS_IN_HAND*NB_MILESTONES)

typedef enum {
    NONE = 0,
    TOP = 1,
    BOTTOM = 2,
} Player;

typedef struct {
    size_t card_idx;
    size_t milestone_idx;
} Move;

typedef struct {
    uint8_t top_hand[NB_CARDS_IN_HAND];
    uint8_t board[NB_CARDS_ON_BOARD];
    uint8_t bottom_hand[NB_CARDS_IN_HAND];
    uint8_t deck[DECK_SIZE];
    size_t deck_size;
    Player player;

    size_t nb_milestone_choice;
    uint8_t milestone_choice[NB_MILESTONES];

    size_t nb_card_choice;
    uint8_t card_choice[NB_CARDS_IN_HAND];
    
    size_t nb_moves;
    Move moves[NB_CARDS_IN_HAND*NB_MILESTONES];
} Schotten;

void compute_valid_moves(Schotten * game);
Player apply_move(Schotten * game, size_t move_idx);
int rand_int(int start, int end);
Schotten * clone_game(Schotten * game);
Schotten * new_game();
void print_game(Schotten * game);


#endif //_H_SCHOTTEN_GAME