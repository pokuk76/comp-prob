# https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&cad=rja&uact=8&ved=2ahUKEwjG_8LVn8f5AhVZAzQIHYboDtYQFnoECBsQAQ&url=https%3A%2F%2Fwww.gnu.org%2Fs%2Fmake%2Fmanual%2Fhtml_node%2FSimple-Makefile.html&usg=AOvVaw1M5Me8rRMnQKm2t7LQH0RF
all : int_mult

int_mult : int_mult.cpp
		g++ int_mult.cpp -o int_mult -lfftw3 -lm

debug : int_mult.cpp
		g++ -g -std=c++11 int_mult.cpp -o debug -lfftw3 -lm

client_wloop : client_wloop.cpp
		g++ -pthread client_wloop.cpp -o client_wloop

writer : writer.cpp
		g++ writer.cpp -o writer
				   
reader : reader.cpp
		g++ reader.cpp -o reader

gk : get_key.c
		gcc get_key.c -o gk

clean :
		rm -f int_mult
