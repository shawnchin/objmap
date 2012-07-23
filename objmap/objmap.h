/*!
 * \file objmap.c
 * \author Shawn Chin <shawn.chin@stfc.ac.uk>
 * \date July 2012
 * \brief ADT for mapping arbitrary objects to an int-based handle
 */
#ifndef OBJMAP_H_
#define OBJMAP_H_
#include <stdint.h>

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
 *   depends on the datatype used for keys. See ::objmap_key_t.
 *   - To keep the code simple, we do not reuse keys from deleted items
 *   - By default, we use 32-bit unsigned integers for keys (uint32_t). Compile
 *     with OBJMAP_USE_64BIT_KEYS to switch to 64-bit unsigned ints (uint64_t)
 * 
 * \note This is a stripped down version and is essentially a wrapper around
 * the internal hashtable implementation (see khash.h).
 * 
 * @{*/

/*! \brief Variable type used as hashtable key */
#ifdef OBJMAP_USE_64BIT_KEYS
typedef uint64_t objmap_key_t;
#else
typedef uint32_t objmap_key_t;
#endif


/*! \brief maximum possible value for key type 
 * 
 * This depends on the datatype used to represent keys. See ::objmap_key_t
 */
#define OBJMAP_KEY_LIMIT ((objmap_key_t)-1)
/* #define OBJMAP_KEY_LIMIT UINT_MAX */


/* return values */
/*! \brief NULL handle */
#define OBJMAP_NULL ((objmap_key_t)0)
#define OBJMAP_ERR_OVERFLOW ((objmap_key_t)(OBJMAP_KEY_LIMIT - 0))
#define OBJMAP_ERR_INTERNAL ((objmap_key_t)(OBJMAP_KEY_LIMIT - 1))
#define OBJMAP_MAX_INDEX    ((objmap_key_t)(OBJMAP_KEY_LIMIT - 2))

/*! \brief Pointer type for functions that can be used in place of free() */
typedef void (*objmap_free_func_t)(void*);

/*! \brief Data Structure representing an object map */
typedef struct {
  objmap_key_t top; /*!< Next key value to assign */
  void* map;        /*!< Pointer to hash table used to storage */
  void (*deallocator)(void*); /*!< Custom deallocator function for members */
} ObjectMap;

/*! 
 * \brief Creates a new object map
 * \return Pointer to the newly created map
 *
 * If an error occurs (e.g. insufficient memory), \c NULL is returned.
 */
ObjectMap* objmap_new(void);

/*!
 * \brief Specify a deallocation function to use when freeing objects
 * \param[in] om Reference to map
 * \param[in] deallocator Deallocation function for objects within map
 *
 * By default, \c free() is used to free objects with the mapper. This routine
 * allows users to specify a custom function to be use - this is useful when
 * memory associated with the object also needs to be freed.
 *
 * When set to \c NULL, the mapper will revert back to the default behaviour. 
 */
void objmap_set_deallocator(ObjectMap *om, void(*deallocator)(void*));

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
 * \brief Deletes all objects within the map
 * \param[in] om Reference to map
 *
 * This will delete all objects within the map while maintaining the internal
 * map datastructure. This allows users to quickly all objects within
 * the map without having to destroy and recreate the object mapper.
 *
 * If \c reset_handles is not \c 0 (default), internal counters will be reset
 * which allow handles to be recycled. Do note that this can potentially lead to
 * confusing errors if stale handles are later used for querying the map.
 */
void objmap_flush(ObjectMap* om);

/*!
 * \brief Deletes all objects within the map and resets handle counter
 * \param[in] om Reference to map
 * 
 * Does the same thing as objmap_flush() except that it also resets the
 * internal counters so that object handles can be recycled.
 *
 * This can help avoid the issue of running out of keys for long running
 * applications that repeatedly populates and flushes the mapper. However, do
 * not that this can potentially lead to confusing errors if stale handles are
 * later used for querying the map.
 */
void objmap_reset(ObjectMap* om);

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
