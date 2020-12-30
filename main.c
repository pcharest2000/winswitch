
/* license {{{ */
/*

winswitch
A command line tool to change windows with an overlay on dekstops to switch window.
Mostly used for tiling window managers

Author, current maintainer: Philippe Charest <philippe.charest@gmail.com>

Copyright (C) 2020

This program is free software which I release under the GNU General Public
License. You may redistribute and/or modify this program under the terms
of that license as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

To get a copy of the GNU General Puplic License,  write to the
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
/* }}} */
#include "main.h"

gboolean labelMatch(char input) {
  uint32_t match = 0;
  // Is the character is not part of the string if not exit
  if (!strchr(labelString, input)) {
    printf("\nNoMatch");
    return FALSE;
  }
  for (uint32_t i = 0; i < numVisibleWindows; i++) {
    if (visibleWindowsArray[i].noMatch)
      continue;
    if (visibleWindowsArray[i].as[charMatchIndex] == input) {
      visibleWindowsArray[i].numCharMatch++;
      match = 1;
      if (visibleWindowsArray[i].numCharMatch == visibleWindowsArray[i].charWidth) {
        requestWindowChange(visibleWindowsArray[i]);
        destroyWindow();
        return TRUE;
      }
    }
  }
  if (match) {
    charMatchIndex++;
    for (uint32_t i = 0; i < numVisibleWindows; i++) {
      if (visibleWindowsArray[i].charWidth == charMatchIndex) {
        visibleWindowsArray[i].noMatch = 1;
      }
    }
    return TRUE;
  }
  return FALSE;
}
void gtkInitWindow() {
  gtkWin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_app_paintable(gtkWin, TRUE);
  gtk_window_set_type_hint(GTK_WINDOW(gtkWin), GDK_WINDOW_TYPE_HINT_DIALOG);
  gtk_window_set_keep_above(GTK_WINDOW(gtkWin), TRUE);
  gtk_window_set_title(GTK_WINDOW(gtkWin), "Switcher");
  gtk_widget_add_events(gtkWin, GDK_KEY_PRESS_MASK);
  GdkScreen *gdkscreen = gdk_screen_get_default();
  GdkVisual *visual = gdk_screen_get_rgba_visual(gdkscreen);
  if (visual != NULL && gdk_screen_is_composited(gdkscreen)) {
    gtk_widget_set_visual(gtkWin, visual);
  }
  /* cr = gdk_drawing_context_get_cairo_context(gtkWindow); */
  g_signal_connect(G_OBJECT(gtkWin), "draw", G_CALLBACK(drawCallback), NULL);
  g_signal_connect(gtkWin, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(gtkWin), "key_press_event", G_CALLBACK(keypressCallback), NULL);
  gtk_window_set_default_size(GTK_WINDOW(gtkWin), screen->width_in_pixels, screen->height_in_pixels);
  gtk_widget_show_all(gtkWin);
  gtk_window_move(GTK_WINDOW(gtkWin), 0, 0);
}

int configTimeOut(char *input){

  int timeOut;
  if (sscanf(input, "%d", &timeOut) == EOF || timeOut < 0 ) {
    fprintf(stderr, "\n Invalid option must be 0 or greater");
    return EXIT_FAILURE;
  }
  config.timeOut = timeOut;

return 0;
}
int configFontAlpha(char *input){

  float alpha;
  if (sscanf(input, "%f", &alpha) == EOF || alpha>1.0 || alpha<0.0) {
    fprintf(stderr, "\n Invalid input for font alpha value must be float between 0 .0and 1.0");
    return EXIT_FAILURE;
  }
  config.selectedAlpha =alpha;

return 0;
}
int configFontSize(char *input) {
  float size;
  if (sscanf(input, "%f", &size) == EOF || size<1.0) {
    fprintf(stderr, "\n Invalid input for font size");
    return EXIT_FAILURE;
  }
  config.fontSize =size;
  return 0;
}

