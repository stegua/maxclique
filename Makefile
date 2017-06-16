

knl:
	icc -axMIC-AVX512 -O3 -o test src/test_maxclique.cpp -I./include -std=c++11 -qopt-report
