// #include <gtk/gtk.h>
#include "../include/fighter.h"
#include "../include/utilities/vector.h"



static void
activate (GtkApplication* app,
          gpointer        user_data)
{
    GtkWidget *window;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

    gobbo_validate_fighter();

    char** stats = malloc(sizeof(char*) *5);
    stats[0] = "Intelligence"; stats[1] = "Wisdom";
    stats[2] = "Charisma"; stats[3] = "Dexterity"; stats[4] = "Strength";
    GobboFighter* fighter = gobbo_fighter_new("Goblin", (const char *const *)stats, 5);
    
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(fighter));

    gtk_widget_show_all (window);


}

int
main (int    argc,
      char **argv)
{
    gobbo_ll_validate();
    gobbo_vector_validate();

    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}