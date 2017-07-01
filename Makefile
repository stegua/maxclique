
laptop:
	g++ -D_LAPTOP -m64 -msse4.2 -mpopcnt -O2 -o test src/test_maxclique.cpp -I./include -std=c++11

knl:
	icpc -axMIC-AVX512 -O3 -o test src/test_maxclique.cpp -I./include
