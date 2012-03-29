libjitdbg (aka gdbnaoplz)
=========================

Inspired by the "[jist-in-time
debugging](http://msdn.microsoft.com/en-us/library/5hs4b7a6.aspx)" support in
Windows, I wanted something similar for Linux. (It's not useful *all* that
often, but it is nice every now and again.)

This library is my attempt at achieving this. It is intended to be used as an
`LD_PRELOAD` library, which will then intercept "abortable" conditions in
other processes. When it detects one, it spawns off a GDB instance and
attaches it to the process which is in its death throes.

The idea is that you'd only use it for your own programs
during development, not system-wide. It will also only work for programs
started from a console.


Building
--------

Simply run `scons` from the root of the directory. The only dependencies are
[SCons](http://scons.org/) itself and `g++`, and you'll need GDB to actually
run it.

You can also rebuild the manpage using `a2x -f manpage -v
libjitdbg.3.txt`. This requires
[Asciidoc](http://www.methods.co.nz/asciidoc/).


Using
-----

To run some command using `libjitdbg`, just run
`LD_PRELOAD=/path/to/libjitdbg/lib/libjitdbg.so your-command`. Or, if you
want to run multiple such commands, you can always `export
LD_PRELOAD=/path/to/libjitdbg/lib/libjitdbg.so` and then do stuff.

Be sure to read the caveats in the manpage! (You can view it with, e.g.,
`less`.)
