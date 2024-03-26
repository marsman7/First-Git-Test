# Projektname
PROJECT = taschenrechner

# Quellcodedateien
SRCS = \
  main.c \
#  button_handler.c

# Headerdateien
HDRS = \
#  button_handler.h

# Bibliotheken
LIBS = \
  -lgtk3

# Objektdateien
OBJS = $(SRCS:.c=.o)

# Flagge für GTK+-Entwicklung
GTK3_CFLAGS = -Wall `pkg-config --cflags glib-2.0` \
			-I/usr/include/gtk-3.0 \
			-I/usr/include/glib-2.0 \
			-I/usr/include/pango-1.0 \
			-I/usr/include/harfbuzz \
			-I/usr/include/cairo \
			-I/usr/include/gdk-pixbuf-2.0 \
			-I/usr/include/atk-1.0

# Flagge für C-Compiler
CFLAGS = $(GTK3_CFLAGS)

# Flagge für Linker
LDFLAGS = `pkg-config --libs gtk+-3.0` -L/usr/local/lib -L/usr/lib
#LDFLAGS = `pkg-config --libs glib-2.0` -L/usr/local/lib -L/usr/lib

# Programmname
PROGRAM = $(PROJECT)

# Output Verzeichnis
OUTDIR = release/

# Ziel: Programm erstellen
all: $(PROGRAM)

$(PROGRAM): $(OBJS) $(HDRS)
	gcc $(CFLAGS) $(OBJS) -o $(OUTDIR)$(PROGRAM) $(LDFLAGS)

# Ziel: Objektdateien erstellen
%.o: %.c $(HDRS)
	gcc $(CFLAGS) -c $< -o $@

# Ziel: Code bereinigen
clean:
	rm -f $(OBJS) $(PROGRAM)

# Ziel: Code anzeigen
l:
	gcc -E $(SRCS) $(HDRS) | less
