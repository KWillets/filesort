#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>

#include <algorithm>

#ifndef FILESORT

typedef struct fp{
  char *name;
  off_t lcp;
  off_t length;
  FILE *file;
  // cache?

  static size_t fsize( char *fname ) {
    struct stat s;
    stat(fname, &s);
    return s.st_size;
  }

  fp(char *_name, ssize_t slen): lcp(0), file(0) {
    name = strndup(_name, slen);
    length = fsize(name);
  }

  void fopen() {
    if(!file)
      file = ::fopen(name, "r");
  }

  void fread(unsigned char * buf, size_t len) {
    ::fread((void *) buf, 1, len, file);
  }

  void fclose() {
    ::fclose(file);
    file = 0;
  }

  void fseek(size_t off) {
    ::fseek(file, off, SEEK_SET);
  }
    
} fptr;

#define BUFSIZE 1024

#define min(X,Y) (X) < (Y) ? (X) : (Y);
#define cmpsgn(X,Y) X < Y ? 1 : X > Y ? -1 : 0

size_t strlcp(unsigned char *s, unsigned char *t, size_t l ) {
  off_t lcp;

  for(lcp = 0; lcp < l-7 && *(uint64_t *)(s + lcp) == *(uint64_t *)(t + lcp); lcp += 8)
    ;

  for(; lcp < l && s[lcp] == t[lcp]; lcp++)
    ;
  
  return lcp;
}

// fseek to initial lcp and compare blockwise
// files already open
int fcmp( fptr *pivot, fptr *f, size_t lcp ) {
  unsigned char buf1[BUFSIZE], buf2[BUFSIZE];

  int cmp = cmpsgn(pivot->length, f->length);
  off_t len = min(pivot->length, f->length);

  pivot->fseek(lcp);
  f->fseek(lcp);

  off_t d = 0;
  while( lcp < len ) {
    off_t rlen = min(BUFSIZE, len-lcp);

    pivot->fread(buf1, rlen);
    f->fread(buf2, rlen);

    d = strlcp( buf1, buf2, rlen );
    lcp += d;

    if( d < rlen ) {
      cmp = buf2[d] - buf1[d];
      break;
    }
  }

  f->lcp = lcp ;
  return cmp;
}

#define FILESORT
#endif
