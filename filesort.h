#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>

#ifndef FILESORT

typedef struct fp{
  char *name;
  off_t lcp;
  off_t length;
  int fd;
  unsigned char *map;
  // cache?

  static size_t fsize( char *fname ) {
    struct stat s;
    stat(fname, &s);
    return s.st_size;
  }

  fp(char *_name, ssize_t slen): lcp(0), map(0), fd(0) {
    name = strndup(_name, slen);
    length = fsize(name);
  }

  void fopen() {
    if(!fd)
      fd = open(name, O_RDONLY);
    map = (unsigned char *) mmap(0, length, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
    if(map == MAP_FAILED)
      fprintf(stderr, "map failed %s %d\n", name, fd);
  }

  void fclose() {
    munmap(map, length);
    close(fd);
    fd = 0;
  }

} fptr;

#define min(X,Y) (X) < (Y) ? (X) : (Y);

off_t strlcp(unsigned char *s, unsigned char *t, size_t l, off_t lcp ) {

  lcp &= ~7; 
  for(; lcp < (l & ~7) && *(uint64_t *)(s + lcp) == *(uint64_t *)(t + lcp); lcp += 8)
    ;

  for(; lcp < l && s[lcp] == t[lcp]; lcp++)
    ;
  
  return lcp;
}

int fcmp( fptr *pivot, fptr *f, off_t lcp ) {

  int cmp;

  off_t len = min(pivot->length, f->length);
  
  lcp = strlcp(pivot->map, f->map, len, lcp );
  f->lcp = lcp;

  if( lcp < len ) 
    cmp = f->map[lcp] - pivot->map[lcp];
  else
    cmp = f->length - pivot->length;

  return cmp;
}

#define FILESORT
#endif
