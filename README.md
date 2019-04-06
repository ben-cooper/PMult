# PMult

Small program to explore ranges of numbers to find the number with the highest multiplicative persistence.  Can also be used to check individual numbers

## Compilation

`make`

## Usage

### pmult

Single-threaded version.

Checking a range:

`./pmult <min> <max>`

or to check a single number:

`./pmult <number>`

### threadpmult

Multi-threaded version (using pthreads)

Checking a range:

`./threadpmult <min> <max> <threads>`

or to check a single number:

`./threadpmult <number>`

### forkpmult

Multi-process version (using fork)

Checking a range:

`./forkpmult <min> <max> <threads>`

or checking a single number:

`./forkpmult <number>`

### Example Usage

Checking all number between 11 and 10000 with 5 threads:

`$ ./forkpmult 11 10000 5

Min: 11 Max: 2008 PID: 12048
Min: 2008 Max: 4005 PID: 12049
Min: 4005 Max: 6002 PID: 12050
Min: 6002 Max: 7999 PID: 12051
Min: 7999 Max: 10000 PID: 12052
Process 12048 complete!
Process 12050 complete!
Process 12049 complete!
Process 12052 complete!
Process 12051 complete!

Best number: 6788
Levels: 6`
