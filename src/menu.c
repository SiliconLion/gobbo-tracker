#include "../include/menu.h"

#include <stdbool.h>

void gobbo_add_fighter_callback(GtkWidget* fighter_container) {
    GobboFighter* fighter = gobbo_fighter_new("Goblin");
    gobbo_fighter_add_stat(fighter, "Health");
    gobbo_fighter_add_stat(fighter, "Armor Class");
    gobbo_fighter_add_stat(fighter, "Strength");
    gobbo_fighter_reformat(fighter);

    gtk_container_add(GTK_CONTAINER(fighter_container), GTK_WIDGET(fighter));
    gtk_widget_show_all(fighter_container);
}

void gobbo_add_stat_callback(GtkWidget* fighter_container) {

}

GtkWidget* gobbo_create_stats_menu(GtkContainer* fighter_container) {
    GtkWidget* wrapper_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget* stats_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(stats_box), gtk_check_button_new_with_label(
        "Health"
    ));
    gtk_container_add(GTK_CONTAINER(stats_box), gtk_check_button_new_with_label(
        "Armor Class"
    ));
    gtk_container_add(GTK_CONTAINER(stats_box), gtk_check_button_new_with_label(
        "Strength"
    ));

    //just a way to pass a pointer with a value of true
    bool location_of_true = true;
    //toggles all the stat checkboxes in the stats_box to 'checked'
    gtk_container_foreach(
        GTK_CONTAINER(stats_box), 
        (GtkCallback)gtk_toggle_button_set_active,
        &location_of_true
    );


    GtkWidget* entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget* stat_entry = gtk_entry_new();
    GtkWidget* stat_entry_bttn = gtk_button_new_with_label("Add");

    gtk_container_add(GTK_CONTAINER(entry_box), stat_entry);
    gtk_container_add(GTK_CONTAINER(entry_box), stat_entry_bttn);
    
    gtk_container_add(GTK_CONTAINER(wrapper_container), entry_box);
    gtk_container_add(GTK_CONTAINER(wrapper_container), stats_box);
    
    return wrapper_container;
}

GtkWidget* gobbo_create_menu(GtkContainer* fighter_container) {
    GtkWidget* menu_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget* add_fighter_bttn = gtk_button_new_with_label("Add Fighter");
    g_signal_connect_swapped(add_fighter_bttn, "clicked", G_CALLBACK(gobbo_add_fighter_callback), fighter_container);

    GtkWidget* stats_menu = gobbo_create_stats_menu(fighter_container);

    gtk_container_add(GTK_CONTAINER(menu_container), add_fighter_bttn);
    gtk_container_add(GTK_CONTAINER(menu_container), stats_menu);

    return menu_container;
}