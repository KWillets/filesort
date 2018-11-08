#include "filesort.h"

int main( int argc, char **argv) {

  if(argc < 2)
    exit(-1);

  std::string fn1(argv[1]), fn2(argv[2]);
  fptr *f1 = new fptr(fn1);
  fptr *f2 = new fptr(fn2);

  f1->fopen();
  f2->fopen();
  
  printf("comparing %s %s\n", f1->name.c_str(), f2->name.c_str());

  int cmp = fcmp(f1, f2, 0);

  printf("%s %c %s\n", f1->name.c_str(), (cmp < 0)-(cmp > 0)+'=', f2->name.c_str());

  printf("cmp = %d lcp = %d\n", cmp, (int) f2->lcp);
}
