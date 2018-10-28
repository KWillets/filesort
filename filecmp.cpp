#include "filesort.h"

int main( int argc, char **argv) {

  //int s = strlcp( "abcde", "abcda", 6 );
  //printf( "strlcp: %d\n", s);

  if(argc < 2)
    exit(-1);
  
  fptr *f1 = new fptr(argv[1], strlen(argv[1])+1);
  fptr *f2 = new fptr(argv[2], strlen(argv[2])+1);

  f1->fopen();
  f2->fopen();
  
  printf("comparing %s %s\n", f1->name, f2->name);
  int cmp = fcmp(f1, f2, 0);

  printf("cmp = %d lcp = %d\n", cmp, (int) f2->lcp);
}
