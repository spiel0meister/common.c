#ifndef OPTION_H_
#define OPTION_H_

#ifndef OPTION_ASSERT
#include <assert.h>
#define OPTION_ASSERT assert
#endif // OPTION_ASSERT

/*
 * Option type:
 * 
 * typedef struct {
 *     bool has_value;
 *     T data;
 * }<...>;
*/

#define some(Type, data_) ((Type) { .has_value = true, data = data_ })
#define none(Type) ((Type) { .has_value = false })

#define option_unwrap(option) (OPTION_ASSERT((option).has_value), (option).data)
#define option_is_some(option) (option).has_value
#define option_is_none(option) !(option).has_value

#endif // OPTION_H_

#ifdef OPTION_IMPLEMENTATION
#warning "no implementations"
#endif // OPTION_IMPLEMENTATION
