# dlink
Implementation of Knuth's dancing links algorithm
Code is in dlink.cpp and is standalone. Compile eg. with:

g++ -Wall -O3 dlink.cpp -o dlink

Run with input on stdin of the options for the exact cover problem,
see eg. iq.txt or test.txt for examples:

```
1000000000000000000000000000000010000000000100000000001100000000001
1000000000000000000001000000000010000000000110000000000100000000000
1000000000000000000000000000000100000000001000000000011000000000010
...
```

Output is written to stdout, one line per solution eg:

```
123456789abcaaa662221114a66822113344998888337c449b555537cc99bbb5777
123456789abc66aaa222111466a822113344998888337c449b555537cc99bbb5777
123456789abc99aaa222111944a822113399448888337c664b555537cc66bbb5777
...
```

If the solution consists of sequence of options o1,o2,o3,.. (in the
order they were presented in the input), the output string contains '1'
for the bits set in o1,'2' for bits set in o2 etc, so the first line
above corresponds to a 11x5 board configuration (the initial
123456789abc are just the items ensuring each piece is used just once
and will be the same in each solution):

```
aaa66222111
4a668221133
44998888337
c449b555537
cc99bbb5777
```

Note that the iq.txt file only contains options for a single orientation
of the "blue" piece:

```
x
x
xxx
```

which ensures that we only generate one solution for each reflected or
rotated solution.