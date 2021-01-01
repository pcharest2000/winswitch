
# winswitch

## What is winswitch?

A work n progress!

A command line tool to switch the active window using the keyboard, displays  an overlay on dekstops to select a window, mostly used for tiling window managers.


![](https://github.com/pcharest2000/winswitch/raw/main/screenshots/screen.png)
![](https://github.com/pcharest2000/winswitch/raw/main/screenshots/sample.gif)

## How do I install winsswitch?

Requirements:

A Windows manager compositor (compiz and picom tested), I am working on fake transparency, but all this X stuff is new to me.

Build requirements:

lib X11-dev/
lib XCB-dev/
lib freetype2.0-dev/
lib gtk3.0-dev/

Clone the repo:/
git clone https://github.com/pcharest2000/winswitch.git/
cd winswitch/
make/

Copy winswitch somewhere in your path/
Bind a keyboard shortcut to winswitch (see your tiling window manger documentation) example for i3:/  
bindsym $mod+s  exec "${HOME}/.local/bin/switcher"/

## Where can I get help?


For support & all other kinds of questions open an issue ticket. 


#### Configuration

Excecute winswitch --help or -h:/

Usage: winswitch [OPTION]/
A command line tool to switch the active window using the keyboard, displays  an overlay\
on the visible dekstop(s) to select window, mostly used for tiling window managers \
and multi screen setup.Get help:   --help \
Actions:
  -t TIME     Set timeout period in seconds to quit, 0 for no timeout, default 10s.\
  -s SIZE     Set the font size in pixels, default 64 pixels\
  -f FILE     Set the ttf or otf font path name\
  -c COLOR    Set the font color format is hex color RRGGBBAA \
              the alpha value will be used to indicate selctcted characters \
  -C COLOR    Set the box color format is hex color RRGGBBAA \
              the alpha value will be used to indicate selctcted characters \
  -w ALPHA    Set the window alpha must be between 0.0 and 1.0, default 0.1  \
  -a          Ignore sticky windows, some applications (docks) do not  \
              set properly its window property \
  -S          A string of characters used to label the windows must be at least 2 character long, \
              and more than the number of active desktops, characters must be not repeated or behavior\
              behavior of the app is undefined\
example:\
winswitch -t 10 -C 3B3C35FF -c FDFFFFFF   -S fhdjsksla \
  -h or --help  for this listing \
Author, current maintainer: Philippe Charest <philippe.charest@gmail.com\
Released under the GNU General Public License.\
Copyright (C) 2020\
