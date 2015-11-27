YEngine
========

YEngine is a homebrew engine seeking to experiment with automatic parallelization.

The idea is to use generic memory pool based allocators to store all data in
pools of memory, while all operations on the data are written with a common
function declaration which operates on all the data in arrays.

Under this structure, traditional classes no longer own data, but rather they
own references to contiguous arrays and must operate on them together.

Functions access the array must declaring their intent in 3 types:
  * Read-only
  * Write-only
  * Read-Write

Each function is registered into a global list of functions along with the
intention of each array they will have access to. They also register any
functions they are dependent on. In this way a global scheduler can use
this information and create a dependency graph having all the information
it needs to execute as much as they can in parallel. In theory the global
scheduler can also understand the current system it is running on and
make choices to warm up the cache to reduce cache misses as well, but this
is further down the road map.

The other advantage of having all data operating functions be generically
defined is that they can easily be scriptable in any language. Much of the
initial code will be written in C++ but I intend to also write a scripting
language based on scheme which can easily script data operations which
can be updated in real-time.
