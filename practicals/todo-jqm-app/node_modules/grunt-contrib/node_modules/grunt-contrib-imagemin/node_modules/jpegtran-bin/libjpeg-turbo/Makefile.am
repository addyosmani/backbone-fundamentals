lib_LTLIBRARIES = libjpeg.la
libjpeg_la_LDFLAGS = -version-info ${LIBTOOL_CURRENT}:${SO_MINOR_VERSION}:${SO_AGE} -no-undefined
include_HEADERS = jerror.h jmorecfg.h jpeglib.h

if WITH_TURBOJPEG
lib_LTLIBRARIES += libturbojpeg.la
libturbojpeg_la_LDFLAGS = -version-info 0:0 -no-undefined
include_HEADERS += turbojpeg.h
endif

nodist_include_HEADERS = jconfig.h


HDRS = jchuff.h jdct.h jdhuff.h jerror.h jinclude.h jmemsys.h jmorecfg.h \
	jpegint.h jpeglib.h jversion.h jsimd.h jsimddct.h jpegcomp.h

libjpeg_la_SOURCES = $(HDRS) jcapimin.c jcapistd.c jccoefct.c jccolor.c \
	jcdctmgr.c jchuff.c jcinit.c jcmainct.c jcmarker.c jcmaster.c \
	jcomapi.c jcparam.c jcphuff.c jcprepct.c jcsample.c jctrans.c \
	jdapimin.c jdapistd.c jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c \
	jddctmgr.c jdhuff.c jdinput.c jdmainct.c jdmarker.c jdmaster.c \
	jdmerge.c jdphuff.c jdpostct.c jdsample.c jdtrans.c jerror.c \
	jfdctflt.c jfdctfst.c jfdctint.c jidctflt.c jidctfst.c jidctint.c \
	jidctred.c jquant1.c jquant2.c jutils.c jmemmgr.c jmemnobs.c

if WITH_ARITH
libjpeg_la_SOURCES += jaricom.c
endif

if WITH_ARITH_ENC
libjpeg_la_SOURCES += jcarith.c
endif

if WITH_ARITH_DEC
libjpeg_la_SOURCES += jdarith.c
endif


SUBDIRS = java


if WITH_TURBOJPEG

libturbojpeg_la_SOURCES = $(libjpeg_la_SOURCES) turbojpeg.c turbojpeg.h \
	transupp.c transupp.h jdatadst-tj.c jdatasrc-tj.c

if WITH_JAVA

libturbojpeg_la_SOURCES += turbojpeg-jni.c
libturbojpeg_la_CFLAGS = ${JNI_CFLAGS}
TJMAPFILE = turbojpeg-mapfile.jni

else

TJMAPFILE = turbojpeg-mapfile

endif

libturbojpeg_la_SOURCES += $(TJMAPFILE)

if VERSION_SCRIPT
libturbojpeg_la_LDFLAGS += $(VERSION_SCRIPT_FLAG)$(srcdir)/$(TJMAPFILE)
endif

endif


if VERSION_SCRIPT
libjpeg_la_LDFLAGS += $(VERSION_SCRIPT_FLAG)libjpeg.map
endif


if WITH_SIMD

SUBDIRS += simd
libjpeg_la_LIBADD = simd/libsimd.la
libturbojpeg_la_LIBADD = simd/libsimd.la

else

libjpeg_la_SOURCES += jsimd_none.c

endif


bin_PROGRAMS = cjpeg djpeg jpegtran rdjpgcom wrjpgcom
noinst_PROGRAMS = jcstest


if WITH_TURBOJPEG

bin_PROGRAMS += tjbench

noinst_PROGRAMS += tjunittest

tjbench_SOURCES = tjbench.c bmp.h bmp.c tjutil.h tjutil.c rdbmp.c rdppm.c \
	wrbmp.c wrppm.c

tjbench_LDADD = libturbojpeg.la libjpeg.la -lm

tjbench_CFLAGS = -DBMP_SUPPORTED -DPPM_SUPPORTED

tjunittest_SOURCES = tjunittest.c tjutil.h tjutil.c

tjunittest_LDADD = libturbojpeg.la

