#pragma once

#include <smmintrin.h>
#include "immintrin.h"
#include <array>

// Basic support for bitsets
typedef uint64_t  bitset64_t;

// Set to true i-th bit of bitset sets bs
inline bitset64_t set_bit(bitset64_t bs, uint64_t i) {
   bs |= (bitset64_t)1 << i; // _mm_slli_si128 (r := a << (imm * 8)) // _mm_sll_epi64 ??
   return bs;
}
// Reset to true i-th bit of bitsetsets bs
inline bitset64_t reset_bit(bitset64_t bs, uint64_t i) {
   bs &= ~((bitset64_t)1 << i);
   return bs;
}
// Set to true all bits of bitsetsets bs
inline bitset64_t set_bits(bitset64_t bs) {
   bs = (bitset64_t)~0; // Complement of 000....00
   return bs;
}
// Set to false all bits of bitsetsets bs
inline uint64_t count_bits(bitset64_t bs) {
#ifdef _WIN64
   return __popcnt64(bs);
#else
#ifdef _LAPTOP
   return __builtin_popcountll(bs);
#else
   return _popcnt64(bs);
#endif
#endif
}
// Return next bit set to true
uint64_t next_1_bit(bitset64_t bs) {
#ifdef _LAPTOP
   return __builtin_ctz(bs);
#else
   return _tzcnt_u64(bs);
#endif
}

class BitGraph64 {
   public:
   BitGraph64() {
      // Reset all the adjacent lists
      adj.fill((bitset64_t)0); // All zeros 0000....000
      // NOTE: Isolated vertices from a 1-clique
   }

   // Add a new edge
   void add_edge(size_t i, size_t j) {
      // Bitwise OR at given bit position
      adj[i] = set_bit(adj[i], j);
      adj[j] = set_bit(adj[j], i);
   }

   // Find Maxclique in graph
   bitset64_t find_maxclique() const {
      bitset64_t Cs = (bitset64_t)0;  // Current clique
      bitset64_t Bs = Cs;           // Best clique
      bitset64_t Ps = Cs;           // Potential vertices to be added to Cs

      size_t idx = 0; // Current position in the stack
      std::array<uint64_t,   64>  SizeCs;
      std::array<bitset64_t, 64>  StackCs;
      std::array<bitset64_t, 64>  StackPs;

      uint64_t n_bs = 0;  // Size best clique
      uint64_t n_cs = 0;  // Size current clique
      uint64_t n_ps = 0;  // Number of potential vertices

      uint64_t v; // next vertex to be checked

      // INIT: set all vertices as candidates
      Ps = set_bits(Ps);

      // Loop to avoid recursive calls
      while (true) {
         n_ps = (count_bits(Ps));
         if (n_ps + n_cs > n_bs) {
            // Set first bit different from zero to branch over
            v = next_1_bit(Ps);
            Ps = reset_bit(Ps, v);
            // Push right branch for 'v' not in clique into the stack
            SizeCs[idx] = n_cs;
            StackCs[idx] = Cs;
            StackPs[idx] = Ps;
            idx++;
            // Remove from Ps vertices not adjacent to vertex v
            Ps &= adj[v];
            // Elaborate left branch: vertex v in current clique
            Cs = set_bit(Cs, v);
            n_cs++;
            // Check for best cliques size
            if (n_cs > n_bs) {
               Bs = Cs;
               n_bs = n_cs;
            }
         } else {
            // Simualte recursive branching calls
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
   std::array<bitset64_t, 64> adj;

   // Number of vertices
   uint64_t n;
};
