#include <assert.h>
#include <stdio.h>
#include "counter.h"

int main(void) {
  unsigned int v;
  counter c1, c2;
  
  printf("Running very simple test ... ");
  
  /* initialise counter manager */
  counter_manager_init();
  
  c1 = counter_new();
  c2 = counter_new();
  assert(counter_peek(c1) == 0);
  assert(counter_peek(c2) == 0);
  
  v = counter_increment(c1);
  assert(v == 1);
  assert(counter_peek(c1) == 1);
  assert(counter_peek(c2) == 0);  
  
  counter_increment(c2);
  counter_increment(c2);
  counter_reset(c1);  
  assert(counter_peek(c1) == 0);
  assert(counter_peek(c2) == 2);  

  counter_delete(&c1);
  assert(c1 == OBJMAP_NULL);
  
  /* even if we don't delete c2, its memory should automatically be freed
   * when the object map is destroyed
   * (test this using valgrind)
   */
  
  /* finalise counter manager */
  counter_manager_finalise();
  
  printf("PASS\n");
  return 0;
}
