using StaticArrays

const Byte=UInt8
const ByteVector=Vector{Byte}
const ByteMatrix=Array{Byte, 2}



const NB_MILESTONES = 9
const NB_SLOT_IN_MILESTONE = 7
const NB_CARDS_IN_HAND = 6
const NB_CARDS_ON_BOARD = NB_MILESTONES*NB_SLOT_IN_MILESTONE
const BOARD_SIZE = (NB_SLOT_IN_MILESTONE, NB_MILESTONES)

@enum Player::Byte none top bottom

function Base.zero(::Type{Tuple{Int64, Int64}})
    return (0, 0)
end


mutable struct Schotten
    
    top_hand::ByteVector
    board::ByteMatrix
    bottom_hand::ByteVector
    deck::ByteVector
    turn::Player
    
    milestone_choice::Vector{Int}
    nb_milestone_choice::Int
    card_choice::Vector{Int}
    nb_card_choice::Int
    valid_moves::Vector{Tuple{Int, Int}}
    nb_valid_moves::Int
    
    
    function Schotten(top_hand::ByteVector, board::ByteMatrix, bottom_hand::ByteVector, deck::ByteVector, turn::Player)
        @assert length(top_hand) == NB_CARDS_IN_HAND
        @assert length(bottom_hand) == NB_CARDS_IN_HAND
        @assert size(board) == BOARD_SIZE

        new(top_hand, board, bottom_hand, deck, turn, zeros(NB_MILESTONES), 0, zeros(NB_CARDS_IN_HAND), 0, zeros(Tuple{Int, Int}, NB_CARDS_IN_HAND * NB_MILESTONES), 0)
    end
end


generatedeck() = shuffle(Byte[i*10 + j for i=1:NB_MILESTONES for j=1:NB_SLOT_IN_MILESTONE]::ByteVector)

function Schotten()
    deck = generatedeck()
    top_hand = deck[1:NB_CARDS_IN_HAND]
    deck = deck[NB_CARDS_IN_HAND+1:end]
    board = zeros(Byte, BOARD_SIZE)
    bottom_hand = deck[1:NB_CARDS_IN_HAND]
    deck = deck[NB_CARDS_IN_HAND+1:end]
    turn = top
    Schotten(
        top_hand,
        board,
        bottom_hand,
        deck,
        top
    )
end

# function make_copy(game::Schotten)::Schotten 
#     Schotten(
#         copy(game.top_hand), 
#         copy(game.board),
#         copy(game.bottom_hand),
#         copy(game.deck),
#         game.turn
#     )
# end

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
    game.nb_milestone_choice = 0
    game.nb_card_choice = 0
    game.nb_valid_moves = 0

    if game.turn == top
        milestone_idx = 1:3
        hand = game.top_hand
    else #bottom
        @assert game.turn == bottom
        milestone_idx = 5:7
        hand = game.bottom_hand
    end

    for i = 1:NB_MILESTONES
        if game.board[4,i] == 0 && count(!iszero, @view game.board[milestone_idx, i]) < 3
            game.nb_milestone_choice += 1
            game.milestone_choice[game.nb_milestone_choice] = i
        end
    end
    for i = 1:NB_CARDS_IN_HAND
        if hand[i] != 0
            game.nb_card_choice += 1
            game.card_choice[game.nb_card_choice] = i
        end
    end
    for card_idx in 1:game.nb_card_choice, milestone_idx in 1:game.nb_milestone_choice
        game.nb_valid_moves += 1
        game.valid_moves[game.nb_valid_moves] = (game.card_choice[card_idx], game.milestone_choice[milestone_idx])
    end
return @view game.valid_moves[1:game.nb_valid_moves]
end

function applymove!(game, move)
    card_idx, milestone_idx = move

    @views milestone_line = game.board[4,:]


    if game.turn == top
        hand = game.top_hand
        @views new_board = game.board[1:3, :]
    else # botttom
        @assert game.turn == bottom
        hand = game.bottom_hand
        @views new_board = game.board[5:end, :]
    end
        
    j=1
    while new_board[j, milestone_idx] != 0
        j += 1
    end

    assert(j < 4)


    # play card
    new_board[j, milestone_idx] = hand[card_idx]

    # draw card
    if length(game.deck) == 0
        hand[card_idx] = 0
    else
        hand[card_idx] = pop!(game.deck)
    end

    # check if can reclaim milestone
    for i in 1:9
        if milestone_line[i] == 0
            @views milestone_line[i] = winsMilestone(game.board[5:7, i], game.board[1:3, i], game.turn)
        end
    end

    # check if finished (current player wins)
    strike = 0
    total = 0
    for i in 1:9
        if milestone_line[i] == Byte(top)
            total += 1
            strike += 1
        else
            strike = 0
        end
        if strike == 3
            break
        end
    end
    if strike == 3 || total == 5
        return top
    end


    strike = 0
    total = 0
    for i in 1:9
        if milestone_line[i] == Byte(bottom)
            total += 1
            strike += 1
        else
            strike = 0
        end
        if strike == 3
            break
        end        
    end
    if strike == 3 || total == 5
        return bottom
    end

    game.turn = game.turn == top ? bottom : top

    return none
end

function Base.show(io::IO, game::Schotten)
    hand = "    "
    for i=game.top_hand
        hand *= lpad(string(i), 2,'0') * " "
    end
    println(io, hand)

    toto = ""
    for i=game.board[4, :]
        if i == Byte(top)
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
        if i == Byte(bottom)
            toto *= "MM "
        else
            toto *= "   "
        end
    end
    println(io, toto)

    hand = "    "
    for i=game.bottom_hand
        hand *= lpad(string(i), 2,'0') * " "
    end
    println(io, hand)

    deck = "\n"
    for card=game.deck
        deck *= lpad(string(card), 2,'0') * " "
    end
    println(deck)
end

@inline function evaluateside(side)
    @inbounds numbers = sort!(@MVector [div(side[1], 10), div(side[2], 10), div(side[3], 10) ])
    @inbounds colors = @SVector [side[1] % 10, side[2] % 10, side[3] % 10]
    total = sum(numbers)
    @inbounds total += numbers[1] + 2 == numbers[2] + 1 == numbers[1] ? 100 : 0 # suite
    @inbounds total += count(c -> c == colors[1], colors) == length(colors) ? 100 :  0 # couleur
    @inbounds total += count(n -> n == numbers[1], numbers) == length(numbers) ? 100 : 0 # brelan
    return total
end


@inline function winsMilestone(topSide, bottomSide, justPlayedCard::Player)
    if (count(!iszero, topSide) < 3  ||  count(!iszero, bottomSide) < 3)
         return none
    end

    topSideScore = evaluateside(topSide)
    bottomSideScore = evaluateside(bottomSide)

    if topSideScore == bottomSideScore
        return justPlayedCard == top ? bottom : top # the first who have played the card wins
    elseif topSideScore > bottomSideScore
        return top
    else
        return bottom
    end
end

function testgame()
    game = Schotten()
    wins = false
    while !wins
        println(game)
        moves = getvalidmoves(game)
        winner =  applymove!(game, rand(moves))
        if winner == top
            println(game)
            println("### TOP WIN ###")
            break
        end 
        if winner == bottom
            println(game)
            println("### BOTTOM WIN ###")
            break
        end
        # println(game.board[4, :])
        # sleep(1)
        println("\n\n@@@@@@@@@@@@@ New Turn @@@@@@@@@@@@@@@\n")
    end
end