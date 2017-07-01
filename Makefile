
laptop:
	g++ -D_LAPTOP -O3 -march=core-avx-i -mtune=core-avx-i -mavx2 -o test src/test_maxclique.cpp -I./include -std=c++11

knl:
	icpc -axMIC-AVX512 -O3 -o test src/test_maxclique.cpp -I./include
