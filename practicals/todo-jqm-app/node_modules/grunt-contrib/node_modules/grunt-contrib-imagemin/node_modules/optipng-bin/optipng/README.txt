
OptiPNG 0.7.4: Advanced PNG optimizer.
Copyright (C) 2001-2012 Cosmin Truta.
======================================

License
-------
  Open-Source (the zlib license)
  See the accompanying LICENSE.txt

Resources
---------
  Home page:
    http://optipng.sourceforge.net/
  Download:
    http://sourceforge.net/project/showfiles.php?group_id=151404
  Announcements:
    https://sourceforge.net/news/?group_id=151404
  Support:
    http://sourceforge.net/tracker/?group_id=151404
    ctruta (at) gmail (dot) com

Build instructions
------------------
  On Unix, or under a Bourne-compatible shell, run ./configure and make:
        cd optipng-0.7.4/
        ./configure
        make
        make test

  Alternatively, use a pre-configured makefile that matches your compiler;
  e.g.:
        cd optipng-0.7.4/
        nmake -f build/visualc.mk
        nmake -f build/visualc.mk test

Installation instructions
-------------------------
  Build the program according to the instructions above.

  On Unix:
  - Make the "install" target:
        sudo make install
  - To uninstall, make the "uninstall" target:
        sudo make uninstall

  On Windows:
  - Copy optipng.exe to a directory found in PATH.