int configFontColor(char *input) {
  printf("\n Input:%s,input",input);
  //VddFunction takes hexadeciin
  //male
  if (strlen(input) != 6) {
    printf("\n Invalid size for color, must be RGB hexadecimal!");
    return EXIT_FAILURE;
  }
  int r, g, b;
  //printf("%s",input);
  sscanf(input, "%02x%02x%02x", &r, &g, &b);
  config.fontR = r / 255.0;
  config.fontB = b / 255.0;
  config.fontG = g / 255.0;
  return 0;
  /* printf("\n R: %f G: %f B: %f", config.fontR, config.fontG, config.fontB); */
}
static void do_drawing(cairo_t *cr) {
  gtk_window_move(GTK_WINDOW(gtkWin), 0, 0);
  // We calcu;ate the font extent only once and fill the info
  cairo_select_font_face(cr, config.font, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, config.fontSize);
  if (!posiInitalized) {
    cairo_select_font_face(cr, config.font, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, config.fontSize);
    posiInitalized = TRUE;
    printf("Hello");
    cairo_text_extents_t te; // tmp extent
    char tmp[] = " ";
    for (uint32_t i = 0; i < numVisibleWindows; i++) {
      visibleWindowsArray[i].numCharMatch = 0;
      cairo_text_extents(cr, visibleWindowsArray[i].as, &te);
      visibleWindowsArray[i].fontPosX = visibleWindowsArray[i].x + visibleWindowsArray[i].width / 2 - te.width / 2;
      visibleWindowsArray[i].fontPosY = visibleWindowsArray[i].y + visibleWindowsArray[i].height / 2 + config.fontSize / 2;
    }
  }

  // Clear the screen
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_set_source_rgba(cr, 0, 0.0, 0, config.winAlpha);
  cairo_paint(cr);
  cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
  char temp[] = " ";
  for (uint32_t i = 0; i < numVisibleWindows; i++) {
    if (visibleWindowsArray[i].noMatch) {
      continue;
    }
    cairo_move_to(cr, visibleWindowsArray[i].fontPosX, visibleWindowsArray[i].fontPosY);
    for (int32_t j = 0; j < visibleWindowsArray[i].charWidth; j++) {
      temp[0] = visibleWindowsArray[i].as[j];
      if (j + 1 <= visibleWindowsArray[i].numCharMatch)
        cairo_set_source_rgba(cr, config.fontR, config.fontG, config.fontB, config.selectedAlpha);
      else
        cairo_set_source_rgb(cr, config.fontR, config.fontG, config.fontB);
      cairo_show_text(cr, temp);
    }
  }
  cairo_stroke(cr);
}

static gboolean drawCallback(GtkWidget *widget, cairo_t *cr,
                             gpointer user_data) {
  printf("\nIn draw callback");
  do_drawing(cr);
  return FALSE;
}

void returnHome() {
  // we need to reset the mouse pointer to the origanle window and Desktop
  xcb_warp_pointer(xcb_con, XCB_NONE, currentActiveWin, 10, 10, 20, 20, 20, 20);
  xcb_ewmh_set_current_desktop(ewmh_con, 0, curentActiveDesktop);
  xcb_ewmh_request_change_active_window(ewmh_con, 0, currentActiveWin, 1, 0, currentActiveWin);
  xcb_flush(xcb_con);
  usleep(500);
}
void requestWindowChange(windowInfo_t winToActivate) {
  ///We also move the cursor to the new window we are so nice!
  xcb_warp_pointer(xcb_con, XCB_NONE, winToActivate.winId, 10, 10, 20, 20, 20, 20);
  xcb_ewmh_request_change_active_window(ewmh_con, 0, winToActivate.winId, 1, 0, currentActiveWin);
  xcb_flush(xcb_con);
  usleep(500); // This is required when quitting sometimes does not honor
}
void printVisibleWindows() {
  printf("\nTotal windows: %d", numVisibleWindows);
  for (uint32_t i = 0; i < numVisibleWindows; i++) {
    printf("\n %d: Win ID:%d", i, visibleWindowsArray[i].winId);
    printf(" Desk: %d", visibleWindowsArray[i].desktop);
    printf(" X: %d Y: %d W: %d, H: %d cWidth: % d ", visibleWindowsArray[i].x, visibleWindowsArray[i].y, visibleWindowsArray[i].width, visibleWindowsArray[i].height, visibleWindowsArray[i].charWidth);
    printf(" as: %s", visibleWindowsArray[i].as);
    printf(" noMathc: %d", visibleWindowsArray[i].noMatch);
  }
  fflush(stdout);
}
void getVisibleWindows() {
  // Get the current window that is active
  xcb_ewmh_get_active_window_reply(ewmh_con, xcb_ewmh_get_active_window(ewmh_con, 0), &currentActiveWin, NULL);
  // Get the curent desktop  that is active
  xcb_ewmh_get_current_desktop_reply(ewmh_con, xcb_ewmh_get_current_desktop(ewmh_con, 0), &curentActiveDesktop, NULL);
  numVisibleWindows = 0;
  /* Get window list */
  xcb_ewmh_get_windows_reply_t windowsReply;
  xcb_get_property_cookie_t propCookie;
  propCookie = xcb_ewmh_get_client_list(ewmh_con, 0);
  xcb_ewmh_get_client_list_reply(ewmh_con, propCookie, &windowsReply, NULL);
  xcb_flush(xcb_con);
  visibleWindowsArray = (windowInfo_t *)malloc(windowsReply.windows_len * sizeof(windowInfo_t));
  fflush(stdout);
  for (uint32_t i = 0; i < windowsReply.windows_len; i++) {
    /*Get Window Attribute 0 is unmapped, 1 is visible and 2 is not*/
    xcb_get_window_attributes_cookie_t winAtriCookie = xcb_get_window_attributes(xcb_con, windowsReply.windows[i]);
    xcb_get_window_attributes_reply_t *winAttriReply = xcb_get_window_attributes_reply(xcb_con, winAtriCookie, NULL);
    uint8_t state = winAttriReply->map_state;
    free(winAttriReply);
    if (state == XCB_MAP_STATE_VIEWABLE) {
      visibleWindowsArray[numVisibleWindows].winId = windowsReply.windows[i];
      /*Get windows Destopps*/
      uint32_t desk;
      xcb_ewmh_get_wm_desktop_reply(ewmh_con, xcb_ewmh_get_wm_desktop(ewmh_con, windowsReply.windows[i]), &desk, NULL);
      visibleWindowsArray[numVisibleWindows].desktop = desk;
      /*Get window Geometry*/
      int x, y, junkx, junky;
      unsigned int wwidth, wheight, bw, depth;
      Window junkroot;
      XGetGeometry(disp_con, windowsReply.windows[i], &junkroot, &junkx, &junky, &wwidth, &wheight, &bw, &depth);
      XTranslateCoordinates(disp_con, windowsReply.windows[i], junkroot, junkx, junky, &x, &y, &junkroot);
      visibleWindowsArray[numVisibleWindows].width = wwidth;
      visibleWindowsArray[numVisibleWindows].height = wheight;
      visibleWindowsArray[numVisibleWindows].x = x;
      visibleWindowsArray[numVisibleWindows].y = y;
      visibleWindowsArray[numVisibleWindows].noMatch = FALSE;
      numVisibleWindows++;
    }
  }
  xcb_ewmh_get_windows_reply_wipe(&windowsReply);
}

