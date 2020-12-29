#ifndef __MAIN_H_
#define __MAIN_H_
#include "atoms.h"
#include "switcherDef.h"
#include <cairo/cairo-xcb.h>
#include <cairo/cairo.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>

#include <X11/Xlib.h>
#include <X11/Xmu/WinUtil.h>
#include <gtk/gtk.h>
#define MAXWINDOWS 100

typedef struct windowsInfo_t {
  xcb_window_t winId;
  int32_t desktop;
  uint16_t width;
  uint16_t height;
  int16_t x;
  int16_t y;
  uint32_t charWidth;
  char as[12]; // If we go above 3 character how many windows do you have
  uint32_t numCharMatch;
  uint32_t noMatch;
  uint32_t fontPosX;
  uint32_t fontPosY;
} windowInfo_t;

typedef struct desktopInfo_t {
  int32_t desktopNum;
  uint32_t indexWindow; // The index where the windows are located
  uint16_t numWindows;
  char firstChar; // The first char to reach the desktop
} desktopInfo_t;

typedef struct
{
float transparency;// = 0.8;
uint32_t timeOut;// = 10; // seconds
double fontSize;// = 64.0;
double fontHalfHeigth;// = 32;
double fontHalfWidth;// = 32;
char quitChar;
char font[];
}configuration;

configuration  config = {
.transparency =0.8,
.timeOut=5000,
.fontSize=32.0,
.fontHalfHeigth=16.0,
.fontHalfWidth=16.0,
.quitChar='q',
.font="Serif",
};




void getVisibleWindows();
void printVisibleWindows();
void getDesktopsInfo();
void requestWindowChange(xcb_window_t win);
void returnHome();
void printDesktopInfo();
void destroyWindow();
void calFontPosition();
// Helpers
void selectionSort(windowInfo_t inpput[], int n);
void swap(windowInfo_t *xp, windowInfo_t *yp);
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
                              gpointer user_data);
void getDesktopsInfo();
void labelWindows();
static void do_drawing(cairo_t *cr);

gint timeOut(); //Function to exit when timer expires
// Globals
xcb_connection_t *xcb_con;
xcb_screen_t *screen;

xcb_ewmh_connection_t *ewmh_con;
Display *disp_con; // X11 Connection
cairo_t *cr;
windowInfo_t *visibleWindowsArray; // Holds information about visible windows on
desktopInfo_t *visibleDesktopsArray; // Holds info on the desktops
uint32_t numVisibleWindows;
uint32_t numVisibleDesktops;
uint32_t curentActiveDesktop;  // Used to got back to it when we exit and
xcb_window_t currentActiveWin; // Used to go back to it when we do nothing
const char labelString[] = "abcde";
const uint32_t numCharInLabelString = sizeof(labelString) - 1;
uint32_t charMatchIndex = 0; // Holds how many charcters we have matched
// GTK Stuff

GtkWidget *gtkWin;
gboolean keypress_function(GtkWidget *widget, GdkEventKey *event,
                           gpointer data);
void gtkInitWindow();
gint gTimerQuit;
#endif // __MAIN_H_
