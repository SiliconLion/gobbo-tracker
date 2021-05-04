
#pragma once
#include <glib-object.h>
#include<gtk/gtk.h>


//the docs say to use `G_BEGIN_DECLS` here, but its just a macro for `extern_c {` and 
//I don't need that. 

//Defines GobboFighter type (as well as several other things). This is an alias of the 
//_GobboFighter struct defined in fighter.c

#define GOBBO_TYPE_FIGHTER gobbo_fighter_get_type()
G_DECLARE_FINAL_TYPE(GobboFighter, gobbo_fighter, GOBBO, FIGHTER, GtkBox)


//Creates a GobboFighter with name `name`. `name` will be copied. Need to 
//call `gobbo_fighter_reformat` before the fighter will be meaningfully visible
GobboFighter* gobbo_fighter_new(const char* name);

//Adds a stat name to GobboFighter. The string pointed to by stat will be copied for 
//the purposes of clean ownership. Must call `gobbo_vector_reformat` to see change 
void gobbo_fighter_add_stat(GobboFighter* f, const  char* stat);

//takes a gobbo_fighter and redraws/reformats it based on the fighter state. 
//Can kinda think of it as a "update/present" method.
void gobbo_fighter_reformat(GobboFighter* f);

//prints a GobboFighter formated to look nice
void gobbo_fighter_print(GobboFighter* fighter);

//debug function. 
void gobbo_validate_fighter(); 
