# using Pkg
# Pkg.add("FFTW")

using FFTW

function int_mult(a, b)
    a_len = len(a)
    b_len = len(b)
end

function test()
    println("TESTING")
end


function naive_mult(f1, len_f1, f2, len_f2)

    len_p = len_f1 + len_f2
    # product = zeros(Int8, len_p, 1)
    product = zeros(Int64, len_p, 1)

    i_this = 0
    i_other = 1
    base = 10

    
    for j = 1:len_f2
        carry = 0
        i_other = 1
        for i = 1:len_f1
            product[i + j-1] += carry + f1[i] * f2[j]
            carry = product[i + j-1] ÷ base  # They got me with the integer divide fr...
            product[i + j-1] = product[i + j-1] % base
        end
        product[j + len_f1] = carry
    end

    println("Product: $product")
    return product
end

function big_mult()
    # TODO: Get input
    n = [2; 3; 0; 0]
    m = [4; 1; 0; 0]

    fft_n = fft([2; 3; 0; 0])
    # println("fft: $fft_n")
    fft_m = fft([4; 1; 0; 0])
    s = size(fft_m)
    println("fft_m size: $s")

    fft_p = fft_n .* fft_m
    # println("fft: $fft_p")

    p = ifft(fft_p)
    # println("fft: $p")

    r = p[1]*100 + p[2]*10 + p[3]  # huh... don't fully understand this carry
    # Oh wait I do
    println("fft: $r")

    product = 0
    for i=1:4-1
        # printf("%3d\n", p.arr[i]);
        power = 4 - (i+1);
        var = p[i]
        println("$i: $var, $power");
        product += p[i] * 10^power
    end
    println("Product: $product")
    
end

function main()
    big_mult()

    # f1 = [2, 3, 9, 5, 8, 2, 3, 3]
    # len_f1 = 8
	# f2 = [5, 8, 3, 0]
    # len_f2 = 4
    # naive_mult(reverse(f1), len_f1, reverse(f2), len_f2)
end

main()