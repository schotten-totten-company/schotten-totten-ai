#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#define NB_MILESTONES 9
#define NB_SLOT_IN_MILESTONE 7
#define NB_CARDS_IN_HAND 6
#define NB_CARDS_ON_BOARD (NB_MILESTONES*NB_SLOT_IN_MILESTONE)

typedef enum {
    TOP,
    BOTTOM,
    NONE,
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



typedef struct {
    uint8_t moves[Move];
    size_t size;
} SchottenValidMoves;


void generate_cards(uint8_t deck[NB_CARDS_ON_BOARD]) {
    for (size_t i=0; i < NB_MILESTONES; i++) {
        for(size_t j=0; j < NB_SLOT_IN_MILESTONE; j++) {
            deck[i*NB_SLOT_IN_MILESTONE + j] = i*10 + j;
        }
    }
}

void shuffle(uint8_t deck[NB_CARDS_ON_BOARD]) {
    for (size_t i=0; i<NB_CARDS_ON_BOARD; i++){
        size_t from_idx = rand(NB_CARDS_ON_BOARD);
        size_t to_idx = rand(NB_CARDS_ON_BOARD);
        uint8_t tmp = deck[from_idx];
        deck[from_idx] = deck[to_idx];
        deck[to_idx] = tmp;    
    }
}

void generate_deck(Schotten * game) {
    generate_cards(&game->deck);
    game->deck = NB_CARDS_ON_BOARD;
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
    Schotten * game = (Schotten * )calloc(1, sizeof(Schotten))
    if(!game) {
        println("Not enough memory!");
        exit(-1);
    }
    generate_deck(game);
    draw_n_card(game, &game->top_hand, NB_CARDS_IN_HAND);
    draw_n_card(game, &game->bottom_hand, NB_CARDS_IN_HAND);
}

SchottenValidMoves * get_valid_moves(Schotten * game) {
    assert(game->player == TOP || game->player == BOTTOM)

    size_t idx_start, idx_end;
    uint8_t * hand;

    if (game->player == TOP) {
        idx_start = 0;
        idx_end = 3;
        hand = &game->top_hand;
    } else { // bottom
        idx_start = 4;
        idx_end = NB_SLOT_IN_MILESTONE;
        hand = &game->bottom_hand;
    }

    game->nb_milestone_choice = 0;

    for(size_t i=0; i<NB_MILESTONES; i++) {
        if(game->board[i*NB_SLOT_IN_MILESTONE + 3] == 0 
            && game->board[i*NB_SLOT_IN_MILESTONE + idx_end] == 0) {
                game−>milestone_choice[game->nb_milestone_choice] = i;
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
            game->moves[game->nb_moves]] = {
                .card_idx = i;
                .milestone_idx = j;
            };
            game->nb_moves++;
        }
    }

    return moves;
}