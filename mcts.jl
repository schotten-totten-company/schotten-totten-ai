

include("./schotten-game.jl")

const C = 1

mutable struct Node
    game_state::Schotten
    N::UInt
    W::Int
    parent::Nullable{Node}
    move:: Tuple{Int64, Int64}
    player::Int
    children::Array{Node, 1}
    function Node(game, parent::Nullable{Node}, player, move)
        @assert player==0 || player==1
        new(
            game,
            0,
            0,
            parent,
            move,
            player,
            []
        )
    end
end


function naive_rollout(node::Node)
    topWon = false
    bottomWon = false
    player = node.player
    game = node.game_state
    while !(topWon || bottomWon)
        actions = getvalidmoves(game)
        action = rand(actions)
        game, topWon, bottomWon = applymove(game, action)
        player = 1 - player
    end

    
    node.N = 1
    node.W = player == 1 && topWon || player == 0 && bottomWon ? 1 : -1
end

function show(io::IO, node::Node, tabul::String)
    str = tabul * "Node G:$node.game_state A:$game.move N: $game.N, W: $game.W"
    println(io, str)
    for child in node.children
        show(io, child, tabul * "  ")
    end
end

function Base.show(io::IO, node::Node)    
    show(io, node, "")
end

function back_propagate(node::Node, n::UInt, w::Int)
    node.W += w
    node.N += n
    if !isnull(node.parent)
        back_propagate(get(node.parent), n, w)
    end
end
    
function expand!(node::Node)
    actions = getvalidmoves(node.game_state)
    if length(actions) == 0
        return
    end
    for a in actions
        child_game,_,_ = applymove(node.game_state, a)
        child_node = Node(child_game, Nullable{Node}(node), 1 - node.player, a)
        naive_rollout(child_node)
        back_propagate(node, child_node.N, child_node.W)
        push!(node.children, child_node)
    end
end

function select(node)
    if length(node.children) == 0
        return node
    end
    sign = node.player == 1 ? -1 : 1
    UCT_scores = [sign * child.W / child.N + C * sqrt(log(node.N)/child.N) for child in node.children]
    _, chosen_child_idx = findmax(UCT_scores)
    return select(node.children[chosen_child_idx])
end

function maxNode(nodes::AbstractArray{Node})
    len = length(nodes)
    @assert len != 0
    maxnode = nodes[1]
    if len > 1
        for node in nodes[2:end]
            if node.N > maxnode.N 
                maxnode = node
            end
        end
    end
    return maxnode
end

function run_mcts!(node, time_budget=10)
    start_time = time()
    nb_selected_node = 0
    while time() - start_time < time_budget
        selected_node = select(node)
        nb_selected_node += 1
        expand!(selected_node)
    end
    println("Selections:", nb_selected_node)
    # print(node)
    return maxNode(node.children).move
end
    
Node(game) = Node(game, Nullable{Node}(), 0, (0,0))

function testperf(numiter, time_budget=10)
    for i in 1:numiter
        game = Schotten()
        # println(game)
        mcts = Node(game)
        move = run_mcts!(mcts, time_budget)
        println("Choosen move:", move)
    end
end

testperf(1,1)


using ProfileView

Profile.clear()
@profile testperf(1)

ProfileView.view()

