#include "../../include/utilities/vector.h"
#include "../../include/utilities/lowlevel.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

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
        bool overflowed;
        new_capacity = gobbo_ll_umul_size_checked(v->capacity, 2, &overflowed);
        if(overflowed) {
            if(!alloc_error) { exit(EXIT_FAILURE);}
            *alloc_error = -1;
            return;
        }
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

    size_t new_byte_count = gobbo_ll_umul_size_checked(v->stride, new_capacity, &overflowed);
    if(overflowed) {
        if (!alloc_error) {exit(EXIT_FAILURE);}
        *alloc_error = -1;
        return;
    }

    void* new_memory = realloc(v->data, new_byte_count);
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

    //1) a)data is a valid pointer, and has a capacity of v->capacity * v->stride allocated. 
    //   b)This means that v->capacity * v-> stride is a valid size_t.
    //2)The conditional calls to gobbo_vector_is_full and gobbo_vector_resize(v, 0, ...) ensure that
    //count <= v->capacity.
    //3) 1a) => data + ((v->capacity - 1) * v->stride) is a valid uintptr_t. This implies that
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

void gobbo_vector_remove(GobboVector* v, size_t index, int* error) {
    if(index >= v->count) {
        if(!error) {exit(EXIT_FAILURE);}
        *error = -1;
        return;
    }

    //where all the elements are being shifted to, ie, where the element being removed was
    void* dest = v->data + (index * v->stride);
    //where all shifted elements start from, ie, the element at index + 1
    void* src = v->data + ( (index + 1) * v->stride);

    //how many elements need to be moved.
    size_t element_count = v->count - (index + 1);

    memmove(dest, src, element_count * v->stride);

    v->count -= 1;
}

void gobbo_vector_get_and_remove(GobboVector* v, void* dest, size_t index, int* error) {
    gobbo_vector_get(v, dest, index, error);
    if (error) {
        if(*error < 0) {
            return;
        }
    }
    gobbo_vector_remove(v, index, error);
    //dont need to check or set the error value, for if it errored and error != NULL, 
    //it will have been set by gobbo_vector_remove, and then we immediately return;
    return;
}

void* gobbo_vector_ptr_to_element(GobboVector* v, size_t index, int* error) {
    if(index >= v->count) {
        if(!error) {exit(EXIT_FAILURE);}
        *error = -1;
        return NULL;
    }

    //as long as the GobboVector and the index are valid, this arithmetic will not overflow
    return (void*) v->data + (v->stride * index) ;
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

    v = gobbo_vector_new(sizeof(unsigned char*), 0, &error);

    for (size_t i = 0; i < 30000; i++) {
        int len = 400;
        unsigned char* str = malloc(sizeof( unsigned char) * (len + 1));
        if(!str) {
            printf("allocation failure in creating str\n");
        }

        for(size_t j = 0;  j< len; j++) {
            //just using j to generate well behaved ascii. Not a
            //particularly special sequence, just avoiding things like '\0\ or '\DEL'
            str[j] = (unsigned char)( ( (j % ('~' - 'A' + 1)) + 'A') );
        }
        //null terminates the byte array
        str[len] = '\0';
        
        gobbo_vector_push(&v, &str, &error);
        if(error < 0) {printf("error when trying to push str at i:%zu\n", i);}
    }

    for (size_t i = 0; i < 30000; i++) {
        unsigned char* str;
        gobbo_vector_get_and_remove(&v, &str, 0, &error);
        if(error <0){printf("could not get and remove str at i:%zu\n", i);}
        //printing them all just gumms up the console.
        if (i == 0 || i == 890 || i == 29999) {printf("%s", str);}
        free(str);
    }
    printf("\n");

    printf("everything seems to be okay in vector...\n");
}
