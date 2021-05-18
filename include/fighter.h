
#pragma once
#include <glib-object.h>
#include<gtk/gtk.h>
#include <stdbool.h>
#include "utilities/vector.h"


//the docs say to use `G_BEGIN_DECLS` here, but its just a macro for `extern_c {` and 
//I don't need that. 

//Defines GobboFighter type (as well as several other things). This is an alias of the 
//_GobboFighter struct defined in fighter.c

#define GOBBO_TYPE_FIGHTER gobbo_fighter_get_type()
G_DECLARE_FINAL_TYPE(GobboFighter, gobbo_fighter, GOBBO, FIGHTER, GtkBox)


typedef struct {
    const char* name;
    float value;
    bool visible;
} GobboStat;

//the name string will be copied
GobboStat gobbo_stat_new(const char* name, float value, bool visible);

//deeply copies a GobboStat
GobboStat gobbo_stat_copy(GobboStat* s);

//Creates a GobboFighter with name `name`. `name` will be copied. Need to 
//call `gobbo_fighter_reformat` before the fighter will be meaningfully visible
GobboFighter* gobbo_fighter_new(const char* name);

const char* gobbo_fighter_get_name(GobboFighter* f);

//sets the name of the fighter. will make a copy of the name, 
//and will exit on allocation failure
void gobbo_fighter_set_name(GobboFighter* f, const char* new_name);

//returns a reference to the GobboVector holding the GobboStats of the GobboFighter.
//I haven't quite figured out what the appropreate aliasing contract for this is yet,
//so be conscientious and thoughtful on holding it too long, or mutating it. Don't
//directly free it for sure lol. 
GobboVector* gobbo_fighter_get_stats(GobboFighter* f);

//Adds a GobboStat to the GobboFighter. The GobboStat will be copied for 
//the purposes of clean ownership. Must call `gobbo_vector_reformat` to see change.
void gobbo_fighter_add_stat(GobboFighter* f, GobboStat* stat);

//takes a gobbo_fighter and redraws/reformats it based on the fighter state. 
//Can kinda think of it as a "update/present" method.
void gobbo_fighter_reformat(GobboFighter* f);

//prints a GobboFighter formated to look nice
void gobbo_fighter_print(GobboFighter* fighter);

// //debug function. 
// void gobbo_validate_fighter(); 
