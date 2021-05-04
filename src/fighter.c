#include "../include/fighter.h"

#include <stdio.h>
#include <string.h>

#include "../include/utilities/vector.h"

// my macro definitions

//checks whether `object` is a given type of GObject
//`library` and `type` should both be uppercase. 
//`library` is the library library the type is defined in. Eg,
//"GOBJECT, GTK, GOBBO". 
//`type` is the type that we are querying if `object` is.
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
    // a vector that holds names of the fighters stats. eg, "wisdom,"
    //This will be upgraded later to hold more complex types.
    GobboVector stat_names; 
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

//functions internal to file

//empties the fighter widget of any children
void m_gobbo_fighter_widget_clear_children(GobboFighter* f) {
    gtk_container_foreach(GTK_CONTAINER(f), (GtkCallback)gtk_widget_destroy, NULL);
}

//functions declared in the header

GobboFighter* gobbo_fighter_new(const char* name) {
    GobboFighter* fighter = g_object_new(GOBBO_TYPE_FIGHTER, NULL);

    //copy name into fighter->name
    size_t len = strlen(name);
    fighter->name = malloc(sizeof(char) * (len + 1));
    if(fighter == NULL) {printf("could not allocate name in fighter_new\n"); exit(EXIT_FAILURE);}
    strcpy(fighter->name, name);

    fighter->stat_names = gobbo_vector_new(sizeof(char*), 0, NULL);

    return fighter;
}

void gobbo_fighter_add_stat(GobboFighter* f, const char* stat) {
    size_t len = strlen(stat);
    char* stat_cpy = malloc(sizeof(char) * (len + 1));
    if(!stat_cpy){
        printf("could not allocate stat in gobo_fighter_add_stat");
        exit(EXIT_FAILURE);
    }

    strcpy(stat_cpy, stat);

    gobbo_vector_push(&(f->stat_names), &stat_cpy, NULL);
}

void gobbo_fighter_reformat(GobboFighter* f) {
    m_gobbo_fighter_widget_clear_children(f);

    //GobboFighter is a GtkBox, which implements orientable.
    gtk_orientable_set_orientation(GTK_ORIENTABLE(f), GTK_ORIENTATION_VERTICAL);
    gtk_container_add(GTK_CONTAINER(f), gtk_label_new(f->name));
    gtk_container_add(GTK_CONTAINER(f), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

    size_t stats_len = f->stat_names.count;
    for(int i = 0; i < stats_len; i++) {

        GtkWidget* stat_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        char* stat;
        gobbo_vector_get(&(f->stat_names), &stat, i, NULL);

        gtk_container_add(GTK_CONTAINER(stat_box), gtk_label_new(stat));
        gtk_container_add(
            GTK_CONTAINER(stat_box), 
            gtk_spin_button_new(
                gtk_adjustment_new(0, 0, INT_MAX, 1.0, 0, 0),
                0, 0
            )
        );
        gtk_container_add(GTK_CONTAINER(f), stat_box);
    }
}

//very incomplete currently. Will be updated as needed or when a more final design
//is settled on.
void gobbo_fighter_print(GobboFighter* fighter) {
    printf("GobboFighter {\n\t%s\n}\n", fighter->name);
}


void gobbo_validate_fighter() {
    GobboFighter* fighter = gobbo_fighter_new("Goblin");
    gobbo_fighter_add_stat(fighter, "Intelligence");
    gobbo_fighter_add_stat(fighter, "Wisdom");
    gobbo_fighter_add_stat(fighter, "Charisma");
    gobbo_fighter_add_stat(fighter, "Dexterity");
    gobbo_fighter_add_stat(fighter, "Strength");
    gobbo_fighter_reformat(fighter);

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