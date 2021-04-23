#include "../include/fighter.h"

#include <stdio.h>
#include <string.h>

// my macro definitions

//checks whether `object` is a given type of GObject
//`library` and `type` should both be uppercase. 
//`library` is the library library the type is defined in. Eg,
//"GOBJECT, GTK, GOBBO". 
//`type` is the type that we are querrying if `object` is.
#define PRINT_IS_TYPE(library, type, object) \
    if( library ## _IS_ ## type  (object) ) {\
        printf(#object " is " #library "_" #type "\n"); \
    } else {\
        printf(#object " is not " #library "_" #type "\n");\
    }




//Fighter type and class boilerplate and definitions

struct _GobboFighter {
    GtkBox parent_instance;
    char* name;
};

//I don't think I need to declare a class since GobboFighter type is declared
//using G_DECLARE_FINAL_TYPE which doesn't seem to be able to have virtual
//methods of its own?

G_DEFINE_TYPE(GobboFighter, gobbo_fighter, GTK_TYPE_BOX)

static void gobbo_fighter_init(GobboFighter* self) {
    //nothing extra
}

static void gobbo_fighter_class_init(GobboFighterClass* klass) {
    //northing extra
}

//static function implimentations below

GobboFighter* gobbo_fighter_new( const char* name) {
    GobboFighter* fighter = g_object_new(GOBBO_TYPE_FIGHTER, NULL);

    //copy name into fighter->name
    size_t len = strlen(name);
    fighter->name = malloc(sizeof(char) * (len + 1));
    strcpy(fighter->name, name);
    
    //GobboFighter is a GtkBox, which impliments orientable.
    gtk_orientable_set_orientation(GTK_ORIENTABLE(fighter), GTK_ORIENTATION_VERTICAL);
    gtk_container_add(GTK_CONTAINER(fighter), gtk_label_new(fighter->name));

    return fighter;
}

void gobbo_fighter_print(GobboFighter* fighter) {
    printf("GobboFighter {\n\t%s\n}\n", fighter->name);
}


void gobbo_validate_fighter() {
    GobboFighter* fighter = gobbo_fighter_new("Goblin");

    if (G_TYPE_CHECK_INSTANCE(fighter) ) {
        printf("fighter is valid instance\n");
    } else {
        printf("fighter is not a valid instance");
    }

    PRINT_IS_TYPE(GTK, WIDGET, fighter);
    PRINT_IS_TYPE(GTK, CONTAINER, fighter)
    PRINT_IS_TYPE(GOBBO, FIGHTER, fighter);
    PRINT_IS_TYPE(GTK, BOX, fighter);

    if (g_type_is_a(gobbo_fighter_get_type(), gtk_box_get_type()) ) {
        printf("fighter is a GtkBox\n");
    } else {
        printf("Fighter is not a GtkBox");
    }

    gobbo_fighter_print(fighter);
}

#undef IS_TYPE