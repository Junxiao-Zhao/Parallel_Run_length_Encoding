## Parallel Run-length Encoding

Run-length encoding (RLE) is a simple yet effective compression algorithm: repeated data are stored as a single data and the count.  

This tool uses POSIX threads to build a threadpool to encode multiple files parallelly into a single file using RLE 

Commands:

    $ ./nyuenc
    Usage: ./nyuenc <file...> [-j <threads>]