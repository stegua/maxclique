#pragma once

#include "immintrin.h"
#include "emmintrin.h"
#include <array>

// Basic support for bitsets
typedef std::array<uint64_t, 2>  bitset128_t;
//typedef __m128  bitset128_t;

// Set to true i-th bit of bitset sets bs
inline void set_bit(bitset128_t& bs, uint64_t i) {
   if (true) {
      if (i < 64)
         //_bittestandset64((long long *)&bs[0], i);
         bs[0] |= (uint64_t)1 << i;
      else
         //_bittestandset64((long long *)&bs[1], i - 64);
         bs[1] |= (uint64_t)1 << (i - 64);
   } else {
      // branchless
      int idx = i / 64;
      int v = i % 64;
      bs[idx] |= (uint64_t)1 << v;
   }
}

// Reset to true i-th bit of bitsetsets bs
inline void reset_bit(bitset128_t& bs, uint64_t i) {
   if (true) {
      if (i < 64)
         //_bittestandreset64((long long *)&bs[0], i);
         bs[0] &= ~(uint64_t)1 << i;
      else
         //_bittestandreset64((long long *)&bs[1], i - 64);
         bs[1] &= ~(uint64_t)1 << (i - 64);
   } else {
      int idx = i / 64;
      int v = i % 64;
      bs[idx] &= ~(uint64_t)1 << v;
   }
}

// Set to true all bits of bitset bs
inline void set_bits(bitset128_t& bs) {
   bs = (bitset128_t&)(_mm_set1_epi64x((uint64_t)~0));
   //bs[0] = (uint64_t)~0; // Complement of 000....00
   //bs[1] = (uint64_t)~0;
}

// Set to false all bits of bitset bs
inline void reset_bits(bitset128_t& bs) {
   bs = (bitset128_t&)_mm_setzero_ps();
   //bs[0] = (uint64_t)0; // Complement of 000....00
   //bs[1] = (uint64_t)0;
}

// Set to false all bits of bitsetsets bs
inline uint64_t count_bits(const bitset128_t& bs) {
#ifdef _WIN64
   return _mm_popcnt_u64(bs[0]) + _mm_popcnt_u64(bs[1]);
#else
   return __builtin_popcountll(bs[0]) + __builtin_popcountll(bs[1]);
#endif
}

// Return next bit set to true
uint64_t next_1_bit(bitset128_t& bs) {
   uint64_t v = _tzcnt_u64(bs[0]); // First bit set to one
   if (v < 64) {
      bs[0] = _blsr_u64(bs[0]);  // Reset the found bit
      return v;
   }
   v = 64 + _tzcnt_u64(bs[1]);  // First bit set to one
   bs[1] = _blsr_u64(bs[1]);    // Reset first bit set to one found bit
   return v;
}

// Intersect two bitsets
inline void inplace_intersect(bitset128_t& a, const bitset128_t& b) {
   //a = (bitset128_t&)(_mm_and_ps((__m128&)a, (__m128&)b));
   a[0] &= b[0];
   a[1] &= b[1];
}

class BitGraph128 {
   public:
   BitGraph128() {
      // Reset all the adjacent lists
      bitset128_t zero = { (uint64_t)0, (uint64_t)0 };
      adj.fill(zero); // All zeros 0000....000
      // NOTE: Isolated vertices form a 1-clique
   }

   // Add a new edge
   void add_edge(size_t i, size_t j) {
      // Bitwise OR at given bit position
      set_bit(adj[i], j);
      set_bit(adj[j], i);
   }

   // Find Maxclique in graph
   bitset128_t find_maxclique() const {
      bitset128_t Cs;    // Current clique
      reset_bits(Cs);
      bitset128_t Bs = Cs;           // Best clique
      bitset128_t Ps = Cs;           // Potential vertices to be added to Cs

      size_t idx = 0; // Current position in the stack
      std::array<uint64_t,    128>  SizeCs;
      std::array<bitset128_t, 128>  StackCs;
      std::array<bitset128_t, 128>  StackPs;

      uint64_t n_bs = 0;  // Size best clique
      uint64_t n_cs = 0;  // Size current clique
      uint64_t n_ps = 0;  // Number of potential vertices

      uint64_t v; // next vertex to be checked

      // INIT: set all vertices as candidates
      set_bits(Ps);

      // Loop to avoid recursive calls
      while (true) {
         n_ps = (count_bits(Ps));
         // QUANDO N_PS + N_CS < 64 PASSO A BIT64 PER TROVARE LA CLIQUE NEL SOTTOGRAFO...
         // COME TORNO INDIETRO?
         if (n_ps + n_cs > n_bs) {
            //fprintf(stdout, "%lu %lu %lu\n", n_ps, n_cs, n_bs);
            // Set first bit different from zero to branch over
            v = next_1_bit(Ps);
            //reset_bit(Ps, v);
            // Push right branch for 'v' not in clique into the stack
            SizeCs[idx] = n_cs;
            StackCs[idx] = Cs;
            StackPs[idx] = Ps;
            idx++;
            // Remove from Ps vertices not adjacent to vertex v
            inplace_intersect(Ps, adj[v]);
            // Elaborate left branch: vertex v in current clique
            set_bit(Cs, v);
            n_cs++;
            // Check for best cliques size
            if (n_cs > n_bs) {
               Bs = Cs;
               n_bs = n_cs;
            }
         } else {
            // Simulate recursive branching calls
            if (idx > 0) {
               // Pop form stack
               idx--;
               n_cs = SizeCs[idx];
               Cs = StackCs[idx];
               Ps = StackPs[idx];
            } else
               break; // No more alternatives: exit from while loop
         }
      }
      return Bs;
   }

   inline void set_n(int _n) {
      n = _n;
   }

   inline uint64_t get_n() const {
      return n;
   }

   private:
   // Adjacent list for each vertex
   std::array<bitset128_t, 128> adj;

   // Number of vertices
   uint64_t n;
};