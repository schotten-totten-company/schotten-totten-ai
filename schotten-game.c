#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NB_MILESTONES 9
#define NB_SLOT_IN_MILESTONE 7
#define NB_CARDS_IN_HAND 6
#define NB_CARDS_ON_BOARD (NB_MILESTONES*NB_SLOT_IN_MILESTONE)

typedef enum {
    NONE,
    TOP,
    BOTTOM,
} Player;

typedef struct {
    size_t card_idx;
    size_t milestone_idx;
} Move;

typedef struct {
    uint8_t top_hand[NB_CARDS_IN_HAND];
    uint8_t board[NB_CARDS_ON_BOARD];
    uint8_t bottom_hand[NB_CARDS_IN_HAND];
    uint8_t deck[NB_CARDS_ON_BOARD];
    size_t deck_size;
    Player player;

    size_t nb_milestone_choice;
    uint8_t milestone_choice[NB_MILESTONES];

    size_t nb_card_choice;
    uint8_t card_choice[NB_CARDS_IN_HAND];
    
    size_t nb_moves;
    Move moves[NB_CARDS_IN_HAND*NB_MILESTONES];
} Schotten;


void generate_cards(uint8_t deck[NB_CARDS_ON_BOARD]) {
    for (size_t i=0; i < NB_MILESTONES; i++) {
        for(size_t j=0; j < NB_SLOT_IN_MILESTONE; j++) {
            deck[i*NB_SLOT_IN_MILESTONE + j] = (i + 1)*10 + j + 1;
        }
    }
}

int rand_int(int min, int max) {
    assert(max > min);
    int range = max - min;
    assert(range < RAND_MAX);
    return rand() % range + min;
}

void shuffle(uint8_t deck[NB_CARDS_ON_BOARD]) {
    for (size_t i=0; i<NB_CARDS_ON_BOARD; i++){
        size_t from_idx = rand_int(0, NB_CARDS_ON_BOARD);
        size_t to_idx = rand_int(0, NB_CARDS_ON_BOARD);
        uint8_t tmp = deck[from_idx];
        deck[from_idx] = deck[to_idx];
        deck[to_idx] = tmp;    
    }
}

void generate_deck(Schotten * game) {
    generate_cards(game->deck);
    game->deck_size = NB_CARDS_ON_BOARD;
}

uint8_t draw_card(Schotten * game) {
    assert(game->deck_size > 0);
    game->deck_size--;
    return game->deck[game->deck_size];
}

void draw_n_card(Schotten * game, uint8_t * hand, size_t nb_cards) {
    assert(nb_cards <= game->deck_size);
    for(size_t i=0; i< nb_cards; i++) {
        hand[i] = draw_card(game);
    }
}

Schotten * new_game() {
    Schotten * game = (Schotten * )calloc(1, sizeof(Schotten));
    if(!game) {
        printf("Not enough memory!\n");
        exit(-1);
    }
    generate_deck(game);
    game->player = rand_int(1,3); //TOP OR BOTTOM
    draw_n_card(game, game->top_hand, NB_CARDS_IN_HAND);
    draw_n_card(game, game->bottom_hand, NB_CARDS_IN_HAND);

    return game;
}

void compute_valid_moves(Schotten * game) {
    assert(game->player == TOP || game->player == BOTTOM);

    size_t idx_start, idx_end;
    uint8_t * hand;

    if (game->player == TOP) {
        idx_start = 0;
        idx_end = 2;
        hand = game->top_hand;
    } else { // bottom
        idx_start = 4;
        idx_end = NB_SLOT_IN_MILESTONE-1;
        hand = game->bottom_hand;
    }

    game->nb_milestone_choice = 0;

    for(size_t i=0; i<NB_MILESTONES; i++) {
        if(game->board[i*NB_SLOT_IN_MILESTONE + 3] == 0 
            && game->board[i*NB_SLOT_IN_MILESTONE + idx_end] == 0) {
                game->milestone_choice[game->nb_milestone_choice] = i;
                game->nb_milestone_choice++;    
        }
    }


    game->nb_card_choice = 0;

    for(size_t i=0; i<NB_CARDS_IN_HAND; i++) {
        if(hand[i] != 0) {
            game->card_choice[game->nb_card_choice] = i;
            game->nb_card_choice++;    
        }
    }

    game->nb_moves = 0;
    for(size_t i=0; i<game->nb_card_choice; i++) {
        for(size_t j=0; j<game->nb_milestone_choice; j++) {
            game->moves[game->nb_moves].card_idx = i;
            game->moves[game->nb_moves].milestone_idx = j;
            game->nb_moves++;
        }
    }
}

void print_game(Schotten * game) {
    printf("    ");
    for (size_t i=0; i<NB_CARDS_IN_HAND; i++) {
        printf("%02u ", game->top_hand[i]);
    }
    printf("\n");

    for (size_t i=0; i<NB_MILESTONES; i++) {
        printf(game->board[i*NB_SLOT_IN_MILESTONE + 3] == TOP ? "MM " : "   "); 
    }
    printf("\n");

    for(size_t  i=0; i<NB_SLOT_IN_MILESTONE; i++) {
        for(size_t j=0; j<NB_MILESTONES; j++) {
            if (i==3) {
                printf(game->board[j*NB_SLOT_IN_MILESTONE + i]==0?"MM ":"   ");
            } else {
                printf("%02u ", game->board[j*NB_SLOT_IN_MILESTONE + i]);
            }
        }
        printf("\n");
    }

    for (size_t i=0; i<NB_MILESTONES; i++) {
        printf(game->board[i*NB_SLOT_IN_MILESTONE + 3] == BOTTOM ? "MM " : "   "); 
    }
    printf("\n");

    printf("    ");
    for (size_t i=0; i<NB_CARDS_IN_HAND; i++) {
        printf("%02u ", game->bottom_hand[i]);
    }
    printf("\n\n");

    for (size_t i=0; i<game->deck_size; i++) {
        printf("%02u ", game->deck[i]);
    }
    printf("\n");
}

char * player_as_str(Player player) {
    switch(player) {
        case TOP: return "TOP";
        case BOTTOM: return "BOTTOM";
        case NONE: return "NONE";
        default: assert(false);
    }
}

void print_moves(Schotten * game) {
    for (size_t i=0; i<game->nb_moves; i++) {
        printf("(%u, %u)", game->moves[i].card_idx, game->moves[i].milestone_idx);
    }
    printf("\n");
}

int main() {
    Schotten * game = new_game();
    print_game(game);
    printf("Player: %s\n\n",player_as_str(game->player));
    compute_valid_moves(game);
    print_moves(game);
}