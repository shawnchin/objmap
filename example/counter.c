#include <assert.h>
#include <stdlib.h>
#include "counter.h"

/*! \brief Internal representation of a counter */
struct _counter {
  unsigned int value;
};

/*! \brief Map to store counters */
static ObjectMap *map = NULL;

/* create the map to store counter objects */
void counter_manager_init(void) {
  if (map != NULL) return; /* already initialised */
  map = objmap_new();
}

/* destroy map and all objects within */
void counter_manager_finalise(void) {
  if (map == NULL) return; /* not yet initialsed or already finalised */
  objmap_delete(&map); /* this should also free all existing counter objs */
}

/* deletes all registered counters */
void counter_manager_delete_all(void) {
  if (map == NULL) return; /* not yet initialsed or already finalised */
  objmap_flush(map);
}

/* allocate new object, store in map and return handle */
counter counter_new(void) {
  counter c;
  struct _counter *obj;
  assert(map != NULL); /* make sure map initialised */
  
  /* allocate and initialise actual counter object */
  obj = malloc(sizeof(struct _counter));
  obj->value = 0;
  
  c = objmap_push(map, (void*)obj);
  assert(c <= OBJMAP_MAX_INDEX); /* check for error conditions */
  return c;
}

/* retrieve object and set value back to 0 */
void counter_reset(counter c) {
  struct _counter *obj;
  assert(map != NULL); /* make sure map initialised */
  
  obj = (struct _counter*)objmap_get(map, c);
  if (obj != NULL) obj->value = 0;
}

/* retrieve object and increment value. Return new value. */
unsigned int counter_increment(counter c) {
  struct _counter *obj;
  assert(map != NULL); /* make sure map initialised */
  
  /* retrieve object */
  obj = (struct _counter*)objmap_get(map, c);
  if (obj == NULL) return 0;  /* invalid key */
  
  obj->value++;
  return obj->value;
}

/* retrieve object and return current value. */
unsigned int counter_peek(counter c) {
  struct _counter *obj;
  assert(map != NULL); /* make sure map initialised */
  
  /* retrieve object */
  obj = (struct _counter*)objmap_get(map, c);
  if (obj == NULL) return 0; /* invalid key */
  
  return obj->value;
}

/* remove from map and deallocate object */
void counter_delete(counter *c_ptr) {
  struct _counter *obj;
  counter c = *c_ptr;   /* get handle */
  assert(map != NULL); /* make sure map initialised */
  
  *c_ptr = COUNTER_NULL; /* set user's handle to NULL value */
  
  obj = objmap_pop(map, c); /* retrieve and remove from map */
  if (obj != NULL) free(obj);
}
