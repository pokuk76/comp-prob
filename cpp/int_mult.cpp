#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fftw3.h>
#include <math.h>
#include <random>
#include <chrono>
#include <fstream>

using namespace std;
using namespace std::chrono;

// 0 -> local, 1 -> ECE cluster, 2 -> AWS
int platform = 0;

class FFT {

	public:
		int8_t * arr;
		int arr_len;

		// Holds results of all [I]FFT calculations (NB: will be overwritten on each call to `this.fft`)
		fftw_complex * fft_arr;
		fftw_plan p;

		bool destroy_plan = true;

		FFT(int8_t * arr_, int arr_len_) {
			arr_len = arr_len_;

			arr = arr_;

			// Initialize complex array with digits of arr
			// TODO: Do the padding here to save space
			int pad_len = 2 * arr_len;
			fft_arr = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * pad_len);
			for (int i = 0; i < arr_len; i++) {
				fft_arr[i][0] = arr[i];
			}
			for (int i = arr_len; i < pad_len; i++) {
				fft_arr[i][0] = 0;
			}

		}

		FFT(fftw_complex * fft_arr_, int arr_len_) {
			// Don't pad in this case

			arr_len = arr_len_;

			fft_arr = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * arr_len);
			memcpy(fft_arr, fft_arr_, sizeof(fftw_complex) * arr_len);		

		}

		~FFT() {
			fftw_destroy_plan(p);
			fftw_free(fft_arr);

		}

		struct InputArray {
			fftw_complex * f_arr;
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

			// Okay assuming the arrays are the same length
			fftw_complex temp;
			for (int i = 0; i < nx; i++) {
				// In case its an inplace operation wrt one of the inputs 
				temp[0] = A[i][0] * B[i][0] - A[i][1] * B[i][1];
				temp[1] = A[i][0] * B[i][1] + A[i][1] * B[i][0];
				R[i][0] = temp[0];
				R[i][1] = temp[1];
			}
		}


		/**
		 * @brief 
		 * 
		 * @param arr - fully padded input array representing the digits of an integer
		 * @param arr_len - length of input array `arr` (which, again, is properly padded)
		 * @param inverse - 
		 * @return fftw_complex* 
		 */
		void fft(bool inverse = false) {

			// In either case, carry of FT in place
			if (inverse) {
				p = fftw_plan_dft_1d(arr_len, fft_arr, fft_arr, FFTW_BACKWARD, FFTW_ESTIMATE);
				fftw_execute(this->p);
				// FFTW performs unnormalized Inverse FFT so normalize
				// TODO: set this->arr?
				for (int i = 0; i < arr_len; i++) {
					// TODO: Saving space by doing things in place
					// this->arr[i] = fft_arr[i][0] / arr_len;
					fft_arr[i][0] = fft_arr[i][0] / arr_len;
				}
			} else {
				p = fftw_plan_dft_1d(arr_len, fft_arr, fft_arr, FFTW_FORWARD, FFTW_ESTIMATE);
				fftw_execute(this->p);
			}

		}

		void test_fftw() {
			int N = 4;
			int in[4] = {2, 3, 0, 0};
			int out[4];

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
		int8_t * digits;
		int len;
		// Naive v.s. FFT implementations
		// TODO: Make the class better? 
		int mode;
		int base = 10;

		Integer() {
			len = 0;
			digits = NULL;
		}

		Integer(int8_t * digits_, int len_) {
			len = len_;
			digits = digits_;
		}

		Integer(int digits_, int len_) {
			len = len_;
		}

		void set_digits(int8_t * digits_, int num_digits=-1){
			if (!(num_digits==-1)) {
				this->len = num_digits;
			}
			digits = digits_;
		}


		void print_int() {
			for (int i = 0; i < this->len; i++) {
				cout << digits[i];
			}
			cout << endl;
		}

		void big_mult(Integer const& other, int * result=NULL) {

			/* Do the padding */
			// TODO: Assuming this and other are of equal length
			int pad_N = this->len * 2;
			int N = this->len;
			// // <https://cplusplus.com/reference/algorithm/copy/>

			
			int product = 0;  //kf

			FFT f1 (this->digits, N);
			FFT f2 (other.digits, N);

			
			f1.fft();
			f2.fft();
			
			// Point-wise multiplication of complex numbers (in-place wrt f1)
			FFT::multiply(&f1, &f2, f1.fft_arr);


			f1.fft(true);  // Inverse FFT
			int power;
			for (int i = 0; i < f1.arr_len-1; i++) {
				power = f1.arr_len - (i+2);
				product += f1.fft_arr[i][0] * pow(10, power);
			}
		}


		/**
		 * @brief 
		 * TODO: Why this function has to be const (if we call it from overloaded operator) <https://stackoverflow.com/questions/13103755/intellisense-the-object-has-type-qualifiers-that-are-not-compatible-with-the-me>
		 * 
		 * @param other 
		 * @param product 
		 * @return int* 
		 */
		void naive_mult(Integer const& other, int * result=NULL) const {
			int len_p = this->len + other.len;
			int * product = (int *) calloc(len_p, sizeof(int));

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
				// If last element of array (i.e. first digit) is 0, discard
			} else {
				// the whole array is valid
			}

			// Cleanup
			free(product);
		}

};

