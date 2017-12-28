#ifndef _H_MCTS
#define _H_MCTS

size_t run_mcts(Schotten * game, uint time_budget_s);
size_t run_mcts_with_determinization(Schotten * game, uint time_budget_s);

#endif //_H_MCTS