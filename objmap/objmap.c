/*!
 * \file objmap.c
 * \author Shawn Chin <shawn.chin@stfc.ac.uk>
 * \date July 2012
 * \brief ADT for mapping arbitrary objects to an int-based handle
 * 
 * \note We use the khash Hashtable available from 
 *       http://www.freewebs.com/attractivechaos/khash.h.html
 */
/* splint directive needed due to khash implementation */
/*@+matchanyintegral -fcnuse@*/
#include <assert.h>
#include "khash.h"
#include "objmap.h"

/* initialise khash of type "objmap" with "uint32_t" key and "void*" value */
KHASH_MAP_INIT_INT(objmap, void*)
/* KHASH_MAP_INIT_INT64(objmap, void*) // uint64_t */

/* shortcut for accessing internal hashtable with correct type */
#define MAP(om) ((khash_t(objmap)*)om->map)

ObjectMap* objmap_new(void) {
  ObjectMap *om = NULL;
  
  /* allocate mem for obj. Return NULL ptr if allocation fails */
  om = malloc(sizeof(ObjectMap));
  assert(om != NULL);
  if (om == NULL) return NULL;
  
  /* initialise counter */
  om->top = 1; /* 0 is reserved for NULL index */
  
  /* init khash of type "objmap". Stored as void* since khash_t(objmap) wouldn't
   * be defined in objmap.h. To access with correct type, use MAP(om). */
  om->map = (void*)kh_init(objmap); 
  assert(om->map != NULL);
  
  om->deallocator = NULL;
  return om;
}

void objmap_set_deallocator(ObjectMap *om, void(*deallocator)(void*)) {
  if (!om) om->deallocator = deallocator;
}

void objmap_flush(ObjectMap* om) {
  khiter_t k;
  khash_t(objmap) *_m;

  if (!om) return;
  _m = MAP(om);
  
  /* deallocate all objects stored within the hashtable */
  for (k = kh_begin(_m); k != kh_end(_m); ++k) {
    if (kh_exist(_m, k)) {
      (om->deallocator) ? om->deallocator(kh_value(_m, k))
                        : free(kh_value(_m, k));
      kh_del(objmap, _m, k);
    }
  }
}

void objmap_reset(ObjectMap* om) {
  if (!om) return;
  om->top = 1;
  objmap_flush(om);
}

void objmap_delete(ObjectMap **om_ptr) {
  ObjectMap *om;
  khash_t(objmap) *_m;
  
  if (om_ptr == NULL) return;
  om = *om_ptr;   /* get ptr to actual object */
  _m = MAP(om);
  *om_ptr = NULL; /* overwrite user's ptr with NULL */
  
  /* deallocate all objects stored within the hashtable */
  objmap_flush(om);
  
  /* delete hashtable */
  kh_destroy(objmap, _m);
  
  /* free map object */
  free(om);
}

objmap_key_t objmap_push(ObjectMap *om, void *obj) {
  int rc;
  khiter_t k;
  objmap_key_t key;
  khash_t(objmap) *_m;

  assert(om != NULL);
  assert(obj != NULL);
  
  /* check if the we've run out of keys */
  if (om->top > OBJMAP_MAX_INDEX) return OBJMAP_ERR_OVERFLOW;
  
  /* create entry in hashtable */
  _m = MAP(om);
  key = om->top++;
  k = kh_put(objmap, _m, key, &rc);
  assert(rc);
  if (!rc) { /* on error, remove entry and return err code */
    kh_del(objmap, _m, k);
    return OBJMAP_ERR_INTERNAL;
  }
  kh_value(_m, k) = obj; /* store value in given position */
  
  /* return object handle */
  return key;
}

void* objmap_get(ObjectMap *om, objmap_key_t handle) {
  khash_t(objmap) *_m;
  khiter_t k;
  
  assert(om != NULL);
  _m = MAP(om);
  
  k = kh_get(objmap, _m, handle);   /* lookup */
  if (k == kh_end(_m)) return NULL; /* if not found, return NULL */
  else return kh_value(_m, k);      /* else, return obj ptr */
}

void* objmap_pop(ObjectMap *om, objmap_key_t handle) {
  khash_t(objmap) *_m;
  khiter_t k;
  void *obj;
  
  assert(om != NULL);
  _m = MAP(om);
  
  k = kh_get(objmap, _m, handle);   /* lookup */
  if (k == kh_end(_m)) return NULL; /* if not found, return NULL */
  
  obj = kh_value(_m, k);  /* retrieve obj ptr store as value */
  kh_del(objmap, _m, k);  /* delete entry */
  
  return obj;
}
