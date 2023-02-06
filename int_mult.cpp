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
		static fftw_complex * multiply(FFT *a, FFT *b) {
			fftw_complex * A = a->fft_arr;
			fftw_complex * B = b->fft_arr;
			fftw_complex * R;
			int nx = a->arr_len;
			int ny = 1;
			double scale = 1.0 / (nx * ny);

			R = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * nx * ny);
			for (int i = 0; i < nx; i++) {
				// int ij = i * (ny / 2 + 1) + j;
				R[i][0] = (A[i][0] * B[i][0] - A[i][1] * B[i][1]) * scale;
				R[i][1] = (A[i][0] * B[i][1] + A[i][1] * B[i][0]) * scale;
			}

			InputArray in = {R, nx*ny};
			// print_fftw(in);
			cout << "Printing multiplication result..." << endl;
			for (int i = 0; i < nx*ny; i++) {
				printf("freq: %3d %+9.5f %+9.5f i\n", i, R[i][0], R[i][1]);
			}
			return (R);
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
		fftw_complex * fft(bool inverse = false) {

			// In either case, carry of FT in place
			if (inverse) {
				p = fftw_plan_dft_1d(arr_len, fft_arr, fft_arr, FFTW_BACKWARD, FFTW_ESTIMATE);
			} else {
				p = fftw_plan_dft_1d(arr_len, fft_arr, fft_arr, FFTW_FORWARD, FFTW_ESTIMATE);
			}
			fftw_execute(this->p);

			cout << "Printing fft result..." << endl;
			this->print_fftw();

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
			fftw_free(fft_in);
			fftw_free(fft_out);
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

		// Integer(string digits_) {

		// }

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

		int * big_mult(Integer const& other) {

			int N = 4;
			// kf
			int * product = (int *) malloc(sizeof(int) * N);
			fftw_complex * fft_product = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);

			FFT f1 (this->digits, this->len);
			FFT f2 (other.digits, other.len);

			f1.fft();
			f2.fft();
			
			// Okay assuming the arrays are the same length
			// fftw_complex * temp;
			// for (int i = 0; i < this->len; i++) {
			// 	temp = FFT::multiply(f1.fft_arr[i], f2.fft_arr[i]);
			// }
			// cout << "Printing f1..." << endl;
			// f1.print_fftw();
			// cout << "Printing f2..." << endl;
			// f2.print_fftw();
			fftw_complex * p = FFT::multiply(&f1, &f2);

			// int p = 0;
			return product;
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
			for (int i = 0; i<len_p; i++) {
				product[i] = 0;
				cout << product[i];
			}
			cout << endl;

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
			for (int i = 0; i<len_p; i++) {
				cout << product[i];
			}
			cout << endl;
			return product;
		}

		int * naive_mult2(Integer const& other) const {
			int len_p = this->len + other.len;
			int product[len_p];
			// this->naive_mult(rhs, product);
			for (int i = 0; i<len_p; i++) {
				product[i] = 0;
				cout << product[i];
			}
			cout << endl;

			int i_this = 0;
			int i_other = 0;

			
			int carry;
			for (int j = 0; j < other.len; j++) {
				carry = 0;
				i_other = 0;
				for (int i = 0; i < this->len; i++) {
					// int sum = this->digits[i] * other.digits[j] + product[i_this + i_other] + carry;
					// carry = sum / base;
					// product[i_this + i_other] = sum % base;
					// i_other++;

					product[i + j-1] += carry + this->digits[i] * other.digits[j];
					carry = product[i + j-1] / base;
					product[i + j-1] = product[i + j-1] % base;
				}
				// if (carry > 0) {
				// 	product[i_this + i_other] += carry;
				// }
				// i_this++;
				
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
	return "integer";
}

void pad(string in, int in_len) {
	float l[4];
	return;
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
	// A.naive_mult2(B);

	int x[4] = {2, 3, 0, 0};
	int y[4] = {4, 1, 0, 0};

	Integer X (x, 4);
	Integer Y (y, 4);
	X.big_mult(Y);

}
