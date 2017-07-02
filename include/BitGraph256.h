/**
* @fileoverview Copyright (c) 2017, Stefano Gualandi,
*               via Ferrata, 1, I-27100, Pavia, Italy
*
* @author stefano.gualandi@gmail.com (Stefano Gualandi)
*
*/

#pragma once

#include "immintrin.h"
#include <array>

// Basic support for bitsets
typedef std::array<uint64_t, 4>  bitset256_t;

// Set to true i-th bit of bitset sets bs
inline void set_bit(bitset256_t& bs, uint64_t i) {
   bs[i / 64] |= (uint64_t)1 << (i % 64);
}

// Reset to true i-th bit of bitsetsets bs
inline void reset_bit(bitset256_t& bs, uint64_t i) {
   bs[i / 64] &= ~(uint64_t)1 << (i % 64);
}

// Set to true all bits of bitsetsets bs
inline void set_bits(bitset256_t& bs) {
   bs = (bitset256_t&)(_mm256_set1_epi64x((uint64_t)~0));
}

// Set to false all bits of bitset bs
inline void reset_bits(bitset256_t& bs) {
   bs = (bitset256_t&)_mm256_setzero_ps();
}

// Intersect two bitsets
inline void inplace_intersect(bitset256_t& a, const bitset256_t& b) {
   a = (bitset256_t&)(_mm256_and_ps((__m256&)a, (__m256&)b));
}

// Set to false all bits of bitsetsets bs
inline uint64_t count_bits(bitset256_t& bs) {
#ifdef _WIN64
   return _mm_popcnt_u64(bs[0]) + _mm_popcnt_u64(bs[1]) + _mm_popcnt_u64(bs[2]) + _mm_popcnt_u64(bs[3]);
#else
   return __builtin_popcountll(bs);
#endif
}
// Return next bit set to true and reset it to false
uint64_t next_1_bit(bitset256_t& bs) {
   uint64_t v1 = _tzcnt_u64(bs[0]);
   uint64_t v2 = _tzcnt_u64(bs[1]);
   uint64_t v3 = _tzcnt_u64(bs[2]);
   uint64_t v4 = _tzcnt_u64(bs[3]);
   if (v1 < 64) {
      bs[0] = _blsr_u64(bs[0]);  // Reset the found bit
      return v1;
   }
   if (v2 < 64) {
      bs[1] = _blsr_u64(bs[1]);  // Reset the found bit
      return 64 + v2;
   }
   if (v3 < 64) {
      bs[2] = _blsr_u64(bs[2]);  // Reset the found bit
      return 128 + v3;
   }
   bs[3] = _blsr_u64(bs[3]);  // Reset the found bit
   return 192 + v4;
}

class BitGraph256 {
   public:
   BitGraph256() {
      // Reset all the adjacent lists
      bitset256_t zero = { (uint64_t)0, (uint64_t)0, (uint64_t)0, (uint64_t)0 };
      adj.fill(zero); // All zeros 0000....000
      // NOTE: Isolated vertices from a 1-clique
   }

   // Add a new edge
   void add_edge(size_t i, size_t j) {
      // Bitwise OR at given bit position
      set_bit(adj[i], j);
      set_bit(adj[j], i);
   }

   // Find Maxclique in graph
   bitset256_t find_maxclique() const {
      bitset256_t Bs;           // Best clique

      std::array<bitset256_t, 256>  StackCs;
      std::array<bitset256_t, 256>  StackPs;

      uint64_t n_bs = 0;  // Size best clique
      uint64_t n_ps = 0;  // Number of potential vertices

      uint64_t v; // next vertex to be checked

      // INIT: set all vertices as candidates
      size_t idx = 0; // Current position in the stack
      set_bits(StackPs[idx]);
      reset_bits(StackCs[idx]);

      // Loop to avoid recursive calls
      while (true) {
         n_ps = count_bits(StackPs[idx]);
         if (n_ps + idx > n_bs) { // n_ps + n_cs > n_bs === n_cs = idx
            // Set first bit different from zero to branch over
            v = next_1_bit(StackPs[idx]);
            // Push right branch for 'v' not in clique into the stack
            StackCs[idx + 1] = StackCs[idx];
            StackPs[idx + 1] = StackPs[idx];
            idx++;
            // Remove from Ps vertices not adjacent to vertex v
            inplace_intersect(StackPs[idx], adj[v]);
            // Elaborate left branch: vertex v in current clique
            set_bit(StackCs[idx], v);
            // Check for best cliques size
            if (idx > n_bs) {
               Bs = StackCs[idx];
               n_bs = idx;
            }
         } else {
            // Simulate recursive branching calls
            if (idx > 0) {
               // Pop from stack
               idx--;
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
   std::array<bitset256_t, 256> adj;

   // Number of vertices
   uint64_t n;
};