endif


cjpeg_SOURCES = cdjpeg.h cderror.h cdjpeg.c cjpeg.c rdbmp.c rdgif.c \
	rdppm.c rdswitch.c rdtarga.c 

cjpeg_LDADD = libjpeg.la

cjpeg_CFLAGS = -DBMP_SUPPORTED -DGIF_SUPPORTED -DPPM_SUPPORTED \
	-DTARGA_SUPPORTED

djpeg_SOURCES = cdjpeg.h cderror.h cdjpeg.c djpeg.c rdcolmap.c rdswitch.c \
	wrbmp.c wrgif.c wrppm.c wrtarga.c

djpeg_LDADD = libjpeg.la

djpeg_CFLAGS = -DBMP_SUPPORTED -DGIF_SUPPORTED -DPPM_SUPPORTED \
	-DTARGA_SUPPORTED

jpegtran_SOURCES = jpegtran.c rdswitch.c cdjpeg.c transupp.c transupp.h

jpegtran_LDADD = libjpeg.la

rdjpgcom_SOURCES = rdjpgcom.c

rdjpgcom_LDADD = libjpeg.la

wrjpgcom_SOURCES = wrjpgcom.c

wrjpgcom_LDADD = libjpeg.la

jcstest_SOURCES = jcstest.c

jcstest_LDADD = libjpeg.la

dist_man1_MANS = cjpeg.1 djpeg.1 jpegtran.1 rdjpgcom.1 wrjpgcom.1

DOCS= coderules.txt jconfig.txt change.log rdrle.c wrrle.c BUILDING.txt \
	ChangeLog.txt

docdir = $(datadir)/doc
dist_doc_DATA = README README-turbo.txt libjpeg.txt structure.txt usage.txt \
	wizard.txt 

exampledir = $(datadir)/doc
dist_example_DATA = example.c


EXTRA_DIST = win release $(DOCS) testimages md5cmp CMakeLists.txt \
	sharedlib/CMakeLists.txt cmakescripts libjpeg.map.in doc doxygen.config \
	jccolext.c jdcolext.c jdmrgext.c

dist-hook:
	rm -rf `find $(distdir) -name .svn`


MD5_JPEG_INT = 9a68f56bc76e466aa7e52f415d0f4a5f
MD5_JPEG_FAST = 0e1502e7fa421835e376a314fac2a39f
MD5_JPEG_FAST_100 = 7bf72a8e741d64eecb960c97323af77c
MD5_JPEG_FLOAT = d1623885ffafcd40c684af09e3d65cd5
MD5_JPEG_FLOAT_NOSIMD = fb4884c35f8273f498cb32879de5c455
MD5_JPEG_INT_GRAY = 72b51f894b8f4a10b3ee3066770aa38d
MD5_PPM_INT = d1ed0d11f076b842525271647716aeb8
MD5_PPM_FAST = 048298a2d2410261c0533cb97bcfef23
MD5_PPM_FLOAT = 7f5b446ee36b2630e06785b8d42af15f
MD5_PPM_FLOAT_NOSIMD = 64072f1dbdc5b3a187777788604971a5
MD5_PPM_INT_2_1 = 9f9de8c0612f8d06869b960b05abf9c9
MD5_PPM_INT_15_8 = b6875bc070720b899566cc06459b63b7
MD5_PPM_INT_7_4 = 06a177eae05f164fac57f7a2c346ee87
MD5_PPM_INT_13_8 = bc3452573c8152f6ae552939ee19f82f
MD5_PPM_INT_3_2 = f5a8b88a8a7f96016f04d259cf82ed67
MD5_PPM_INT_11_8 = d8cc73c0aaacd4556569b59437ba00a5
MD5_PPM_INT_5_4 = 32775dd9ad2ab90f4c5b219b53e0c86c
MD5_PPM_INT_9_8 = d25e61bc7eac0002f5b393aa223747b6
MD5_PPM_INT_7_8 = ddb564b7c74a09494016d6cd7502a946
MD5_PPM_INT_3_4 = 8ed8e68808c3fbc4ea764fc9d2968646
MD5_PPM_INT_5_8 = a3363274999da2366a024efae6d16c9b
MD5_PPM_INT_1_2 = e692a315cea26b988c8e8b29a5dbcd81
MD5_PPM_INT_3_8 = 79eca9175652ced755155c90e785a996
MD5_PPM_INT_1_4 = 79cd778f8bf1a117690052cacdd54eca
MD5_PPM_INT_1_8 = 391b3d4aca640c8567d6f8745eb2142f
MD5_PPM_FAST_1_2 = f30bcf6d32ccd44cbdd9aeaacbd9454f
MD5_BMP_256 = 4980185e3776e89bd931736e1cddeee6
MD5_JPEG_ARI = e986fb0a637a8d833d96e8a6d6d84ea1
MD5_PPM_ARI = 72b59a99bcf1de24c5b27d151bde2437
MD5_JPEG_PROG = 1c4afddc05c0a43489ee54438a482d92
MD5_JPEG_CROP = b4197f377e621c4e9b1d20471432610d

