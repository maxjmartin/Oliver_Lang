# Oliver
## NOTE: the current update is a WIP update.  Focusing on safe and unsafe code design.  The current files should reflect that.  Further updates will be made as different components are redone.        
An (experimental) interpreted programming language. Written in C++, and inspired by Python, Lisp, Forth, and Erlang. This was written in order to understand how programming languages can be implemented, and to learn how to effectively program in C++.

The current implementation is a rebuild of the initial design, which was fragile and poorly executed.  But that is how you learn.  Create something, then improve it!

Package management for external libs is handled through [`vcpkg`](https://vcpkg.io/en/).  

The Current ToDo List:
- Incorporate Modules in CMake.
- Finish integrating fmt argument integration, moving away from streams.
- Multithreading.
- Update the number class for arbitrary precision math.
- Add the remaining data classes back to the interpreter.
- Add algorithms, and views.  
- Add CMake testing.
- Get rid of boiler plate project code, in place while learning CMake.

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
