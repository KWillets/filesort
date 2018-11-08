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
    if ( ascending ? files[i]->lcp > pivot : files[i]->lcp < pivot ) {
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
    if( lo < lt )
      files[lt]->lcp = lo_max;
  } else { 
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
      } else {
	pivot->lcp = pivot->length;
	i++;
      }
    }

  pivot->fclose();
  
  files[lt]->lcp = lo_max;

  lcpsort<true> ( files, lo, lt-1 );

  if( gt < hi ) {
    lcpsort<false>( files, gt+1, hi );
    files[gt+1]->lcp = hi_max;
  }
};


int main(int argc, char **argv) {

  vector<fptr *> files;

  char buf[1024];
  char *pbuf = buf;
  size_t blen = 1024;
  ssize_t slen=0;

  for( string line; std::getline(std::cin, line);)
    files.push_back( new fptr(line));

  filesort(files, 0, files.size()-1, 0);
  files[0]->lcp = 0;

  int ndup=0, i=0;

  for(i=0; i<files.size()-1; i++) {
    if(ndup == 0 && files[i+1]->lcp == files[i+1]->length)
      ndup=1;
    printf("% 3.0d % 6lld %s\n", ndup,  files[i]->lcp, files[i]->name.c_str());
    if(files[i+1]->lcp == files[i+1]->length)
      ndup++;
    else
      ndup = 0;
  }
  printf("% 3.0d % 6lld %s\n", ndup,  files[i]->lcp, files[i]->name.c_str());
}

  
