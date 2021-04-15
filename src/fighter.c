#include "../include/fighter.h"

#include <stdio.h>
#include <string.h>

//Fighter typ and class boilerplate and definitions

struct _GobboFighter {
    GObject parent_instance;
    char* name;
};

//I don't think I need to declare a class since GobboFighter type is declared
//using G_DECLARE_FINAL_TYPE which doesn't seem to be able to have virtual
//methods of its own?

G_DEFINE_TYPE(GobboFighter, gobbo_fighter, G_TYPE_OBJECT)

static void gobbo_fighter_init(GobboFighter* self) {
    //do nothing
}

static void gobbo_fighter_class_init(GobboFighterClass* klass) {
    //do nothing
}

//static function implimentations below

GobboFighter* gobbo_fighter_new( const char* name) {
    GobboFighter* fighter = g_object_new(GOBBO_TYPE_FIGHTER, NULL);

    //copy name into fighter->name
    size_t len = strlen(name);
    fighter->name = malloc(sizeof(char) * (len + 1));
    strcpy(fighter->name, name);

    return fighter;
}

void gobbo_fighter_print(GobboFighter* fighter) {
    printf("GobboFighter {\n\t%s\n}\n", fighter->name);
}

void test_fn() {
    GobboFighter* fighter = gobbo_fighter_new("Goblin");
    gobbo_fighter_print(fighter);
}