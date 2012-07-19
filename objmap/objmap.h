/*!
 * \file objmap.c
 * \author Shawn Chin <shawn.chin@stfc.ac.uk>
 * \date July 2012
 * \brief ADT for mapping arbitrary objects to an int-based handle
 */
#ifndef OBJMAP_H_
#define OBJMAP_H_
#include <limits.h>

/*! \defgroup OBJMAP Utility: Object Mapper 
 * 
 * The Object Mapper allows a library to store internal objects within a hash 
 * table and access them via int-based handles. This provides an implementation 
 * of opaque objects with the following pros and cons:
 *
 * Pros:
 * - Object handles returned to users are integer based which makes it more 
 *   identifiable when debugging
 * - The actual objects are completely isolated from users
 * - The mapper holds a pointer to all mapped objects which means the library 
 *   developer can manage the deallocation of objects at the end of its 
 *   lifespan. Deleting the mapper also automatically deallocates all objects
 *   stored within it.
 * 
 * Cons:
 * - Slower than using opaque pointers due to the hashtable lookup
 * - Can potentially run out of keys (generated incrementally). The limit 
 *   depends on the datatype used for keys (unsigned int, unsigned long, or even
 *   unsigned long long). See ::objmap_key_t.
 *   - To keep the code simple, we do not reuse keys from deleted items
 * 
 * \note This is a stripped down version and is essentially a wrapper around
 * the internal hashtable implementation (see khash.h).
 * 
 * @{*/

/* To use a different datatype for keys, the following needs to be modified:
 * - the type definition of objmap_key_t
 * - the value of OBJMAP_KEY_LIMIT. 
 * - the khash initialisation call (in objmap.c) : KHASH_MAP_INIT_?()
 */
 
/*! \brief Variable type used as hashtable key */
typedef unsigned int objmap_key_t;
/* typedef unsigned long objmap_key_t; */

/*! \brief maximum possible value for key type 
 * 
 * This depends on the datatype used to represent keys. See ::objmap_key_t
 */
#define OBJMAP_KEY_LIMIT UINT_MAX 
/* #define OBJMAP_KEY_LIMIT ULONG_MAX */

/* return values */
/*! \brief NULL handle */
#define OBJMAP_NULL 0
#define OBJMAP_ERR_OVERFLOW (OBJMAP_KEY_LIMIT - 0)
#define OBJMAP_ERR_INTERNAL (OBJMAP_KEY_LIMIT - 1)
#define OBJMAP_MAX_INDEX    (OBJMAP_KEY_LIMIT - 2)

/*! \brief Data Structure representing an object map */
typedef struct {
  objmap_key_t top; /*!< Next key value to assign */
  void* map;        /*!< Pointer to hash table used to storage */
} ObjectMap;

/*! 
 * \brief Creates a new object map
 * \return Pointer to the newly created map
 * 
 * If an error occurs (e.g. insufficient memory), \c NULL is returned.
 */
ObjectMap* objmap_new(void);

/*!
 * \brief Adds a new object to the map
 * \param[in] om Reference to map
 * \param[in] obj Address of object to be added
 * \return Object handle (if successful) or error code
 * 
 * Users are encouraged to check that the returned handle is less than
 * ::OBJMAP_MAX_INDEX. Values above :OBJMAP_MAX_INDEX are reserved for error
 * indicators.
 * 
 * Possible error codes:
 * - ::OBJMAP_ERR_OVERFLOW (We've run out of keys)
 * - ::OBJMAP_ERR_INTERNAL (The hashtable implementation return an error)
 */
objmap_key_t objmap_push(ObjectMap *om, void *obj);

/*!
 * \brief Retrieve an object associated with a handle
 * \param[in] om Reference to map
 * \param[in] handle Object handle
 * \return Object address, or \c NULL if \c handle is invalid
 * 
 * If the \c handle is invalid, \c NULL is returned.
 * 
 * Users MUST NOT free the object that is returned since it is still referenced
 * by the map.
 */
void* objmap_get(ObjectMap *om, objmap_key_t handle);

/*!
 * \brief Removes an object from the map and return its address
 * \param[in] om Reference to map
 * \param[in] handle Object handle
 * \return Object address, or \c NULL if \c handle is invalid
 * 
 * If the \c handle is invalid, \c NULL is returned.
 * 
 * Users are EXPECTED TO free the object that is returned since it is no longer
 * referenced by the map.
 */
void* objmap_pop(ObjectMap *om, objmap_key_t handle);

/*!
 * \brief Deletes the map and all objects stored within it
 * \param[in] om_ptr Variable address storing pointer to the map
 * 
 * All objects still referenced by the map will be free along with the map.
 * 
 * The map pointer will be set to \c NULL.
 */
void objmap_delete(ObjectMap **om_ptr);

/*! @} */
#endif  /* OBJMAP_H_ */
