
#pragma once
#include <glib-object.h>


//the docs say to use `G_BEGIN_DECLS` here, but its just a macro for `extern_c {` and 
//I don't need that. 

//Defines GobboFighter type (as well as several other things). This is an alias of the 
//_GobboFighter struct defined in fighter.c

#define GOBBO_TYPE_FIGHTER gobbo_fighter_get_type()
G_DECLARE_FINAL_TYPE(GobboFighter, gobbo_fighter, GOBBO, FIGHTER, GObject)


//takes a name for the fighter, which will be copied.
GobboFighter* gobbo_fighter_new( const char* name);
//prints a GobboFighter formated to look nice
void gobbo_fighter_print(GobboFighter* fighter);

void test_fn(); 
