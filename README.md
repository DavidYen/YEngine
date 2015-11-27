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

How to build
========
YEngine has converted to using chromium's depot_tools to manage dependencies
and build rules instead of git submodules. Once depot_tools has been added
to your path simply use "gclient sync" to synchronize all the dependent
repositories.

There are 4 configurations (Debug, Release, Gold, Platinum). The differences
are roughly as follows:
 * Debug - Optimizations disabled, all symbols available, debug code
  paths/checks enabled, helper tools (profilers/JIT scripting) enabled.
 * Release - Optimizations enabled, symbols available, debug code
  paths/checks enabled, helper tools (profilers/JIT scripting) enabled.
 * Gold - Optimizations enabled, symbols available, debug code
  paths/checks disabled, helper tools (profilers/JIT scripting) enabled.
 * Platinum - Optimizations enabled, symbols disabled, all extra code
  paths/helper tools disabled. (This is in theory for shipping code).

Builds are done through ninja, simple run "ninja -C out/{config} {target}"
