Byte=UInt8
ByteArray=Array{Byte, 1}
ByteMatrix=Array{Byte, 2}



const NB_MILESTONES = 9
const NB_SLOT_IN_MILESTONE = 7
const NB_CARDS_IN_HAND = 6
const NB_CARDS_ON_BOARD = NB_MILESTONES*NB_SLOT_IN_MILESTONE
const BOARD_SIZE = (NB_SLOT_IN_MILESTONE, NB_MILESTONES)

struct Schotten
    prev_player_hand::ByteArray
    board::ByteMatrix
    next_player_hand::ByteArray
    deck::ByteArray
    function Schotten(prev_player_hand::ByteArray, board::ByteMatrix, next_player_hand::ByteArray, deck::ByteArray)
        @assert length(prev_player_hand) == NB_CARDS_IN_HAND
        @assert length(next_player_hand) == NB_CARDS_IN_HAND
        @assert size(board) == BOARD_SIZE
        # TODO: validate card completion and deck

        new(prev_player_hand, board, next_player_hand, deck)
    end
end

generatedeck() = shuffle(Byte[i*10 + j for i=1:NB_MILESTONES for j=1:NB_SLOT_IN_MILESTONE]::ByteArray)

function Schotten()
    deck = generatedeck()
    prev_player_hand = deck[1:NB_CARDS_IN_HAND]
    deck = deck[NB_CARDS_IN_HAND+1:end]
    board = zeros(Byte, BOARD_SIZE)
    next_player_hand = deck[1:NB_CARDS_IN_HAND]
    deck = deck[NB_CARDS_IN_HAND+1:end]
    Schotten(
        prev_player_hand,
        board,
        next_player_hand,
        deck
    )
end

# function Schotten(player_view)
#     deck = Game.generate_deck()
#     board = copy(player_view[1:NB_CARDS_ON_BOARD])
#     next_player_hand = copy(board[NB_CARDS_ON_BOARD+1:end])
#     deck = Byte[card for card in deck if card not in player_view]
#     prev_player_hand = deck[1:NB_CARDS_IN_HAND]
#     deck = [NB_CARDS_IN_HAND+1:end]
#     Schotten(
#         prev_player_hand,
#         board,
#         next_player_hand,
#         deck
#     )
# end



function getvalidmoves(game::Schotten)
    milestone_choice = [i for i=1:NB_MILESTONES  if game.board[4,i] == 0 && countnz(game.board[5:end,i]) < 3]
    card_choice = [i for i=1:NB_CARDS_IN_HAND if  game.next_player_hand[i] != 0]
    return [(card, milestone) for card in card_choice for milestone in milestone_choice]
end

function applymove(game, move)
    card_idx, milestone_idx = move

    # swap board
    new_board = [game.board[j, i] for j=[5, 6, 7, 4, 1, 2, 3],i=1:9]

    ## swap milestones
    for i=1:9
        milestone = new_board[4, i]
        if milestone == 1
            new_board[4, i] = 2
        elseif milestone == 2
            new_board[4, i] = 1
        else
            new_board[4, i] = 0 
        end
    end

    j=1
    while new_board[j, milestone_idx] != 0
        j += 1
    end

    assert(j < 4)
 


    # play card
    new_board[j, milestone_idx] = game.next_player_hand[card_idx]

    new_prev_player_hand = copy(game.next_player_hand)
    new_deck = copy(game.deck)

    # draw card
    if length(new_deck) == 0
        new_prev_player_hand[card_idx] = 0
    else
        drawn_card = new_deck[1]
        new_deck = new_deck[2:end]
        new_prev_player_hand[card_idx] = drawn_card
    end

    # check if can reclaim milestone
    for i in 1:9
        if new_board[4, i] == 0
            new_board[4, i] = winsMilestone(new_board[5:7, i], new_board[1:3, i], i == milestone_idx)
        end
    end

    # check if finished (current player wins)
    strike = 0
    total = 0
    for i in 1:9
        if new_board[4,i] == 1
            total += 1
            strike += 1
        else
            strike = 0
        end
        if strike == 3
            break
        end
    end
    precWon = strike == 3 || total == 5

    strike = 0
    total = 0
    for i in 1:9
        if new_board[4, i] == 2
            total += 1
            strike += 1
        else
            strike = 0
        end
        if strike == 3
            break
        end        
    end
    curWon = strike == 3 || total == 5

    return Schotten(new_prev_player_hand, new_board, copy(game.prev_player_hand), new_deck), curWon, precWon
end

function Base.show(io::IO, game::Schotten)
    hand = "    "
    for i=game.prev_player_hand
        hand *= lpad(string(i), 2,'0') * " "
    end
    println(io, hand)

    toto = ""
    for i=game.board[4, :]
        if i == 2
            toto *= "MM "
        else
            toto *= "   "
        end
    end
    println(io, toto)

    for i=1:NB_SLOT_IN_MILESTONE
        line = ""
        for j=1:NB_MILESTONES
            if i == 4
                if game.board[i,j] == 0
                    line *= "MM "
                else
                    line *= "   "
                end
            else 
                line *= lpad(string(game.board[i,j]), 2,'0') * " "
            end
        end
        println(io, line)
    end

    toto = ""
    for i=game.board[4, :]
        if i == 1
            toto *= "MM "
        else
            toto *= "   "
        end
    end
    println(io, toto)

    hand = "    "
    for i=game.next_player_hand
        hand *= lpad(string(i), 2,'0') * " "
    end
    println(io, hand)

    deck = "\n"
    for card=game.deck
        deck *= lpad(string(card), 2,'0') * " "
    end
    println(deck)
end

function count(array, elem)
    count = 0
    for a in array
        count += a == elem ? 1 : 0 
    end
end

function evaluateside(side)
    numbers = sort([div(i, 10) for i=side])
    colors = [i % 10 for i=side]
    total = sum(numbers)
    total += sum([x - y for (x,y)=zip(numbers[2:end], numbers[1:end-1])]) == length(numbers) - 1 ? 100 : 0 # suite
    total += count(colors, colors[1]) == length(colors) ? 100 :  0 # couleur
    total += count(numbers, numbers[1]) == length(numbers) ? 100 : 0 # brelan
    return total
end


function winsMilestone(prevSide, currSide, justPlayedCard)
    if !(all([el != 0 for el=prevSide]) && all([el != 0 for el=currSide]))
         return 0
    end

    prevSideScore = evaluateside(prevSide)
    currSideScore = evaluateside(currSide)

    if prevSideScore == currSideScore
        return justPlayedCard ? 1 : 2 
    elseif prevSideScore > currSideScore
        return 1
    else
        return 2
    end
end

function testgame()
    game = Schotten()
    wins = false
    while !wins
        println(game)
        moves = getvalidmoves(game)
        game, topWon, bottomWon =  applymove(game, rand(moves))
        if topWon
            println(game)
            println("### TOP WIN ###")
            break
        end
        if bottomWon
            println(game)
            println("### BOTTOM WIN ###")
            break
        end
        # sleep(1)
        println("\n\n@@@@@@@@@@@@@ New Turn @@@@@@@@@@@@@@@\n")
    end
end