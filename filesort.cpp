#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "filesort.h"

void filesort(fptr **files, int lo, int hi, size_t d );

template <bool ascending>
void lcpsort( fptr **files, int lo, int hi ) {
  if ( hi <= lo ) return;
  int lt = lo, gt = hi;

  size_t pivot = files[lo]->lcp;
  for( int i = lo + 1; i <= gt; ) {
    if      ( ascending ? files[i]->lcp > pivot : files[i]->lcp < pivot ) std::swap( files[i], files[gt--]);
    else if ( ascending ? files[i]->lcp < pivot : files[i]->lcp > pivot ) std::swap( files[lt++], files[i++]);
    else            i++;
  }

  filesort( files, lt, gt, pivot);
  lcpsort<ascending>( files, lo, lt-1 );
  lcpsort<ascending>( files, gt+1, hi );
};

void filesort(fptr **files, int lo, int hi, size_t d )
{
  if ( hi <= lo ) return;
  int lt = lo, gt = hi;

  fptr *pivot = files[lo];
  pivot->fopen();

  for( int i = lo + 1; i <= gt; )
    {
      files[i]->fopen();
      int cmpr = fcmp(pivot, files[i], d);
      files[i]->fclose();
      if      (cmpr < 0) std::swap(files[lt++], files[i++]);
      else if (cmpr > 0) std::swap(files[i],    files[gt--]);
      else            i++;
    }

  lcpsort<true> ( files, lo, lt-1 );
  lcpsort<false>( files, gt+1, hi );  
};


int main(int argc, char **argv) {

  fptr *files[1000];
  int n = 0;
  char buf[1024];
  char *pbuf = buf;
  size_t blen = 1024;
  ssize_t slen=0;

  while( (slen = getline(&pbuf, &blen, stdin)) > 0)
    files[n++] = new fptr(buf, slen-1);

  filesort(files, 0, n-1, 0);

  for(int i=0; i<n; i++)
    printf("%s\n", files[i]->name);
}

  
