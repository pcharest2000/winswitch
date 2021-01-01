/* license {{{ */
/*

winswitch
A command line tool to change the active windows with the keyboard, display  an overlay
on dekstops to switch window, ostly used for tiling window managers

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

void strToUpper(char *input) {
  // char *ori = input;
  for (int i = 0; i < strlen(input); i++) {
    //We transform lower to caps
    if (*input >= 97 && *input <= 122) {
      *input -= 32;
    }
    input++;
  }
  //input = ori;
}

gboolean labelMatch(char input) {
  uint32_t match = 0;
  //Convert lower to upper
  if (input >= 97 && input <= 122) {
    input -= 32;
  }
  // Is the character is not part of the string if not exit
  if (!strchr(config.labelString, input)) {
    return FALSE;
  }
  for (uint32_t i = 0; i < numVisibleWindows; i++) {
    if (visibleWindowsArray[i].noMatch)
      continue;
    //We transform lower to upper

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
  g_signal_connect(G_OBJECT(gtkWin), "draw", G_CALLBACK(drawCallback), NULL);
  g_signal_connect(gtkWin, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(gtkWin), "key_press_event", G_CALLBACK(keypressCallback), NULL);
  gtk_window_set_default_size(GTK_WINDOW(gtkWin), screen->width_in_pixels, screen->height_in_pixels);
  gtk_widget_show_all(gtkWin);
  gtk_window_move(GTK_WINDOW(gtkWin), 0, 0);
}

int configLabel(char *input) {
  if (strlen(input) > MAXLABELLENGTH || strlen(input) < 2) {
    fprintf(stderr, "\n Invalid string length for label must be between 2 and %d", MAXLABELLENGTH);
    return EXIT_FAILURE;
  }
  config.labelString[0] = '\0';
  strcat(config.labelString, input);
  strToUpper(config.labelString);
  numCharInLabelString = strlen(config.labelString);
  return 0;
}

int configTimeOut(char *input) {
  int timeOut;
  if (sscanf(input, "%d", &timeOut) == EOF || timeOut < 0) {
    fprintf(stderr, "\n Invalid option must be 0 or greater");
    return EXIT_FAILURE;
  }
  config.timeOut = timeOut;

  return 0;
}

int configWindowAlpha(char *input) {
  float alpha;
  if (sscanf(input, "%f", &alpha) == EOF || alpha > 1.0 || alpha < 0.0) {
    fprintf(stderr, "\n Invalid input for font alpha value must be float between 0.0 and 1.0");
    return EXIT_FAILURE;
  }
  config.winAlpha = alpha;
  return 0;
}

int configFontSize(char *input) {
  float size;
  if (sscanf(input, "%f", &size) == EOF || size < 1.0) {
    fprintf(stderr, "\n Invalid input for font size");
    return EXIT_FAILURE;
  }
  config.fontSize = size;
  config.rect.radius = config.fontSize * 0.15;
  return 0;
}

int configFontPath(char *input) {
  free(config.cff);
  FT_Library value;
  FT_Error status;
  FT_Face face;
  status = FT_Init_FreeType(&value);
  status = FT_New_Face(value, input, 0, &face);
  if (status != 0) {
    fprintf(stderr, "Error %d opening %s.\n", status, input);
  }
  config.cff = cairo_ft_font_face_create_for_ft_face(face, 0);
  return 0;
}
int configBoxColor(char *input) {
  //VddFunction takes hexadeciin
  //male
  if (strlen(input) != 8) {
    printf("\n Invalid string size for color, must be RGBA hexadecimal!");
    exit(EXIT_FAILURE);
  }
  int r, g, b, a;
  sscanf(input, "%02x%02x%02x%02x", &r, &g, &b, &a);
  config.rect.color.r = r / 255.0;
  config.rect.color.g = g / 255.0;
  config.rect.color.b = b / 255.0;
  config.rect.color.alpha = a / 255.0;
  return 0;
}
int configFontColor(char *input) {
  //VddFunction takes hexadeciin
  //male
  if (strlen(input) != 8) {
    printf("\n Invalid size for color, must be RGB hexadecimal!");
    return EXIT_FAILURE;
  }
  int r, g, b, a;
  //printf("%s",input);
  sscanf(input, "%02x%02x%02x%02x", &r, &g, &b, &a);
  config.fontColor.r = r / 255.0;
  config.fontColor.g = g / 255.0;
  config.fontColor.b = b / 255.0;
  config.fontColor.alpha = a / 255.0;
  return 0;
  /* printf("\n R: %f G: %f B: %f", config.fontR, config.fontG, config.fontB); */
}

