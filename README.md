# Memory-allocator
Simple memory allocator. Implemented malloc(), free(), calloc(), realloc(). <br>
Used linux sbrk() system calls for heap manegment.The brk and sbrk functions are historical curiosities left over from earlier days before the advent of virtual memory management.<br>
TODO : Learn about nmap() and implement a modern memory allocator.<br>
#### Compile and Run ####
```
gcc -o memory_allocator.so -fPIC -shared memory_allocator.c
export LD_PRELOAD=$PWD/memalloc.so
```
Running ls successfully means the memory allocator runs correctly.
```
ls
```
Stop using the allocator by
```
unset LD_PRELOAD
```
Based on this [Tutorial](https://arjunsreedharan.org/post/148675821737/write-a-simple-memory-allocator)