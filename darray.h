/* MIT License
 *
 * Copyright (c) 2017, Victor Cushman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef _DARRAY_H_
#define _DARRAY_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* DARRAY MEMORY LAYOUT
 * ====================
 * +--------+---------+---------+-----+------------------+
 * | header | elem[0] | elem[1] | ... | elem[capacity-1] |
 * +--------+---------+---------+-----+------------------+
 *          ^
 *          Handle to the darray points to the first
 *          element of the array.
 *
 * HEADER DATA
 * ===========
 *  size_t : sizeof contained element
 *  size_t : length of the darray
 *  size_t : capacity of the darray
 */

/**@function
 * @brief Allocate a darray of `nelem` elements each of size `size`.
 *
 * @param nelem : Initial number of elements in the darray.
 * @param size : `sizeof` each element.
 *
 * @return Pointer to a new darray.
 */
static inline void* da_alloc(size_t nelem, size_t size);

/**@function
 * @brief Free a darray.
 *
 * @param darr : Target darray to be freed.
 */
static inline void da_free(void* darr);

/**@function
 * @brief Returns the number of elements in the darray.
 *
 * @param darr : Target darray.
 * @return Number of elements in the darray.
 */
static inline size_t da_length(void* darr);

/**@function
 * @brief Returns the maximum number of elements the darray can hold without
 *  requiring resizing.
 *
 * @param darr : Target darray.
 * @return Total number of allocated elements in the darray.
 */
static inline size_t da_capacity(void* darr);

/**@function
 * @brief Returns the `sizeof` contained elements in the darray.
 *
 * @param darr : Target darray.
 * @return `sizeof` elements in the darray.
 */
static inline size_t da_sizeof_elem(void* darr);

/**@function
 * @brief Change the length of the darray to `nelem`. Data for elements with
 *  indices >= `nelem` may be lost when downsizing.
 *
 * @param darr : Target darray. Upon function completion, `darr` may or may not
 *  point to its previous block on the heap, potentially breaking references.
 * @return Pointer to the new location of the darray upon successful function
 *  completion. If `da_resize` returns `NULL`, allocation failed and `darr` is
 *  left untouched.
 *
 * @note Affects the length attribute of the darray.
 */
static inline void* da_resize(void* darr, size_t nelem);

/**@function
 * @brief Guarantee that at least `nelem` elements beyond the current length of
 *  the darray can be inserted/pushed without requiring resizing.
 *
 * @param darr : Target darray. Upon function completion, `darr` may or may not
 *  point to its previous block on the heap, potentially breaking references.
 * @return Pointer to the new location of the darray upon successful function
 *  completion. If `da_reserve` returns `NULL`, allocation failed and `darr` is
 *  left untouched.
 *
 * @note Does NOT affect the length attribute of the darray.
 */
static inline void* da_reserve(void* darr, size_t nelem);

/**@macro
 * @brief Insert a value at the back of `darr`.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param value : Value to be pushed onto the back of the darray.
 *
 * @note Affects the length of the darray.
 * @note This macro implimentation is the fast version of `da_spush`. Unlike the
 *  rest of the API, failed allocations from resizing with `da_push` may
 *  cause your program to blow up as reallocs are always reassigned back to
 *  `darr`. With this version of push, the user sacrifices safety for speed.
 */
#define /* void */da_push(/* void* */darr, /* ELEM_TYPE */value)               \
                                                           _da_push(darr, value)

/**@macro
 * @brief Push a value to the back of `darr`. This is the safe version of
 *  `da_push`.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param value : Value to be pushed onto the back of the darray.
 * @param backup : lvalue that will store a backup of `darr` in in case of
 *  reallocation failure.
 *
 * @note Affects the length of the darray.
 * @note If malloc fails to allocate memory during automatic array resizeing
 *  a backup of the darray will be saved to `backup` and `darr` will be
 *  set to `NULL`.
 */
#define /* void* */da_spush(/* void* */darr, /* ELEM_TYPE */value,             \
    /* void* */backup)                                                         \
                                              _da_safe_push(darr, value, backup)

