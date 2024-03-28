# Projektname
PROJECT = taschenrechner

# Quellcodedateien
SRCS = \
  main.c \
  calc.c

# Headerdateien
HDRS = \
  calc.h

# Bibliotheken
LIBS = \
  -lgtk4

# Objektdateien
OBJS = $(SRCS:.c=.o)

# Flagge für GTK+-Entwicklung
GTK3_CFLAGS = -Wall `pkg-config --cflags gtk4`

# Flagge für C-Compiler
CFLAGS = $(GTK3_CFLAGS)

# Flagge für Linker
LDFLAGS = `pkg-config --libs gtk4` -L/usr/local/lib -L/usr/lib

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
