import random
import math
import time

from game import Game
import numpy as np

class Node:
    C = 1
    def __init__(self, game, parent, player, move):
        self.game_state = game
        self.N = 0
        self.W = 0
        self.parent = parent
        self.move = move
        self.player = player
        self.children = [] # not sure it is useful

    def naive_rollout(self):
        topWon = False
        bottomWon = False
        player = self.player
        game = self.game_state
        while not (topWon or bottomWon):
            actions = game.get_valid_moves()
            action = random.choice(actions)
            game, topWon, bottomWon = game.apply_move(action)
            player = 1 - player
        
        self.N = 1
        self.W = 1 if player == 1 and topWon or player == 0 and bottomWon else -1

    def to_string(self, tabul):
        str = tabul + "Node G:{} A:{} N: {}, W: {}\n".format(self.game_state, self.move, self.N, self.W)
        for child in self.children:
            str+= child.to_string(tabul + "  ")
        return str
    
    def __str__(self):
        return self.to_string("")
    
    def back_propagate(self, n, w):
        self.W += w
        self.N += n
        if self.parent != None:
            self.parent.back_propagate(n, w)
    
    def expand(self):
        actions = self.game_state.get_valid_moves()
        if len(actions) == 0:
            return
        for a in actions:
            child_game,_,_ = self.game_state.apply_move(a)
            child_node = Node(child_game, self, 1 - self.player, a)
            child_node.naive_rollout()
            self.back_propagate(child_node.N, child_node.W)
            self.children.append(child_node)

    def select(self):
        if len(self.children) == 0:
            return self

        sign = -1 if self.player == 1 else 0

        UCT_scores = [sign * child.W / child.N + Node.C * math.sqrt(math.log(self.N)/child.N) for child in self.children]
        chosen_child_idx = np.argmax(UCT_scores)
        return self.children[chosen_child_idx].select()

    def run_mcts(node, time_budget=10):
        start_time = time.process_time()
        nb_selected_node = 0
        while time.process_time() - start_time < time_budget:
            selected_node = node.select()
            nb_selected_node += 1
            selected_node.expand()
        print("Selections:", nb_selected_node)
        # print(node)
        return max(node.children, key=lambda child: child.N).move
    
    def new_mcts_root(game):
        return Node(game, None, 0, None)

if __name__ == "__main__":
    game = Game.new()
    game.print()
    mcts = Node.new_mcts_root(game)
    move = Node.run_mcts(mcts)
    print("Choosen move:", move)



