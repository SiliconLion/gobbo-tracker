#include "../../include/utilities/lowlevel.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>



//pulled right from https://clang.llvm.org/docs/LanguageExtensions.html#langext-feature-check
//looks like gcc10 may also have this feature

#ifndef __has_builtin         // Optional of course.
  #define __has_builtin(x) 0  // Compatibility with non-clang compilers.
#endif

void gobbo_ll_validate() {
    if (sizeof(uintmax_t) == sizeof(unsigned long long int)) {
        printf("uintmax_t and unsigned long long int are the same size on this platform\n");
    } else {
        printf("uintmax_t and unsigned long long int do not have the same size on this platform\n");
    }

    printf("size of size_t is: %u bytes long\n", sizeof(size_t));
    printf("size of uintmax_t is: %u bytes long\n", sizeof(uintmax_t));
}



unsigned long long int gobbo_ll_umulll_checked(
        unsigned long long int a,
        unsigned long long int b,
        bool* overflowed
    ) {
    #if __clang__
        #if __has_builtin(__builtin_mul_overflow)
            unsigned long long int result;
            bool did_overflow = __builtin_umulll_overflow(a, b, &result);
            *overflowed = did_overflow;
            return result;
        #else 
            printf("this clang verstion does not support __builtin_mul_overflow needed for lowlevel.h");
            exit(EXIT_FAILURE);
        #endif
    #elif __GNUC__ > 5
        unsigned long long int result;
        bool did_overflow = __builtin_umulll_overflow(a, b, &result);
        *overflowed = did_overflow;
        return result;
    #else
        //not dealing with this right now
        printf("haven't implimented the code in lowlevel.h for compilers other than gcc and clang\n");
        exit(EXIT_FAILURE);
    #endif
}


//probably wrong. will unit test later

//since on my machine and compiler, size_t and unsigned long long int have the same size, 
//it might be interesting to try and find a way to use the preprocessor to skip the extra code.
//but the compiler might be smart enough to compile it out anyways, and I doubt this will be a bottle-neck.
//if is however, and it's not compiled out, an unnecessary branch is annoying
size_t gobbo_ll_umul_size_checked(size_t a, size_t b, bool* overflowed) {
    
    bool longlongoverflow;
    unsigned long long int result = gobbo_ll_umulll_checked(
        (unsigned long long int)a,
        (unsigned long long int)b,
        &longlongoverflow
    );

    size_t simple_mult_result = a*b;
    //if the result computed using the "longest" type and the result from the size_t are different, 
    //then it overflowed. otherwise if they are the same size, then it still overflow and checks that.
    if (result != simple_mult_result || longlongoverflow == true) {
        *overflowed = true;
        //if longlongoverflow is true, then size_t and unsigned long long have the same sizes, in
        //which case result and simple_mult_result will be the same.
        //if size_t and unsigned long long are different sizes, then longlongoverflow will be false, and
        //we want to return the value that comes from the size_t multiplication
        return simple_mult_result;
    } else {
        *overflowed = false;
        return simple_mult_result;
    }
    //unreachable
}


//probably wrong. will unit test later
//checked addition for unsigned long long int's 
unsigned long long int gobbo_ll_uaddll_checked(
        unsigned long long int a,
        unsigned long long int b,
        bool* overflowed
    ) {
    #if __clang__
        #if __has_builtin(__builtin_add_overflow)
            unsigned long long int result;
            *overflowed = _builtin_uaddll_overflow(a, b, &result);
            return result;
        #else 
            printf("this clang verstion does not support __builtin_add_overflow needed for lowlevel.h");
            exit(EXIT_FAILURE);
        #endif
    #elif __GNUC__ > 5
        unsigned long long int result;
        *overflowed = __builtin_uaddll_overflow(a, b, &result);
        return result;
    #else 
        /not dealing with this right now
        printf("haven't implimented the code in lowlevel.h for compilers other than gcc and clang\n");
        exit(EXIT_FAILURE);
    #endif
}

size_t gobbo_ll_add_size_checked(size_t a, size_t b, bool* overflowed) {
    //do operation using "longest" type
    bool longlongoverflowed;
    unsigned long long int result = gobbo_ll_uaddll_checked(
        (unsigned long long int) a,
        (unsigned long long int) b,
        &longlongoverflowed
    );

    size_t simple_add_res = a+b;
    
    //same as the coresponding part in gobbo_ll_mul_size_checked. look there for explication
    if( longlongoverflowed || result != simple_add_res) {
        *overflowed = true;
        return simple_add_res;
    } else {
        *overflowed = false;
        return simple_add_res;
    }
    //unreachable
}