/**@macro
 * @brief Remove a value from the back of `darr` and return it.
 *
 * @param darr : const lvalue pointing to the target darray.
 *
 * @return Value popped off of the back of the darray.
 *
 * @note Affects the length of the darray.
 * @note `da_pop` will never reallocate memory, so popping is always
 *  allocation-safe.
 */
#define /* ELEM_TYPE */da_pop(/* void* */darr)                                 \
                                                                   _da_pop(darr)

/**@macro
 * @brief Insert a value into `darr` at the specified index, moving the values
 * beyond `index` back one element.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param index : Array index where the new value will appear.
 * @param value : Value to be inserted onto the darray.
 *
 * @note Affects the length of the darray.
 * @note This macro implimentation is the fast version of `da_sinsert`. Unlike
 *  the rest of the API, failed allocations from resizing with `da_insert` may
 *  cause your program to blow up as reallocs are always reassigned back to
 *  `darr`. With this version of insert, the user sacrifices safety for
 *  speed.
 */
#define /* void */da_insert(/* void* */darr, /* size_t */index,                \
    /* ELEM_TYPE */value)                                                      \
                                                  _da_insert(darr, index, value)

/**@macro
 * @brief Insert a value into `darr` at the specified index, moving the values
 * beyond `index` back one element. This is the safe version of `da_insert`.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param index : Array index where the new value will appear.
 * @param value : Value to be inserted onto the array.
 * @param backup : lvalue that will store a backup of `darr` in in case of
 *  resize failure.
 *
 * @note Affects the length of the darray.
 * @note If malloc fails to allocate memory during automatic array resizeing
 *  a backup of the darray will be saved to `backup` and `darr` will be set to
 *  `NULL`.
 */
#define /* void* */da_sinsert(/* void* */darr, /* size_t */index,              \
    /* ELEM_TYPE */value, /* void* */backup)                                   \
                                     _da_safe_insert(darr, index, value, backup)

/**@macro
 * @brief Remove the value at index from `darr` and return it, moving the
 * values past `index` up one element.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param index : Array index of the value to be removed.
 *
 * @return Value removed from the darray.
 *
 * @note Affects the length of the darray.
 * @note `da_remove` will never reallocate memory, so removing is always
 *  allocation-safe.
 */
#define /* ELEM_TYPE */da_remove(/* void* */darr, /* size_t */index)           \
                                                         _da_remove(darr, index)

/**@macro
 * @brief Set every element of `darr` to `value`.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param VALUE_TYPE : type of `value`.
 * @param value : Value to fill the array with.
 */
#define /* void */da_fill(/* void* */darr, VALUE_TYPE, /* VALUE_TYPE */value)  \
                                               _da_fill(darr, VALUE_TYPE, value)

/**@macro
 * @brief `da_foreach` acts as a loop-block that forward iterates through all
 *  elements of `darr`. In each iteration a variable with identifier `itername`
 *  will point to an element of `darr` starting at at its first element.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param ELEM_TYPE : type of the elements of darr.
 * @param itername : identifier for the iterator within the foreach block.
 */
#define da_foreach(/* void* */darr, ELEM_TYPE, itername)                       \
                                          _da_foreach(darr, ELEM_TYPE, itername)

/**@macro
 * @brief `da_foreachr` acts as a loop-block that reverse iterates through all
 *  elements of `darr`. In each iteration a variable with identifier `itername`
 *  will point to an element of `darr` starting at its last element.
 *
 * @param darr : const lvalue pointing to the target darray.
 * @param ELEM_TYPE : type of the elements of darr.
 * @param itername : identifier for the iterator within the foreachr block.
 */
#define da_foreachr(/* void* */darr, ELEM_TYPE, itername)                      \
                                         _da_foreachr(darr, ELEM_TYPE, itername)

/**@function
 * @brief Swap the values of the two specified elements of `darr`.
 *
 * @param darr : Target darray.
 * @param index_a : Index of the first element.
 * @param index_b : Index of the second element.
 */
