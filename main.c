#include <gtk/gtk.h>
// https://docs.gtk.org/gtk4/

#define BUTTON_NUM  20

GtkWidget *window;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *grid;
GtkWidget *buttons[BUTTON_NUM];
GtkWidget *box;

const char *buttonlabels[BUTTON_NUM] = {"C","/","*","-","7","8","9","+","4","5","6","(","1","2","3",")"," ","0",",","="};

// Funktion zum Behandeln des Button-Klicks
void button_clicked(GtkWidget *button, gpointer data) 
{
  const char *text = gtk_editable_get_text(GTK_EDITABLE(data));
  printf("Der eingegebene Text ist: %s\n", text);
}

// Funktion zum Behandeln der Ziffern-Buttons
void number_button_clicked(GtkWidget *button, gpointer data) 
{
  const char *label = gtk_button_get_label(GTK_BUTTON(button));
  char button_char = *label;
  switch (button_char) {
    case 'C':
//      gtk_entry_set_text (GTK_ENTRY(data), "");
      gtk_editable_set_text(GTK_EDITABLE(data), "");
      break;
    default:
      GtkEntryBuffer *buffer = gtk_entry_get_buffer (GTK_ENTRY(data));
      gtk_entry_buffer_insert_text (buffer, -1, label, -1);
  }
}

static void activate (GtkApplication *app, gpointer user_data)
{
  // Erstellen Sie ein Fenster
  window = gtk_application_window_new (app);
  gtk_window_set_title(GTK_WINDOW(window), "Hello World");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);

  // Erstellen Sie ein Eingabefeld
  entry = gtk_entry_new();

  // Erstellen Sie einen Button
  button = gtk_button_new_with_label("Eingabe ausgeben");

  // Verbinden Sie den Button-Klick mit der Funktion button_clicked
  g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), entry);

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
  gtk_box_append(GTK_BOX(box), entry);
  gtk_box_append(GTK_BOX(box), button);
  gtk_box_append(GTK_BOX(box), grid);

  // Fügen Sie den Box-Container zum Fenster hinzu
  gtk_window_set_child (GTK_WINDOW (window), box);

  gtk_window_present (GTK_WINDOW (window));
}


int main (int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}