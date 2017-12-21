using Profile

@inline function winsMilestone(topSide, bottomSide)
    a = 0

    if (count(!iszero, topSide) < 3  ||  count(!iszero, bottomSide) < 3)
         return 1
    end

    return 2
end

function test(n_iter)
    A = zeros(UInt8,(7, 9))
    sum = 0
    for i=1:n_iter
        for j=1:9
            sum += @views winsMilestone(A[1:3, j], A[4:7, j])
        end
    end
    return sum
end

@time test(1)

Profile.clear_malloc_data()
toto = @time test(1000)

@macroexpand @time test(1000)

println(toto)