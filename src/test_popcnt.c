#include <cstdio>
#include <cstdint>
#include "immintrin.h"

int main(int argc, char* argv[]) {
	uint64_t t = (uint64_t)31;

	if (argc == 2)
		t = (uint64_t)atoi(argv[1]);

	int n = __builtin_popcountll(t);
	int a = __builtin_ctzll(t);
	int b = __builtin_clzll(t);
	int c = __builtin_ffsll(t);

	fprintf(stdout, "number: %lu\n", t);
	fprintf(stdout, "popcnt: %d\n", n);
	fprintf(stdout, "ctz: %d\n", a);
	fprintf(stdout, "clz: %d\n", b);
	fprintf(stdout, "ffs: %d\n", c);

	return 0;
}