void draw_rounded_path(cairo_t *ctx, double x, double y, double width, double height, double radius) {
  double degrees = M_PI / 180.0;
  cairo_set_source_rgba(ctx, config.rect.color.r, config.rect.color.g, config.rect.color.b, config.rect.color.alpha);
  cairo_new_path(ctx);
  cairo_arc(ctx, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
  cairo_arc(ctx, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
  cairo_arc(ctx, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
  cairo_arc(ctx, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
  cairo_close_path(ctx);
  cairo_fill(ctx);
}

static void do_drawing(cairo_t *cr) {
  gtk_window_move(GTK_WINDOW(gtkWin), 0, 0);
  // We calcu;ate the font extent only once and fill the info
  //cairo_select_font_face(cr, config.font, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  if (!posiInitalized) {
    //if font face is null revert to cairo internal
    if (config.cff == NULL)
      cairo_select_font_face(cr, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    else
      cairo_set_font_face(cr, config.cff);
    cairo_set_font_size(cr, config.fontSize);
    posiInitalized = TRUE;
    cairo_text_extents_t te; // tmp extent
    //:wchar tmp[] = " ";
    for (uint32_t i = 0; i < numVisibleWindows; i++) {
      visibleWindowsArray[i].numCharMatch = 0;
      cairo_text_extents(cr, visibleWindowsArray[i].as, &te);
      visibleWindowsArray[i].fontPosX = visibleWindowsArray[i].x + visibleWindowsArray[i].width / 2 - (te.width / 2 + te.x_bearing);
      visibleWindowsArray[i].fontPosY = visibleWindowsArray[i].y + visibleWindowsArray[i].height / 2 - (te.height / 2 + te.y_bearing);
      visibleWindowsArray[i].rbackRect.x = visibleWindowsArray[i].fontPosX + te.x_bearing - 0.4 * te.width / 2;
      visibleWindowsArray[i].rbackRect.y = visibleWindowsArray[i].fontPosY + te.y_bearing - 0.4 * te.height / 2;
      visibleWindowsArray[i].rbackRect.width = te.width * 1.4;
      visibleWindowsArray[i].rbackRect.heigth = te.height * 1.4;
    }
  }
  //Do the real drawing
  // Clear the screen
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_set_source_rgba(cr, 0, 0.0, 0, config.winAlpha);
  cairo_paint(cr);
  cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

  for (uint32_t i = 0; i < numVisibleWindows; i++) {
    if (visibleWindowsArray[i].noMatch) {
      continue;
    }
    char temp[] = " ";
    cairo_set_font_size(cr, config.fontSize);
    cairo_set_font_face(cr, config.cff);
    draw_rounded_path(cr, visibleWindowsArray[i].rbackRect.x, visibleWindowsArray[i].rbackRect.y, visibleWindowsArray[i].rbackRect.width, visibleWindowsArray[i].rbackRect.heigth, config.rect.radius);
    cairo_move_to(cr, visibleWindowsArray[i].fontPosX, visibleWindowsArray[i].fontPosY);
    for (int32_t j = 0; j < visibleWindowsArray[i].charWidth; j++) {
      temp[0] = visibleWindowsArray[i].as[j];
      if (j + 1 <= visibleWindowsArray[i].numCharMatch)
        cairo_set_source_rgba(cr, config.fontColor.r, config.fontColor.g, config.fontColor.g, config.fontColor.alpha);
      else
        cairo_set_source_rgb(cr, config.fontColor.r, config.fontColor.g, config.fontColor.b);
      cairo_show_text(cr, temp);
    }
  }
  cairo_stroke(cr);
}

static gboolean drawCallback(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
  do_drawing(cr);
  return FALSE;
}
void getWindowGeometrie(xcb_window_t drawable) {
  xcb_translate_coordinates_cookie_t tc = xcb_translate_coordinates(xcb_con, screen->root, drawable, 0, 0);
  xcb_translate_coordinates_reply_t *tr = xcb_translate_coordinates_reply(xcb_con, tc, NULL);
  printf("\n Trans coo x %d y %d", tr->dst_x, tr->dst_y);
}
void printDesktop_geometrie() {
  xcb_get_property_cookie_t co;
  co = xcb_ewmh_get_desktop_viewport(ewmh_con, 0);
  xcb_ewmh_get_desktop_viewport_reply_t reply;
  xcb_ewmh_get_desktop_viewport_reply(ewmh_con, co, &reply, NULL);
  for (uint32_t i = 0; i < reply.desktop_viewport_len; i++) {
    printf("\nDesktop x  %d, y %d ", reply.desktop_viewport[i].x, reply.desktop_viewport[i].y);
  }
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

      /*Get windows Destopps*/
      int32_t desk;
      xcb_ewmh_get_wm_desktop_reply(ewmh_con, xcb_ewmh_get_wm_desktop(ewmh_con, windowsReply.windows[i]), &desk, NULL);
      visibleWindowsArray[numVisibleWindows].desktop = desk;
      if (desk == -1 && config.ignoreSticky == TRUE) {
        continue;
      }
      visibleWindowsArray[numVisibleWindows].winId = windowsReply.windows[i];
      /*Get window Geometry*/
      xcb_get_geometry_cookie_t gc = xcb_get_geometry(xcb_con, windowsReply.windows[i]);
      xcb_get_geometry_reply_t *gr = xcb_get_geometry_reply(xcb_con, gc, NULL);

      xcb_translate_coordinates_cookie_t tc = xcb_translate_coordinates(xcb_con, screen->root, windowsReply.windows[i],0, 0);
      xcb_translate_coordinates_reply_t *tr = xcb_translate_coordinates_reply(xcb_con, tc, NULL);
      visibleWindowsArray[numVisibleWindows].width = gr->width;
      visibleWindowsArray[numVisibleWindows].height = gr->height;
      visibleWindowsArray[numVisibleWindows].x = 0-tr->dst_x; //I don't care abut window borders for now
      visibleWindowsArray[numVisibleWindows].y = 0-tr->dst_y;
      visibleWindowsArray[numVisibleWindows].noMatch = FALSE;
      numVisibleWindows++;
      free(tr);
      free(gr);
    }
  }
  xcb_ewmh_get_windows_reply_wipe(&windowsReply);
}

int connectToServers() {
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
  usleep(500);
  gtk_widget_destroy(gtkWin);
  gtk_main_quit();
}

gboolean keypressCallback(GtkWidget *widget, GdkEventKey *event,
                          gpointer data) {
  // Reset the timer to quit application
  if (config.timeOut > 0) {
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

  // If we have a keyboard match redraw
  if (labelMatch(event->keyval)) {
    gtk_widget_queue_draw(gtkWin);
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
  if (argc == 1) {
    return 1;
  }
  /* make "--help" and "--version" work. I don't want to use
   * getopt_long for portability reasons */
  if (argc == 2 && argv[1]) {
    if (strcmp(argv[1], "--help") == 0) {
      fputs(help, stdout);
      exit(EXIT_SUCCESS);
    } else if (strcmp(argv[1], "-h") == 0) {
      fputs(help, stdout);
      exit(EXIT_SUCCESS);
    } else if (strcmp(argv[1], "--version") == 0) {
      puts(VERSION);
      exit(EXIT_SUCCESS);
    }
  }

  int opt;
  // put ':' in the starting of the
  // string so that program can
  //distinguish between '?' and ':'
  while ((opt = getopt(argc, argv, "ic:C:s:a:t:w:S:f:")) != -1) {
    switch (opt) {
    case 'f':
      configFontPath(optarg);
      break;
    case 'C':
      configBoxColor(optarg);
      break;
    case 'c':
      configFontColor(optarg);
      break;
    case 's':
      configFontSize(optarg);
      break;
    case 'w':
      configWindowAlpha(optarg);
      break;
    case 'S':
      configLabel(optarg);
      break;
    case 't':
      configTimeOut(optarg);
      break;
    case 'i':
      config.ignoreSticky = TRUE;
      break;
    case 'h':
      fputs(help, stdout);
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

  return 1;
}

int main(int argc, char *argv[]) {
  strToUpper(config.labelString);
  loadFont();
  parseArguments(argc, argv);
  /* if (parseArguments(argc, argv) == EXIT_SUCCESS) { */
  /*   return 0; */
  /* } */
  if (connectToServers()) {
    printf("Cannot connect to X");
    exit(EXIT_FAILURE);
  }
  getVisibleWindows();
  selectionSort(visibleWindowsArray, numVisibleWindows);
  getDesktopsInfo();
  labelWindows();
  printVisibleWindows();

  if (numVisibleWindows <= 1) {
    destroyWindow();
    return 0;
  }
  gtk_init(&argc, &argv);
  gtkInitWindow();
  if (config.timeOut > 0) {
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

  visibleDesktopsArray = (desktopInfo_t *)malloc(numVisibleDesktops * sizeof(desktopInfo_t));

  if (numVisibleDesktops == 1) {
    visibleDesktopsArray[0].indexWindow = 0;
    visibleDesktopsArray[0].desktopNum = Desk;
    visibleDesktopsArray[0].firstChar = config.labelString[0];
    visibleDesktopsArray[0].numWindows = numVisibleWindows;
  } else {

    uint32_t Desk = visibleWindowsArray[0].desktop;
    uint32_t arrayIndex = 0;
    uint32_t TotalWindows = 0;
    visibleDesktopsArray[0].indexWindow = 0;
    visibleDesktopsArray[0].firstChar = config.labelString[0];
    visibleDesktopsArray[0].desktopNum = Desk;
    for (uint32_t i = 1; i < numVisibleWindows; i++) {
      TotalWindows++;
      if (visibleWindowsArray[i].desktop != Desk) {
        visibleDesktopsArray[arrayIndex].numWindows = TotalWindows;
        Desk = visibleWindowsArray[i].desktop;
        arrayIndex++;
        visibleDesktopsArray[arrayIndex].firstChar = config.labelString[arrayIndex];
        visibleDesktopsArray[arrayIndex].indexWindow = i;
        visibleDesktopsArray[arrayIndex].desktopNum = Desk;
        TotalWindows = 0;
      }
    }
    visibleDesktopsArray[arrayIndex].numWindows = TotalWindows + 1;
    visibleDesktopsArray[arrayIndex].desktopNum = Desk;
    visibleDesktopsArray[arrayIndex].firstChar = config.labelString[arrayIndex];
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
        visibleWindowsArray[i].as[charIndex] = config.labelString[homeCharIndex];
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
          visibleWindowsArray[i].as[charIndex] = config.labelString[homeCharIndex];
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
        xcb_intern_atom(c, 0,
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
#include <cairo-ft.h>
#include <cairo.h>
#include <ft2build.h>
#include <stdio.h>
#include <stdlib.h>
#include FT_SFNT_NAMES_H
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BBOX_H
#include FT_TYPE1_TABLES_H

void loadFont() {

  FT_Library value;
  FT_Error status;
  FT_Face face;

  status = FT_Init_FreeType(&value);
  if (status != 0) {
    fprintf(stderr, "Error %d opening library.\n", status);
    exit(EXIT_FAILURE);
  }
  status = FT_New_Face(value, config.fontPath, 0, &face);
  if (status != 0) {
    fprintf(stderr, "Error %d opening %s.\n", status, config.fontPath);
  }
  config.cff = cairo_ft_font_face_create_for_ft_face(face, 0);
}
