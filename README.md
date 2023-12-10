# Oliver
An (experimental) interpreted programming language. Written in C++, and inspired by Python, Lisp, Forth, and Erlang. This was written in order to understand how programming languages can be implemented, and to learn how to effectivley program in C++.

The current implimentation is a rebuild of the initial design, which was fragile and pourly executed.  But that is how you learn.  Create something, then improve it!

The Current ToDo List:
    * Incorporate Modules in CMake.
    * Finish integrating fmt argument integration, moving away from streams.
    * Multithreading.
    * Update the number class for arbitrary precision math.
    * Add the remaining data classes back to the interpreter.
    * Add algorithyms, and views.  

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