static inline void da_swap(void* darr, size_t index_a, size_t index_b);

///////////////////////////////// DEFINITIONS //////////////////////////////////
#define DA_SIZEOF_ELEM_OFFSET 0
#define DA_LENGTH_OFFSET   (1*sizeof(size_t))
#define DA_CAPACITY_OFFSET (2*sizeof(size_t))
#define DA_HANDLE_OFFSET   (3*sizeof(size_t))

#define DA_HEAD_FROM_HANDLE(darr_h) \
    (((char*)(darr_h)) - DA_HANDLE_OFFSET)
#define DA_P_SIZEOF_ELEM_FROM_HANDLE(darr_h) \
    ((size_t*)(DA_HEAD_FROM_HANDLE(darr_h) + DA_SIZEOF_ELEM_OFFSET))
#define DA_P_LENGTH_FROM_HANDLE(darr_h) \
    ((size_t*)(DA_HEAD_FROM_HANDLE(darr_h) + DA_LENGTH_OFFSET))
#define DA_P_CAPACITY_FROM_HANDLE(darr_h) \
    ((size_t*)(DA_HEAD_FROM_HANDLE(darr_h) + DA_CAPACITY_OFFSET))

#define DA_CAPACITY_FACTOR 1.3
#define DA_CAPACITY_MIN 10
#define DA_NEW_CAPACITY_FROM_LENGTH(length) \
    (length < DA_CAPACITY_MIN ? DA_CAPACITY_MIN : (length*DA_CAPACITY_FACTOR))

