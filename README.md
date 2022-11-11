# Oliver
An (experimental) immutable interpreted programming language. Written in C++, and inspired by Python, Lisp, Forth, and Erlang. It is designed to be thread safe and suitable for real time systems. This was written in order to understand how programming languages can be implemented.

The current implimentation is a rebuild of the initial design, which was fragile and pourly executed.  But that is how you learn.  Create something, then improve it!

### Initial Build Configuration
```
mkdir build 
cd build
cmake ../
```
### After Intital Build
From the root Oliver directory.
```
cmake build
cmake --build build
```
