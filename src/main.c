// #include <gtk/gtk.h>
#include "../include/fighter.h"
#include "../include/utilities/vector.h"
#include "../include/utilities/lowlevel.h"


void gobbo_add_fighter_callback(GtkWidget* fighter_container) {
    GobboFighter* fighter = gobbo_fighter_new("Goblin");
    gobbo_fighter_add_stat(fighter, "Intelligence");
    gobbo_fighter_add_stat(fighter, "Wisdom");
    gobbo_fighter_add_stat(fighter, "Charisma");
    gobbo_fighter_add_stat(fighter, "Dexterity");
    gobbo_fighter_add_stat(fighter, "Strength");
    gobbo_fighter_reformat(fighter);

    gtk_container_add(GTK_CONTAINER(fighter_container), GTK_WIDGET(fighter));
    gtk_widget_show_all(fighter_container);
}

GtkWidget* gobbo_create_menu(GtkContainer* fighter_container) {
    GtkWidget* menu_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget* add_fighter_bttn = gtk_button_new_with_label("Add Fighter");
    g_signal_connect_swapped(add_fighter_bttn, "clicked", G_CALLBACK(gobbo_add_fighter_callback), fighter_container);

    gtk_container_add(GTK_CONTAINER(menu_container), add_fighter_bttn);

    return menu_container;
}

static void
activate (GtkApplication* app,
          gpointer        user_data)
{
    GtkWidget *window;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

    GtkWidget* fighter_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget* menu_container = gobbo_create_menu(GTK_CONTAINER(fighter_container));

    GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(main_box), menu_container);
    gtk_container_add(GTK_CONTAINER(main_box), fighter_container);

    gtk_container_add(GTK_CONTAINER(window), main_box);

    gtk_widget_show_all (window);


}

int
main (int    argc,
      char **argv)
{
    gobbo_ll_validate();
    // gobbo_vector_validate();

    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}