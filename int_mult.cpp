#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fftw3.h>
#include <math.h>
#include <random>
#include <chrono>       /* time */
#include <fstream>
#include <vector>
#include <algorithm>    // std::copy

using namespace std;
using namespace std::chrono;

class FFT {

	public:
		vector<int8_t> arr;
		int arr_len;

		// Holds results of all [I]FFT calculations (NB: will be overwritten on each call to `this.fft`)
		fftw_complex * fft_arr;
		fftw_plan p;

		bool destroy_plan = true;

		FFT(vector<int8_t> arr_, int arr_len_) {
			arr_len = arr_len_;

			arr = arr_;

			// Initialize complex array with digits of arr
			fft_arr = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * arr_len);
			for (int i = 0; i < arr_len; i++) {
				fft_arr[i][0] = arr[i];
			}

		}

		FFT(fftw_complex * fft_arr_, int arr_len_) {
			// destroy_plan = false;
			arr_len = arr_len_;

			vector<int8_t> temp (arr_len, 0);
			this->arr = temp;


			fft_arr = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * arr_len);
			memcpy(fft_arr, fft_arr_, sizeof(fftw_complex) * arr_len);		
			// cout << "\n\nTesting..." << endl;
			// for (int i = 0; i < arr_len; i++) {
			// 	printf("freq: %3d %+9.5f %+9.5f i\n", i, this->fft_arr[i][0], this->fft_arr[i][1]);
			// }
		}

		~FFT() {
			// if (destroy_plan) {
			// 	fftw_destroy_plan(p);
			// }
			fftw_destroy_plan(p);
			fftw_free(fft_arr);

		}

		struct InputArray {
			fftw_complex * f_arr;
			// int * arr;
			int len;
		};


		/**
		 * @brief Print an array of `fftw_complex` objects
		 * 
		 * @param a 
		 * @param a_len 
		 */
		// template <class T>
		void print_fftw(InputArray * a = NULL) {
			if (a) {
				for (int i = 0; i < a->len; i++) {
					printf("freq: %3d %+9.5f %+9.5f i\n", i, a->f_arr[i][0], a->f_arr[i][1]);
				}
				return;
			}

			for (int i = 0; i < arr_len; i++) {
				printf("freq: %3d %+9.5f %+9.5f i\n", i, this->fft_arr[i][0], this->fft_arr[i][1]);
			}
		}

		// static fftw_complex * multiply(fftw_complex *A, fftw_complex *B) {
		// static fftw_complex * multiply(FFT *a, FFT *b, fftw_complex * r) {
		static void multiply(FFT *a, FFT *b, fftw_complex * R) {
			fftw_complex * A = a->fft_arr;
			fftw_complex * B = b->fft_arr;
			// fftw_complex * R;
			int nx = a->arr_len;
			int ny = 1;
			double scale = 1.0 / (nx * ny);

			// Okay assuming the arrays are the same length
			// R = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * nx * ny);
			for (int i = 0; i < nx; i++) {
				R[i][0] = A[i][0] * B[i][0] - A[i][1] * B[i][1];
				R[i][1] = A[i][0] * B[i][1] + A[i][1] * B[i][0];
			}

			// InputArray in = {R, nx*ny};
			// print_fftw(in);
			
			// return (R);
		}


		/**
		 * @brief 
		 * 
		 * @param arr - fully padded input array representing the digits of an integer
		 * @param arr_len - length of input array `arr` (which, again, is properly padded)
		 * @param inverse - 
		 * @return fftw_complex* 
		 */
		// fftw_complex * fft(int * arr, int arr_len, bool inverse) {
		void fft(bool inverse = false) {

			// In either case, carry of FT in place
			if (inverse) {
				p = fftw_plan_dft_1d(arr_len, fft_arr, fft_arr, FFTW_BACKWARD, FFTW_ESTIMATE);
				fftw_execute(this->p);
				// FFTW performs unnormalized Inverse FFT so...
				// TODO: set this->arr ?
				for (int i = 0; i < arr_len; i++) {
					// this->arr[i] = static_cast<int>(fft_arr[i][0]) / arr_len;
					this->arr[i] = fft_arr[i][0] / arr_len;
				}
			} else {
				p = fftw_plan_dft_1d(arr_len, fft_arr, fft_arr, FFTW_FORWARD, FFTW_ESTIMATE);
				fftw_execute(this->p);
			}
			// fftw_execute(this->p);

			// cout << "Printing fft result..." << endl;
			// this->print_fftw();
			// return fft_arr;

		}

		void test_fftw() {
			int N = 4;
			int in[N] = {2, 3, 0, 0};
			int out[N];

			// fftw_complex fft_in[N] = {2, 3, 0, 0};
			fftw_complex *fft_in;
			fft_in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
			// Just assumed that fft_in[i][1] (the imaginary part) is initialized to 0
			fft_in[0][0] = 2;
			fft_in[1][0] = 3;
			fft_in[2][0] = 0;
			fft_in[3][0] = 0;
			fftw_complex *fft_out;
			fft_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
			fftw_plan p;

			p = fftw_plan_dft_1d(N, fft_in, fft_out, FFTW_FORWARD, FFTW_ESTIMATE);
			fftw_execute(p);
			// cout << "FFT output: [" << fft_out[0] << fft_out[1] << fft_out[2] << fft_out[3] << endl;
			cout << "FFT output: " << endl;
			InputArray a = { fft_out, N };
			print_fftw(&a);
			cout << "Expected FFT output: [ 5.0000 + 0.0000i 2.0000 - 3.0000i -1.0000 + 0.0000i 2.0000 + 3.0000i]" << endl;
			fftw_destroy_plan(p);
			fftw_free(fft_in);
			fftw_free(fft_out);
		}
};

