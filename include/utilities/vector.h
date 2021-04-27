#ifndef VECTOR_H
#define VECTOR_H
#include <stdint.h>
#include <stdlib.h>

#include <stdbool.h>



//simple utilites. Much of what goes here will have a GLib equivelent, but I 
//don't feel like using them

//need to add an error type, but debating on the best way to do that. for now will
//punt and just annotate functions with what the values returned as errors represent. 
//Error values will always be negative though

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
//v->count will not be updated unless element succesfully copied into v.
void gobbo_vector_push(GobboVector* v, void* element, int* error);

void gobbo_vector_free(GobboVector* v);

//a very temporary function just to test if vector is kinda working
void gobbo_vector_validate();

#endif