AC_INIT(
  [iRoCS Toolbox], [1.1.2],
  [falk@cs.uni-freiburg.de],
  [irocs-toolbox],
  [http://lmb.informatik.uni-freiburg.de/lmbsoft/iRoCS])
AC_CONFIG_HEADERS([src/config.hh])
AM_INIT_AUTOMAKE([subdir-objects])

AC_ARG_ENABLE([debug],
	    AC_HELP_STRING([--enable-debug], []), [enable_debug=1],
	    [enable_debug=0])
AC_ARG_ENABLE([static-tools],
	    AC_HELP_STRING([--enable-static-tools], []),
	    [enable_statictools=1], [enable_statictools=0])
AM_CONDITIONAL([STATIC_TOOLS], [test "x$enable_statictools" == "x1"])

AC_ARG_WITH([qt4],
            AC_HELP_STRING([--with-qt4], [QT4 base path]), [],
	    [with_qt4=autodetect])
AC_ARG_WITH([blitz],
            AC_HELP_STRING([--with-blitz], [blitz++ base path]), [],
	    [with_blitz=autodetect])
AC_ARG_WITH([hdf5],
            AC_HELP_STRING([--with-hdf5], [HDF5 base path]), [],
	    [with_hdf5=autodetect])
AC_ARG_WITH([fftw],
            AC_HELP_STRING([--with-fftw], [FFTW base path]), [],
	    [with_fftw=autodetect])
AC_ARG_WITH([gsl],
            AC_HELP_STRING([--with-gsl], [GNU scientific library base path]),
            [], [with_gsl=autodetect])
AC_ARG_WITH([opencv],
            AC_HELP_STRING([--with-opencv], [OpenCV library base path]),
            [], [with_opencv=autodetect])

AC_CANONICAL_HOST
AC_PROG_CC
AC_PROG_CXX
AC_LANG(C++)
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_CACHE_SAVE
AC_OPENMP

# Setup build environment
CFLAGS="-Wall -Wextra -pedantic -fPIC"
CXXFLAGS="-Wall -Wextra -pedantic -fPIC"
if test "$enable_debug" == "1"; then
   CFLAGS+=" -O0 -g"
   CXXFLAGS+=" -O0 -g"
   AC_DEFINE([DEBUG], [1], [Enable general debugging])
   AC_DEFINE([BZ_DEBUG], [1], [Enable blitz++ debugging])
else
   CFLAGS+=" -O3 -g"
   CXXFLAGS+=" -O3 -g"
fi

# Setup OpenMP environment
if test "$enable_debug" == "0"; then
   if test "x$OPENMP_CXXFLAGS" != "x"; then
     AC_CHECK_LIB(
       gomp, omp_get_num_threads, [
         AC_DEFINE([BZ_THREADSAFE], [1], [Enable blitz++ threadsafe mode])
         CXXFLAGS+=" $OPENMP_CXXFLAGS"
         LIBS+="-lgomp"],
       AC_CHECK_LIB(
         omp, omp_get_num_threads, [
           AC_DEFINE([BZ_THREADSAFE], [1], [Enable blitz++ threadsafe mode])
           CXXFLAGS+=" $OPENMP_CXXFLAGS"
           LIBS+="-lomp"],
         AC_MSG_WARN([OpenMP library not found. Parallel processing will not be available])))
   fi
fi

#
# Basic libraries
#
AC_CHECK_LIB([z], [compress2], [],
  AC_MSG_ERROR([libz is missing (Needed for Qt4 and HDF5)]))

#
# Check for libjpeg
#
AC_CHECK_HEADER([jpeglib.h], [], AC_MSG_WARN(
  [jpeg headers not found. JPEG related parts will be disabled.]))
AC_CHECK_LIB([jpeg], [jpeg_read_scanlines], [],
  AC_MSG_WARN([libjpeg not found. JPEG related parts will be disabled.]))

#
# Check for libtiff
#
AC_CHECK_HEADER([tiffio.h], [], AC_MSG_WARN(
  [tiff headers not found. TIFF related parts will be disabled.]))
AC_CHECK_LIB([tiff], [TIFFGetField], [],
  AC_MSG_WARN([libtiff not found. TIFF related parts will be disabled.]))

#
# Check for blitz++
#
if test "$with_blitz" != "autodetect"; then
  CPPFLAGS+=" -I$with_blitz/include"
  LDFLAGS+=" -L$with_blitz/lib"
  AC_CHECK_HEADER([blitz/array.h], [],
    AC_MSG_ERROR([blitz++ headers not found in $with_blitz/include.]),
    [#ifdef _OPENMP
#include <omp.h>
#endif])
  AC_CHECK_LIB([blitz], [_blitz_id], [
    AC_DEFINE([HAVE_BLITZ_V9], [1], [Set blitz++ v0.9 compatibility])
    AC_MSG_WARN([A blitz++ version < 0.10 was detected. Setting compatibility mode. Future versions will not support older blitz++ versions any more. You need to define HAVE_CONFIG_H and or HAVE_BLITZ_V9 when building tools based on this build of the iRoCS Toolbox libraries.])
    ], [
    AC_MSG_CHECKING([for blitz::firstDim in -lblitz])
    LIBS="-lblitz $LIBS"
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
        [[#include <blitz/array.h>]], [[int d = blitz::firstDim]])],
        [HAVE_BLITZ=1], [HAVE_BLITZ=0])
    if test "x$HAVE_BLITZ" == "x1"; then
      AC_MSG_RESULT([yes])
    else
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([blitz++ library (libblitz) not found in $with_blitz/lib])
    fi
    AC_DEFINE([HAVE_BLITZ_V10], [1], [Set blitz++ v0.10 compatibility])
    ]
  )
else	
  AC_CHECK_HEADER([blitz/array.h], [],
    AC_MSG_ERROR([blitz++ headers not found in standard include path. Please provide the path to your blitz++ installation using the --with-blitz option]),
    [#ifdef _OPENMP
#include <omp.h>
#endif])
  AC_CHECK_LIB([blitz], [_blitz_id], [
    AC_DEFINE([HAVE_BLITZ_V9], [1], [Set blitz++ v0.9 compatibility])
    AC_MSG_WARN([A blitz++ version < 0.10 was detected. Setting compatibility mode. Future versions will not support older blitz++ versions any more. You need to define HAVE_CONFIG_H and or HAVE_BLITZ_V9 when building tools based on this build of the iRoCS Toolbox libraries.])
    ], [
    AC_MSG_CHECKING([for blitz::firstDim in -lblitz])
    LIBS="-lblitz $LIBS"
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
        [[#include <blitz/array.h>]], [[int d = blitz::firstDim]])],
        [HAVE_BLITZ=1], [HAVE_BLITZ=0])
    if test "x$HAVE_BLITZ" == "x1"; then
      AC_MSG_RESULT([yes])
    else
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([blitz++ library not found in standard library path. Please provide the path to your blitz++ installation using the --with-blitz option])
    fi
    AC_DEFINE([HAVE_BLITZ_V10], [1], [Set blitz++ v0.10 compatibility])
    ]
  )
fi

#
# Check for GSL
#
if test "$with_gsl" != "autodetect"; then
  CPPFLAGS+=" -I$with_gsl/include"
  LDFLAGS+=" -L$with_gsl/lib"
  AC_CHECK_HEADER([gsl/gsl_eigen.h], [],
    AC_MSG_ERROR([gsl headers not found in $with_gsl/include]))
  AC_CHECK_LIB(m, sin, [],
    AC_MSG_ERROR([libm is missing (Needed for GNU scientific library)]))
  AC_CHECK_LIB(gslcblas, cblas_scopy, [],
    AC_MSG_ERROR([GSL cblas library not found in $with_gsl/lib]))
  AC_CHECK_LIB(gsl, gsl_eigen_symmv, [],
    AC_MSG_ERROR([GNU scientific library not found in $with_gsl/lib]))
else	
  AC_CHECK_HEADER([gsl/gsl_eigen.h], [],
    AC_MSG_ERROR([GNU scientific library headers not found in standard include path. Please provide the path to your GNU scientific library installation using the --with-gsl option]))
  AC_CHECK_LIB(m, sin, [],
    AC_MSG_ERROR([libm is missing (Needed for GNU scientific library)]))
  AC_CHECK_LIB(gslcblas, cblas_scopy, [],
    AC_MSG_ERROR([GSL cblas library not found in $with_gsl/lib]))
  AC_CHECK_LIB(gsl, gsl_eigen_symmv, [],
    AC_MSG_ERROR([GNU scientific library not found in standard library path. Please provide the path to your GNU scientific library installation using the --with-gsl option]))
fi

#
# Check for HDF5
#
if test "$with_hdf5" != "autodetect"; then
  CPPFLAGS+=" -I$with_hdf5/include"
  LDFLAGS+=" -L$with_hdf5/lib"
  AC_CHECK_HEADER([hdf5.h], [],
    AC_MSG_ERROR([hdf5 headers not found in $with_hdf5/include]))
  AC_CHECK_LIB(hdf5, H5Dcreate2, [],
    AC_MSG_ERROR([hdf5 library not found in $with_hdf5/lib]))
  AC_CHECK_HEADER([H5Cpp.h], [],
    AC_MSG_ERROR([hdf5 c++ headers not found in $with_hdf5/include]))
  AC_MSG_CHECKING([for H5::FileCreatPropList in -lhdf5_cpp])
  LIBS="-lhdf5_cpp $LIBS"
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
      [[#include <H5Cpp.h>]], [[H5::FileCreatPropList plist]])],
      [HAVE_HDF5_CPP=1], [HAVE_HDF5_CPP=0])
  if test "x$HAVE_HDF5_CPP" == "x1"; then
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([HDF5 C++ library (libhdf5_cpp) not found in $with_hdf5/lib])
  fi
else	
  AC_CHECK_HEADER([hdf5.h], [],
    AC_MSG_ERROR([hdf5 headers not found in standard include path. Please provide the path to your hdf5 installation using the --with-hdf5 option]))
  AC_CHECK_LIB(hdf5, H5Dcreate2, [],
    AC_MSG_ERROR([hdf5 library not found in standard library path. Please provide the path to your hdf5 installation using the --with-hdf5 option]))  
  AC_CHECK_HEADER([H5Cpp.h], [],
    AC_MSG_ERROR([hdf5 c++ headers not found in standard include path. Please provide the path to your hdf5 installation using the --with-hdf5 option]))
  AC_MSG_CHECKING([for H5::FileCreatPropList in -lhdf5_cpp])
  LIBS="-lhdf5_cpp $LIBS"
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
      [[#include <H5Cpp.h>]], [[H5::FileCreatPropList plist]])],
      [HAVE_HDF5_CPP=1], [HAVE_HDF5_CPP=0])
  if test "x$HAVE_HDF5_CPP" == "x1"; then
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
    AC_MSG_ERROR(
      [HDF5 C++ library (libhdf5_cpp) not found in standard library path. Please provide the path to your hdf5 installation using the --with-hdf5 option])
  fi
fi

#
# Check for FFTW
#
if test "$with_fftw" != "autodetect"; then
  CPPFLAGS+=" -I$with_fftw/include"
  LDFLAGS+=" -L$with_fftw/lib"
  AC_CHECK_HEADER([fftw3.h], [],
    AC_MSG_ERROR([fftw headers not found in $with_fftw/include]))
  AC_CHECK_LIB(fftw3, fftw_malloc, [],
    AC_MSG_ERROR([fftw library not found in $with_fftw/lib]))
  AC_CHECK_LIB(fftw3f, fftw_malloc, [],
    AC_MSG_ERROR([fftwf library not found in $with_fftw/lib]))
else	
  AC_CHECK_HEADER([fftw3.h], [],
    AC_MSG_ERROR([fftw headers not found in standard include path. Please provide the path to your fftw installation using the --with-fftw option]))
  AC_CHECK_LIB(fftw3, fftw_malloc, [],
    AC_MSG_ERROR([fftw library not found in standard library path. Please provide the path to your fftw installation using the --with-fftw option]))  
  AC_CHECK_LIB(fftw3f, fftw_malloc, [],
    AC_MSG_ERROR([fftwf library not found in standard library path. Please provide the path to your fftw installation using the --with-fftw option]))  
fi

#
# Check for OpenCV
#
if test "$with_opencv" != "autodetect"; then
  CPPFLAGS+=" -I$with_opencv/include"
  LDFLAGS+=" -L$with_opencv/lib"
  AC_CHECK_HEADER([opencv2/core/core.hpp], [],
    AC_MSG_ERROR([OpenCV core header (opencv2/core/core.hpp) not found in $with_opencv.]))
  AC_CHECK_HEADER([opencv2/imgproc/imgproc.hpp], [],
    AC_MSG_ERROR([OpenCV Imageprocessing header (opencv2/imgproc/imgproc.hpp) not found in $with_opencv.]))
  AC_CHECK_LIB(opencv_core, cvError, [
    LIBS="-lopencv_core $LIBS"
    AC_CHECK_LIB(opencv_imgproc, cvFitEllipse2, [],
      AC_MSG_ERROR([OpenCV Image Processing (libopencv_imgproc) library not found in $with_opencv/lib]))], [
    AC_CHECK_LIB(cxcore, cvError, [
      LIBS="-lcxcore $LIBS"
      AC_CHECK_LIB(cv, cvFitEllipse2, [], [
        AC_MSG_ERROR([OpenCV (libcv) library not found in $with_openvc/lib])])], [
      AC_MSG_ERROR([OpenCV core (libcxcore) library not found in $with_opencv/lib])])])
else	
  AC_CHECK_HEADER([opencv2/core/core.hpp], [],
    AC_MSG_ERROR([OpenCV core header (opencv2/core/core.hpp) not found in standard include path. Please provide the path to your OpenCV installation using the --with-opencv option.]))
  AC_CHECK_HEADER([opencv2/imgproc/imgproc.hpp], [],
    AC_MSG_ERROR([OpenCV Imageprocessing header (opencv2/imgproc/imgproc.hpp) not found in standard include path. Please provide the path to your OpenCV installation using the --with-opencv option.]))
  AC_CHECK_LIB(opencv_core, cvError, [
    LIBS="-lopencv_core $LIBS"
    AC_CHECK_LIB(opencv_imgproc, cvFitEllipse2, [],
      AC_MSG_ERROR([OpenCV Image Processing (libopencv_imgproc) library not found in standard search path. Please provide the path to your OpenCV installation using --with-opencv]))], [
    AC_CHECK_LIB(cxcore, cvError, [
      LIBS="-lcxcore $LIBS"
      AC_CHECK_LIB(cv, cvFitEllipse2, [], [
        AC_MSG_ERROR([OpenCV (libcv) library not found in in standard search path. Please provide the path to your OpenCV installation using --with-opencv])])], [
      AC_MSG_ERROR([OpenCV core (libcxcore) library not found in standard search path. Please provide the path to your OpenCV installation using --with-opencv])])])
fi

#
# Check for OpenGL
#
AC_CHECK_HEADER(GL/gl.h,[], AC_MSG_ERROR([OpenGL headers not found.]))
AC_CHECK_LIB(GL,glBegin, [], AC_MSG_ERROR([OpenGL library (libGL) not found.]))
AC_CHECK_LIB(GLU,gluLookAt, [], AC_MSG_ERROR([OpenGL utility library (libGLU) not found.]))

#
# Check for QT4
#
if test "$with_qt4" != "autodetect"; then
  CPPFLAGS+=" -I$with_qt4/include/"
  LDFLAGS+=" -L$with_qt4/lib -L/usr/X11R6/lib"

  AC_CHECK_HEADER(QtCore/QString,[],
    AC_MSG_ERROR([QT4 headers not found in $with_qt4/include]))

  # Check Qt4 version (should be >= 4.6.0)
  AC_MSG_CHECKING([for suitable Qt 4 version])
  AC_COMPILE_IFELSE(
    [AC_LANG_SOURCE(
      [[#include <Qt/qglobal.h>
        #if (QT_VERSION < QT_VERSION_CHECK(4, 6, 0))
        #error Qt too old (Qt >=4.6.0 required)
        #endif
      ]])], [AC_MSG_RESULT([yes])],
      [AC_MSG_RESULT([no])
       AC_MSG_ERROR([Your Qt version is too old. Qt >= 4.6 required.])])

  AC_CHECK_LIB(png, png_malloc, [],
    AC_MSG_ERROR([libpng is missing (Needed for QT4)]))
  AC_CHECK_LIB(pthread, pthread_join, [],
    AC_MSG_ERROR([libpthread is missing (Needed for QT4)]))
  AC_CHECK_LIB(m, sin, [],
    AC_MSG_ERROR([libm is missing (Needed for QT4)]))
  AC_CHECK_LIB(X11, XDrawPoint, [],
    AC_MSG_ERROR([libX11 is missing (Needed for QT4)]))
  AC_CHECK_LIB(Xext, DPMSInfo, [],
    AC_MSG_ERROR([libXext is missing (Needed for QT4)]))
#  AC_CHECK_LIB(fontconfig, FcAtomicCreate, [],
#    AC_MSG_ERROR([libfontconfig is missing (Needed for QT4)]))
#  AC_CHECK_LIB(freetype, FT_Stream_Open, [],
#    AC_MSG_ERROR([libfreetype is missing (Needed for QT4)]))
#  AC_CHECK_LIB(Xinerama, XPanoramiXAllocInfo, [],
#    AC_MSG_ERROR([libXinerama is missing (Needed for QT4)]))
#  AC_CHECK_LIB(Xcursor, XcursorCursorsCreate, [],
#    AC_MSG_ERROR([libXcursor is missing (Needed for QT4)]))
#  AC_CHECK_LIB(Xrender, XRenderComposite, [],
#    AC_MSG_ERROR([libXrender is missing (Needed for QT4)]))
#  AC_CHECK_LIB(Xrandr, XRRCreateMode, [],
#    AC_MSG_ERROR([libXrandr is missing (Needed for QT4)]))
#  AC_CHECK_LIB(Xi, XCloseDevice, [],
#    AC_MSG_ERROR([libXi is missing (Needed for QT4)]))
#  AC_CHECK_LIB(ICE, IceAcceptConnection, [],
#    AC_MSG_ERROR([libICE is missing (Needed for QT4)]))
#  AC_CHECK_LIB(SM, SmcCloseConnection, [],
#    AC_MSG_ERROR([libSM is missing (Needed for QT4)]))
  AC_CHECK_LIB(QtCore,main,[],
    AC_MSG_ERROR([QT4 libraries not found in $with_qt4/lib]))
  AC_CHECK_LIB(QtGui,main,[],
    AC_MSG_ERROR([QT4 libraries not found in $with_qt4/lib]))
  AC_CHECK_LIB(QtOpenGL,main,[],
    AC_MSG_ERROR([QT4 libraries not found in $with_qt4/lib]))
  AC_CHECK_LIB(QtXml,main,[],
    AC_MSG_ERROR([QT4 libraries not found in $with_qt4/lib]))
  AC_CHECK_LIB(QtSvg,main,[],
    AC_MSG_ERROR([QT4 libraries not found in $with_qt4/lib]))
  
  AC_PATH_PROG(QT4_MOC, moc,
    AC_MSG_ERROR(
      [Could not find $with_qt4/bin/moc. Please correct your QT4 path]),
    [$with_qt4/bin])
  AC_PATH_PROG(QT4_UIC, uic,
    AC_MSG_ERROR(
      [Could not find $with_qt4/bin/uic. Please correct your QT4 path]),
    [$with_qt4/bin])
  AC_PATH_PROG(QT4_RCC, rcc,
    AC_MSG_ERROR(
      [Could not find $with_qt4/bin/rcc. Please correct your QT4 path]),
    [$with_qt4/bin])
  AC_PATH_PROG(QT4_QMAKE, qmake,
    AC_MSG_ERROR(
      [Could not find $with_qt4/bin/qmake. Please correct your QT4 path]),
    [$with_qt4/bin])
else
  AC_CHECK_HEADER(QtCore/QString,[],
    AC_MSG_ERROR([QT4 headers not found in standard include path. Please provide the path to your Qt4 installation using the --with-qt4 option]))

  # Check Qt4 version (should be >= 4.6.0)
  AC_MSG_CHECKING([for suitable Qt 4 version])
  AC_COMPILE_IFELSE(
    [AC_LANG_SOURCE(
      [[#include <Qt/qglobal.h>
        #if (QT_VERSION < QT_VERSION_CHECK(4, 6, 0))
        #error Qt too old (Qt >=4.6.0 required)
        #endif
      ]])], [AC_MSG_RESULT([yes])],
      [AC_MSG_RESULT([no])
       AC_MSG_ERROR([Your Qt version is too old. Qt >= 4.6 required.])])

  AC_CHECK_LIB(png, png_malloc, [],
    AC_MSG_ERROR([libpng is missing (Needed for QT4)]))
  AC_CHECK_LIB(pthread, pthread_join, [],
    AC_MSG_ERROR([libpthread is missing (Needed for QT4)]))
  AC_CHECK_LIB(m, sin, [],
    AC_MSG_ERROR([libm is missing (Needed for QT4)]))
  AC_CHECK_LIB(z, compress2, [],
    AC_MSG_ERROR([libz is missing (Needed for QT4)]))
  AC_CHECK_LIB(X11, XDrawPoint, [],
    AC_MSG_ERROR([libX11 is missing (Needed for QT4)]))
  AC_CHECK_LIB(Xext, DPMSInfo, [],
    AC_MSG_ERROR([libXext is missing (Needed for QT4)]))
#  AC_CHECK_LIB(fontconfig, FcAtomicCreate, [],
#    AC_MSG_ERROR([libfontconfig is missing (Needed for QT4)]))
#  AC_CHECK_LIB(freetype, FT_Stream_Open, [],
#    AC_MSG_ERROR([libfreetype is missing (Needed for QT4)]))
#  AC_CHECK_LIB(Xinerama, XPanoramiXAllocInfo, [],
#    AC_MSG_ERROR([libXinerama is missing (Needed for QT4)]))
#  AC_CHECK_LIB(Xcursor, XcursorCursorsCreate, [],
#    AC_MSG_ERROR([libXcursor is missing (Needed for QT4)]))
#  AC_CHECK_LIB(Xrender, XRenderComposite, [],
#    AC_MSG_ERROR([libXrender is missing (Needed for QT4)]))
#  AC_CHECK_LIB(Xrandr, XRRCreateMode, [],
#    AC_MSG_ERROR([libXrandr is missing (Needed for QT4)]))
#  AC_CHECK_LIB(Xi, XCloseDevice, [],
#    AC_MSG_ERROR([libXi is missing (Needed for QT4)]))
#  AC_CHECK_LIB(ICE, IceAcceptConnection, [],
#    AC_MSG_ERROR([libICE is missing (Needed for QT4)]))
#  AC_CHECK_LIB(SM, SmcCloseConnection, [],
#    AC_MSG_ERROR([libSM is missing (Needed for QT4)]))
  AC_CHECK_LIB(QtCore,main,[],
    AC_MSG_ERROR([QT4 libraries not found in standard libaray path. Please provide the path to your Qt4 installation using the --with-qt4 option]))
  AC_CHECK_LIB(QtGui,main,[],
    AC_MSG_ERROR([QT4 libraries not found in standard library path. Please provide the path to your Qt4 installation using the --with-qt4 option]))
  AC_CHECK_LIB(QtOpenGL,main,[],
    AC_MSG_ERROR([QT4 libraries not found in standard library path. Please provide the path to your Qt4 installation using the --with-qt4 option]))
  AC_CHECK_LIB(QtXml,main,[],
    AC_MSG_ERROR([QT4 libraries not found in standard library path. Please provide the path to your Qt4 installation using the --with-qt4 option]))
  AC_CHECK_LIB(QtSvg,main,[],
    AC_MSG_ERROR([QT4 libraries not found in standard library path. Please provide the path to your Qt4 installation using the --with-qt4 option]))
  
  AC_PATH_PROG(QT4_MOC, moc,
    AC_MSG_ERROR([Could not find Qt4 meta object compiler 'moc'. Please provide the path to your Qt4 installation using the --with-qt4 option]))

  if test "x$($QT4_MOC -v 2>&1 | grep 'Qt 4.8')" == "x"; then
    AC_MSG_ERROR([$QT4_MOC is too old, you need at least version 4.8 of QT])
  fi
  AC_PATH_PROG(QT4_UIC, uic,
    AC_MSG_ERROR(
      [Could not find uic. Please provide the path to your Qt4 installation using the --with-qt4 option]))
  AC_PATH_PROG(QT4_RCC, rcc,
    AC_MSG_ERROR(
      [Could not find rcc. Please provide the path to your Qt4 installation using the --with-qt4 option]))
  AC_PATH_PROG(QT4_QMAKE, qmake,
    AC_MSG_ERROR(
      [Could not find qmake. Please provide the path to your Qt4 installation using the --with-qt4 option]))
fi
AC_SUBST(QT4_MOC)
AC_SUBST(QT4_UIC)
AC_SUBST(QT4_RCC)
AC_SUBST(QT4_QMAKE)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/libcmdline/Makefile])
AC_CONFIG_FILES([src/libsvmtl/Makefile])
AC_CONFIG_FILES([src/libProgressReporter/Makefile])
AC_CONFIG_FILES([src/libBaseFunctions/Makefile])
AC_CONFIG_FILES([src/libBlitzAnalyze/Makefile])
AC_CONFIG_FILES([src/libBlitzHdf5/Makefile])
AC_CONFIG_FILES([src/libBlitzFFTW/Makefile])
AC_CONFIG_FILES([src/libBlitz2DGraphics/Makefile])
AC_CONFIG_FILES([src/lmbs2kit/Makefile])
AC_CONFIG_FILES([src/libArrayToolbox/Makefile])
AC_CONFIG_FILES([src/libsegmentation/Makefile])
AC_CONFIG_FILES([src/liblabelling_qt4/Makefile])
AC_CONFIG_FILES([src/labelling/Makefile])
AC_CONFIG_FILES([src/libIRoCS/Makefile])
AC_CONFIG_FILES([src/plugins/Makefile])
AC_CONFIG_FILES([src/plugins/PluginConvertMasksToMarkers/Makefile])
AC_CONFIG_FILES([src/plugins/DetectNuclei/Makefile])
AC_CONFIG_FILES([src/plugins/EpidermisLabelling/Makefile])
AC_CONFIG_FILES([src/plugins/AttachIRoCS/Makefile])
AC_CONFIG_FILES([src/plugins/LayerAssignment/Makefile])
AC_CONFIG_FILES([src/plugins/MitosisClassification/Makefile])
AC_CONFIG_FILES([src/plugins/RootSegmentation/Makefile])
AC_CONFIG_FILES([src/plugins/AttachIRoCSToCellSegmentation/Makefile])
AC_CONFIG_FILES([src/plugins/AttachIRoCSSCTToCellSegmentation/Makefile])
AC_CONFIG_FILES([src/plugins/LayerAssignmentToCellSegmentation/Makefile])
AC_CONFIG_FILES([src/plugins/TrainDetector/Makefile])
AC_CONFIG_FILES([src/plugins/TrainEpidermisLabelling/Makefile])
AC_CONFIG_FILES([src/plugins/TrainLayerAssignment/Makefile])
AC_CONFIG_FILES([src/plugins/DetectSpheres/Makefile])
AC_CONFIG_FILES([src/tools/Makefile])
AC_CONFIG_FILES([src/tools/detectNuclei/Makefile])
AC_CONFIG_FILES([src/tools/labelEpidermis/Makefile])
AC_CONFIG_FILES([src/tools/segmentCells/Makefile])
AC_CONFIG_FILES([src/tools/attachIRoCS/Makefile])
AC_CONFIG_FILES([src/tools/attachIRoCSToCellSegmentation/Makefile])
AC_CONFIG_FILES([src/tools/assignLayers/Makefile])
AC_CONFIG_FILES([src/tools/computeCellFeatures/Makefile])
AC_CONFIG_FILES([src/tools/assignLayersToSegmentation/Makefile])
AC_CONFIG_FILES([src/tools/trainLayerAssignmentForSegmentation/Makefile])
AC_CONFIG_FILES([src/tools/detectSpheres/Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_CONFIG_FILES([test/libcmdline/Makefile])
AC_CONFIG_FILES([test/libsvmtl/Makefile])
AC_CONFIG_FILES([test/libProgressReporter/Makefile])
AC_CONFIG_FILES([test/libBaseFunctions/Makefile])
AC_CONFIG_FILES([test/libBlitzAnalyze/Makefile])
AC_CONFIG_FILES([test/libBlitzHdf5/Makefile])
AC_CONFIG_FILES([test/libBlitzFFTW/Makefile])
AC_CONFIG_FILES([test/libBlitz2DGraphics/Makefile])
AC_CONFIG_FILES([test/lmbs2kit/Makefile])
AC_CONFIG_FILES([test/libArrayToolbox/Makefile])
AC_CONFIG_FILES([test/liblabelling_qt4/Makefile])
AC_OUTPUT

if test "x$enable_statictools" == "x1"; then
    toollinktype="static"
else
    toollinktype="dynamic"
fi

AC_MSG_RESULT([
-------------------------------------------------
iRoCS-Toolbox $PACKAGE_VERSION configured.

Command line tools link type: $toollinktype

Using
  blitz++:                             $with_blitz
  hdf5:                                $with_hdf5
  fftw:                                $with_fftw
  GNU scientific library:              $with_gsl
  OpenCV:                              $with_opencv
  Qt4:                                 $with_qt4

Environment:
  CPPFLAGS                             $CPPFLAGS
  CFLAGS                               $CFLAGS
  CXXFLAGS                             $CXXFLAGS
  LDFLAGS                              $LDFLAGS
  LIBS                                 $LIBS

Type 'make' to build and 'make install' to install
afterwards

Type 'doxygen' to generate documentation
-------------------------------------------------])