class Integer {

	public:
		// int * digits = NULL;
		vector<int8_t> digits;
		int len;
		// Naive v.s. FFT implementations
		// TODO: Make this better? 
		int mode;
		int base = 10;

		Integer() {}

		// Integer(string digits_) {

		// }

		Integer(string digits_, int len_) {
			len = len_;
		}

		Integer(vector<int8_t> digits_, int len_) {
			len = len_;

			// digits = (int *) malloc(sizeof(int) * len);
			digits = digits_;
			// memcpy(digits, digits_, sizeof(int)*len);
		}

		Integer(int digits_, int len_) {
			// digits = (int *) malloc(sizeof(int) * len_);
			// digits = digits_;
			// *digits = digits_;
			len = len_;
		}

		// ~Integer() {
		// 	if (digits){
		// 		free(digits);
		// 	}
		// }

		// void set_digits(int * digits_, int num_digits=-1){
		void set_digits(vector<int8_t> digits_, int num_digits=-1){
			// if (this->digits) {
			// 	free(this->digits);
			// }
			if (!(num_digits==-1)) {
				this->len = num_digits;
			}
			// digits = (int *) malloc(sizeof(int) * this->len);
			// memcpy(digits, digits_, sizeof(int)*this->len);
			digits = digits_;
			
		}


		void print_int() {
			for (int i = 0; i < this->len; i++) {
				cout << digits[i];
			}
			cout << endl;
		}

		// int * big_mult(Integer const& other) {
		void big_mult(Integer const& other, int * result=NULL) {
			// int N = 4;

			/* Do the padding */
			// TODO: Assuming this and other are of equal length
			int N = this->len * 2;
			vector<int8_t> this_digits(N, 0);
			vector<int8_t> other_digits(N, 0);
			// <https://cplusplus.com/reference/algorithm/copy/>
			copy( this->digits.begin(), this->digits.begin()+this->len, this_digits.begin() );
			copy( other.digits.begin(), other.digits.begin()+other.len, other_digits.begin() );

			// cout << "Printing memcpy..." << endl;
			// printf("Padding [2, 3]: [");
			// for (int i = 0; i < N; i++) {
			// 	printf("%3d ", this_digits[i]);
			// }
			// printf("]\n");

			
			// kf
			int product = 0;
			fftw_complex * fft_product = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);
			// FFT product ();

			FFT f1 (this_digits, N);
			FFT f2 (other_digits, N);

			
			f1.fft();
			f2.fft();
			
			// fftw_complex * p = FFT::multiply(&f1, &f2, fft_product);
			FFT::multiply(&f1, &f2, fft_product);

			// cout << "Printing multiplication result..." << endl;
			// for (int i = 0; i < N; i++) {
			// 	printf("freq: %3d %+9.5f %+9.5f i\n", i, fft_product[i][0], fft_product[i][1]);
			// }