int connectToServers() {
  /* Open the connection to the X11 server */
  if (!(disp_con = XOpenDisplay(NULL))) {
    fputs("\nCannot open display.", stderr);
    return EXIT_FAILURE;
  }
  /* Open the connection to the XCB server */
  xcb_con = xcb_connect(NULL, NULL);
  if (xcb_connection_has_error(xcb_con)) {
    printf("\nXCB has erros!!");
    return EXIT_FAILURE;
  }

  /////// Get the first screen */
  const xcb_setup_t *set;
  set = xcb_get_setup(xcb_con);
  const xcb_setup_t *xcb_get_setup(xcb_connection_t * c);
  screen = xcb_setup_roots_iterator(xcb_get_setup(xcb_con)).data;
  /*Open the connection to ewmh*/
  ewmh_con = malloc(sizeof(xcb_ewmh_connection_t));
  xcb_intern_atom_cookie_t *ewmhCookie;
  if (!xcb_ewmh_init_atoms_replies(ewmh_con, xcb_ewmh_init_atoms(xcb_con, ewmh_con), NULL)) {
    puts("\nEWHM init failed");
    return EXIT_FAILURE;
  } else
    puts("\nEWHM init succes");
  return 0;
}
void destroyWindow() {
  xcb_flush(xcb_con);
  xcb_disconnect(xcb_con);
  XCloseDisplay(disp_con);
  usleep(500);
  gtk_widget_destroy(gtkWin);
  gtk_main_quit();
}
gboolean keypressCallback(GtkWidget *widget, GdkEventKey *event,
                          gpointer data) {
  // Reset the timer to quit application
  if(config.timeOut>0){
  g_source_remove(gTimerQuit);
  gTimerQuit = g_timeout_add_seconds(config.timeOut, timeOutCallback, NULL);
  }
  // Quit application
  if (event->keyval == config.quitChar || event->keyval == GDK_KEY_Escape) {
    returnHome();
    destroyWindow();
    gtk_main_quit();
    return FALSE;
  }
  printf("\nChar key: %c", event->keyval);
  // If we have a keyboard match redraw
  gtk_widget_queue_draw(gtkWin);
  if (labelMatch(event->keyval)) {
    // gtk_widget_queue_draw(gtkWin);
  }
  return FALSE;
}

gint timeOutCallback() {
  printf("\nTimeoOut");
  returnHome();
  destroyWindow();
  return FALSE; //Do not repeat the timer
}

