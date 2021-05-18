// #include <gtk/gtk.h>
#include "../include/fighter.h"
#include "../include/utilities/vector.h"
#include "../include/utilities/lowlevel.h"
#include "../include/menu.h"



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