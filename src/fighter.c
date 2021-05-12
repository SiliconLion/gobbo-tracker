#include "../include/fighter.h"

#include <stdio.h>
#include <string.h>


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
    GobboVector stats; 
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

GobboStat gobbo_stat_new(const char* name, float value, bool visible) {
    size_t len = strlen(name);
    char* name_cpy = malloc(sizeof(char) * (len + 1));
    if(name_cpy == NULL) {printf("could not allocate name_cpy in gobbo_stat_new\n"); exit(EXIT_FAILURE);}
    strcpy(name_cpy, name);

    return (GobboStat){name_cpy, value, visible};
}

GobboStat gobbo_stat_copy(GobboStat* s) {
    return gobbo_stat_new(s->name, s->value, s->visible);
}

const char* gobbo_fighter_get_name(GobboFighter* f) {
    return f->name;
}

void gobbo_fighter_set_name(GobboFighter* f, const char* new_name) {
    free(f->name);
    size_t len = strlen(new_name);
    char* new_name_cpy = malloc(sizeof(char) * (len + 1));
    if(new_name_cpy == NULL) {printf("could not allocate name_cpy in gobbo_fighter_set_name\n"); exit(EXIT_FAILURE);}
    strcpy(new_name_cpy, new_name);
    f->name = new_name_cpy;
    return;
}

GobboVector* gobbo_fighter_get_stats(GobboFighter* f) {
    return &(f->stats);
}

GobboFighter* gobbo_fighter_new(const char* name) {
    GobboFighter* fighter = g_object_new(GOBBO_TYPE_FIGHTER, NULL);

    //copy name into fighter->name
    size_t len = strlen(name);
    fighter->name = malloc(sizeof(char) * (len + 1));
    if(fighter->name == NULL) {printf("could not allocate name in fighter_new\n"); exit(EXIT_FAILURE);}
    strcpy(fighter->name, name);

    //I may want to change this so that instead of storing a GobboStat, it stores a GobboStat*.
    //because in other places I need a GobboStat*, and currently I have to take a pointer to 
    //an element of fighter->stats. And as that changes, I could see a potential for a callback
    //to act on the wrong element. This shouldn't be a problem if things are synchronous (and I'm 
    //not sure they are), and if reformat is always called when it should be (which it probably will
    //be because otherwise the app doesn't work). 
    fighter->stats = gobbo_vector_new(sizeof(GobboStat), 0, NULL);

    return fighter;
}

void gobbo_fighter_add_stat(GobboFighter* f, GobboStat* stat) {
    GobboStat stat_cpy = gobbo_stat_copy(stat);
    gobbo_vector_push(&(f->stats), &stat_cpy, NULL);
}

void m_gobbo_fighter_stat_value_changed(GtkSpinButton* control, GobboStat* s) {
    s->value = gtk_spin_button_get_value(control);
}

void gobbo_fighter_reformat(GobboFighter* f) {
    m_gobbo_fighter_widget_clear_children(f);

    //GobboFighter is a GtkBox, which implements orientable.
    gtk_orientable_set_orientation(GTK_ORIENTABLE(f), GTK_ORIENTATION_VERTICAL);
    gtk_container_add(GTK_CONTAINER(f), gtk_label_new(f->name));
    gtk_container_add(GTK_CONTAINER(f), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

    size_t stats_len = f->stats.count;
    for(int i = 0; i < stats_len; i++) {
        GobboStat* stat = gobbo_vector_ptr_to_element(&(f->stats), i, NULL);

        //if this particular stat is not currently visible, skip it
        if(!stat->visible) {
            continue;
        }

        GtkWidget* stat_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_container_add(GTK_CONTAINER(stat_box), gtk_label_new(stat->name));

        GtkWidget* control = gtk_spin_button_new(
            gtk_adjustment_new(stat->value, 0, INT_MAX, 1.0, 0, 0),
            0, 0
        );
       
        g_signal_connect(control, "value-changed", G_CALLBACK(m_gobbo_fighter_stat_value_changed), stat);

        gtk_container_add(GTK_CONTAINER(stat_box), control);
        gtk_container_add(GTK_CONTAINER(f), stat_box);
    }
}

//very incomplete currently. Will be updated as needed or when a more final design
//is settled on.
void gobbo_fighter_print(GobboFighter* fighter) {
    printf("GobboFighter {\n\t%s\n}\n", fighter->name);
}


// void gobbo_validate_fighter() {
//     GobboFighter* fighter = gobbo_fighter_new("Goblin");
//     gobbo_fighter_add_stat(fighter, "Intelligence");
//     gobbo_fighter_add_stat(fighter, "Wisdom");
//     gobbo_fighter_add_stat(fighter, "Charisma");
//     gobbo_fighter_add_stat(fighter, "Dexterity");
//     gobbo_fighter_add_stat(fighter, "Strength");
//     gobbo_fighter_reformat(fighter);

//     if (G_TYPE_CHECK_INSTANCE(fighter) ) {
//         printf("fighter is valid instance\n");
//     } else {
//         printf("fighter is not a valid instance");
//     }

//     PRINT_IS_TYPE(GTK, WIDGET, fighter);
//     PRINT_IS_TYPE(GTK, CONTAINER, fighter)
//     PRINT_IS_TYPE(GOBBO, FIGHTER, fighter);
//     PRINT_IS_TYPE(GTK, BOX, fighter);

//     if (g_type_is_a(gobbo_fighter_get_type(), gtk_box_get_type()) ) {
//         printf("fighter is a GtkBox\n");
//     } else {
//         printf("Fighter is not a GtkBox");
//     }

//     gobbo_fighter_print(fighter);
// }

#undef IS_TYPE