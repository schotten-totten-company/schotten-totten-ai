#include "schotten-game.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "mcts.h"

#define C 1
#define NB_DETERMINIZATIONS (10)

typedef struct Node {
    Schotten * game_state;
    uint N;
    int W;
    struct Node * parent;
    size_t move_idx;
    Player player;
    struct Node * children[NB_CARDS_IN_HAND*NB_MILESTONES]; // try to store children directly
    size_t nb_children;
} Node;

Node * new_node(
    Schotten * game,
    Node * parent,
    Player player,
    size_t move_idx
) {
    Node * node = (Node *)calloc(1, sizeof(Node));
    if(!node) {
        printf("Not enough memory!\n");
        exit(-1);
    }
    node->game_state = game;
    node->parent = parent;
    node->player = player;
    node->move_idx = move_idx;
    return node;
}

void naive_rollout(Node * node, Player winner_already) {
    Player winner = winner_already;
    Schotten game = {0}; //TODO: try with dyn allocation and static perf everywhere
    memcpy(&game, node->game_state, sizeof(Schotten));
    while(winner == NONE) {
        compute_valid_moves(&game);
        winner = apply_move(&game, rand_int(0, game.nb_moves));
    }
    
    node->N = 1;
    node->W = node->player == winner ? 1:-1;
}

void back_propagate(Node * node, uint n, int w) {
    node->W += w;
    node->N += n;
    if(node->parent != NULL) {
        back_propagate(node->parent, n, w);
    }
}

void expand(Node * node) {
    compute_valid_moves(node->game_state);
    if(node->game_state->nb_moves == 0) {
        return;
    }
    for(size_t i=0; i<node->game_state->nb_moves; i++) {
        Schotten * child_game = clone_game(node->game_state);
        Player winner = apply_move(child_game, i);
        Node * child_node = new_node(child_game, node, node->player, i);
        naive_rollout(child_node, winner);
        back_propagate(node, child_node->N, child_node->W);
        node->children[node->nb_children] = child_node;
        node->nb_children++;
    }
}

size_t find_max(double values[], size_t size) {
    assert(size > 0);
    size_t idx = 0;
    for(size_t i=1; i<size; i++) {
        if(values[i]>values[idx]) {
            idx = i;
        }
    }
    return idx;
}

Node *  select_node(Node * node) {
    if(node->nb_children == 0) {
        return node;
    }
    double sign = node->player == node->game_state->player ? 1 : -1;
    double UTC_scores[node->nb_children];
    for(size_t i =0; i<node->nb_children; i++) {
        Node * child = node->children[i];
        UTC_scores[i] = sign * (double)child->W / (double)child->N + C * sqrt(log((double)node->N)/(double)child->N);
    }
    size_t chosen_child_idx = find_max(UTC_scores, node->nb_children);
    return select_node(node->children[chosen_child_idx]);
}

Node * max_node(Node * nodes[], size_t nb_nodes) {
    assert(nb_nodes > 0);
    Node * max_node  = nodes[0];
    for(size_t i=0; i<nb_nodes; i++) {
        if(nodes[i]->N > max_node->N) {
            max_node = nodes[i];
        }
    }
    return max_node;
}

// wall time
double get_time_s() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec + 1e-9*time.tv_nsec;
}

void free_node(Node * node) {
    for(size_t i=0; i<node->nb_children; i++) {
        free_node(node->children[i]);
    }
    free(node->game_state);
    free(node);
}

void run_mcts_on_node(Node * node, uint time_budget_s) {
    uint nb_selected_node = 0;
    double start_time = get_time_s();
    while(get_time_s() - start_time < time_budget_s) {
        Node * selected_node = select_node(node);
        nb_selected_node++;
        expand(selected_node);
    }
    printf("Selections: %d\n", nb_selected_node);
}

size_t run_mcts(Schotten * game, uint time_budget_s) {
    Node * node = new_node(game, NULL, game->player, 0);
    run_mcts_on_node(node, time_budget_s);
    size_t move_idx = max_node(node->children, node->nb_children)->move_idx;
    free_node(node);
    return move_idx;
}

size_t run_mcts_with_determinization(Schotten * game, uint time_budget_s) {
    Node * nodes[NB_DETERMINIZATIONS] = {0};
    for(size_t i=0; i<NB_DETERMINIZATIONS;i++) {
        Schotten * game_clone = clone_game(game);
        determinize_game(game_clone);
        nodes[i] = new_node(game_clone, NULL, game->player, 0);
        run_mcts_on_node(nodes[i] ,time_budget_s);
    }
    Node * main_node = nodes[0];
    for(size_t j=1; j<NB_DETERMINIZATIONS; j++) {
        assert(nodes[j]->nb_children == main_node->nb_children);
        for(size_t i=0; i<main_node->nb_children;i++) {
            main_node->children[i]->N += nodes[j]->children[i]->N;
        }
    }
    size_t move_idx = max_node(main_node->children, main_node->nb_children)->move_idx;
    for(size_t i=0; i<NB_DETERMINIZATIONS;i++) {
        free_node(nodes[i]);
    }
    free(game);
    return move_idx;
}

/*int main() {
    Schotten * game = new_game();
    size_t move_idx = run_mcts(game, 10);
    Move move = game->moves[move_idx];
    printf("Choosen move: hand_idx->%d, milestone_idx->%d\n", move.card_idx, move.milestone_idx);
}*/