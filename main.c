#include <gtk/gtk.h>

GtkWidget *window;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *grid;
GtkWidget *buttons[10];
GtkWidget *box;

// Funktion zum Behandeln des Button-Klicks
void button_clicked(GtkWidget *button, gpointer data) {
  const char *text = gtk_entry_get_text(GTK_ENTRY(data));
  printf("Der eingegebene Text ist: %s\n", text);
}

// Funktion zum Behandeln der Ziffern-Buttons
void number_button_clicked(GtkWidget *button, gpointer data) {
  const char *number = gtk_button_get_label(GTK_BUTTON(button));
  gtk_entry_set_text(GTK_ENTRY(data), number);
//  gtk_entry_append_text(GTK_ENTRY(data), number);
}

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);

  // Erstellen Sie ein Fenster
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Hello World");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);

  // Erstellen Sie ein Eingabefeld
  entry = gtk_entry_new();

  // Erstellen Sie einen Button
  button = gtk_button_new_with_label("Eingabe ausgeben");

  // Verbinden Sie den Button-Klick mit der Funktion button_clicked
  g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), entry);

  // Verbinden Sie den delete-event-Handler mit gtk_main_quit()
  g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

/**/

  // Erstellen Sie ein GtkGrid-Widget für den Tastenblock
  grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 5);

  // Erstellen Sie 10 Buttons für die Ziffern 0-9
  for (int i = 0; i < 10; i++) {
    buttons[i] = gtk_button_new_with_label(g_strdup_printf("%d", i));
    g_signal_connect(buttons[i], "clicked", G_CALLBACK(number_button_clicked), entry);
  }

  // Fügen Sie die Buttons in das Grid-Widget ein
  int row = 0, column = 0;
  for (int i = 0; i < 10; i++) {
    gtk_grid_attach(GTK_GRID(grid), buttons[i], column, row, 1, 1);
    if (column == 2) {
      column = 0;
      row++;
    } else {
      column++;
    }
  }

/**/
  // Erstellen Sie einen Box-Container und fügen Sie das Eingabefeld, den Button und den Tastenblock hinzu
  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);

  // Fügen Sie den Box-Container zum Fenster hinzu
  gtk_container_add(GTK_CONTAINER(window), box);

  // Zeigen Sie alle Widgets an
  gtk_widget_show_all(window);

  // Starten Sie die GTK+-Hauptschleife
  gtk_main();

  return 0;
}
