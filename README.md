# wordle
Solves wordles with brute force

Example usage:
```
Word entered    : stare
Result (colors) :     y

116 possible words remaining
Top choices:
12950 loden
12934 olden
12918 indol
12908 colin
12904 lined
12900 nicol
12874 noily
12868 dynel
12866 doily
12856 boned
12852 incel
12848 coden
12842 cibol
12840 lownd
12838 coned
12836 honed
12830 pelon
12824 bidon
12824 lobed
12820 oiled

Word entered    : loden
Result (colors) :    gy

1 possible words remaining
0 piney

Top choices:

Word entered    : new game


Starting new game...

Word entered    : crane
Result (colors) : eval
5176938 crane

Word entered    : crane
Result (colors) : compare
Second word     : stare
stare is better by ~0.333015%
```


Notes for compiling:
- needs C++17 or later
- with gcc, install Intel TBB and compile with `-ltbb` to take advantage of multithreading (TBB not needed for msvc)
