#include "BitGraph64.h"
#include "BitGraph128.h"

#include "immintrin.h"

// Read a dimacs instance
#include <fstream>
template <typename BitGraph>
void read_graph(const std::string& filename, BitGraph& G) {
   std::ifstream infile(filename);

   // p edges 60 89
   int n = 0;
   int m = 0;
   char buffer[1024];
   infile >> buffer >> buffer >> n >> m;
   G.set_n(n);

   int i = 0;
   int j = 0;
   while (infile >> buffer >> i >> j) {
      G.add_edge(i - 1, j - 1);
   }
}

int main(int argc, char* argv[]) {
   if (argc != 2)
      fprintf(stdout, "Usage:  ./MaxClique <filename>\n\n");

   BitGraph64 G;
   read_graph(argv[1], G);
   int64_t a = __rdtsc();
   bitset64_t clique = G.find_maxclique();
   int64_t b = __rdtsc();
   fprintf(stdout, "w(G) = %lld   -   Time stamp counter: %lld %lld\n", count_bits(clique), b - a, G.get_n());


   BitGraph128 H;
   read_graph(argv[1], H);
   a = __rdtsc();
   auto clique2 = H.find_maxclique();
   b = __rdtsc();
   fprintf(stdout, "w(G) = %lld   -   Time stamp counter: %lld %lld\n", count_bits(clique2), b - a, G.get_n());
}