static inline void _da_memswap(void* p1, void* p2, size_t sz)
{
    char tmp, *a = p1, *b = p2;
    for (size_t i = 0; i < sz; ++i)
    {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
// Performes the following transform:
// [0][1][2][3][rest...] => [0][2][3][1][rest...]
//     ^                              ^
//     target index                   target moved to back
static inline int _da_remove_mem_mov(void* darr, size_t target_index)
{
    register size_t length = da_length(darr);
    register size_t elsz = da_sizeof_elem(darr);

    // Try and use a temporary bit of memory for storage.
    register void* tmp = malloc(elsz);
    // If the memory is avaliable then run the fast version of the algorithm
    // using memcpy and memmove.
    if (tmp != NULL)
    {
        memcpy(tmp, (char*)darr + target_index*elsz, elsz); // tmp = arr[target]
        memmove(
            (char*)darr + target_index*elsz,
            (char*)darr + (target_index+1)*elsz,
            elsz*(length-target_index-1)
        );
        memcpy((char*)darr + (length-1)*elsz, tmp, elsz); // arr[length-1] = tmp
        free(tmp);
    }
    // If the memory is NOT avaliable then run the slow, but alloc-free version
    // of the algorithm using memory swapping.
    else
    {
        register char* p_curr = (char*)darr + (target_index * elsz);
        register char* p_last = (char*)darr + ((length-1) * elsz);
        // Swap target and last elements.
        // [0][1][2][3][4][rest...] => [0][4][2][3][1][rest...]
        _da_memswap(p_curr, p_last, elsz);
        // Bubble the "new" target element (previously last element) to the back
        // [0][4][2][3][1][rest...] => [0][3][2][4][1][rest...]
        // [0][3][2][4][1][rest...] => [0][2][3][4][1][rest...]
        while ((p_curr += elsz) < p_last)
        {
            _da_memswap(
                p_curr,
                p_curr - elsz, // element before curr
                elsz
            );
        }
    }
}
#pragma GCC diagnostic pop

static inline void* da_alloc(size_t nelem, size_t size)
{
    size_t capacity = DA_NEW_CAPACITY_FROM_LENGTH(nelem);
    void* mem = malloc(capacity*size + DA_HANDLE_OFFSET);
    if (mem == NULL)
    {
        return mem;
    }
    (*(size_t*)((char*)mem + DA_SIZEOF_ELEM_OFFSET)) = size;
    (*(size_t*)((char*)mem + DA_LENGTH_OFFSET))      = nelem;
    (*(size_t*)((char*)mem + DA_CAPACITY_OFFSET))    = capacity;
    return (char*)mem + DA_HANDLE_OFFSET;
}

static inline void da_free(void* darr)
{
    free(DA_HEAD_FROM_HANDLE(darr));
}

static inline size_t da_length(void* darr)
{
    return *DA_P_LENGTH_FROM_HANDLE(darr);
}

static inline size_t da_capacity(void* darr)
{
    return *DA_P_CAPACITY_FROM_HANDLE(darr);
}

static inline size_t da_sizeof_elem(void* darr)
{
    return *DA_P_SIZEOF_ELEM_FROM_HANDLE(darr);
}

static inline void* da_resize(void* darr, size_t nelem)
{
    size_t new_capacity = DA_NEW_CAPACITY_FROM_LENGTH(nelem);
    void* ptr = realloc(DA_HEAD_FROM_HANDLE(darr),
        new_capacity*da_sizeof_elem(darr) + DA_HANDLE_OFFSET);
    if (ptr == NULL)
    {
        return NULL;
    }
    darr = ptr;
    *((size_t*)((char*)ptr + DA_CAPACITY_OFFSET)) = new_capacity;
    *((size_t*)((char*)ptr + DA_LENGTH_OFFSET))   = nelem;
    return (char*)ptr + DA_HANDLE_OFFSET;
}

static inline void* da_reserve(void* darr, size_t nelem)
{
    size_t curr_capacity = da_capacity(darr);
    size_t curr_length = da_length(darr);
    size_t min_capacity = curr_length + nelem;
    if (curr_capacity >= min_capacity)
    {
        return darr;
    }
    size_t new_capacity = DA_NEW_CAPACITY_FROM_LENGTH(min_capacity);
    void* ptr = realloc(DA_HEAD_FROM_HANDLE(darr),
        new_capacity*da_sizeof_elem(darr) + DA_HANDLE_OFFSET);
    if (ptr == NULL)
    {
        return NULL;
    }
    *((size_t*)((char*)ptr + DA_CAPACITY_OFFSET)) = new_capacity;
    return (char*)ptr + DA_HANDLE_OFFSET;
}

#define /* void* */_da_push(/* void* */darr, /* ELEM_TYPE */value)             \
do                                                                             \
{                                                                              \
    register size_t* __p_len = DA_P_LENGTH_FROM_HANDLE(darr);                  \
    if (*__p_len == *DA_P_CAPACITY_FROM_HANDLE(darr))                          \
    {                                                                          \
        (darr) = da_resize((darr), *__p_len);                                  \
        __p_len  = DA_P_LENGTH_FROM_HANDLE(darr);                              \
    }                                                                          \
    (darr)[(*__p_len)++] = (value);                                            \
}while(0)

#define /* void* */_da_safe_push(/* void* */darr, /* ELEM_TYPE */value,        \
    /* void* */backup)                                                         \
do                                                                             \
{                                                                              \
    register size_t* __p_len = DA_P_LENGTH_FROM_HANDLE(darr);                  \
    if (*__p_len == *DA_P_CAPACITY_FROM_HANDLE(darr))                          \
    {                                                                          \
        (backup) = (darr);                                                     \
        (darr) = da_resize((darr), *__p_len);                                  \
        __p_len  = DA_P_LENGTH_FROM_HANDLE(darr);                              \
        if ((darr) == NULL)                                                    \
        {                                                                      \
            /* Allocation failed, but we still have the original darray */     \
            /* stored in the back backup variable. */                          \
            break;                                                             \
        }                                                                      \
        /* Allocation succeeded continue on as normal. */                      \
    }                                                                          \
    (darr)[(*__p_len)++] = (value);                                            \
}while(0)

#define /* ELEM_TYPE */_da_pop(/* void* */darr)                                \
(                                                                              \
    (darr)[--(*DA_P_LENGTH_FROM_HANDLE(darr))]                                 \
)

#define /* void */_da_insert(/* void* */darr, /* size_t */index,               \
    /* ELEM_TYPE */value)                                                      \
