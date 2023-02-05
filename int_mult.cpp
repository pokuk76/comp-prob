#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fftw3.h>

using namespace std;

class FFT {

	public:
		int * arr;
		int arr_len;

		// Holds results of all [I]FFT calculations (NB: will be overwritten on each call to `this.fft`)
		fftw_complex * fft_arr;
		fftw_plan p;

		FFT(int * arr_, int arr_len_) {
			arr = arr_;
			arr_len = arr_len_;

			fft_arr = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * arr_len);
			// Initialize complex array with digits of arr
			for (int i = 0; i < arr_len; i++) {
				fft_arr[i][0] = arr[i];
			}

		}

		~FFT() {
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
		void print_fftw(InputArray * a = NULL) {
			if (a) {
				for (int i = 0; i < a->len; i++) {
					printf("freq: %3d %+9.5f %+9.5f i\n", i, a->f_arr[i][0], a->f_arr[i][1]);
				}
				return;
			}

			for (int i = 0; i < arr_len; i++) {
				printf("freq: %3d %+9.5f %+9.5f i\n", i, fft_arr[i][0], fft_arr[i][1]);
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
		fftw_complex * fft(int * arr, int arr_len, bool inverse) {

			// In either case, carry of FT in place
			if (inverse) {
				p = fftw_plan_dft_1d(arr_len, fft_arr, fft_arr, FFTW_BACKWARD, FFTW_ESTIMATE);
			} else {
				p = fftw_plan_dft_1d(arr_len, fft_arr, fft_arr, FFTW_FORWARD, FFTW_ESTIMATE);
			}

			return fft_arr;

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
		}
};

class Integer {

	public:
		int * digits;
		int len;
		// Naive v.s. FFT implementations
		// TODO: Make this better? 
		int mode;
		int base = 10;

		Integer(string digits_) {

		}

		Integer(string digits_, int len_) {
			len = len_;
		}

		Integer(int * digits_) {
			digits = digits_;
		}

		Integer(int * digits_, int len_) {
			// digits = (int *) malloc(sizeof(int) * len_);
			// *digits = *digits_;
			digits = digits_;
			len = len_;
		}

		Integer(int digits_, int len_) {
			digits = (int *) malloc(sizeof(int) * len_);
			// *digits = *digits_;
			*digits = digits_;
			len = len_;
		}

		// ~Integer() {

		// }

		void print_int() {
			for (int i = 0; i < this->len; i++) {
				cout << digits[i];
			}
			cout << endl;
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
		int * naive_mult(Integer const& other) const {
			int len_p = this->len + other.len;
			int product[len_p];
			// this->naive_mult(rhs, product);

			
			int carry;
			for (int j = other.len - 1; j >= 0; j--) {
				carry = 0;
				for (int i = this->len - 1; i >= 0; i--) {
					product[i + j-1] += carry + this->digits[i] * other.digits[j];
					carry = product[i + j-1] / base;
					product[i + j-1] = product[i + j-1] % base;
				}
				product[j + this->len] = carry;
			}
			// Integer p (product, len_p);
			// return p;
			for (int i = 0; i<len_p; i++) {
				cout << product[i];
			}
			cout << endl;
			return product;
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

string generate_integer() {

}

float * pad(string in, int in_len) {

}


string big_mult(string a, string b) {
	

	string r = "";
	return r;
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

int main(int argc, char* argv[]) {

	// string a = "23958233";
	// string b = "5830";
	// cout << "a: " << a << " | b: " << b << endl;
	// naive_mult(a, b);


	// big_mult("a", "b");
	int a[8] = {2, 3, 9, 5, 8, 2, 3, 3};
	int b[4] = {5, 8, 3, 0};
	
	Integer A (a, 8);
	cout << "A: ";
	A.print_int();

	Integer B (b, 4);
	cout << "B: ";
	B.print_int();
	
	// Integer C = A * B;
	// C.print_int();
	A.naive_mult(B);

}
