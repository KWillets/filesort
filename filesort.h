#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <algorithm>

#ifndef FILESORT

typedef struct fp{
  std::string name;
  off_t lcp;
  off_t length;
  int fd;
  unsigned char *map;

  static size_t fsize( std::string fname ) {
    struct stat s;
    if(stat(fname.c_str(), &s) == 0)
      return s.st_size;
    else {
      perror("stat failed");
      return 0;
    }
  };

  fp(std::string _name): lcp(0), map(0), fd(0) {
    name = _name;
    length = fsize(name);
  }

  void fopen() {
    if(!fd)
      fd = open(name.c_str(), O_RDONLY);
    if(fd < 0)
      perror("open failed");
    
    map = (unsigned char *) mmap(0, length, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
    if(map == MAP_FAILED)
      perror("map failed");
  }

  void fclose() {
    munmap(map, length);
    close(fd);
    fd = 0;
  }

} fptr;

off_t strlcp(unsigned char *s, unsigned char *t, size_t l, off_t lcp ) {

  lcp &= ~7; 
  for(; lcp < (l & ~7) && *(uint64_t *)(s + lcp) == *(uint64_t *)(t + lcp); lcp += 8)
    ;

  for(; lcp < l && s[lcp] == t[lcp]; lcp++)
    ;
  
  return lcp;
}

int fcmp( fptr *pivot, fptr *f, off_t lcp ) {

  off_t len = std::min(pivot->length, f->length);

  lcp = strlcp(pivot->map, f->map, len, lcp);
  f->lcp = lcp;

  int cmp;

  if( lcp < len )
    cmp = f->map[lcp] - pivot->map[lcp];
  else
    cmp = f->length - pivot->length;

  return cmp;
}

#define FILESORT
#endif
