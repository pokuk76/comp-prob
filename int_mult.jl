# using Pkg
# Pkg.add("FFTW")

using TickTock
using FFTW


function naive_mult(f1, f2, len_f1, len_f2)

    # The order the algorithm expects it in
    f1 = reverse(f1)
    f2 = reverse(f2)

    len_p = len_f1 + len_f2
    # product = zeros(Int8, len_p, 1)
    product = zeros(Int64, len_p)

    println("f1: $f1")
    println("f2: $f2")

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

    reverse_product = reverse(product)
    # if product[end] == 0
    if reverse_product[1] == 0
        result = join(string.(reverse_product[2:len_p]))
    else
        result = join(string.(reverse_product))
    end

    println("reverse_product: $reverse_product")
    println("Result: $result")
    return result
end

function big_mult(X, Y, len)
    base = 10
    
    # TODO: Padding
    fft_len = 2*len
    x = zeros(Int64, fft_len)
    x[1:len] = X
    y = zeros(Int64, fft_len)
    y[1:len] = Y

    println("Padded X: $x")


    fft_x = fft(x)
    # println("fft: $fft_n")
    fft_y = fft(y)

    # Element-wise multiplication of the arrays of complex numbers
    fft_p = fft_x .* fft_y
    # println("fft: $fft_p")

    # Array
    p = ifft(fft_p)
    # println("fft: $p")

    # r = p[1]*100 + p[2]*10 + p[3]  # huh... don't fully understand this carry
    # # Oh wait I do
    # println("fft: $r")

    # product = zeros(Int8, len-1)
    product = ""
    product_int = 0
    for i=1:fft_len-1
        # printf("%3d\n", p.arr[i]);
        power = (fft_len-1) - i;
        var = p[i]
        println("$i: $var, $power");
        product_int += convert(Int64, p[i]) * (base^power)
    end
    println("Product_int: $product_int")
    product = string(product_int)
    println("Product: $product")

    return product
end

function generate_integer(out, num_digits)
    out[1] = rand(1:9)
    out[2:end] = rand(0:9, num_digits-1)
end

function main()

    # Fake input
    # n = [2; 3; 0; 0]
    # m = [4; 1; 0; 0]
    # big_mult(n, m, 4)

    # f1 = [2, 3, 9, 5, 8, 2, 3, 3]
    # len_f1 = 8
	# f2 = [5, 8, 3, 0]
    # len_f2 = 4
    # naive_mult(reverse(f1), len_f1, reverse(f2), len_f2)

    # EXPERIMENT
    execution_time = 0;  # In microseconds
	limit = 10 * (60 * 10^6);  # 10 minutes in microseconds (I hope)

	num_digits = 1
    # while execution_time < limit
    while num_digits < 3
        println("\nNUMBER OF DIGITS: $num_digits")
		X = zeros(Int64, num_digits)
        Y = zeros(Int64, num_digits)
		generate_integer(X, num_digits)
		generate_integer(Y, num_digits)
        println("X: $X | Y: $Y")
		
        println("NAIVE_MULT")
		# Execution Time (in seconds I believe...)
        execution_time = @elapsed naive_mult(X, Y, num_digits, num_digits)
        execution_time_ms = execution_time * 10^6  # convert to microseconds
        # @printf "" typeof(execution_time)
        # var = typeof(execution_time)
        # println("$var")
		println("\tExecution time [s]: $execution_time")

        println("BIG_MULT")
        execution_time = @elapsed big_mult(X, Y, num_digits)
        execution_time_ms = execution_time * 10^6  # convert to microseconds
		println("\tExecution time [s]: $execution_time")
		
		num_digits *= 2
	end
end

function test_naive()
    num_digits = 2
    X = [5, 9]
    Y = [1, 0]
    println("X: $X | Y: $Y")
    
    println("NAIVE_MULT")
    # Execution Time (in seconds I believe...)
    execution_time = @elapsed naive_mult(X, Y, num_digits, num_digits)
end

function test_big()
    num_digits = 2
    X = [2, 3]
    Y = [4, 1]
    println("X: $X | Y: $Y")
    
    println("BIG_MULT")
    # Execution Time (in seconds I believe...)
    execution_time = @elapsed big_mult(X, Y, num_digits)
end
main()
# test_big()