int parseArguments(int argc, char *argv[]) {
  int opt;
  // put ':' in the starting of the
  // string so that program can
  //distinguish between '?' and ':'
  while ((opt = getopt(argc, argv, "c:s:a:t:")) != -1) {
    switch (opt) {
    case 'c':
      configFontColor(optarg);
      break;
    case 's':
      configFontSize(optarg);
      break;
    case 'a':
      configFontAlpha(optarg);
      break;
    case 't':
      configTimeOut(optarg);
      break;
    case ':':
      printf("\nOption needs a value");
      break;
    case '?':
      printf("unknown option:%c\n ", optopt);
      break;
    }
  }

  // optind is for the extra arguments
  // which are not parsed
  for (; optind < argc; optind++) {
    printf("extra arguments %s\n ", argv[optind]);
  }

  fflush(stdout);

  return 0;
}
int main(int argc, char *argv[]) {
  parseArguments(argc, argv);
  if (connectToServers()) {
    printf("Cannot connect to X");
    return 0;
  }
  getVisibleWindows();
  selectionSort(visibleWindowsArray, numVisibleWindows);
  getDesktopsInfo();
  labelWindows();
  printVisibleWindows();

  if (numVisibleWindows == 1) {
    destroyWindow();
    return 0;
  }
  gtk_init(&argc, &argv);
  gtkInitWindow();
  if(config.timeOut>0){
  gTimerQuit = g_timeout_add_seconds(config.timeOut, timeOutCallback, NULL);
  }
  gtk_widget_show_all(gtkWin);
  gtk_main();
  return 0;
}
void swap(windowInfo_t *xp, windowInfo_t *yp) {
  windowInfo_t temp = *xp;
  *xp = *yp;
  *yp = temp;
}
// Function to perform Selection Sort
void selectionSort(windowInfo_t input[], int n) {
  int i, j, min_idx;
  // One by one move boundary of unsorted subarray
  for (i = 0; i < n - 1; i++) {

    // Find the minimum element in unsorted array
    min_idx = i;
    for (j = i + 1; j < n; j++)
      if (input[j].desktop < input[min_idx].desktop)
        min_idx = j;
    // Swap the found minimum element
    // with the first element
    swap(&input[min_idx], &input[i]);
  }
}
void getDesktopsInfo() {
  numVisibleDesktops = 1;
  uint32_t Desk = visibleWindowsArray[0].desktop;
  for (uint32_t i = 1; i < numVisibleWindows; i++) {
    if (visibleWindowsArray[i].desktop != Desk) {
      numVisibleDesktops++;
      Desk = visibleWindowsArray[i].desktop;
    }
  }

  visibleDesktopsArray =
      (desktopInfo_t *)malloc(numVisibleDesktops * sizeof(desktopInfo_t));

  if (numVisibleDesktops == 1) {
    visibleDesktopsArray[0].indexWindow = 0;
    visibleDesktopsArray[0].desktopNum = Desk;
    visibleDesktopsArray[0].firstChar = labelString[0];
    visibleDesktopsArray[0].numWindows = numVisibleWindows;
  } else {

    uint32_t Desk = visibleWindowsArray[0].desktop;
    uint32_t arrayIndex = 0;
    uint32_t TotalWindows = 0;
    visibleDesktopsArray[0].indexWindow = 0;
    visibleDesktopsArray[0].firstChar = labelString[0];
    visibleDesktopsArray[0].desktopNum = Desk;
    for (uint32_t i = 1; i < numVisibleWindows; i++) {
      TotalWindows++;
      if (visibleWindowsArray[i].desktop != Desk) {
        visibleDesktopsArray[arrayIndex].numWindows = TotalWindows;
        Desk = visibleWindowsArray[i].desktop;
        arrayIndex++;
        visibleDesktopsArray[arrayIndex].firstChar = labelString[arrayIndex];
        visibleDesktopsArray[arrayIndex].indexWindow = i;
        visibleDesktopsArray[arrayIndex].desktopNum = Desk;
        TotalWindows = 0;
      }
    }
    visibleDesktopsArray[arrayIndex].numWindows = TotalWindows + 1;
    visibleDesktopsArray[arrayIndex].desktopNum = Desk;
    visibleDesktopsArray[arrayIndex].firstChar = labelString[arrayIndex];
  }
}
void printDesktopInfo() {
  printf("\nTotal Visible Desktops: %d", numVisibleDesktops);
  // Printt desktop info
  for (uint32_t i = 0; i < numVisibleDesktops; i++) {
    printf("\nDesk info: D: %d  WT:  %d  ind: %d Char %c", visibleDesktopsArray[i].desktopNum, visibleDesktopsArray[i].numWindows, visibleDesktopsArray[i].indexWindow, visibleDesktopsArray[i].firstChar);
  }
}
void labelWindows() {
  // This is a speciale case where we have only one desktop
  if (numVisibleDesktops == 1) {
    uint32_t charIndex = 0;
    uint32_t homeCharIndex = 0;
    uint32_t startingWindow = 0;
    uint32_t charWidth = 1;
    while (1) {
      for (uint32_t i = startingWindow; i < numVisibleWindows; i++) {
        visibleWindowsArray[i].as[charIndex] = labelString[homeCharIndex];
        visibleWindowsArray[i].as[charIndex + 1] = '\0';
        visibleWindowsArray[i].charWidth = charWidth;
        if (numCharInLabelString - 1 > homeCharIndex) {
          homeCharIndex++;
        }
      }
      if (visibleWindowsArray[visibleDesktopsArray[0].numWindows - 1].as[charIndex] == visibleWindowsArray[visibleDesktopsArray[0].numWindows - 2].as[charIndex]) {
        charIndex++;
        startingWindow += numCharInLabelString - 1;
        homeCharIndex = 0;
        charWidth++;
      } else
        break;
    }
    return;
  }

  if (numVisibleDesktops > 1) {
    // Fill with first character
    for (uint32_t k = 0; k < numVisibleDesktops; k++) {
      uint32_t startingWindow = visibleDesktopsArray[k].indexWindow;
      uint32_t endWindow = visibleDesktopsArray[k].numWindows + startingWindow;
      for (uint32_t i = startingWindow; i <= endWindow; i++) {
        visibleWindowsArray[i].as[0] = visibleDesktopsArray[k].firstChar;
        visibleWindowsArray[i].as[1] = '\0';
      }
    }
    for (uint32_t k = 0; k < numVisibleDesktops; k++) {
      uint32_t charIndex =
          1; // We already filled the first one durint dekstop init
      uint32_t homeCharIndex = 0;
      uint32_t startingWindow = visibleDesktopsArray[k].indexWindow;
      uint32_t endWindow = visibleDesktopsArray[k].numWindows + visibleDesktopsArray[k].indexWindow;
      uint32_t charWidth = 2;

      while (1) {
        for (uint32_t i = startingWindow; i <= endWindow; i++) {
          // Make sure we don't use the desktop character
          /* if (visibleDesktopsArray[k].firstChar == homeRow[homeCharIndex])
           */
          /*   homeCharIndex++; */
          visibleWindowsArray[i].as[charIndex] = labelString[homeCharIndex];
          visibleWindowsArray[i].as[charIndex + 1] = '\0';
          visibleWindowsArray[i].charWidth = charWidth;
          if (numCharInLabelString - 1 > homeCharIndex) {
            homeCharIndex++;
          }
        }
        if (visibleWindowsArray[endWindow].as[charIndex] == visibleWindowsArray[endWindow - 1].as[charIndex]) {
          charIndex++;
          homeCharIndex = 0;
          charWidth++;
          startingWindow += numCharInLabelString - 1;
        } else
          break;
      }
    }
  }
}

