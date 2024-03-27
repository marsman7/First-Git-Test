#include <gtk/gtk.h>
// https://docs.gtk.org/gtk3/
// https://developer-old.gnome.org/gtk3/stable

#define BUTTON_NUM  20

GtkWidget *window;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *grid;
GtkWidget *buttons[BUTTON_NUM];
GtkWidget *box;

const char *buttonlabels[BUTTON_NUM] = {"C","/","*","-","7","8","9","+","4","5","6","(","1","2","3",")"," ","0",",","="};

// Funktion zum Behandeln des Button-Klicks
void button_clicked(GtkWidget *button, gpointer data) {
  const char *text = gtk_entry_get_text(GTK_ENTRY(data));
  printf("Der eingegebene Text ist: %s\n", text);
}

// Funktion zum Behandeln der Ziffern-Buttons
void number_button_clicked(GtkWidget *button, gpointer data) {
  const char *label = gtk_button_get_label(GTK_BUTTON(button));
  char button_char = *label;
  switch (button_char) {
    case 'C':
      gtk_entry_set_text (GTK_ENTRY(data), "");
      break;
    default:
      GtkEntryBuffer *buffer = gtk_entry_get_buffer (GTK_ENTRY(data));
      gtk_entry_buffer_insert_text (buffer, -1, label, -1);
  }
}

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);

  // Erstellt ein Fenster
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Hello World");
  gtk_window_set_default_size(GTK_WINDOW(window), 320, 100);

  // Erstellt ein Eingabefeld
  entry = gtk_entry_new();

  // Erstellt einen Button
  button = gtk_button_new_with_label("Eingabe ausgeben");
  
  // Verbindt den Button-Klick mit der Funktion button_clicked
  g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), entry);

  // Verbindt den delete-event-Handler mit gtk_main_quit()
  g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

  // Erstellt ein GtkGrid-Widget für den Tastenblock
  grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 5);

  for (int i = 0; i < BUTTON_NUM; i++) {
    // Erstellen der Buttons
    buttons[i] = gtk_button_new_with_label(buttonlabels[i]);
    g_signal_connect(buttons[i], "clicked", G_CALLBACK(number_button_clicked), entry);

    // Fügt die Buttons in das Grid-Widget ein
    gtk_grid_attach(GTK_GRID(grid), buttons[i], i % 4, i / 4, 1, 1);
  }

  // Erstellt einen Box-Container und fügt das Eingabefeld, den Button und den Tastenblock hinzu
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 5);
  gtk_widget_set_margin_start(entry, 10);
  gtk_widget_set_margin_end(entry, 10);

  gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 5);
  gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);

  gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 5);

  // Fügt den Box-Container zum Fenster hinzu
  gtk_container_add(GTK_CONTAINER(window), box);

  // Zeigt alle Widgets an
  gtk_widget_show_all(window);

  // Startet die GTK+-Hauptschleife
  gtk_main();

  return 0;
}
