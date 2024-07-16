#ifndef RESULT_H_
#define RESULT_H_

#ifndef RESULT_ASSERT
#include <assert.h>
#define RESULT_ASSERT assert
#endif // RESULT_ASSERT

/*
 * Result type:
 * 
 * typedef struct {
 *     bool is_ok;
 *     T data;
 * }<...>;
*/


#define generic_ok(Type, data_) ((Type) { .is_ok = true, data = data_ })
#define generic_err(Type, data_) ((Type) { .is_ok = false, data = data_ })

#define result_unwrap(result) (RESULT_ASSERT((result).is_ok), (result).data)
#define result_unwrap_err(result) (RESULT_ASSERT(!(result).is_ok), (result).data)
#define result_is_ok(result) ((result).is_ok)
#define result_is_err(result) (!(result).is_ok)

#ifdef RESULT_REMOVE_PREFIX
#define ok generic_ok
#undef generic_ok

#define err generic_err
#undef generic_err

#define unwrap result_unwrap
#undef result_unwrap

#define unwrap_err result_unwrap_err
#undef result_unwrap_err

#define is_ok result_is_ok
#undef result_is_ok

#define is_err result_is_err
#undef result_is_err

#endif

#endif // RESULT_H_
