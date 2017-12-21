#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "schotten-game.h"


void compute_valid_moves(Schotten * game);

Schotten * clone_game(Schotten * game) {
    Schotten * game_clone = (Schotten *)calloc(1, sizeof(Schotten));
    if(!game) {
        printf("Not enough memory!\n");
        exit(-1);
    }
    memcpy(game_clone, game, sizeof(Schotten));
    return game_clone;
}

void generate_cards(uint8_t deck[NB_CARDS_ON_BOARD]) {
    for (size_t i=0; i < NB_MILESTONES; i++) {
        for(size_t j=0; j < NB_CARDS_IN_HAND; j++) {
            deck[i*NB_CARDS_IN_HAND + j] = (i + 1)*10 + j + 1;
        }
    }
}

int rand_int(int start, int end) {
/* return a random number between 0 and limit exclusive.
 */

    int range = end - start;

    int divisor = RAND_MAX/range;
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > range - 1);

    return retval + start;
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

    compute_valid_moves(game);

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
            game->moves[game->nb_moves].card_idx = game->card_choice[i];
            game->moves[game->nb_moves].milestone_idx = game->milestone_choice[j];
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

// test sort at insert
void sort_side(uint8_t side[NB_CARDS_IN_SIDE]) {
    if (side[1] < side[0]) {
        uint8_t tmp = side[0];
        side[0] = side[1];
        side[1] = tmp;
    }
    if (side[2] < side[1]) {
        uint8_t tmp = side[1];
        side[1] = side[2];
        side[2] = tmp;
    }
    if (side[1] < side[0]) {
        uint8_t tmp = side[0];
        side[0] = side[1];
        side[1] = tmp;
    }
}

uint evaluate_side(uint8_t side[NB_CARDS_IN_SIDE]) {
    uint8_t numbers[NB_CARDS_IN_SIDE] = {side[0] / 10, side[1] / 10, side[2] / 10};
    uint8_t colors[NB_CARDS_IN_SIDE] = {side[0] % 10, side[1] % 10, side[2] % 10};

    sort_side(numbers);

    uint total = numbers[0] + numbers[1] + numbers[2];
    total += numbers[0] + 1 == numbers[1]  && numbers[1] + 1 == numbers[2]  ? 100 : 0; // suite
    total += colors[0] == colors[1] && colors[1] == colors[2] ? 200 :  0; // couleur
    total += numbers[0] == numbers[1] && numbers[1] == numbers[2] ? 250 : 0; // brelan

    return total;
}

Player wins_milestone(
    uint8_t top_side[NB_CARDS_IN_SIDE],
    uint8_t bottom_side[NB_CARDS_IN_SIDE],
    Player just_played_card
) {
    if (
        top_side[0] == 0
        || top_side[1] == 0
        || top_side[2] == 0
        || bottom_side[0] == 0
        || bottom_side[1] == 0
        || bottom_side[2] == 0
    ) {
        return NONE;
    }
    
    uint top_side_score = evaluate_side(top_side);
    uint bottom_side_score = evaluate_side(bottom_side);

    if (top_side_score == bottom_side_score){
        return just_played_card == TOP ? BOTTOM: TOP; // the first who have played the card wins
    } else if (top_side_score > bottom_side_score) {
        return TOP;
    } else {
        return BOTTOM;
    }
}
        
Player apply_move(Schotten * game, size_t move_idx) {
    assert(move_idx < game->nb_moves);
    Move move = game->moves[move_idx];
    size_t card_idx = move.card_idx;
    size_t milestone_idx = move.milestone_idx;
    size_t side_idx = 0;
    uint8_t * hand = NULL;

    if (game->player == TOP) {
        hand = game->top_hand;
        side_idx = 0;
    } else { // bottom
        assert(game->player == BOTTOM);
        hand = game->bottom_hand;
        side_idx = 4;
    }
        
    size_t j=side_idx;
    while (game->board[milestone_idx*NB_SLOT_IN_MILESTONE +j] != 0){
        j += 1;
    }

    // play card
    game->board[milestone_idx*NB_SLOT_IN_MILESTONE +j] = hand[card_idx];

    // draw card
    if (game->deck_size == 0)
        hand[card_idx] = 0;
    else
        hand[card_idx] = draw_card(game);
    

    // check if can reclaim milestone
    for (size_t i=0; i<NB_MILESTONES; i++) {
        if (game->board[i*NB_SLOT_IN_MILESTONE + 3] == 0) {
            game->board[i*NB_SLOT_IN_MILESTONE + 3] = wins_milestone(
                &game->board[i*NB_SLOT_IN_MILESTONE + 4], 
                &game->board[i*NB_SLOT_IN_MILESTONE],
                game->player
            );
        }
    }

    // check if finished (current player wins)
    uint strike = 0;
    uint total = 0;
    for (size_t i=0; i<NB_MILESTONES; i++) {
        if (game->board[i*NB_SLOT_IN_MILESTONE + 3] == TOP) {
            total += 1;
            strike += 1;
        } else {
            strike = 0;
        }
        if (strike == 3) {
            break;
        }
    }
    if (strike == 3 || total == 5) {
        return TOP;
    }


    strike = 0;
    total = 0;
    for (size_t i=0; i<NB_MILESTONES; i++) {
        if (game->board[i*NB_SLOT_IN_MILESTONE + 3] == BOTTOM) {
            total += 1;
            strike += 1;
        } else {
            strike = 0;
        }
        if (strike == 3) {
            break;
        }
    }
    if (strike == 3 || total == 5) {
        return BOTTOM;
    }

    game->player = game->player == TOP ? BOTTOM : TOP;

    compute_valid_moves(game);

    return NONE;
}


/*int main() {
    srand(time(NULL)); 
    Schotten * game = new_game();
    while(true) {
        print_game(game);
        compute_valid_moves(game);
        int move_idx = rand_int(0, game->nb_moves);
        Player winner = apply_move(game, move_idx);
        if(winner == TOP) {
            print_game(game);
            printf("### TOP WIN ###\n");
            break;
        }
        if(winner == BOTTOM) {
            print_game(game);
            printf("### BOTTOM WIN ###\n");
            break;   
        }
        printf("\n\n@@@@@@@@@@@@@ New Turn @@@@@@@@@@@@@@@\n\n");

    }
}*/