Byte=UInt8
ByteArray=Array{Byte, 1}


const NB_MILESTONES = 9
const NB_SLOT_IN_MILESTONE = 7
const NB_CARDS_IN_HAND = 6
const NB_CARDS_ON_BOARD = NB_MILESTONES*NB_SLOT_IN_MILESTONE

struct Schotten
    prev_player_hand::ByteArray
    board::ByteArray
    next_player_hand::ByteArray
    deck::ByteArray
    function Schotten(prev_player_hand::ByteArray, board::ByteArray, next_player_hand::ByteArray, deck::ByteArray)
        @assert length(prev_player_hand) == NB_CARDS_IN_HAND
        @assert length(next_player_hand) == NB_CARDS_IN_HAND
        @assert length(board) == NB_CARDS_ON_BOARD
        # TODO: validate card completion and deck

        new(prev_player_hand, board, next_player_hand, deck)
    end
end

generatedeck() = shuffle(Byte[i*10 + j for i=1:NB_MILESTONES for j=1:NB_SLOT_IN_MILESTONE]::ByteArray)

function Schotten()
    deck = generatedeck()
    prev_player_hand = deck[1:NB_CARDS_IN_HAND]
    deck = deck[NB_CARDS_IN_HAND+1:end]
    board = zeros(Byte, NB_CARDS_ON_BOARD)
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
    milestone_choice = [i for i=1:NB_MILESTONES  if game.board[(i-1)*7 + 3 + 1] == 0 && any([game.board[(i-1)*7 + (j-1) + 1] == 0 for j=4:7])]
    card_choice = [i for i=1:NB_CARDS_IN_HAND if  game.next_player_hand[i] != 0]
    return [(card, milestone) for card in card_choice for milestone in milestone_choice]
end


# TODO: board to matrix