test: testclean all
if WITH_TURBOJPEG
if WITH_JAVA
	$(JAVA) -cp java/turbojpeg.jar -Djava.library.path=.libs TJUnitTest
	$(JAVA) -cp java/turbojpeg.jar -Djava.library.path=.libs TJUnitTest -bi
	$(JAVA) -cp java/turbojpeg.jar -Djava.library.path=.libs TJUnitTest -yuv
	$(JAVA) -cp java/turbojpeg.jar -Djava.library.path=.libs TJUnitTest -yuv -bi
endif
	./tjunittest
	./tjunittest -alloc
	./tjunittest -yuv
endif
	./cjpeg -dct int -outfile testoutint.jpg $(srcdir)/testimages/testorig.ppm
	sh $(srcdir)/md5cmp $(MD5_JPEG_INT) testoutint.jpg
	./cjpeg -dct fast -opt -outfile testoutfst.jpg $(srcdir)/testimages/testorig.ppm
	sh $(srcdir)/md5cmp $(MD5_JPEG_FAST) testoutfst.jpg
	./cjpeg -dct fast -quality 100 -opt -outfile testoutfst100.jpg $(srcdir)/testimages/testorig.ppm
	sh $(srcdir)/md5cmp $(MD5_JPEG_FAST_100) testoutfst100.jpg
	./cjpeg -dct float -outfile testoutflt.jpg $(srcdir)/testimages/testorig.ppm
if WITH_SSE_FLOAT_DCT
	sh $(srcdir)/md5cmp $(MD5_JPEG_FLOAT) testoutflt.jpg
else
	sh $(srcdir)/md5cmp $(MD5_JPEG_FLOAT_NOSIMD) testoutflt.jpg
endif
	./cjpeg -dct int -grayscale -outfile testoutgray.jpg $(srcdir)/testimages/testorig.ppm
	sh $(srcdir)/md5cmp $(MD5_JPEG_INT_GRAY) testoutgray.jpg
	./djpeg -dct int -fast -ppm -outfile testoutint.ppm $(srcdir)/testimages/testorig.jpg
	sh $(srcdir)/md5cmp $(MD5_PPM_INT) testoutint.ppm
	./djpeg -dct fast -ppm -outfile testoutfst.ppm $(srcdir)/testimages/testorig.jpg
	sh $(srcdir)/md5cmp $(MD5_PPM_FAST) testoutfst.ppm
	./djpeg -dct float -ppm -outfile testoutflt.ppm $(srcdir)/testimages/testorig.jpg
if WITH_SSE_FLOAT_DCT
	sh $(srcdir)/md5cmp $(MD5_PPM_FLOAT) testoutflt.ppm
else
	sh $(srcdir)/md5cmp $(MD5_PPM_FLOAT_NOSIMD) testoutflt.ppm
