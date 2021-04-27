#include "../../include/utilities/vector.h"
#include "../../include/utilities/lowlevel.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

//this indicates that we want bounds checking where applicable. Putting it here to have
//and as a reminder, but left as a comment because it is not really appropriate to define this
//macro in this file as such as it could cause include order dependant behavior and I don't want that
//unless it is intentionall.
// #define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#include <stdbool.h>



GobboVector gobbo_vector_new(size_t stride, size_t inital_capacity, int* alloc_error) {
    if (inital_capacity == 0) {inital_capacity = 1;}
    GobboVector v = {stride, 0, inital_capacity, NULL};


    bool overflowed;
    size_t num_bytes = gobbo_ll_umul_size_checked(stride, inital_capacity, &overflowed);
    if(overflowed) {
        if (!alloc_error) {exit(EXIT_FAILURE);}
        *alloc_error = -1;
        v.data = NULL;
        return v;
    }
 
    v.data = malloc(num_bytes);
    if (v.data == NULL) {
        if (!alloc_error) {exit(EXIT_FAILURE);}
        *alloc_error = -1;
        return v;
    }

    if (alloc_error) {
        *alloc_error = 0;
    }
    return v;
}


bool gobbo_vector_is_full(GobboVector* v) {
    //obviously assumes a well formed GobboVector
    return v->count < v->capacity  ? false : true;
}


void gobbo_vector_resize(GobboVector* v, size_t new_capacity, int* alloc_error) {
    if (new_capacity == 0) {
        new_capacity = v->capacity * 2;
    }

    bool overflowed;
    size_t num_bytes = gobbo_ll_umul_size_checked(v->stride, new_capacity, &overflowed);
    if(overflowed) {
        if (!alloc_error) {exit(EXIT_FAILURE);}
        *alloc_error = -1;
        return;
    }

    size_t current_bytes = v->stride * v->capacity;

    if (num_bytes < current_bytes) {
        if (!alloc_error) {exit(EXIT_FAILURE);}
        *alloc_error = -2;
        return;
    }

    void* new_memory = realloc(v->data, v->stride * new_capacity);
    if (new_memory == NULL) {
        if (!alloc_error) {exit(EXIT_FAILURE);}
        *alloc_error = -1;
        return;
    } else {
        v->data = new_memory;
        v->capacity = new_capacity;
        if(alloc_error) {*alloc_error = 0;}
        return;
    }
}


void gobbo_vector_get(GobboVector* v, void* dest, size_t index, int* bounds_error) {
    //v->count is maintained as valid so I do not need to check it. 
    if(index >= v->count) {
        if (!bounds_error) {exit(EXIT_FAILURE);}
        *bounds_error = -1;
        return;
    } else {
        //see "proof" in gobbo_vector_push for validity, with the stipulation that index < v->count
        void* elem_address = v->data + ( index * v->stride);
        memcpy(dest, elem_address, v->stride);
        if(bounds_error) { *bounds_error = 0;}
        return;
    }
    //unreachable
}

void gobbo_vector_set(GobboVector* v, void* element, size_t index, int* bounds_error) {
    //v->count is maintained as valid so I do not need to check it. 
    if(index >= v->count) {
        if (!bounds_error) {exit(EXIT_FAILURE);}
        *bounds_error = -1;
        return;
    } else {
        //see "proof" in gobbo_vector_push for validity, with the stipulation that index < v->count
        void* dest_address = v->data + ( index * v->stride);
        memcpy(dest_address, element, v->stride);
        if(bounds_error) {*bounds_error = 0;}
        return;
    }
}

void gobbo_vector_push(GobboVector* v, void* element, int* error) {
    if (gobbo_vector_is_full(v)) {
        int alloc_error;
        gobbo_vector_resize(v, 0, &alloc_error);
        if (alloc_error < 0) {
            if (!error) {exit(EXIT_FAILURE);}
            *error = alloc_error;
            return;
        }
    }


    size_t count = v->count + 1;

    //starting at v->data, then moving by v->stride bytes, v->count -1 times
    
    //below is an a perhaps overkill "proof" as to why this can't overflow but here it is

    //1)data is a valid pointer, and has a capacity of v->capacity * v->stride allocated. 
    //2)The conditional calls to gobbo_vector_is_full and gobbo_vector_resize(v, 0, ...) ensure that
    //count <= v->capacity.
    //3) 1) => data + ((v->capacity - 1) * v->stride) is a valid uintptr_t. This implies that
    //v->data + ( (count -1) * v->stride) is a valid uintptr_t. Thus it will not overflow
    //and is a valid pointer. 
    void* dest = v->data + ( (count - 1) * v->stride) ;
    //considering using memcpy_s. However I have seen some debate about the wiseness about things like
    //rsize_t, and have not taken the time to look into constraint handler functions. So for now
    //going with good old memcpy. However as I would like to detect if the copy doesn't work, I will
    //likely switch to memcpy_s in the future.
    memcpy(dest, element, v->stride);

    v->count += 1;
    if(error) {*error = 0;}
    return;
}

void gobbo_vector_free(GobboVector* v) {
    free(v->data);

    //unnecessary if memory is managed  correctly but this seems like it will make errors easier to
    //detect if it is not.
    v->capacity = 0;
    v->stride = 0;
    v->count = 0;
}


void gobbo_vector_validate() {
    int error;
    size_t capacity = 0;
    GobboVector v = gobbo_vector_new(sizeof(size_t), capacity, &error);
    if(error < 0) {
        printf("error code for vector_new is %i\n", error);
        return;
    }

    for(size_t i = 0; i < 300000000; i++) {
        size_t k = i;
        gobbo_vector_push(&v, &k, &error);
        if(error < 0) {
            printf("error code for vector_push is %i when i is %zu\n", error, i);
            return;
        }
    }

    for(size_t i = 0; i < 300000000; i++) {
        size_t k = SIZE_MAX - 5;
        gobbo_vector_set(&v, &k, i, &error);
        if(error < 0) {
            printf("error code for vector_set is %i when i is %zu\n", error, i);
            return;
        }
    }

    for(size_t i = 0; i < 300000000; i++) {
        size_t element;
        gobbo_vector_get(&v, &element, i, &error);
        if(error < 0) {
            printf("error code for vector_set is %i when i is %zu\n", error, i);
            return;
        }

        if(element != SIZE_MAX - 5) {
            printf("something went wrong. element #%zu is %zu, which"
                   "is not SIZE_MAX - 5\n",
                   i, element);
            return;
        }
    }
    gobbo_vector_free(&v);

    printf("everything seems to be okay in vector...\n");
}
