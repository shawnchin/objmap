/*!
 * \brief A simple example of how objmap can be used to manage and store 
 * internal objects that are completely isolated from users.
 * 
 * For brevity, we use only assertions to check for errorneous input and usage.
 * In practice, one should handle potential error conditions and return an
 * appropriate error code instead of bailing out (which is what assert() does).
 * 
 * \note Assertions are disabled when NDEBUG is defined.
 */

#ifndef COUNTER_H_
#define COUNTER_H_
#include "objmap/objmap.h"

/*! \brief Datatype used to reference counter object */
typedef objmap_key_t counter;

/*! \brief NULL handle */
#define OBJMAP_NULL COUNTER_NULL

/*! \brief Initiase the module so counters can be created 
 *
 * This should be called at the beginning of the program before any other
 * counter_*() routines are called. 
 */
void counter_manager_init(void);

/*! \brief Finalises the module 
 * 
 * This should be called before the program ends. No other counter_*() routines
 * should be called onces the module is finalised.
 * 
 * All counter objects that have not been manually destroyed will automatically
 * be freed. 
 */
void counter_manager_finalise(void);

/*! 
 * \brief Creates a new counter
 * \return Handle for the new counter object
 */
counter counter_new(void);

/*!
 * \brief Resets the counter 
 * \param[in] c Handle to the counter object
 * 
 * If the handle is invalid, this routine does nothing
 */
void counter_reset(counter c);

/*!
 * \brief Increments the counter
 * \param[in] c Handle to the counter object
 * \return New counter value
 * 
 * If the handle is invalid, 0 is returned.
 */
unsigned int counter_increment(counter c);

/*!
 * \brief Returns the current value of a counter
 * \param[in] c Handle to the counter object
 * \return Current counter value
 * 
 * If the handle is invalid, 0 is returned.
 */
unsigned int counter_peek(counter c);

/*!
 * \brief Returns the current value of a counter
 * \param[in,out] c_ptr Address of the var holding the handle 
 * 
 * The counter object is deleted and \c c_ptr is set to :COUNTER_NULL
 */
void counter_delete(counter *c_ptr);

#endif  /* COUNTER_H_ */
