# Shadow Stack

(runtime binary instrumentation for security)

This implements a shadow stack, which keeps a copy of every return address to prevent ROP attacks. It also handles `setjmp`/`longjmp`, unix signals, and C++ exception handling (Itanium ABI).

How to use
----

Put [intel pin](http://pintool.org) in `../pin-2.14`

`make` to build the pintool. `make debug` will cause the tool to print stuff for every `call`/`ret`

`make run` to run hello world. There are more sample programs in `samples/`

