#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

destroylastplot(Display *dpy)
{
	Window root, parent, *children = NULL, win;
	unsigned int nchildren;
	int n;
	char *name = NULL;

	win = RootWindow(dpy, DefaultScreen(dpy));
	XQueryTree(dpy, win, &root, &parent, &children, &nchildren);
	for (n=0; n< nchildren; n++) {
		if (XFetchName(dpy, children[n], &name))
			if (!strcmp(name, "xplot")) {
				tellwin(children[n], dpy);
			}
		if (name) free(name);
	}
}

tellwin(Window w, Display *dpy)
{
	XButtonEvent ev;

	ev.type = ButtonPress;
	ev.display = dpy;
	ev.window = w;
	ev.x = ev.y = 10;
	ev.state = 0;
	ev.button = Button1;
	XSendEvent(dpy, w, False, ButtonPressMask, &ev);
}









