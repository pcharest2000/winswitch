#ifndef __MAIN_H_
#define __MAIN_H_
#include "atoms.h"
#include <X11/Xlib.h>
#include <X11/Xmu/WinUtil.h>
#include <cairo/cairo.h>
#include <gtk/gtk.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>

#define  MAXLABELLENGTH 50


typedef struct windowsInfo_t {
  xcb_window_t winId;
  int32_t desktop;
  uint16_t width;
  uint16_t height;
  int16_t x;
  int16_t y;
  uint32_t charWidth;
  char as[22];          // If we go above 3 character how many windows do you have
  int32_t numCharMatch; // Numbers of charcters that have been matched
  uint32_t noMatch;
  uint32_t fontPosX;
  uint32_t fontPosY;
} windowInfo_t;

typedef struct desktopInfo_t {
  int32_t desktopNum;
  uint32_t indexWindow; // The index where the windows are located
  uint16_t numWindows;  // Total number of
  char firstChar;       // The first char to reach the desktop
} desktopInfo_t;

typedef struct {
  float winAlpha;   // = 0.8;
  uint32_t timeOut; // = 10; // seconds
  double fontSize;  // = 64.0;
  char quitChar;
  float fontR;
  float fontG;
  float fontB;
  float selectedAlpha;
  gboolean ignoreSticky;
  char labelString[MAXLABELLENGTH];
  char font[];
} configuration;


configuration config = {
    .winAlpha = 0.4,
    .timeOut = 10,
    .fontSize = 64.0,
    .quitChar = 'q',
    .fontR = 0.99216, //FontColor
    .fontG = 1.0,
    .fontB = 0.945098,
    .selectedAlpha = 0.3,
    .ignoreSticky=FALSE,
    .labelString= "fjdklaghei;nvop\0",
    .font = "Serif",
};
 uint32_t numCharInLabelString = sizeof(config.labelString) - 1;

void getVisibleWindows();
void printVisibleWindows();
void getDesktopsInfo();
void printDesktopInfo();
void requestWindowChange(windowInfo_t win);
void returnHome();
void destroyWindow();
// Helpers
void selectionSort(windowInfo_t inpput[], int n);
void swap(windowInfo_t *xp, windowInfo_t *yp);
void getDesktopsInfo();
void labelWindows();
//Config functions for parameters
int configFontColor(char *input);
int configFontSize(char *input);
int configFontAlpha(char *input);
int configTimeOut(char *input);
int configWindowAlpha(char *input);
// Globals
xcb_connection_t *xcb_con;
xcb_screen_t *screen;
xcb_ewmh_connection_t *ewmh_con;
Display *disp_con;                   // X11 Connection
windowInfo_t *visibleWindowsArray;   // Holds information about visible windows on
desktopInfo_t *visibleDesktopsArray; // Holds info on the desktops
uint32_t numVisibleWindows;
uint32_t numVisibleDesktops;
uint32_t curentActiveDesktop;  // Used to got back to it when we exit and
xcb_window_t currentActiveWin; // Used to go back to it when we do nothing
gboolean posiInitalized = FALSE;
uint32_t charMatchIndex = 0; // Holds how many charcters we have matched
// GTK Stuff
GtkWidget *gtkWin; //The gtik window really is a widget
gint gTimerQuit;   //The GTK timer reference
void gtkInitWindow();
gboolean keypressCallback(GtkWidget *widget, GdkEventKey *event, gpointer data);
static gboolean drawCallback(GtkWidget *widget, cairo_t *cr, gpointer user_data);
static void do_drawing(cairo_t *cr);
gint timeOutCallback(); // Function to exit when timer expires

// clang-format off
#define VERSION "1.0"

/* help {{{ */
/* #define HELP */
const char help[]={
  "winswitch " VERSION "\n"
  "Usage: winswitch [OPTION]...overlay to change windows using the keyboard mostly used with tiling window managers. \n"
  "Get help:   --help \n"
  "Actions:\n"
  "  -t <TIME>           Set timeout period in seconds, 0 for no timeout\n"
  "  -s <SIZE>           Set the font size in pixels \n"
  "  -f <NAME>           Set the font name\n"
  "  -a <ALPHA>          Set the inactive font alpha alpha \n"
  "  -c <ALPHA>          Set the font color format is hex color FFFFFF \n"
  "  -w <ALPHA>         Set the window alpha must be between 0.0 and 1.0  \n"
  "  -a                  Ignore sticky windows, some applications (docks) do not  \n"
  "                      set properly its window property \n"
  "  -S                  A string of characters used to label the windows must be at least 2 character long, \n"
  "                      and more than the number of active desktops, characters must be not repeated or behavior\n"
  "                      behavior of the app is undefined\n"

  "  -h or --help  for this listing \n"
  "Author, current maintainer: Philippe Charest <philippe.charest@gmail.com\n"                \
  "Released under the GNU General Public License.\n"
  "Copyright (C) 2020\n"};
/* /\* }}} *\/ */

/* // clang-format off */


#endif                  // __MAIN_H_