do                                                                             \
{                                                                              \
    register size_t* __p_len  = DA_P_LENGTH_FROM_HANDLE(darr);                 \
    register size_t __index = (index);                                         \
    if ((*__p_len) == (*DA_P_CAPACITY_FROM_HANDLE(darr)))                      \
    {                                                                          \
        (darr) = da_resize((darr), *__p_len);                                  \
        __p_len = DA_P_LENGTH_FROM_HANDLE(darr);                               \
    }                                                                          \
    memmove(                                                                   \
        (darr)+(__index)+1,                                                    \
        (darr)+(__index),                                                      \
        (*DA_P_SIZEOF_ELEM_FROM_HANDLE(darr))*((*__p_len)-(__index))           \
    );                                                                         \
    (darr)[__index] = (value);                                                 \
    (*__p_len)++;                                                              \
}while(0)

#define _da_safe_insert(/* void* */darr, /* size_t */index,                    \
    /* ELEM_TYPE*/value, /* void* */backup)                                    \
do                                                                             \
{                                                                              \
    register size_t* __p_len  = DA_P_LENGTH_FROM_HANDLE(darr);                 \
    register size_t __index = (index);                                         \
    if ((*__p_len) == (*DA_P_CAPACITY_FROM_HANDLE(darr)))                      \
    {                                                                          \
        (backup) = (darr);                                                     \
        (darr) = da_resize((darr), *__p_len);                                  \
        if ((darr) == NULL)                                                    \
        {                                                                      \
            /* Allocation failed, but we still have the original darray */     \
            /* stored in the back backup variable. */                          \
            break;                                                             \
        }                                                                      \
        /* Allocation succeeded continue on as normal. */                      \
        __p_len = DA_P_LENGTH_FROM_HANDLE(darr);                               \
    }                                                                          \
    memmove(                                                                   \
        (darr)+(__index)+1,                                                    \
        (darr)+(__index),                                                      \
        (*DA_P_SIZEOF_ELEM_FROM_HANDLE(darr))*((*__p_len)-(__index))           \
    );                                                                         \
    (darr)[__index] = (value);                                                 \
    (*__p_len)++;                                                              \
}while(0)

#define /* ELEM_TYPE */_da_remove(/* void* */darr, /* size_t */index)          \
(                                                                              \
    (/* "then" paren(s) */                                                     \
    /* move element to be removed to the back of the array */                  \
    _da_remove_mem_mov(darr, index)                                            \
    ), /* then */                                                              \
    /* return darr[--length] (i.e the removed element) */                      \
    (darr)[--(*DA_P_LENGTH_FROM_HANDLE(darr))]                                 \
)

#define /* void */_da_fill(/* void* */darr, VALUE_TYPE, /* VALUE_TYPE */value) \
do                                                                             \
{                                                                              \
    register size_t __len = *DA_P_LENGTH_FROM_HANDLE(darr);                    \
    register VALUE_TYPE __value = (value);                                     \
    for (size_t __i = 0; __i < __len; ++__i)                                   \
    {                                                                          \
        (darr)[__i] = (__value);                                               \
    }                                                                          \
}while(0)

#define _da_foreach(/* void* */darr, ELEM_TYPE, itername)                      \
for (ELEM_TYPE* itername = darr;                                               \
    itername < (darr) + da_length(darr);                                       \
    itername++)                                                                \

#define _da_foreachr(/* void* */darr, ELEM_TYPE, itername)                     \
for (ELEM_TYPE* itername = &(darr)[da_length(darr)-1];                         \
    itername >= (darr);                                                        \
    itername--)                                                                \

#endif // !_DARRAY_H_

static inline void da_swap(void* darr, size_t index_a, size_t index_b)
{
    register size_t size = da_sizeof_elem(darr);
    _da_memswap(
        ((char*)darr) + (index_a * size),
        ((char*)darr) + (index_b * size),
        size
    );
}
