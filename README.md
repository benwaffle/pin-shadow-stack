# Shadow Stack

DynamoRIO version - https://github.com/benwaffle/dynamorio-shadow-stack

(runtime binary instrumentation for security)

This implements a shadow stack, which keeps a copy of every return address to prevent ROP attacks. It also handles `setjmp`/`longjmp`, unix signals, and C++ exception handling (Itanium ABI).

How to use
----

Put [intel pin](http://pintool.org) in `../pin-2.14`

`make` (silent) or `make debug` (very verbose)

`make run` uses hello world
