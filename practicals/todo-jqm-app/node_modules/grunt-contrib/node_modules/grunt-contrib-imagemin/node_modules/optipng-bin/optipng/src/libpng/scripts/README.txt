
Makefiles for  libpng version 1.4.12 - July 10, 2012

 makefile.linux    =>  Linux/ELF makefile
                       (gcc, creates libpng14.so.14.1.4.12)
 makefile.gcc      =>  Generic makefile (gcc, creates static libpng.a)
 makefile.knr      =>  Archaic UNIX Makefile that converts files with
                       ansi2knr (Requires ansi2knr.c from
                       ftp://ftp.cs.wisc.edu/ghost)
 makefile.acorn    =>  Acorn makefile
 makefile.aix      =>  AIX/gcc makefile
 makefile.amiga    =>  Amiga makefile
 makefile.atari    =>  Atari makefile
 makefile.bc32     =>  32-bit Borland C++ (all modules compiled in C mode)
 makefile.beos     =>  beos makefile
 makefile.bor      =>  Borland makefile (uses bcc)
 makefile.cegcc    =>  minge32ce for Windows CE makefile
 makefile.cygwin   =>  Cygwin/gcc makefile
 makefile.darwin   =>  Darwin makefile, can use on MacosX
 makefile.dec      =>  DEC Alpha UNIX makefile
 makefile.dj2      =>  DJGPP 2 makefile
 makefile.elf      =>  Linux/ELF makefile symbol versioning,
                       (gcc, creates libpng14.so.14.1.4.12)
 makefile.freebsd  =>  FreeBSD makefile
 makefile.gcc      =>  Generic gcc makefile
 makefile.hpgcc    =>  HPUX makefile using gcc
 makefile.hpux     =>  HPUX (10.20 and 11.00) makefile
 makefile.hp64     =>  HPUX (10.20 and 11.00) makefile, 64-bit
 makefile.ibmc     =>  IBM C/C++ version 3.x for Win32 and OS/2 (static)
 makefile.intel    =>  Intel C/C++ version 4.0 and later
 makefile.mingw    =>  Mingw makefile
 makefile.mips     =>  MIPS makefile
 makefile.msc      =>  Microsoft C makefile
 makefile.netbsd   =>  NetBSD/cc makefile, makes libpng.so.
 makefile.openbsd  =>  OpenBSD makefile
 makefile.os2      =>  OS/2 Makefile (gcc and emx, requires pngos2.def)
 makefile.sco      =>  For SCO OSr5  ELF and Unixware 7 with Native cc
 makefile.sggcc    =>  Silicon Graphics (gcc,
                       creates libpng14.so.14.1.4.12)
 makefile.sgi      =>  Silicon Graphics IRIX makefile (cc, creates static lib)
 makefile.solaris  =>  Solaris 2.X makefile (gcc,
                       creates libpng14.so.14.1.4.12)
 makefile.so9      =>  Solaris 9 makefile (gcc,
                       creates libpng14.so.14.1.4.12)
 makefile.std      =>  Generic UNIX makefile (cc, creates static libpng.a)
 makefile.sunos    =>  Sun makefile
 makefile.32sunu   =>  Sun Ultra 32-bit makefile
 makefile.64sunu   =>  Sun Ultra 64-bit makefile
 makefile.tc3      =>  Turbo C 3.0 makefile
 makefile.vcwin32  =>  makefile for Microsoft Visual C++ 4.0 and later
 makefile.watcom   =>  Watcom 10a+ Makefile, 32-bit flat memory model
 makevms.com       =>  VMS build script
 smakefile.ppc     =>  AMIGA smakefile for SAS C V6.58/7.00 PPC compiler
                       (Requires SCOPTIONS, copied from scripts/SCOPTIONS.ppc)

Other supporting scripts:
 descrip.mms       =>  VMS makefile for MMS or MMK
 libpng-config-body.in => used by several makefiles to create libpng-config
 libpng-config-head.in => used by several makefiles to create libpng-config
 libpng.pc.in      =>  Used by several makefiles to create libpng.pc
 pngos2.def        =>  OS/2 module definition file used by makefile.os2
 pngwin.def        =>  Module definitions for makefile.cygwin and mingw
 png32ce.def       =>  Module definition file used by makefile.cegcc
 pngwin.rc         =>  Used by the visualc6 and visualc71 projects.
 SCOPTIONS.ppc     =>  Used with smakefile.ppc

Further information can be found in comments in the individual makefiles.
