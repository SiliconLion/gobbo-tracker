#pragma once

#include <gtk/gtk.h>
// #include "fighter.h"

typedef struct {
    //currently no members
} GobboMenuState;

typedef struct {
    GtkContainer* fighter_container;
    GtkWidget* other_widget;
} GobboMenuData;

GtkWidget* gobbo_create_menu(GtkContainer* fighter_container);

