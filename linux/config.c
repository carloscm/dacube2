
#include <stdlib.h>
#include <stdio.h>

#include <gtk/gtk.h>

#include "config.h"

#include "fznback2.xbm"

static int result = 0;
static GtkWidget *window;

static GtkCheckButton* ch_oldskool = NULL;
static GtkCheckButton* ch_windowed = NULL;

static void start_event(GtkWidget *widget, gpointer data) {
	result = 0;
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch_oldskool)))
		result = C_OLD;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ch_windowed)))
		result |= C_WIN;
	gtk_widget_destroy(GTK_WIDGET(window));
	gtk_main_quit();
}

static void quit_event(GtkWidget *widget, gpointer data) {
	result = -1;
	gtk_widget_destroy(GTK_WIDGET(window));
	gtk_main_quit();
}

static gint delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
	result = -1;
	gtk_widget_destroy(GTK_WIDGET(window));
	gtk_main_quit();
	return FALSE;
}

int config_dialog(char* title, int argc, char** argv) {
	GtkWidget *box1, *but_quit, *but_start;
	GdkBitmap *mask;
	GdkPixmap *background;
	GdkColormap* colormap;
	GdkColor white, black;

	if (gtk_init_check(&argc, &argv) == FALSE)
		return -1;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_app_paintable(GTK_WIDGET(window), TRUE);
	gtk_window_set_title(GTK_WINDOW(window), title);
	gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, FALSE);
	gtk_widget_set_usize(GTK_WIDGET(window), 120, 400);

	gtk_window_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	gtk_signal_connect(GTK_OBJECT(window), "delete_event",
		GTK_SIGNAL_FUNC(delete_event), NULL);

	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

		box1 = gtk_vbox_new(FALSE, 0);

		gtk_container_add(GTK_CONTAINER(window), box1);

		ch_windowed = gtk_check_button_new_with_label("Windowed");
		gtk_box_pack_start(GTK_BOX(box1), ch_windowed, FALSE, FALSE, 0);
		gtk_widget_show(ch_windowed);
		
		ch_oldskool = gtk_check_button_new_with_label("Oldskool");
		gtk_box_pack_start(GTK_BOX(box1), ch_oldskool, FALSE, FALSE, 0);
		gtk_widget_show(ch_oldskool);

		but_quit = gtk_button_new_with_label("Quit");
		gtk_box_pack_end(GTK_BOX(box1), but_quit, FALSE, FALSE, 0);
		gtk_signal_connect(GTK_OBJECT(but_quit), "clicked", GTK_SIGNAL_FUNC(quit_event), NULL);
		gtk_widget_show(but_quit);

		but_start = gtk_button_new_with_label("Start");
		gtk_box_pack_end(GTK_BOX(box1), but_start, FALSE, FALSE, 5);
		gtk_signal_connect(GTK_OBJECT(but_start), "clicked", GTK_SIGNAL_FUNC(start_event), NULL);
		gtk_widget_show(but_start);

		gtk_widget_show(box1);

	gtk_widget_realize(GTK_WIDGET(window));
	gdk_window_set_decorations(GTK_WIDGET(window)->window, 0);
	colormap = gdk_colormap_get_system();
	gdk_color_white(colormap, &white);
	gdk_color_black(colormap, &black);
	background = gdk_pixmap_create_from_data(GTK_WIDGET(window)->window,
		fznback2_bits, fznback2_width, fznback2_height, -1, &black, &white);
	gdk_window_set_back_pixmap(GTK_WIDGET(window)->window, background, FALSE);
        gdk_window_clear(GTK_WIDGET(window)->window);

	gtk_widget_show(window);

	gtk_main();

	return result;
}

