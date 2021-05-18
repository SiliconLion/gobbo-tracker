#ifndef VECTOR_H
#define VECTOR_H
#include <stdint.h>
#include <stdlib.h>

#include <stdbool.h>



//simple utilites. Much of what goes here will have a GLib equivelent, but I 
//don't feel like using them

//need to add an error type, but debating on the best way to do that. for now will
//punt and just annotate functions with what the values returned as errors represent. 
//Error values will always be negative though. 

typedef struct {
    size_t stride; //how many bytes a given element in the GobboVector is
    size_t count; //how many elements in the GobboVector
    size_t capacity; //how many elements the GobboVector can hold without reallocating
    void* data; //The actual data pointed to by the GobboVector
} GobboVector;

//ONLY VALID WAY TO CREATE A GobboVector
//creates a new GobboVector where an element is `stride` bytes long, with enough memory to 
//hold `initial_capacity` elements. Will always allocate at least 1 element.
//`*alloc_error` will be set to -1 if GobboVector cannot allocate enough memory to match 
//the specifications or if `stride * initial_capacity` is larger than a `size_t` can hold.
//If `alloc_error` is `NULL`, then will call `exit(EXIT_FAILURE)` rather than setting an error. 
//Doesn't protect against `stride` being 0 bytes. You can of course check this is not the case in the 
//returned GobboVector. 
//sets `*alloc_error = 0` on succes.
GobboVector gobbo_vector_new(size_t stride, size_t inital_capacity, int* alloc_error);

//returns if the vector has filled `v->data` to its capacity
bool gobbo_vector_is_full(GobboVector* v);

//changes the capacity of the vector. `new_capacity` is the target number of elements the
//GobboVector should be able to store. 
//If GobboVector is unable to increase its memory to meet `new_capacity`, then `*alloc_error` will
//be set to -1.
//`*alloc_error` will also be set to -1 if `v->stride * new_capacity` is overflows a `size_t`
//If `new_capacity` does not have enough space for the elements in v, no operation is performed, 
//and `*alloc_error` will be set to -2.
//If `new_capacity` is set to 0, then GobboVector will double the capacity.
//In that case, `*alloc_error` will be set to -1 if GobboVector is unable to allocate as much as it wants.
//Sets `*alloc_error = 0` on succes.
void gobbo_vector_resize(GobboVector* v, size_t new_capacity, int* alloc_error);

//element in the GobboVector v at index `index` is copied into `dest`. DOES NOT ALLOCATE so
//make sure `dest` is appropriately allocated. 
//`*bounds_error` is set to -1 if index is out of bounds. set to 0 otherwise.
void gobbo_vector_get(GobboVector* v, void* dest, size_t index, int* bounds_error);

void gobbo_vector_set(GobboVector* v, void* element, size_t index, int* bounds_error);

//copies the value pointed to by `element` to the end of the GobboVector. 
//assumes the size of the pointed element is `v.stride`
//if the vector is full, `gobbo_vector_resize` is called with a `new_capacity` of 0 (doubling the size).
//if that fails, `*error` will be set to -1.
//on success, `*error ` will be set to 0.
//If `error` is NULL, then will call exit if it can't reallocate.
//v->count will not be updated unless element successfully copied into v.
void gobbo_vector_push(GobboVector* v, void* element, int* error);

//removes the element at `index` from the GobboVector, and shifts all elements
//after over to fill in the gap. `*error` will be set to -1 if the index is out of bounds.
//`exit(EXIT_FAILURE)` on error if `error == NULL`.
//In the case of the element type of the GobboVector being a pointer type, to prevent
//memory leaks, it may be necessary to call free on the element, which this function does
//not do. If this is needed, use `gobbo_vector_get_and_remove()`.
void gobbo_vector_remove(GobboVector* v, size_t index, int* error);

//copies the element at `index` into `dest`. DOES NOT ALLOCATE so make sure `dest` is 
//appropriately allocated. Then removes the element from the GobboVector following the 
//semantics of `gobbo_vector_remove()`. If the index is out of bounds, `*error` will be 
//set to -1. `exit(EXIT_FAILURE)` on error if `error == NULL`.
//This function is particularly useful if the element type is a pointer that needs to 
//be freed. To do this, call `gobbo_vector_get_and_remove()` then call `free(*dest)`. 
void gobbo_vector_get_and_remove(GobboVector* v, void* dest, size_t index, int* error);

//returns a pointer to an element, rather than the element itself. The lifetime of the
//returned pointer will be invalidated as soon as any operation that changes the count of the 
//GobboVector is performed. `*error` will be set to -1 on index out of bounds, and 0 on success.
//Additionally the pointer returned will be NULL on error.
//if `error == NULL`, will call `exit(EXIT_FAILURE)` on error.
void* gobbo_vector_ptr_to_element(GobboVector* v, size_t index, int* error);

//Frees the `v->data` pointer, and zero's out all the fields of `v`. 
void gobbo_vector_free(GobboVector* v);

//a very temporary function just to test if vector is kinda working
void gobbo_vector_validate();

#endif