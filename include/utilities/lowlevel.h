
//This specifies the lower level operations that get reused. 
//Particuarly ones that rely on compiler intrinsics

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>



void gobbo_ll_validate();


//probably wrong. will unit test later
unsigned long long int gobbo_ll_umulll_checked(
        unsigned long long int a,
        unsigned long long int b,
        bool* overflowed
    );


//probably wrong. will unit test later
size_t gobbo_ll_umul_size_checked(size_t a, size_t b, bool* overflowed);


//probably wrong. will unit test later
//checked addition for unsigned long long int's 
unsigned long long int gobbo_ll_uaddll_checked(
        unsigned long long int a,
        unsigned long long int b,
        bool* overflowed
    );
size_t gobbo_ll_add_size_checked(size_t a, size_t b, bool* overflowed);