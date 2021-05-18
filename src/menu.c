#include "../include/menu.h"

#include <stdbool.h>
#include "../include/fighter.h"

void gobbo_add_fighter_callback(GobboMenuData* data) {
    //just making it clearer 
    GtkContainer* fighter_container = data->fighter_container;
    GtkContainer* stats_box = GTK_CONTAINER(data->other_widget);

    GobboFighter* fighter = gobbo_fighter_new("Goblin");

    GList* stat_children = gtk_container_get_children(stats_box);
    for(GList* child = stat_children; child != NULL; child = child->next) {
        const char* stat_name = gtk_button_get_label(GTK_BUTTON(child->data));
        GobboStat stat = gobbo_stat_new(stat_name, 0, true);
        gobbo_fighter_add_stat(fighter, &stat);
    }

    gobbo_fighter_reformat(fighter);

    gtk_container_add(fighter_container, GTK_WIDGET(fighter));
    gtk_widget_show_all(GTK_WIDGET(fighter_container));
}

void gobbo_toggle_stat_callback(GtkCheckButton* check_button, GtkWidget* fighter_container) {
    //doing it this way rather than gtk_container_for each because I don't want to write another callback
    GList* children_list = gtk_container_get_children(GTK_CONTAINER(fighter_container));
    for(GList* node_ptr = children_list; node_ptr != NULL; node_ptr = node_ptr->next) {
        GobboFighter* fighter = GOBBO_FIGHTER(node_ptr->data);
        GobboVector* stats = gobbo_fighter_get_stats(fighter);

        for(size_t i = 0; i < stats->count; i++) {
            GobboStat* stat = gobbo_vector_ptr_to_element(stats, i, NULL);

            const char* button_label = gtk_button_get_label(GTK_BUTTON(check_button));
            int comparison = strcmp(stat->name, button_label);
            if(comparison == 0) { //strcmp returns 0 when the strings are equal
                stat->visible = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button));
                //theoretically there should only one stat with a given name per fighter, but
                //I don't enforce that varient and I'm not sure I should? Probably should 
                //but intentionally not right now.
            }
        }

        gobbo_fighter_reformat(fighter);
    }

    gtk_widget_show_all(fighter_container);
}

void gobbo_add_stat_callback(GtkEntry* entry, GobboMenuData* data) {
    const char * input_str = gtk_entry_get_text(entry);

    GtkWidget* new_stat_chkbx = gtk_check_button_new_with_label(input_str);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(new_stat_chkbx), true);
    g_signal_connect(new_stat_chkbx, "toggled", G_CALLBACK(gobbo_toggle_stat_callback), data->fighter_container);

    GtkContainer* stats_container = GTK_CONTAINER(data->other_widget);
    gtk_container_add(stats_container, new_stat_chkbx);

    GList* children_list = gtk_container_get_children(GTK_CONTAINER(data->fighter_container));
    for(GList* node_ptr = children_list; node_ptr != NULL; node_ptr = node_ptr->next) {
        GobboFighter* fighter = GOBBO_FIGHTER(node_ptr->data);
        GobboVector* stats = gobbo_fighter_get_stats(fighter);

        GobboStat new_stat = gobbo_stat_new(input_str, 0, true);
        gobbo_vector_push(stats, &new_stat, NULL);

        gobbo_fighter_reformat(GOBBO_FIGHTER(fighter));
    }

    //input_str no longer is used past here, so we don't have to worry about aliases. 

    gtk_entry_set_text(entry, ""); //clears the entry

    gtk_widget_show_all(GTK_WIDGET(stats_container));
    gtk_widget_show_all(GTK_WIDGET(data->fighter_container));
}

//if only this could be a nested function 
//just tells the entry that it's been activated somehow
void gobbo_stat_entry_bttn_callback(GtkEntry* entry) {
    g_signal_emit_by_name(entry, "activate");
}
//the value of out_data will be set to the address of a GobboMenuData that is created with the box that
//holds all the stats. 
GtkWidget* gobbo_create_stats_menu(GtkContainer* fighter_container, GobboMenuData** out_data) {
    GtkWidget* stats_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget* health_chkbx = gtk_check_button_new_with_label("Health");
    GtkWidget* strength_chkbx = gtk_check_button_new_with_label("Strength");
    GtkWidget* ac_chkbx = gtk_check_button_new_with_label("Armor Class");
    
    g_signal_connect(health_chkbx, "toggled", G_CALLBACK(gobbo_toggle_stat_callback), fighter_container);
    g_signal_connect(strength_chkbx, "toggled", G_CALLBACK(gobbo_toggle_stat_callback), fighter_container);
    g_signal_connect(ac_chkbx, "toggled", G_CALLBACK(gobbo_toggle_stat_callback), fighter_container);

    gtk_container_add(GTK_CONTAINER(stats_box), health_chkbx);
    gtk_container_add(GTK_CONTAINER(stats_box), strength_chkbx);
    gtk_container_add(GTK_CONTAINER(stats_box), ac_chkbx);

    //just a way to pass a pointer with a value of true
    bool location_of_true = true;
    //toggles all the stat checkboxes in the stats_box to 'checked'
    gtk_container_foreach(
        GTK_CONTAINER(stats_box), 
        (GtkCallback)gtk_toggle_button_set_active,
        &location_of_true
    );

    GtkWidget* stats_scroll_wrapper = gtk_scrolled_window_new(NULL, NULL);\
    gtk_container_add(GTK_CONTAINER(stats_scroll_wrapper), stats_box);

    //now make the input fields and buttons and that stuff

    GtkWidget* entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget* stat_entry = gtk_entry_new();
    GtkWidget* stat_entry_bttn = gtk_button_new_with_label("Add");

    //I'm feeling iffy about these memory lifetimes. Theoretically shouldn't need to ever
    //be free'ed but... Also I kinda feel like I should be RC copying the widget pointers,
    //even though I don't /think/ that is correct. 
    GobboMenuData* data = malloc(sizeof(GobboMenuData));
    if(data == NULL) {exit(EXIT_FAILURE);}
    *data = (GobboMenuData){.fighter_container = fighter_container, .other_widget = stats_box};

    *out_data = data;

    g_signal_connect(stat_entry, "activate", G_CALLBACK(gobbo_add_stat_callback), data);
    //tells stat_entry that clicking the button is the same as hitting enter.
    g_signal_connect_swapped(stat_entry_bttn, "clicked", G_CALLBACK(gobbo_stat_entry_bttn_callback), stat_entry);

    gtk_container_add(GTK_CONTAINER(entry_box), stat_entry);
    gtk_container_add(GTK_CONTAINER(entry_box), stat_entry_bttn);


    
    GtkWidget* wrapper_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(wrapper_box), entry_box);
    gtk_container_add(GTK_CONTAINER(wrapper_box), stats_scroll_wrapper);
    
    return wrapper_box;
}

GtkWidget* gobbo_create_menu(GtkContainer* fighter_container) {
    GtkWidget* menu_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    //see thoughts on the relavent code in gobbo_create_stats_menu
    GobboMenuData* stats_data;
    GtkWidget* stats_menu = gobbo_create_stats_menu(fighter_container, &stats_data);

    GtkWidget* add_fighter_bttn = gtk_button_new_with_label("Add Fighter");
    g_signal_connect_swapped(add_fighter_bttn, "clicked", G_CALLBACK(gobbo_add_fighter_callback), stats_data);

    gtk_container_add(GTK_CONTAINER(menu_container), add_fighter_bttn);
    gtk_container_add(GTK_CONTAINER(menu_container), stats_menu);

    return menu_container;
}