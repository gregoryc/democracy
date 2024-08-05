
#include <X11/extensions/Xfixes.h>
#include <foundationallib.h>
#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <X11/Xlib.h>


// Global variable.
char *str = NULL;
int will_catch=0;
const char *cmd="cat";

// gcc -O2 -s -fwhole-program -Wall -Wextra clip_filter.c -o clip_filter `pkg-config --libs --cflags gtk+-3.0` -lX11 -lXfixes && ./clip_filter

Bool PrintSelection(Display *display, Window window, const char *bufname, const char *fmtname)
{
  char *result;
  unsigned long ressize, restail;
  int resbits;
  Atom bufid = XInternAtom(display, bufname, False),
       fmtid = XInternAtom(display, fmtname, False),
       propid = XInternAtom(display, "XSEL_DATA", False),
       incrid = XInternAtom(display, "INCR", False);
  XEvent event;

  XSelectInput (display, window, PropertyChangeMask);
  XConvertSelection(display, bufid, fmtid, propid, window, CurrentTime);
  do {
    XNextEvent(display, &event);
  } while (event.type != SelectionNotify || event.xselection.selection != bufid);

  if (event.xselection.property)
  {
    XGetWindowProperty(display, window, propid, 0, LONG_MAX/4, True, AnyPropertyType,
      &fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);
    if (fmtid != incrid) {
    	if (str) free(str);
      str = dup_format("%.*s", (int)ressize, result);
     }
    XFree(result);

    if (fmtid == incrid)
      do {
        do {
          XNextEvent(display, &event);
        } while (event.type != PropertyNotify || event.xproperty.atom != propid || event.xproperty.state != PropertyNewValue);

        XGetWindowProperty(display, window, propid, 0, LONG_MAX/4, True, AnyPropertyType,
          &fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);
           	if (str) free(str);
str=dup_format("%.*s", (int)ressize, result);
        XFree(result);
      } while (ressize > 0);

    return True;
  }
  else // request failed, e.g. owner can't convert to the target format
    return False;
}


void transform_selection2(const char* string) {
will_catch=1;
	char* cat = concatenate_strings(cmd, "|xclip -selection c");
	puts(cat);
    // Open a pipe to xclip command
    FILE *pipe = popen(cat, "w");
    free(cat);
    if (!pipe) {
        perror("popen");
        return;
    }

    // Write the string to the pipe
    fprintf(pipe, "%s", string);

    // Close the pipe
    pclose(pipe);
}
void transform_selection(const char* string, Display *display, Window window, const char *bufname) {
    puts("CAUGHT");
    // Transform the string
    //char *transformedStr = concatenate_three_strings("<b>", string, "</b>");
    transform_selection2(string);
    
}

void WatchSelection(Display *display, Window window, const char *bufname)
{
  int event_base, error_base;
  XEvent event;
  Atom bufid = XInternAtom(display, bufname, False);

  assert( XFixesQueryExtension(display, &event_base, &error_base) );
  XFixesSelectSelectionInput(display, DefaultRootWindow(display), bufid, XFixesSetSelectionOwnerNotifyMask);

  while (True)
  {
    XNextEvent(display, &event);

    if (event.type == event_base + XFixesSelectionNotify &&
        ((XFixesSelectionNotifyEvent*)&event)->selection == bufid)
    {
    	if (will_catch) {
    		will_catch=0;
    		continue;
    	}
      if (!PrintSelection(display, window, bufname, "UTF8_STRING"))
        PrintSelection(display, window, bufname, "STRING");
		transform_selection(str, display,window, bufname);
      fflush(stdout);
    }
  }
}

void* update_label(void*)
{
/*	if (argc != 2) {
		fputs("Usage: prog-name transform-program\n", stderr);
		return 1;
	}
	printf("Transform program is ");
	char * json = string_to_json(argv[1]);
	fputs(json, stdout);
	free(json);
	puts("");
	cmd =argv[1]; */
  Display *display = XOpenDisplay(NULL);
  unsigned long color = BlackPixel(display, DefaultScreen(display));
  Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0,0, 1,1, 0, color, color);
  Bool result;

  WatchSelection(display,window,"CLIPBOARD");

  XDestroyWindow(display, window);
  XCloseDisplay(display);


  if (str) free(str);
return 0;
}

#include <gtk/gtk.h>

// Function to run the specified script with the provided input
void run_script(const gchar *script) {
cmd=script;
}

// Function to handle the "Run" button click event
void on_run_button_clicked(GtkButton *button, gpointer user_data) {
    // Get the script from the GtkEntry widget
    GtkEntry *entry = GTK_ENTRY(user_data);
    const gchar *script = gtk_entry_get_text(entry);

    run_script(script);
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Clipboard Filter");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to hold the entry and button
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Create the GtkEntry widget
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter script to run");
    gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 0);

    // Create the "Run" button
    GtkWidget *button = gtk_button_new_with_label("Set clipboard filter path");
    g_signal_connect(button, "clicked", G_CALLBACK(on_run_button_clicked), entry);
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);

    // Show all widgets
    gtk_widget_show_all(window);



    // Start the GTK main loop
    if (fork()==0) {
    pthread_t thread;
    pthread_create(&thread, NULL, update_label, (void *)NULL);
    gtk_main();
    }

    return 0;
}
