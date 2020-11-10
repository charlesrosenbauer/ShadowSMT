all:
	clang -O3 -static *.c

zen:
	clang -O3 -static -march=znver1 *.c

debug:
	clang -O2 -g *.c