			FFT p (fft_product, N);
			// FFT p (other.digits, other.len);

			// cout << "\nIFFT step: " << endl;
			p.fft(true);
			int power;
			for (int i = 0; i < p.arr_len-1; i++) {
				// printf("%3d\n", p.arr[i]);
				power = p.arr_len - (i+2);
				// cout << i << ": " << p.arr[i] << ", " << power << endl;
				product += p.arr[i] * pow(10, power);
			}

			// cout << "`big_mult` product: " << product << endl;


			/* CLEAN UP */
			// free(this_digits);
			// free(other_digits);
			free(fft_product);
		}

		/**
		 * @brief 
		 * TODO: Why this function has to be const <https://stackoverflow.com/questions/13103755/intellisense-the-object-has-type-qualifiers-that-are-not-compatible-with-the-me>
		 * 
		 * @param other 
		 * @param product 
		 * @return int* 
		 */
		// int * naive_mult(Integer const& other, int * product) const {
		void naive_mult(Integer const& other, int * result=NULL) const {
			int len_p = this->len + other.len;
			// int product[len_p];
			int * product = (int *) calloc(len_p, sizeof(int));
			// Print product to make sure its 0s
			// for (int i = 0; i<len_p; i++) {
			// 	// product[i] = 0;
			// 	cout << product[i];
			// }
			// cout << endl;

			int i_this = 0;
			int i_other = 0;
			
			int carry;
			for (int j = other.len - 1; j >= 0; j--) {
				carry = 0;
				i_other = 0;
				for (int i = this->len - 1; i >= 0; i--) {
					int sum = this->digits[i] * other.digits[j] + product[i_this + i_other] + carry;
					carry = sum / base;
					product[i_this + i_other] = sum % base;
					i_other++;

				}
				if (carry > 0) {
					product[i_this + i_other] += carry;
				}
				i_this++;
			}

			// Product is reversed
			if (product[len_p-1] == 0) {
				// If last element of array (i.e. first digit) is 0, throw away
			}
    //     result = join(string.(reverse_product[2:len_p]))
    // else
    //     result = join(string.(reverse_product))
    // end

			// Print product
			// for (int i = 0; i<len_p; i++) {
			// 	cout << product[i];
			// }
			// cout << endl;

			// Cleanup
			free(product);
		}

		/**
		 * @brief 
		 * NB: this (i.e., LHS) takes precedent for type of multiplication carried out
		 * 
		 * @param rhs 
		 * @return Integer 
		 */
		Integer operator*(Integer const& rhs) const {
			// int len_p = this->len + rhs.len;
			// int product[len_p];
			// this->naive_mult(rhs, product);

			// Integer p (product, len_p);
			// return p;
			int test[4] = {1, 2, 3, 4};
			Integer t (*test, 4);
			return t;
		}

    	Integer& operator*=(Integer const& rhs);

		Integer& operator=(Integer const& other) {

		}
};

void generate_integer(vector<int8_t> bin, int len) {

	// Seed with a real random value, if available
    random_device r;
    // Choose a random mean between 1 and 6
    default_random_engine e1(r());
    uniform_int_distribution<int8_t> uniform_dist_(1, 9);
    
	bin[0] = uniform_dist_(e1);

    uniform_int_distribution<int> uniform_dist(0, 9);
	for (int i=1; i < len; i++) {
		bin[i] = uniform_dist(e1);
	}
}


void test_generate_integer() {
	// Test integer generation
	int n = 8;
	int bin[n];

	// generate_integer(bin, n);	
}


string naive_mult(string a, string b) {
	int a_len = a.length();
	int b_len = b.length();
	string product (a_len+b_len, '0');
	for (int i=0; i<a_len; i++) {
		cout << a[i] - '0' << endl;
	}
	return product;
}

void print_array(int * bin, int n, string term="") {
	cout << term << "[ ";
	for (int i=0; i<n; i++) {
		cout << bin[i] << " ";
	}
	cout << "]" << endl;
}

