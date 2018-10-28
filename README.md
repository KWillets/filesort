# filesort
High-performance tool to sort and deduplicate files by their contents

filesort accepts a list of files and sorts them into the lexicographic order of their contents (as unsigned char), using an
algorithm suited to long strings.  Since it avoids reading past the distinguishing prefix of each file, it will outperform hashing 
on many filesets.

## Usage 


### filesort 

Currently filesort reads the file list from stdin:

    ls *.txt | ./filesort

and outputs the file list in sorted order.

Example:

    $ echo a >y.txt
    $ echo b >x.txt
    $ ls *.txt |./filesort 
    y.txt
    x.txt
   
It currently doesn't identify duplicates (todo).

### filecmp

filecmp compares two files using the same comparison method as the sort (useful for checking results):

    $ ./filecmp y.txt x.txt 
    comparing y.txt x.txt
    cmp = 1 lcp = 0

    
## Building

    make filesort
    make filecmp