void generate_integer(int8_t * bin, int len) {

	// Seed with a real random value, if available
    random_device r;
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
	int8_t bin[n];

	generate_integer(bin, n);	
}


void print_array(int * bin, int n, string term="") {
	cout << term << "[ ";
	for (int i=0; i<n; i++) {
		cout << bin[i] << " ";
	}
	cout << "]" << endl;
}

int main(int argc, char* argv[]) {

	/* Experiment */

	int execution_time_naive = 0;  // In milliseconds
	int execution_time_big = 0;
	double mem_big;
	auto limit = 1 * (60 * pow(10, 3));  // 10 minutes in milliseconds (I hope)
	double max_mem = 0; 
	if (platform == 1) {
		// ECE
		max_mem = 0.6 * 32;  // In GBs
	} else {
		// local and AWS
		max_mem = 0.6*16;
	}

	int num_digits = 1;
	Integer X, Y;


	// Data Collection
	ofstream data;
	string naive_file = "n_squared_cpp.csv";
	string big_file = "n_logn_cpp.csv";

	cout << "-----------NAIVE_MULT------------" << endl;
	data.open(naive_file);
	data << "NUM_DIGITS, EXECUTION_TIME_NSQUARED[ms]\n";
	while (execution_time_naive < limit) {
	// while (num_digits < 5) {
        cout << "NUMBER OF DIGITS: " << num_digits << endl;

		int8_t * digitsX = (int8_t *) calloc(num_digits, sizeof(int8_t));
		int8_t * digitsY = (int8_t *) calloc(num_digits, sizeof(int8_t));

		generate_integer(digitsX, num_digits);
		generate_integer(digitsY, num_digits);

		X.set_digits(digitsX, num_digits);
		Y.set_digits(digitsY, num_digits);
		
		// Get starting timepoint
		auto start = high_resolution_clock::now();
		// Call the function, here sort()
		cout << "\tCalling naive_mult..." << endl;
		X.naive_mult(Y);
		// Get ending timepoint
		auto stop = high_resolution_clock::now();
		// Get duration
		execution_time_naive = duration_cast<milliseconds>(stop - start).count();
		cout << "Time taken by function: "
			 << execution_time_naive << " milliseconds" << endl;

		data << num_digits << ", " << execution_time_naive << "\n";
		
		num_digits *= 2;
		free(digitsX);
		free(digitsY);
	}
	data.close();

    
	cout << "\n--------------BIG_MULT--------------" << endl;
	// Reset things
	num_digits = 1;
	data.open(big_file);
	data << "NUM_DIGITS, EXECUTION_TIME_NLOGN[ms]\n";
	// while (num_digits < 134217728) {
	while (num_digits < 256) {

        cout << "NUMBER OF DIGITS: " << num_digits << endl;

		int8_t * digitsX = (int8_t *) calloc(num_digits, sizeof(int8_t));
		int8_t * digitsY = (int8_t *) calloc(num_digits, sizeof(int8_t));

		generate_integer(digitsX, num_digits);
		generate_integer(digitsY, num_digits);

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
		mem_big = (2*sizeof(double) * num_digits) * pow(10, -9);
		
		cout << "Time taken by function: "
			 << execution_time_big << " milliseconds" << endl;

		cout << "Size of FFTW array [gigabytes I hope]: "
			 << mem_big << endl;
		data << num_digits << ", " << execution_time_big << "\n";
		
		num_digits *= 2;
		free(digitsX);
		free(digitsY);
	}
	data.close();

}