endif
	MD5[21]=$(MD5_PPM_INT_2_1); \
	MD5[158]=$(MD5_PPM_INT_15_8); \
	MD5[74]=$(MD5_PPM_INT_7_4); \
	MD5[138]=$(MD5_PPM_INT_13_8); \
	MD5[32]=$(MD5_PPM_INT_3_2); \
	MD5[118]=$(MD5_PPM_INT_11_8); \
	MD5[54]=$(MD5_PPM_INT_5_4); \
	MD5[98]=$(MD5_PPM_INT_9_8); \
	MD5[78]=$(MD5_PPM_INT_7_8); \
	MD5[34]=$(MD5_PPM_INT_3_4); \
	MD5[58]=$(MD5_PPM_INT_5_8); \
	MD5[12]=$(MD5_PPM_INT_1_2); \
	MD5[38]=$(MD5_PPM_INT_3_8); \
	MD5[14]=$(MD5_PPM_INT_1_4); \
	MD5[18]=$(MD5_PPM_INT_1_8); \
	for scale in 2_1 15_8 7_4 13_8 3_2 11_8 5_4 9_8 7_8 3_4 5_8 1_2 3_8 1_4 1_8; do \
		scalearg=`echo $$scale | sed s@_@/@g`; \
		arrayindex=`echo $$scale | sed s@_@@g`; \
		./djpeg -dct int -nosmooth -scale $$scalearg -ppm -outfile testoutint$$scale.ppm $(srcdir)/testimages/testorig.jpg; \
		sh $(srcdir)/md5cmp $${MD5[$$arrayindex]} testoutint$$scale.ppm; \
	done
	./djpeg -dct fast -scale 1/2 -ppm -outfile testoutfst1_2.ppm $(srcdir)/testimages/testorig.jpg
	sh $(srcdir)/md5cmp $(MD5_PPM_FAST_1_2) testoutfst1_2.ppm
	./djpeg -dct int -bmp -colors 256 -outfile testout.bmp $(srcdir)/testimages/testorig.jpg
	sh $(srcdir)/md5cmp $(MD5_BMP_256) testout.bmp
if WITH_ARITH_ENC
	./cjpeg -dct int -arithmetic -outfile testoutari.jpg $(srcdir)/testimages/testorig.ppm
	sh $(srcdir)/md5cmp $(MD5_JPEG_ARI) testoutari.jpg
	./jpegtran -arithmetic -outfile testouta.jpg $(srcdir)/testimages/testimgint.jpg
	sh $(srcdir)/md5cmp $(MD5_JPEG_ARI) testouta.jpg
endif
if WITH_ARITH_DEC
	./djpeg -dct int -fast -ppm -outfile testoutari.ppm $(srcdir)/testimages/testimgari.jpg
	sh $(srcdir)/md5cmp $(MD5_PPM_ARI) testoutari.ppm
	./jpegtran -outfile testouta.jpg $(srcdir)/testimages/testimgari.jpg
	sh $(srcdir)/md5cmp $(MD5_JPEG_INT) testouta.jpg
endif
	./cjpeg -dct int -progressive -outfile testoutp.jpg $(srcdir)/testimages/testorig.ppm
	sh $(srcdir)/md5cmp $(MD5_JPEG_PROG) testoutp.jpg
	./jpegtran -outfile testoutt.jpg testoutp.jpg
	sh $(srcdir)/md5cmp $(MD5_JPEG_INT) testoutt.jpg
	./jpegtran -crop 120x90+20+50 -transpose -perfect -outfile testoutcrop.jpg $(srcdir)/testimages/testorig.jpg
	sh $(srcdir)/md5cmp $(MD5_JPEG_CROP) testoutcrop.jpg


testclean:
	rm -f testout*
	rm -f *_GRAY_*.bmp
	rm -f *_GRAY_*.png
	rm -f *_GRAY_*.ppm
	rm -f *_GRAY_*.jpg
	rm -f *_GRAY.yuv
	rm -f *_420_*.bmp
	rm -f *_420_*.png
	rm -f *_420_*.ppm
	rm -f *_420_*.jpg
	rm -f *_420.yuv
	rm -f *_422_*.bmp
	rm -f *_422_*.png
	rm -f *_422_*.ppm
	rm -f *_422_*.jpg
	rm -f *_422.yuv
	rm -f *_444_*.bmp
	rm -f *_444_*.png
	rm -f *_444_*.ppm
	rm -f *_444_*.jpg
	rm -f *_444.yuv
	rm -f *_440_*.bmp
	rm -f *_440_*.png
	rm -f *_440_*.ppm
	rm -f *_440_*.jpg
	rm -f *_440.yuv


