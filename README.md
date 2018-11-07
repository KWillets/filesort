# filesort
High-performance tool to sort and deduplicate files by their contents

filesort accepts a list of files and sorts them into the lexicographic order of their contents (as unsigned char), using an
algorithm suited to long strings.  Since it avoids reading past the distinguishing prefix of each file, it will outperform hashing 
on many filesets.

While sorting, it determines the longest common prefix (LCP) of each file vs. its predecessor.  If this prefix is the entire file, it 
is a duplicate.

## Usage 


### filesort 

Currently filesort reads the file list from stdin:

    ls *.txt | ./filesort

and outputs the file list in sorted order.

Example:

    $ echo a >y.txt
    $ echo b >x.txt
    $ ls *.txt |./filesort 
         0 y.txt
         0 x.txt
   
It outputs the longest common prefix of each file with the previous, and if that matches the file length, the letter 'D' is added to indicate a duplicate (interface work in progress)

Example:

    find . -name \*.txt |gshuf |./filesort
          0 ./y.txt
    D     2 ./test/a.txt
          1 ./test/aa2.txt
    D     3 ./aa3.txt
    D     3 ./test/aa.txt
          2 ./test/aaa.txt


### filecmp

filecmp compares two files using the same comparison method as the sort (useful for checking results):

    $ ./filecmp y.txt x.txt 
    comparing y.txt x.txt
    cmp = 1 lcp = 0

    
## Building

    make



