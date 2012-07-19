ObjectMap
=========

Object Mapper allows a library store and lookup internal objects using int-based
keys. This provides an implementation of opaque objects with the following pros 
and cons:

Pros:
- Object handles returned to users are integer based which makes it more 
  identifiable when debugging
- The actual objects are completely isolated from users (compared to opaque 
  pointers where users still have the address to the actual object)
- The mapper holds a pointer to all mapped objects which means the library 
   developer can manage the deallocation of objects at the end of its 
   lifespan. 
- Stored objects are automatically deallocated when the Mapper is destroyed.
 
Cons:
- Slower than using opaque pointers due to lookups
- Can potentially run out of keys (generated incrementally). The limit 
   depends on the datatype used for keys (unsigned int, unsigned long, or even
   unsigned long long).
 - To keep the code simple, we do not reuse keys from deleted items
 

This is a stripped down version of a module used within an existing product.
At this bare-bone level, it is essentially a wrapper around the internal 
hashtable implementation (we use 
[khash.h](http://www.freewebs.com/attractivechaos/khash.h.html) which 
is now released under [klib](https://github.com/attractivechaos/klib)).

Usage
=====

See the `example/` directory for an example.


-----

*Copyright (C) 2012 Shawn Chin*

*This software is released under the MIT License. See LICENSE.txt for details.*