xcb_atom_t getatom(xcb_connection_t *c, char *atom_name) {
  xcb_intern_atom_cookie_t atom_cookie;
  xcb_atom_t atom;
  xcb_intern_atom_reply_t *rep;

  atom_cookie = xcb_intern_atom(c, 0, strlen(atom_name), atom_name);
  rep = xcb_intern_atom_reply(c, atom_cookie, NULL);
  if (NULL != rep) {
    atom = rep->atom;
    free(rep);
    printf("\natom: %x", atom);
    fflush(stdout);
    return atom;
  }
  printf("\nError getting atom.\n");
  exit(1);
}

void load_atoms(xcb_connection_t *c) {
  /* load atoms */
  xcb_intern_atom_cookie_t atom_cookies[SB_ATOM_MAX];
  for (int i = 0; i < SB_ATOM_MAX; i++) {
    atom_cookies[i] =
        xcb_intern_atom(c, 0, /* "atom created if it doesn't already exist"
                               */
                        SB_ATOM_STRING[i].len, SB_ATOM_STRING[i].name);
  }
  for (int i = 0; i < SB_ATOM_MAX; i++) {
    xcb_intern_atom_reply_t *reply =
        xcb_intern_atom_reply(c, atom_cookies[i], NULL);
    atoms[i] = reply->atom;
    //    printf("\nAtom:%d", atoms[i]);
    free(reply);
  }
}
