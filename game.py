import random
from copy import copy
import time

class Game:
    def __init__(self, prev_player_hand, board, next_player_hand, deck):
        self.prev_player_hand = prev_player_hand
        self.board = board
        self.next_player_hand = next_player_hand
        self.deck = deck

    def generate_deck():
        all_cards = [ i*10 + j for i in range(1,10) for j in range(1, 7)]
        random.shuffle(all_cards)
        return all_cards

    def new():
        deck = Game.generate_deck()
        board = [0 for i in range(9) for j in range(7)]
        prev_player_hand = deck[:6]
        deck = deck[6:]
        next_player_hand = deck[:6]
        deck = deck[6:]
        return Game(prev_player_hand, board, next_player_hand, deck)
    
    def new_rnd_from_player_view(player_view):
        deck = Game.generate_deck()
        board = copy(player_view[:9*7])
        next_player_hand = copy(board[9*7:])
        deck = [card for card in deck if card not in player_view]
        prev_player_hand = deck[:6]
        deck = deck[6:]
        return Game(prev_player_hand, board, next_player_hand, deck)


    def get_valid_moves(self):
        milestone_choice = [i for i in range(9)  if self.board[i*7 + 3] == 0 and any([self.board[i*7 + j] == 0 for j in range(4, 7)])]
        card_choice = [i for i in range(6) if self.next_player_hand[i] != 0]
        return [(card, milestone) for card in card_choice for milestone in milestone_choice]

    def apply_move(self, move):
        card_idx, milestone_idx = move
        # swap board
        new_board = [self.board[i*7 + j] for i in range(9) for j in [4, 5, 6, 3, 0, 1, 2]]

        # swap milestones
        for i in range(9):
            milestone = new_board[i*7 + 3]
            if milestone == 1:
                new_board[i*7 + 3] = 2
            elif milestone == 2:
                new_board[i*7 + 3] = 1
            else:
                new_board[i*7 + 3] = 0 
        
        for j in range(0, 3):
            if new_board[milestone_idx*7 + j] == 0:
                break
        
        # play card
        new_board[milestone_idx*7 + j] = self.next_player_hand[card_idx]

        new_prev_player_hand = copy(self.next_player_hand)
        new_deck = copy(self.deck)

        # draw card
        if len(new_deck) == 0:
            new_prev_player_hand[card_idx] = 0
        else:
            drawn_card = new_deck[0]
            new_deck = new_deck[1:]
            new_prev_player_hand[card_idx] = drawn_card

        # check if can reclaim milestone
        for i in range(9):
            if(new_board[i*7 + 3] == 0):
                new_board[i*7 + 3] = Game.winsMilestone(new_board[i*7 + 4:i*7 + 7], new_board[i*7:i*7 + 3], i == milestone_idx)

        # check if finished (current player wins)
        strike = 0
        total = 0
        for i in range(9):
            if new_board[i*7 + 3] == 1:
                total += 1
                strike += 1
            else:
                strike = 0
        precWon = strike == 3 or total == 5

        #bug on strike cf julia

        strike = 0
        total = 0
        for i in range(9):
            if new_board[i*7 + 3] == 2:
                total += 1
                strike += 1
            else:
                strike = 0
        curWon = strike == 3 or total == 5


        

        return Game(new_prev_player_hand, new_board, copy(self.prev_player_hand), new_deck), curWon, precWon

    def print(self):
        hand = "    "
        for i in range(6):
            hand += str(self.prev_player_hand[i]).ljust(2,'0') + " "
        print(hand)

        toto = ""
        for i in range(9):
            if self.board[i*7 + 3] == 2:
                toto += "MM "
            else:
                toto += "   "
        print(toto)

        for i in range(7):
            line = ""
            for j in range(9):
                if i == 3:
                    if int(self.board[j*7 + i]) == 0:
                        line += "MM "
                    else:
                        line += "   "
                else: 
                    line += str(int(self.board[j*7 + i])).ljust(2,'0') + " "
            print(line)

        tata = ""
        for i in range(9):
            if int(self.board[i*7 + 3]) == 1:
                tata += "MM "
            else:
                tata += "   "
        print(tata)

        hand = "    "
        for i in range(6):
            hand += str(int(self.next_player_hand[i])).ljust(2,'0') + " "
        print(hand)

        deck = "\n"
        for card in self.deck:
            deck += str(card).ljust(2,'0') + " "
        print(deck)
    
    def winsMilestone(prevSide, currSide, justPlayedCard):
        def evaluateSide(side):
            numbers = sorted([side[i] // 10 for i in range(len(side))])
            colors = [side[i] % 10 for i in range(len(side))]
            total = sum(numbers)
            total += 100 if sum([x - y for x,y in zip(numbers[1:], numbers[:-1])]) == len(numbers) - 1 else 0 # suite
            total += 200 if colors.count(colors[0]) == len(colors) else 0 # couleur
            total += 250 if numbers.count(numbers[0]) == len(numbers) else 0 # brelan
            return total

       
        if not (all([el != 0 for el in prevSide]) and all([el != 0 for el in currSide])):
            return 0

        prevSideScore = evaluateSide(prevSide)
        currSideScore = evaluateSide(currSide)

        if prevSideScore == currSideScore:
            return 1 if justPlayedCard else 2 

        elif prevSideScore > currSideScore:
            return 1

        else:
            return 2

def print(str):
    pass
            
if __name__ == "__main__":
    game = Game.new()
    wins = False
    while not wins:
        game.print()
        moves = game.get_valid_moves()
        game, topWon, bottomWon =  game.apply_move(random.choice(moves))
        if topWon:
            game.print()
            print("### TOP WIN ###")
            break
        if bottomWon:
            game.print()
            print("### BOTTOM WIN ###")
            break
        # time.sleep(1)
        print("\n\n@@@@@@@@@@@@@ New Turn @@@@@@@@@@@@@@@\n")




        
        

        


        