int main(int argc, char* argv[]) {
	// string a = "23958233";
	// string b = "5830";
	// cout << "a: " << a << " | b: " << b << endl;
	// naive_mult(a, b);


	// big_mult("a", "b");
	// int a[8] = {2, 3, 9, 5, 8, 2, 3, 3};
	// int b[4] = {5, 8, 3, 0};
	
	// Integer A (a, 8);
	// cout << "A: ";
	// A.print_int();

	// Integer B (b, 4);
	// cout << "B: ";
	// B.print_int();
	
	// Integer C = A * B;
	// C.print_int();
	// A.naive_mult(B);
	// A.naive_mult2(B);

	/* Big Mult Test */

	int x[4] = {2, 3};
	int y[4] = {4, 1};

	// Integer X (x, 2);
	// Integer Y (y, 2);
	// X.big_mult(Y);

	// FFT p (b, 4);
	// p.fft();

	/* Experiment */

	// auto execution_time = 0;  // In microseconds
	// chrono::milliseconds execution_time_naive;  // In milliseconds
	int execution_time_naive = 0;  // In milliseconds
	// cout << "Initialized execution time: " << execution_time_naive.count() << endl;
	int execution_time_big = 0;
	auto limit = 1 * (60 * pow(10, 3));  // 10 minutes in milliseconds (I hope)
	limit /= 2;

	Integer X, Y;
	int num_digits = 1;

	// Data Collection
	ofstream data;
	string naive_file = "n_squared_cpp.csv";
	string big_file = "n_logn_cpp.csv";

	// cout << "-----------NAIVE_MULT------------" << endl;
	// data.open(naive_file);
	// data << "NUM_DIGITS, EXECUTION_TIME_NSQUARED[ms]\n";
	// while (execution_time_naive < limit) {
	// // while (num_digits < 5) {
    //     cout << "\nNUMBER OF DIGITS: " << num_digits << endl;

	// 	int digitsX[num_digits];
	// 	int digitsY[num_digits];
	// 	generate_integer(digitsX, num_digits);
	// 	generate_integer(digitsY, num_digits);
	// 	// print_array(digitsX, num_digits, "digitsX: ");
	// 	// print_array(digitsY, num_digits, "digitsY: ");
	// 	X.set_digits(digitsX, num_digits);
	// 	Y.set_digits(digitsY, num_digits);
		
	// 	// Get starting timepoint
	// 	auto start = high_resolution_clock::now();
	// 	// Call the function, here sort()
	// 	cout << "\tCalling naive_mult..." << endl;
	// 	X.naive_mult(Y);
	// 	// Get ending timepoint
	// 	auto stop = high_resolution_clock::now();
	// 	// Get duration
	// 	execution_time_naive = duration_cast<milliseconds>(stop - start).count();
	// 	// cout << "Time taken by function: "
	// 	// 	 << execution_time_naive.count() << " milliseconds" << endl;

	// 	data << num_digits << ", " << execution_time_naive << "\n";
		
	// 	num_digits *= 2;
	// }
	// data.close();

    
	cout << "--------------BIG_MULT--------------" << endl;
	// Reset things
	num_digits = 1;
	data.open(big_file);
	data << "NUM_DIGITS, EXECUTION_TIME_NLOGN[ms]\n";
	while (execution_time_big < limit) {
	// while (num_digits < 5) {
        cout << "\nNUMBER OF DIGITS: " << num_digits << endl;

		// int digitsX[num_digits];
		// int digitsY[num_digits];
		vector<int8_t> digitsX (num_digits, 0);
		vector<int8_t> digitsY (num_digits, 0);
		generate_integer(digitsX, num_digits);
		generate_integer(digitsY, num_digits);
		// print_array(digitsX, num_digits, "digitsX: ");
		// print_array(digitsY, num_digits, "digitsY: ");
		X.set_digits(digitsX, num_digits);
		Y.set_digits(digitsY, num_digits);
		
		// Get starting timepoint
		auto start = high_resolution_clock::now();
		// Call the function, here sort()
		cout << "\tCalling big_mult..." << endl;
		X.big_mult(Y);
		// Get ending timepoint
		auto stop = high_resolution_clock::now();
		// Get duration
		execution_time_big = duration_cast<milliseconds>(stop - start).count();
		cout << "Time taken by function: "
			 << execution_time_big << " milliseconds" << endl;

		cout << "Size of digitsX array [bytes I hope]: "
			 << sizeof(std::vector<int>) + (sizeof(int) * digitsX.size()) << endl;

		data << num_digits << ", " << execution_time_big << "\n";
		
		num_digits *= 2;
	}
	data.close();

}
