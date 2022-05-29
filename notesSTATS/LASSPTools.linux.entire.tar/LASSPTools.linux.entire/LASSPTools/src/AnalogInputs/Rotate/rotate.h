#define XtNxColor "xColor"
#define XtNyColor "yColor"
#define XtNzColor "zColor"
#define XtNcircleColor "circleColor"

#define ROTATE_AREA_DEFAULT_WIDTH  160
#define ROTATE_AREA_DEFAULT_HEIGHT 160
#define BUTTON_WIDTH  160
#define BUTTON_HEIGHT 20

#ifdef X11R3
#ifdef __STDC__
typedef void*           XtPointer;
#else
typedef char*           XtPointer;
#endif
#endif

#ifndef _NO_PROTO
Widget  CreateQuitButton(Widget parent);
void    CreateMouseTracker(Widget gTheCanvas);
void	GetResources();
void	CreateMouseTracker(Widget gRotateArea);
void	StartTracking(Widget gRotateArea, XtPointer clientData,
		      XEvent *theEventPtr);
void	TrackMousePosition(Widget gRotateArea, XtPointer clientData,
			   XEvent *theEventPtr);
void	EndTracking(Widget gRotateArea, XtPointer clientData, 
		    XEvent *theEventPtr);
void	DrawNewAxes(void);
void	HandleExposure(Widget w, caddr_t clientData, caddr_t callData);
#else
Widget  CreateQuitButton();
void    CreateMouseTracker();
void	GetResources();
void	CreateMouseTracker();
void	StartTracking();
void	TrackMousePosition();
void	EndTracking();
void	DrawNewAxes();
void	HandleExposure();
#endif

typedef struct {
	Pixel   xColor;
        Pixel   yColor;
        Pixel   zColor;
        Pixel   circleColor;
	} ApplicationData, *ApplicationDataPtr;

#define X_coOrd 0
#define Y_coOrd 1
#define Z_coOrd 2

extern GC        gRotateGC;
extern Widget    gTopLevel;
extern Widget    gRotateArea;
extern Widget    gQuitButton;
extern Widget    gResetButton;
extern Dimension gHeight2, gWidth2, gCircleBorder;
extern Pixel     gbgColor;
extern ApplicationData gRotateAreaData;
