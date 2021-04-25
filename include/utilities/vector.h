#pragma once
#include <stdint.h>
#include <stdlib.h>

//this indicates that we want bounds checking where applicable. Putting it here to have
//and as a reminder, but left as a comment because it is not really appropreate to define this
//macro in this file as such as it could cause include order dependant behavior and I don't want that
//unless it is intentionall.
// #define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>

#include "lowlevel.h"


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
GobboVector gobbo_vector_new(size_t stride, size_t inital_capacity, int* alloc_error) {
    GobboVector v = {stride, 0, inital_capacity, NULL};
    if (inital_capacity = 0) {inital_capacity = 1;}

    bool overflowed;
    size_t num_bytes = gobbo_ll_umul_size_checked(stride, inital_capacity, &overflowed);
    if(overflowed) {
        *alloc_error = -1;
        v.data = NULL;
        return v;
    }
 
    v.data = malloc(num_bytes);
    if (v.data == NULL && alloc_error == NULL) {
        exit(EXIT_FAILURE);
    } else if (v.data = NULL) {
        v.data = NULL;
        *alloc_error = -1;
    }

    *alloc_error = 0;
    return v;
}

//returns if the vector has filled `v->data` to its capacity
bool gobbo_vector_is_full(GobboVector* v) {
    //obviously assumes a well formed GobboVector
    return v->count < v->capacity  ? false : true;
}

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
void gobbo_vector_resize(GobboVector* v, size_t new_capacity, int* alloc_error) {
    if (new_capacity == 0) {
        new_capacity = v->capacity * 2;
    }

    bool overflowed;
    size_t num_bytes = gobbo_ll_umul_size_checked(v->stride, new_capacity, &overflowed);
    if(overflowed) {
        *alloc_error = -1;
        return;
    }

//Dont need checked arithmetic because this is already a valid number of bytes for an
//array (which means it fits in a size_t)
    size_t current_bytes = v->stride * v->capacity;

    if (num_bytes < current_bytes) {
        *alloc_error = -2;
        return;
    }

    void* new_memory = realloc(v->data, v->stride * new_capacity);
    if (new_memory == NULL) {
        *alloc_error = -1;
        return;
    } else {
        v->data = new_memory;
        v->capacity = new_capacity;
        *alloc_error = 0;
        return;
    }
}

//element in the GobboVector v at index `index` is copied into `dest`. DOES NOT ALLOCATE so
//make sure `dest` is appropriately allocated. 
//`*bounds_error` is set to -1 if index is out of bounds. set to 0 otherwise.
void gobbo_vector_get(GobboVector* v, void* dest, size_t index, int* bounds_error) {
    //v->count is maintained as valid so I do not need to check it. 
    if(index >= v->count) {
        *bounds_error = -1;
        return;
    } else {
        //see "proof" in gobbo_vector_push for validity, with the stipulation that index < v->count
        uintptr_t elem_address = (uintptr_t)v->data + (uintptr_t)( index * v->stride);
        memcpy((void*)dest, (void*)elem_address, v->stride);
        //return
    }
    //unreachable
}

void gobbo_vector_set(GobboVector* v, void* element, size_t index, int* bounds_error) {
    //v->count is maintained as valid so I do not need to check it. 
    if(index >= v->count) {
        *bounds_error = -1;
        return;
    } else {
        //see "proof" in gobbo_vector_push for validity, with the stipulation that index < v->count
        uintptr_t dest_address = (uintptr_t)v->data + (uintptr_t)( index * v->stride);
        memcpy((void*)dest_address, element, v->stride);
    }
}

//copies the value pointed to by `element` to the end of the GobboVector. 
//assumes the size of the pointed element is `v.stride`
//if the vector is full, `gobbo_vector_resize` is called with a `new_capacity` of 0 (doubling the size).
//if that fails, `*alloc_error` will be set to -1.
//on success, `*alloc_error ` will be set to 0.
//If `alloc_error` is NULL, then will call exit if it can't reallocate
void gobbo_vector_push(GobboVector* v, void* element, int* alloc_error) {
    if (gobbo_vector_is_full(v)) {
        gobbo_vector_resize(v, 0, alloc_error);
        if (alloc_error < 0) {
            return;
        }
    }

    v->count += 1;

    //starting at v->data, then moving by v->stride bytes, v->count -1 times
    
    //below is an a perhaps overkill "proof" as to why this can't overflow but here it is

    //1)data is a valid pointer, and has a capacity of v->capacity * v->stride allocated. 
    //2)The conditional calls to gobbo_vector_is_full and gobbo_vector_resize(v, 0, ...) ensure that
    //v->count <= v->capacity.
    //3) 1) => data + ((v->capacity - 1) * v->stride) is a valid uintptr_t. This implies that
    //v->data + ( (v->count -1) * v->stride) is a valid uintptr_t. Thus it will not overflow
    //and is a valid pointer. 
    uintptr_t dest = (uintptr_t)v->data + (uintptr_t)( (v->count - 1) * v->stride) ;
    //considering using memcpy_s. However I have seen some debate about the wiseness about things like
    //rsize_t, and have not taken the time to look into constraint handler functions. So for now
    //going with good old memcpy
    memcpy((void*)dest, element, v->stride);
}

void gobbo_vector_free(GobboVector* v) {
    free(v->data);

    //unnecessary if memory is managed  correctly but this seems like it will make errors easier to
    //detect if it is not.
    v->capacity = 0;
    v->stride = 0;
    v->count = 0;
}