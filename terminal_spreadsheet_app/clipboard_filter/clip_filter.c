#include <X11/extensions/Xfixes.h>
#include <foundationallib.h>
#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <X11/Xlib.h>


// Global variable.
char *str = NULL;
int will_catch=0;
char *cmd;

// gcc -o clip_filter clip_filter.c -lX11 -lXfixes

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

int main(int argc, char ** argv)
{
	if (argc != 2) {
		fputs("Usage: prog-name transform-program\n", stderr);
		return 1;
	}
	printf("Transform program is ");
	char * json = string_to_json(argv[1]);
	fputs(json, stdout);
	free(json);
	puts("");
	cmd =argv[1];
  Display *display = XOpenDisplay(NULL);
  unsigned long color = BlackPixel(display, DefaultScreen(display));
  Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0,0, 1,1, 0, color, color);
  Bool result;

  WatchSelection(display,window,"CLIPBOARD");

  XDestroyWindow(display, window);
  XCloseDisplay(display);


  if (str) free(str);
  return !result;
}
