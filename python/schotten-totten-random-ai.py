

#
#  Simple Pirate worker
#  Connects REQ socket to tcp://*:5556
#  Implements worker part of LRU queueing
#
#   Author: Daniel Lundin <dln(at)eintr(dot)org>
#


from random import randint
import time
import zmq
import binascii
import time
import random
import struct


class DataStream(bytearray):

    def append(self, v, fmt='>B'):
        self.extend(struct.pack(fmt, v))



def print_board(board) :
    toto = ""
    for i in range(9):
        if int(board[i*7 + 3]) == 2:
            toto += "MM "
        else:
            toto += "   "
    print(toto)

    for i in range(7):
        line = ""
        for j in range(9):
            if i == 3:
                if int(board[j*7 + i]) == 0:
                    line += "MM "
                else:
                    line += "   "
            else: 
                line += str(int(board[j*7 + i])).ljust(2,'0') + " "
        print(line)
    
    tata = ""
    for i in range(9):
        if int(board[i*7 + 3]) == 1:
            tata += "MM "
        else:
            tata += "   "
    print(tata)
    
    hand = "    "
    for i in range(6):
        hand += str(int(board[9*7+ i])).ljust(2,'0') + " "
    print(hand)


context = zmq.Context(1)
worker = context.socket(zmq.REQ)

identity = "%04X-%04X" % (randint(0, 0x10000), randint(0, 0x10000))
worker.setsockopt_string(zmq.IDENTITY, identity)
worker.connect("tcp://localhost:5555")

print("I: (%s) worker ready" % identity)

# empty keys means that we don't have game id nor client id
worker.send_multipart([b"",b""])

def getValueFromUser(prompt, validValues, initialValue = -1):
    choice = initialValue
    while choice not in validValues:    
        print(prompt)
        try:
            choice = int(input())
        except ValueError:
            pass
    return choice

cycles = 0
testing_milestones = True
to_test = []
while True:
    print("Wating for your turn ...")
    [playerKey, gameKey, errorCode, board] = worker.recv_multipart()
    if not playerKey:
        break

    print("\n@@@@@@@@@ NEW TURN @@@@@@@@@@@@@@@@\n")
    print("Player Key : ", binascii.hexlify(playerKey).decode())
    print("Game Key : ", binascii.hexlify(gameKey).decode())
    errorCode = int(errorCode[0])
    if errorCode == 1:
        print("##########################")
        print_board(board)
        print("##########################")
        print("You won!")
        break
    if errorCode == 2:
        print("##########################")
        print_board(board)
        print("##########################")
        print("You lost!")
        break
    if errorCode == 3:
        print("Coup invalide!")
        break
        errorCode = 0

    if errorCode == 0:
        print("##########################")
        print_board(board)
        print("##########################")
    
    moveType = 2

    cardId = 255
    milestoneId = -1

    if not testing_milestones:
        to_test = [i for i in range(9) if board[i*7 + 3] == 0 and all([board[i*7 + j] != 0 for j in range(4, 7)])]
        print("to_test", to_test)

    if len(to_test) > 0:
        moveType = 1
        milestoneId = random.choice(to_test)
        to_test.remove(milestoneId)
        testing_milestones = True
    else: 
        milestoneIdChoice = [i for i in range(9)  if board[i*7 + 3] == 0 and any([board[i*7 + j] == 0 for j in range(4, 7)])]
        if len(milestoneIdChoice) == 0:
           milestoneIdChoice = [i for i in range(9)  if any([board[i*7 + j] == 0 for j in range(4, 7)])] 
        milestoneId = random.choice(milestoneIdChoice)
        testing_milestones = False
    
    cardIdChoice = [i for i in range(6) if board[9*7+ i] != 0]
    if len(cardIdChoice) > 0:
        cardId = random.choice(cardIdChoice)
    else: # end of game
        moveType = 1
        to_test = [i for i in range(9) if board[i*7 + 3] == 0 and all([board[i*7 + j] != 0 for j in range(4, 7)])]
        milestoneId = random.choice(to_test)



    if moveType == 1:
        print("Reclaiming milestone:", milestoneId)
    else:
        print("Playing card", cardId, "to milestone", milestoneId)
    

    #time.sleep(1)
    
    buffer = DataStream()
    buffer.append(moveType)
    buffer.append(cardId)
    buffer.append(milestoneId)

    worker.send_multipart([playerKey, gameKey, buffer])

   




