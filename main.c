#include <gtk/gtk.h>
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

  // Erstellen Sie ein GtkGrid-Widget für den Tastenblock
  grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 5);

  // Erstellen Sie 10 Buttons für die Ziffern 0-9
  for (int i = 0; i < BUTTON_NUM; i++) {
    buttons[i] = gtk_button_new_with_label(buttonlabels[i]);
    g_signal_connect(buttons[i], "clicked", G_CALLBACK(number_button_clicked), entry);
  }

  // Fügen Sie die Buttons in das Grid-Widget ein
  for (int i = 0; i < BUTTON_NUM; i++) {
    gtk_grid_attach(GTK_GRID(grid), buttons[i], i % 4, i / 4, 1, 1);
  }

  // Erstellen Sie einen Box-Container und fügen Sie das Eingabefeld, den Button und den Tastenblock hinzu
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
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
