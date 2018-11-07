#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <vector>
using namespace std;

#include "filesort.h"

void filesort(vector<fptr *> &files, int lo, int hi, size_t d );

// need to fixup 3 partitions in each node
// ascending: lower carries to middle, middle to upper, after sorts
// descending:  restore same initial element after sorts
template <bool ascending>
void lcpsort( vector<fptr *> &files, int lo, int hi ) {
  if ( hi <= lo ) return;

  int lt = lo, gt = hi;

  size_t pivot = files[lo]->lcp;

  off_t lo_max=0, hi_max=0;
  
  for( int i = lo + 1; i <= gt; ) {
    if      ( ascending ? files[i]->lcp > pivot : files[i]->lcp < pivot ) {
      hi_max = hi_max < files[i]->lcp ? files[i]->lcp : hi_max;
      std::swap( files[i], files[gt--]);
    } else if ( ascending ? files[i]->lcp < pivot : files[i]->lcp > pivot ) {
      lo_max = lo_max < files[i]->lcp ? files[i]->lcp : lo_max;
      std::swap( files[lt++], files[i++]);
    } else
      i++;
  }

  filesort( files, lt, gt, pivot);

  lcpsort<ascending>( files, lo, lt-1 );
  lcpsort<ascending>( files, gt+1, hi );

  if( ascending ) {
    if( gt < hi ) 
      files[gt+1]->lcp = pivot;

    // carry lower
    if( lo < lt )
      files[lt]->lcp = lo_max;  // first lcp of middle is max of lower 
  } else { 
    // undo subsort lcp changes
    if( gt < hi )
      files[gt+1]->lcp = hi_max;

    files[lt]->lcp = pivot; 
  }

};

void filesort(vector<fptr *> &files, int lo, int hi, size_t d )
{
  if ( hi <= lo ) return;
  int lt = lo, gt = hi;

  fptr *pivot = files[lo];
  pivot->fopen();

  pivot->lcp = pivot->length;
  off_t lo_max=0, hi_max=0;
  for( int i = lo + 1; i <= gt; )
    {
      files[i]->fopen();
      int cmpr = fcmp(pivot, files[i], d);
      files[i]->fclose();
      if (cmpr < 0) {
	lo_max = lo_max < files[i]->lcp ? files[i]->lcp : lo_max;
	std::swap(files[lt++], files[i++]);
      } else if (cmpr > 0) {
	hi_max = hi_max < files[i]->lcp ? files[i]->lcp : hi_max;
	std::swap(files[i],    files[gt--]);
      } else            i++;
    }

  files[lt]->lcp = lo_max;

  lcpsort<true> ( files, lo, lt-1 );
    
  if( gt < hi ) {
    lcpsort<false>( files, gt+1, hi );  // gt+1 gets rewritten with later lcp''s
    files[gt+1]->lcp = hi_max;
  }
};


int main(int argc, char **argv) {

  vector<fptr *> files;

  char buf[1024];
  char *pbuf = buf;
  size_t blen = 1024;
  ssize_t slen=0;

  while( (slen = getline(&pbuf, &blen, stdin)) > 0) {
    files.push_back( new fptr(buf, slen-1));
  }

  filesort(files, 0, files.size()-1, 0);

  for(int i=0; i<files.size(); i++)
    printf("% 6lld %s\n", files[i]->lcp, files[i]->name);
}

  
