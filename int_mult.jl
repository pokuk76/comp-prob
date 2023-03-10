using FFTW
using TickTock
using CSV
using DataFrames

function naive_mult(f1, f2, len_f1, len_f2)

    # The order the algorithm expects it in
    f1 = reverse(f1)
    f2 = reverse(f2)

    len_p = len_f1 + len_f2
    # product = zeros(Int8, len_p, 1)
    product = zeros(Int64, len_p)

    # println("f1: $f1")
    # println("f2: $f2")

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

    # println("reverse_product: $reverse_product")
    # println("Result: $result")
    return result
end

function big_mult(X, Y, len)
    base = 10
    
    fft_len = 2*len
    x = zeros(Int64, fft_len)
    x[1:len] = X
    y = zeros(Int64, fft_len)
    y[1:len] = Y

    # println("Padded X: $x")


    fft_x = fft(x)
    fft_y = fft(y)

    # Element-wise multiplication of the arrays of complex numbers
    fft_p = fft_x .* fft_y

    # Array
    p = ifft(fft_p)

    # product = zeros(Int8, len-1)
    product = ""
    product_int = 0
    for i=1:fft_len-1
        # printf("%3d\n", p.arr[i]);
        power = (fft_len-1) - i;
        # var = p[i]
        # println("$i: $var, $power");

        # product_int += convert(Int64, p[i]) * (base^power)
        # There was an `InexactError: Int64(13.999999999999996)` error one time
        product_int += round(Int64, real(p[i])) * (base^power)
    end
    # println("Product_int: $product_int")
    product = string(product_int)
    # println("Product: $product")

    return product
end

function generate_integer(out, num_digits)
    out[1] = rand(1:9)
    out[2:end] = rand(0:9, num_digits-1)
end

function write_data_(times, num_digits, inputs_X, inputs_Y, products, file_path)
    # TODO: I guess it would be good to check if the file already exists...
    touch(file_path)
    df = DataFrame(
        EXECUTION_TIME_S = times,
        NUM_DIGITS = num_digits,
        INPUT_1 = inputs_X,
        INPUT_2 = inputs_Y,
        RESULT = products,
    )
    CSV.write(file_path, df)
end

function write_data(times_naive, times_big, num_digits, file_path)
    # TODO: I guess it would be good to check if the file already exists...
    touch(file_path)
    df = DataFrame(
        EXECUTION_TIME_NSQUARED_S = times_naive,
        EXECUTION_TIME_NLOGN_S = times_big,
        NUM_DIGITS = num_digits
    )
    CSV.write(file_path, df)
end

function main()

    # EXPERIMENT
    execution_time = 0;  # In seconds
	limit = 1 * 60;  # 10 minutes in seconds (I hope)
	num_digits = 1

    # Data collection arrays
    execution_time_naive = Vector{Float64}()
    execution_time_big = Vector{Float64}()
    num_digits_arr = Vector{Int64}()


    while execution_time < limit
    # while num_digits < 5
        println("\nNUMBER OF DIGITS: $num_digits")
		X = zeros(Int64, num_digits)
        Y = zeros(Int64, num_digits)
		generate_integer(X, num_digits)
		generate_integer(Y, num_digits)
        # println("X: $X | Y: $Y")

        push!(num_digits_arr, num_digits)
		

        # println("BIG_MULT")
        execution_time = @elapsed big_mult(X, Y, num_digits)
        execution_time_ms = execution_time * 10^6  # convert to microseconds
		# println("\tExecution time [s]: $execution_time")
        push!(execution_time_big, execution_time)


        # println("NAIVE_MULT")
		# Execution Time (in seconds I believe...)
        execution_time = @elapsed result = naive_mult(X, Y, num_digits, num_digits)
        execution_time_ms = execution_time * 10^6  # convert to microseconds
        # @printf "" typeof(execution_time)
		# println("\tExecution time [s]: $execution_time")
        # append value to array before overwriting it
        push!(execution_time_naive, execution_time)

		
		num_digits *= 2
	end
    # write_data_(execution_time_naive, num_digits_arr, inputs_X, inputs_Y, products_naive, "test_naive.csv")
    write_data(execution_time_naive, execution_time_big, num_digits_arr, "result_1min.csv")
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

# test_big()
main()
