all:
	gcc -o count `pkg-config --cflags --libs glib-2.0` -I../include count.c ../lib/libstemmer.o `pkg-config --libs glib-2.0`
	gcc -o predictor `pkg-config --cflags --libs glib-2.0` -I../include predictor.c ../lib/libstemmer.o `pkg-config --libs glib-2.0`
	gcc -o stemmer -I../include stemmer.c ../lib/libstemmer.o 