tjtest:
	sh ./tjbenchtest
if WITH_JAVA
	sh ./tjbenchtest.java
endif


if X86_64

install-exec-hook:
	__PREFIX=`echo ${prefix} | sed -e 's@\/*$$@@'`;  \
	if [ "$$__PREFIX" = "/opt/libjpeg-turbo" ]; then  \
		cd $(DESTDIR)/${prefix};  \
		if [ -d lib -a ! -d lib64 -a ! -h lib64 ]; then  \
			$(LN_S) lib lib64;  \
		fi  \
	fi

else

install-exec-hook:
	__PREFIX=`echo ${prefix} | sed -e 's@\/*$$@@'`;  \
	if [ "$$__PREFIX" = "/opt/libjpeg-turbo" ]; then  \
		cd $(DESTDIR)/${prefix};  \
		if [ -d lib -a ! -d lib32 -a ! -h lib32 ]; then  \
			$(LN_S) lib lib32;  \
		fi  \
	fi

endif

rpm: all
	TMPDIR=`mktemp -d /tmp/${PACKAGE_NAME}-build.XXXXXX`; \
	mkdir -p $$TMPDIR/RPMS; \
	ln -fs `pwd` $$TMPDIR/BUILD; \
	rm -f ${PACKAGE_NAME}-${VERSION}.${RPMARCH}.rpm; \
	rpmbuild -bb --define "_blddir $$TMPDIR/buildroot"  \
		--define "_topdir $$TMPDIR" \
		--target ${RPMARCH} pkgscripts/libjpeg-turbo.spec; \
	cp $$TMPDIR/RPMS/${RPMARCH}/${PACKAGE_NAME}-${VERSION}-${BUILD}.${RPMARCH}.rpm \
		${PACKAGE_NAME}-${VERSION}.${RPMARCH}.rpm; \
	rm -rf $$TMPDIR

srpm: dist-gzip
	TMPDIR=`mktemp -d /tmp/${PACKAGE_NAME}-build.XXXXXX`; \
	mkdir -p $$TMPDIR/RPMS; \
	mkdir -p $$TMPDIR/SRPMS; \
	mkdir -p $$TMPDIR/BUILD; \
	mkdir -p $$TMPDIR/SOURCES; \
	mkdir -p $$TMPDIR/SPECS; \
	rm -f ${PACKAGE_NAME}-${VERSION}.src.rpm; \
	cp ${PACKAGE_NAME}-${VERSION}.tar.gz $$TMPDIR/SOURCES; \
	cat pkgscripts/libjpeg-turbo.spec | sed s/%{_blddir}/%{_tmppath}/g \
		| sed s/#--\>//g \
		> $$TMPDIR/SPECS/libjpeg-turbo.spec; \
	rpmbuild -bs --define "_topdir $$TMPDIR" $$TMPDIR/SPECS/libjpeg-turbo.spec; \
	cp $$TMPDIR/SRPMS/${PACKAGE_NAME}-${VERSION}-${BUILD}.src.rpm \
		${PACKAGE_NAME}-${VERSION}.src.rpm; \
	rm -rf $$TMPDIR

deb: all
	sh pkgscripts/makedpkg

if X86_64

udmg: all
	sh pkgscripts/makemacpkg -build32 ${BUILDDIR32}

iosdmg: all
	sh pkgscripts/makemacpkg -build32 ${BUILDDIR32} -buildarmv6 ${BUILDDIRARMV6} -buildarmv7 ${BUILDDIRARMV7} -buildarmv7s ${BUILDDIRARMV7S}

else

iosdmg: all
	sh pkgscripts/makemacpkg -buildarmv6 ${BUILDDIRARMV6} -buildarmv7 ${BUILDDIRARMV7} -buildarmv7s ${BUILDDIRARMV7S}

endif

dmg: all
	sh pkgscripts/makemacpkg

cygwinpkg: all
	sh pkgscripts/makecygwinpkg
