#ifndef AFFINEDECODER_H
#define AFFINEDECODER_H

#include "gatorom.h"
#include <array>
/* This is a general purpose "affine decoder". It aims to
make a template for a good variety of decoder bit patterns
with only small code changes. There are 3 arrays used to construct it:
- counts: each entry indicates how many times the stride should be repeated
- col_strides: how much to increment col by each time this stride is taken
- row_strides: how much to increment row by each time this stride is taken

The strides act like an affine map, the column increment and row increment are
multiplied by an index vector to produce a row offset and column offset.
The index vector increments based on the counts, incrementing idx_vec[0]
until it reaches count[0], at which point it "carries" over to increment
idx_vec[1]. As an example, suppose counts = [2, 3, 4]. The index vector would
increment through: [0,0,0] [1,0,0] [0,1,0] [1,1,0] [0,2,0] [1,2,0] [0,0,1]

Combining this with the row and column strides, it's possible to represent many
interleaved and non-interleaved bit-read patterns.

A simple layout:
1234
5678

would look like
counts: [4, 2]
col_strides: [1, 0]
row_strides: [0, 1]

and would iterate as
idx vector: col, row
00: (0*1)+(0*0), (0*0)+(0*1)
...
30: (3*1)+(0*0), (3*0)+(0*1)
01: (0*1)+(1*0), (0*0)+(1*1)
...
31: (3*1)+(1*0), (3*0)+(1*1)


colsdownl can be represented with:
initial col offset: numcols-numcols/wordsize
initial row offset: 0
counts     : [wordsize        , numrows, numcols/wordsize]
col_strides: [-numcols/wordsize, 0      , 1]
row_strides: [0               , 1      , 0]

so for example with wordsize=2, numrows=4, numcols=4:
1908
3b2a
5d4c
7f6e

so:
counts: [2,4,2]
col_strides: [-2,0,1]
row_strides: [0,1,0]
idx: col, row
000: 2, 0
100: 2-2+0+0, 0+0+0
010: 2+0+0+0, 0+1+0
110: 2-2+0+0, 0+1+0
020: 2+0+0+0, 0+2+0
120: 0,2
030: 2,3
130: 0,3
001: 2+0+0+1, 0+0+0
101: 2-2+0+1, 0+0+0
...

*/
template <size_t N> class AffineDecoder : public GatoDecoder {
private:
  std::array<unsigned int, N> counts;
  std::array<int, N> row_strides;
  std::array<int, N> col_strides;
  int row_start;
  int col_start;

public:
  AffineDecoder(std::array<unsigned int, N> counts,
                std::array<int, N> row_strides, std::array<int, N> col_strides,
                int row_start, int col_start);
  void decode(GatoROM *gr);
  void inc_idx_counts(std::array<unsigned int, N> &idx_counts);
  int get_row_offset(std::array<unsigned int, N> &idx_counts);
  int get_col_offset(std::array<unsigned int, N> &idx_counts);
};

template <size_t N>
AffineDecoder<N>::AffineDecoder(std::array<unsigned int, N> counts,
                                std::array<int, N> row_strides,
                                std::array<int, N> col_strides, int row_start,
                                int col_start) {

  this->row_start = row_start;
  this->col_start = col_start;
  this->name = "Raw Affine Decoder";
  this->counts = counts;
  this->row_strides = row_strides;
  this->col_strides = col_strides;
}

template <size_t N>
int AffineDecoder<N>::get_col_offset(std::array<unsigned int, N> &idx_counts) {
  int col_offset = this->col_start;
  for (size_t i = 0; i < N; i++) {
    col_offset += idx_counts[i] * this->col_strides[i];
  }
  return col_offset;
}

template <size_t N>
int AffineDecoder<N>::get_row_offset(std::array<unsigned int, N> &idx_counts) {
  int row_offset = this->row_start;
  for (size_t i = 0; i < N; i++) {
    row_offset += idx_counts[i] * this->row_strides[i];
  }
  return row_offset;
}

template <size_t N>
void AffineDecoder<N>::inc_idx_counts(std::array<unsigned int, N> &idx_counts) {
  size_t cur_idx = 0;
  idx_counts[cur_idx]++;
  cur_idx++;
  while (cur_idx < N &&
         (idx_counts[cur_idx - 1] == this->counts[cur_idx - 1])) {
    idx_counts[cur_idx - 1] = 0;
    idx_counts[cur_idx]++;
    cur_idx++;
  }
};

template <size_t N> void AffineDecoder<N>::decode(GatoROM *gr) {

  uint32_t adr = 0;
  QByteArray ba, bad; // data and damage

  int wordsize = gr->wordsize;
  std::array<unsigned int, N> idx_counts = {0};
  gr->eval();

  unsigned int num_bits = gr->outputrows * gr->outputcols;
  if (num_bits % wordsize != 0) {
    return;
  }
  // we'll go bit-by-bit, grouping by words
  unsigned int num_words = num_bits / wordsize;
  for (int word = 0; word < num_words; word++) {
    uint32_t w = 0, wd = 0;

    for (int bit = wordsize - 1; bit >= 0; bit--) {
      unsigned int row = this->get_row_offset(idx_counts);
      unsigned int col = this->get_col_offset(idx_counts);
      this->inc_idx_counts(idx_counts);
      GatoBit *B = gr->outputbit(row, col);
      assert(B);

      B->adr = adr;
      B->mask = 1 << bit;
      if (B->getVal())
        w |= B->mask;
      if (B->ambiguous)
        wd |= B->mask;
    }

    // This is implicitly little endian
    for (int bitcount = wordsize; bitcount > 0; bitcount -= 8) {
      ba.append(w & 0xFF);
      w = w >> 8;
      bad.append(wd & 0xff);
      wd = wd >> 8;

      adr++;
    }
  }
  gr->decoded = ba;
  gr->decodedDamage = bad;
}
#endif // AFFINEDECODER_H
