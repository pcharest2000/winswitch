#ifndef __MAIN_H_
#define __MAIN_H_
#include "atoms.h"
#include <X11/Xlib.h>
#include <X11/Xmu/WinUtil.h>
#include <cairo-ft.h>
#include <cairo/cairo.h>
#include <ft2build.h>
#include <gtk/gtk.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <string.h>
#include <ctype.h>
#include FT_FREETYPE_H

#define MAXLABELLENGTH 50

typedef struct color_t {
  float r;
  float g;
  float b;
  float alpha;
} color_t;

typedef struct rect_t {
  uint32_t x;
  uint32_t y;
  uint32_t width;
  uint32_t heigth;
  float radius;
  color_t color;
} rect_t;

typedef struct rectConfig_t {
  float radius;
  color_t color;
} rectConfig_t;

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
  rect_t rbackRect;
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
  color_t fontColor;
  float selectedAlpha;
  gboolean ignoreSticky;
  rectConfig_t rect;
  char labelString[MAXLABELLENGTH];
  cairo_font_face_t *cff;
  char fontPath[];
} configuration_t;

configuration_t config = {
    .winAlpha = 0.2,
    .timeOut = 10,
    .fontSize = 64.0,
    .quitChar = 'q',
    .fontColor.r = 0.99216,
    .fontColor.g = 1.0,
    .fontColor.b = 0.945098,
    .fontColor.alpha = 0.3,
    .ignoreSticky = FALSE,
    .rect.radius = 9.6,
    .rect.color.r = 0.23137,
    .rect.color.g = 0.23529,
    .rect.color.b = 0.20784,
    .rect.color.alpha = 0.6,
    .labelString = "fjdklaghei;nvop\0",
    .fontPath = "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf"
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
void loadFont();
void strToUpper(char *input);
//Config functions for parameters

void initConfig();
int configFontColor(char *input);
int configFontSize(char *input);
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
"Usage: winswitch [OPTION]\n"
"A command line tool to switch the active window using the keyboard, displays  an overlay\n"
"on dekstops to select window, mostly used for tiling window managers.\n"
"Get help:   --help \n"
"Actions:\n"
"  -t <TIME>   Set timeout period in seconds to quit, 0 for no timeout, default 10.\n"
"  -s <SIZE>   Set the font size in pixels, default 64 \n"
"  -f <FILE>   Set the ttf or otf font path name\n"
"  -c <COLOR>  Set the font color format is hex color RRGGBBAA \n"
"              the alpha value will be used to indicate selctcted characters \n"
"  -w <ALPHA>  Set the window alpha must be between 0.0 and 1.0, default 0.2  \n"
"  -a          Ignore sticky windows, some applications (docks) do not  \n"
"              set properly its window property \n"
"  -S          A string of characters used to label the windows must be at least 2 character long, \n"
"              and more than the number of active desktops, characters must be not repeated or behavior\n"
"              behavior of the app is undefined\n"

"  -h or --help  for this listing \n"
"Author, current maintainer: Philippe Charest <philippe.charest@gmail.com\n"                \
"Released under the GNU General Public License.\n"
"Copyright (C) 2020\n"};
/* /\* }}} *\/ */

/* // clang-format off */


#endif                  // __MAIN_H_
