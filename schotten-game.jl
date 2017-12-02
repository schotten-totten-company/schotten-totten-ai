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
    milestone_choice = [i for i=1:NB_MILESTONES  if game.board[5,i] == 0 && any(game.board[6:end,i])]
    card_choice = [i for i=1:NB_CARDS_IN_HAND if  game.next_player_hand[i] != 0]
    return [(card, milestone) for card in card_choice for milestone in milestone_choice]
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


# TODO: board to matrix
