AC_INIT(
  [iRoCS Toolbox], [1.1.2],
  [falk@cs.uni-freiburg.de],
  [irocs-toolbox],
  [http://lmb.informatik.uni-freiburg.de/lmbsoft/iRoCS])
AC_CONFIG_HEADERS([src/config.hh])
AM_INIT_AUTOMAKE([subdir-objects])

AC_ARG_ENABLE([debug], [AC_HELP_STRING([--enable-debug], [])],
  [], [enable_debug=no])
AC_ARG_ENABLE([static-tools], [AC_HELP_STRING([--enable-static-tools], [])],
  [], [enable_static_tools=no])
AM_CONDITIONAL([STATIC_TOOLS], [test "x$enable_statictools" != "xno"])

AC_ARG_WITH([qt4], AC_HELP_STRING([--with-qt4], [QT4 base path]), [],
  [with_qt4=autodetect])
AC_ARG_WITH([blitz], AC_HELP_STRING([--with-blitz], [blitz++ base path]), [],
  [with_blitz=autodetect])
AC_ARG_WITH([hdf5], AC_HELP_STRING([--with-hdf5], [HDF5 base path]), [],
  [with_hdf5=autodetect])
AC_ARG_WITH([fftw], AC_HELP_STRING([--with-fftw], [FFTW base path]), [],
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
if test "x$enable_debug" != "xno"; then
   CFLAGS+=" -O0 -g"
   CXXFLAGS+=" -O0 -g"
   AC_DEFINE([DEBUG], [1], [Enable general debugging])
   AC_DEFINE([BZ_DEBUG], [1], [Enable blitz++ debugging])
else
   CFLAGS+=" -O3"
   CXXFLAGS+=" -O3"
fi

# Setup OpenMP environment
if test "x$enable_debug" == "xno"; then
   if test "x$OPENMP_CXXFLAGS" != "x"; then
     AC_CHECK_LIB(
       gomp, omp_get_num_threads, [
         AC_DEFINE([BZ_THREADSAFE], [1], [Enable blitz++ threadsafe mode])
         CXXFLAGS+=" $OPENMP_CXXFLAGS"
         OPENMP_LIBS="-lgomp"],
       AC_CHECK_LIB(
         omp, omp_get_num_threads, [
           AC_DEFINE([BZ_THREADSAFE], [1], [Enable blitz++ threadsafe mode])
           CXXFLAGS+=" $OPENMP_CXXFLAGS"
           OPENMP_LIBS="-lomp"],
         AC_MSG_WARN([OpenMP library not found. Parallel processing will not be available])))
     AC_SUBST(OPENMP_LIBS)
   fi
fi

PKG_PROG_PKG_CONFIG

#
# If pkg-config is available try to automatically configure libraries
#
if test "x$PKG_CONFIG" != "x"; then
  if test "$with_blitz" == "autodetect"; then
    PKG_CHECK_MODULES(BLITZ, [blitz >= 0.10],
      [AC_DEFINE([HAVE_BLITZ_V10], [1], [Set blitz++ v0.10 compatibility])],
      [PKG_CHECK_MODULES(BLITZ, [blitz >= 0.9],
        [AC_DEFINE([HAVE_BLITZ_V9], [1], [Set blitz++ v0.9 compatibility])
         AC_MSG_WARN(A blitz++ version < 0.10 was detected. Setting compatibility mode. Future versions will not support older blitz++ versions any more. You need to define HAVE_CONFIG_H and or HAVE_BLITZ_V9 when building tools based on this build of the iRoCS Toolbox libraries.)],
        [AC_MSG_NOTICE(pkgconfig for blitz not found... Deferring)])])
  fi
  PKG_CHECK_MODULES(JPEG, [jpeg], [],
    [AC_MSG_NOTICE(pkgconfig for jpeg not found... Deferring)])
  PKG_CHECK_MODULES(TIFF, [libtiff-4], [],
    [AC_MSG_NOTICE(pkgconfig for libtiff-4 not found... Deferring)])
  if test "$with_gsl" == "autodetect"; then
    PKG_CHECK_MODULES(GSL, [gsl], [],
      [AC_MSG_NOTICE(pkgconfig for gsl not found... Deferring)])
  fi
  if test "$with_hdf5" == "autodetect"; then
    PKG_CHECK_MODULES(HDF5, [hdf5], [],
      [AC_MSG_NOTICE(pkgconfig for hdf5 not found... Deferring)])
  fi
  if test "$with_fftw" == "autodetect"; then
    PKG_CHECK_MODULES(FFTW, [fftw3 fftw3f], [],
      [AC_MSG_NOTICE(pkgconfig for fftw not found... Deferring)])
  fi
#  if test "$with_opencv" == "autodetect"; then
#    PKG_CHECK_MODULES(OPENCV, [opencv], [],
#      [AC_MSG_NOTICE(pkgconfig for OpenCV not found... Deferring)])
#  fi
  PKG_CHECK_MODULES(GLU, [glu], [],
    [AC_MSG_NOTICE(pkgconfig for glu not found... Deferring)])
  if test "$with_qt4" == "autodetect"; then
    PKG_CHECK_MODULES(QT4, [QtCore QtGui QtOpenGL QtXml QtSvg >= 4.6.0], [
      AC_PATH_PROGS(QT4_MOC, [moc-qt4 moc], moc,`eval $PKG_CONFIG --variable=exec_prefix QtCore`/bin)
      AC_PATH_PROG(QT4_RCC, rcc, rcc,`eval $PKG_CONFIG --variable=exec_prefix QtCore`/bin)
      AC_PATH_PROGS(QT4_UIC, [uic-qt4 uic], uic,`eval $PKG_CONFIG --variable=exec_prefix QtCore`/bin)], [AC_MSG_NOTICE(pkgconfig for Qt4 not found... Deferring)])
    AC_SUBST(QT4_MOC)
    AC_SUBST(QT4_RCC)
    AC_SUBST(QT4_UIC)
  fi

  # Also export static variants for non-gui modules if static-tools are enabled
  if test "x$enable_static_tools" != "xno"; then
    tmp_PKG_CONFIG="$PKG_CONFIG"
    PKG_CONFIG="$PKG_CONFIG --static"
    if test "x$BLITZ_LIBS" != "x"; then
      if test "x$HAVE_BLITZ_V10" == "x1"; then
        PKG_CHECK_MODULES(BLITZ_STATIC, [blitz >= 0.10],
          [], [AC_MSG_ERROR(No static blitz++ 0.10 library available)])
      else
        PKG_CHECK_MODULES(BLITZ_STATIC, [blitz >= 0.9],
          [], [AC_MSG_ERROR(No static blitz++ 0.9 library available)])
      fi
    fi
    if test "x$JPEG_LIBS" != "x"; then
      PKG_CHECK_MODULES(JPEG_STATIC, [jpeg], [],
        [AC_MSG_ERROR(No static JPEG library available)])
    fi
    if test "x$TIFF_LIBS" != "x"; then
      PKG_CHECK_MODULES(TIFF_STATIC, [libtiff-4], [],
        [AC_MSG_ERROR(No static TIFF library available)])
    fi
    if test "x$GSL_LIBS" != "x"; then
      PKG_CHECK_MODULES(GSL_STATIC, [gsl], [],
        [AC_MSG_ERROR(No static GSL library available)])
    fi
    if test "x$HDF5_LIBS" != "x"; then
      PKG_CHECK_MODULES(HDF5_STATIC, [hdf5], [],
        [AC_MSG_ERROR(No static HDF5 library available)])
    fi
    if test "x$FFTW_LIBS" != "x"; then
      PKG_CHECK_MODULES(FFTW_STATIC, [fftw3 fftw3f], [],
        [AC_MSG_ERROR(No static FFTW library available)])
    fi
#    PKG_CHECK_MODULES(OPENGL_STATIC, [gl], [],
#      [AC_MSG_ERROR(No static OpenGL library available)])
#    PKG_CHECK_MODULES(ZLIB_STATIC, [zlib], [],
#      [AC_MSG_ERROR(No static zlib library available)])
    if test "x$OPENCV_LIBS" != "x"; then
      PKG_CHECK_MODULES(OPENCV_STATIC, [opencv], [],
        [AC_MSG_ERROR(No static OpenCV library available)])
    fi
    PKG_CONFIG="$tmp_PKG_CONFIG"
  fi
fi

# No pkg-config 

#
# Check for blitz++
#
if test "x$BLITZ_LIBS" == "x"; then
  tmp_CPPFLAGS="$CPPFLAGS"
  tmp_LDFLAGS="$LDFLAGS"
  tmp_LIBS="$LIBS"

  if test "$with_blitz" != "autodetect"; then
    BLITZ_LDFLAGS="-L$with_blitz/lib"
    CPPFLAGS+=" -I$with_blitz/include"
    LDFLAGS+="$BLITZ_LDFLAGS"
  fi
  AC_CHECK_HEADER([blitz/array.h], [],
    AC_MSG_ERROR([blitz++ headers not found. Check the --with-blitz conigure option.]),
    [#ifdef _OPENMP
#include <omp.h>
#endif])
  AC_CHECK_LIB([blitz], [_blitz_id], [
    AC_DEFINE([HAVE_BLITZ_V9], [1], [Set blitz++ v0.9 compatibility])
    AC_MSG_WARN([A blitz++ version < 0.10 was detected. Setting compatibility mode. Future versions will not support older blitz++ versions any more. You need to define HAVE_CONFIG_H and or HAVE_BLITZ_V9 when building tools based on this build of the iRoCS Toolbox libraries.])],
    [AC_MSG_CHECKING([for blitz::firstDim in -lblitz])
    LIBS="-lblitz $LIBS"
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM(
        [[#include <blitz/array.h>]], [[int d = blitz::firstDim]])],
        [HAVE_BLITZ=1], [HAVE_BLITZ=0])
    if test "x$HAVE_BLITZ" == "x1"; then
      AC_MSG_RESULT([yes])
    else
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([blitz++ library (libblitz) not found. Check the --with-blitz configure option.])
    fi
    AC_DEFINE([HAVE_BLITZ_V10], [1], [Set blitz++ v0.10 compatibility])])
  BLITZ_CFLAGS="$CPPFLAGS"
  BLITZ_LIBS="$BLITZ_LDFLAGS $LIBS"
  AC_SUBST(BLITZ_CFLAGS)
  AC_SUBST(BLITZ_LIBS)

  if test "x$enable_static_tools" != "xno"; then
    LDFLAGS="-static $BLITZ_LDFLAGS $tmp_LDFLAGS"
    LIBS="$tmp_LIBS"

    if test "x$HAVE_BLITZ_V9" == "x1"; then
      $as_unset ac_cv_lib_blitz__blitz_id
      AC_CHECK_LIB([blitz], [_blitz_id], [],
        [AC_MSG_ERROR([No static blitz++ 0.9 library available.])])
    else
      AC_MSG_CHECKING([for blitz::firstDim in -lblitz])
      LIBS+=" -lblitz"
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
          [[#include <blitz/array.h>]], [[int d = blitz::firstDim]])],
          [HAVE_BLITZ=1], [HAVE_BLITZ=0])
      if test "x$HAVE_BLITZ" == "x1"; then
        AC_MSG_RESULT([yes])
      else
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([No static blitz++ 0.10 library available.])
      fi
    fi
    BLITZ_STATIC_LIBS="$BLITZ_LDFLAGS $LIBS"
    AC_SUBST(BLITZ_STATIC_LIBS)
  fi

  CPPFLAGS="$tmp_CPPFLAGS"
  LDFLAGS="$tmp_LDFLAGS"
  LIBS="$tmp_LIBS"
fi

#
# Check for libjpeg
#
if test "x$JPEG_LIBS" == "x"; then
  tmp_CPPFLAGS="$CPPFLAGS"
  tmp_LDFLAGS="$LDFLAGS"
  tmp_LIBS="$LIBS"

  AC_CHECK_HEADER([jpeglib.h], [], AC_MSG_WARN(
    [jpeg headers not found. JPEG related parts will be disabled.]))
  AC_CHECK_LIB([jpeg], [jpeg_read_scanlines], [],
    AC_MSG_WARN([libjpeg not found. JPEG related parts will be disabled.]))
  JPEG_CFLAGS="$CPPFLAGS"
  JPEG_LIBS="$LIBS"
  AC_SUBST(JPEG_CFLAGS)
  AC_SUBST(JPEG_LIBS)

  if test "x$enable_static_tools" != "xno"; then
    $as_unset ac_cv_lib_jpeg_jpeg_read_scanlines
    CPPFLAGS="$tmp_CPPFLAGS"
    LDFLAGS="-static $tmp_LDFLAGS"
    LIBS="$tmp_LIBS"
    AC_CHECK_LIB([jpeg], [jpeg_read_scanlines], [],
      [AC_MSG_ERROR([No static JPEG library available.])])
    JPEG_STATIC_LIBS="$LIBS"
    AC_SUBST(JPEG_STATIC_LIBS)
  fi

  CPPFLAGS="$tmp_CPPFLAGS"
  LDFLAGS="$tmp_LDFLAGS"
  LIBS="$tmp_LIBS"
fi

#
# Check for libtiff
#
if test "x$TIFF_LIBS" == "x"; then
  tmp_CPPFLAGS="$CPPFLAGS"
  tmp_LDFLAGS="$LDFLAGS"
  tmp_LIBS="$LIBS"

  AC_CHECK_HEADER([tiffio.h], [], AC_MSG_WARN(
    [tiff headers not found. TIFF related parts will be disabled.]))
  AC_CHECK_LIB([tiff], [TIFFGetField], [],
    AC_MSG_WARN([libtiff not found. TIFF related parts will be disabled.]))
  TIFF_CFLAGS="$CPPFLAGS"
  TIFF_LIBS="$LIBS"
  AC_SUBST(TIFF_CFLAGS)
  AC_SUBST(TIFF_LIBS)

  if test "x$enable_static_tools" != "xno"; then
    $as_unset ac_cv_lib_tiff_TIFFGetField
    CPPFLAGS="$tmp_CPPFLAGS"
    LDFLAGS="-static $tmp_LDFLAGS"
    LIBS="$tmp_LIBS"
    AC_CHECK_LIB([tiff], [TIFFGetField], [],
      [AC_MSG_ERROR([No static TIFF library available.])])
    TIFF_STATIC_LIBS="$LIBS"
    AC_SUBST(TIFF_STATIC_LIBS)
  fi

  CPPFLAGS="$tmp_CPPFLAGS"
  LDFLAGS="$tmp_LDFLAGS"
  LIBS="$tmp_LIBS"
fi

#
# Check for GSL
#
if test "x$GSL_LIBS" == "x"; then
  tmp_CPPFLAGS="$CPPFLAGS"
  tmp_LDFLAGS="$LDFLAGS"
  tmp_LIBS="$LIBS"

  if test "$with_gsl" != "autodetect"; then
    GSL_LDFLAGS="-L$with_gsl/lib"
    CPPFLAGS+="-I$with_gsl/include"
    LDFLAGS+="$GSL_LDFLAGS"
  fi

  AC_CHECK_HEADER([gsl/gsl_eigen.h], [],
    AC_MSG_ERROR([gsl headers not found. Check the --with-gsl configure option.]))
  AC_CHECK_LIB(gslcblas, cblas_scopy, [],
    AC_MSG_ERROR([GSL cblas library not found. Check the --with-gsl configure option.]))
  AC_CHECK_LIB(gsl, gsl_eigen_symmv, [],
    AC_MSG_ERROR([GNU scientific library not found. Check the --with-gsl configure option.]))
  GSL_CFLAGS="$CPPFLAGS"
  GSL_LIBS="$GSL_LDFLAGS $LIBS"
  AC_SUBST(GSL_CFLAGS)
  AC_SUBST(GSL_LIBS)

  if test "x$enable_static_tools" != "xno"; then
    $as_unset ac_cv_lib_gslcblas_cblas_scopy
    $as_unset ac_cv_lib_gsl_gsl_eigen_symmv
    LDFLAGS="-static $GSL_LDFLAGS $tmp_LDFLAGS"
    LIBS="$tmp_LIBS"
    AC_CHECK_LIB(m, sin, [],
      AC_MSG_ERROR([libm is missing (Needed for GNU scientific library)]))
    AC_CHECK_LIB(gslcblas, cblas_scopy, [],
      AC_MSG_ERROR([Static GSL cblas library not found.]))
    AC_CHECK_LIB(gsl, gsl_eigen_symmv, [],
      AC_MSG_ERROR([Static GNU scientific library not found.]))
    GSL_STATIC_LIBS="$GSL_LDFLAGS $LIBS"
    AC_SUBST(GSL_STATIC_LIBS)
  fi

  CPPFLAGS="$tmp_CPPFLAGS"
  LDFLAGS="$tmp_LDFLAGS"
  LIBS="$tmp_LIBS"
fi

#
# Check for HDF5
#
if test "x$HDF5_LIBS" == "x"; then
  tmp_CPPFLAGS="$CPPFLAGS"
  tmp_LDFLAGS="$LDFLAGS"
  tmp_LIBS="$LIBS"

  if test "$with_hdf5" != "autodetect"; then
    HDF5_LDFLAGS="-L$with_hdf5/lib"
    CPPFLAGS+="-I$with_hdf5/include"
    LDFLAGS+="$HDF5_LDFLAGS"
  fi

  AC_CHECK_HEADER([hdf5.h], [],
    AC_MSG_ERROR([hdf5 headers not found. Check the --with-hdf5 configure option.]))
  AC_CHECK_LIB(hdf5, H5Dcreate2, [],
    AC_MSG_ERROR([hdf5 library not found. Check the --with-hdf5 configure option.]))
  AC_CHECK_HEADER([H5Cpp.h], [],
    AC_MSG_ERROR([hdf5 c++ headers not found. Check the --with-hdf5 configure option.]))
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
    AC_MSG_ERROR([HDF5 C++ library (libhdf5_cpp) not found. Check the --with-hdf5 configure option.])
  fi
  HDF5_CFLAGS="$CPPFLAGS"
  HDF5_LIBS="$HDF5_LDFLAGS $LIBS"
  AC_SUBST(HDF5_CFLAGS)
  AC_SUBST(HDF5_LIBS)

  if test "x$enable_static_tools" != "xno"; then
    $as_unset ac_cv_lib_hdf5_H5Dcreate2
    LDFLAGS="-static $HDF5_LDFLAGS $tmp_LDFLAGS"
    LIBS="$tmp_LIBS"
    AC_CHECK_LIB([z], [compress2], [],
      AC_MSG_WARN([libz is missing (HDF5 compression will not be available)]))
    AC_CHECK_LIB([dl], [dlopen], [],
      AC_MSG_ERROR([Dynamic link library -ldl not found.]))
    AC_CHECK_LIB(hdf5, H5Dcreate2, [],
      AC_MSG_ERROR([Static hdf5 library not found.]))
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
      AC_MSG_ERROR([Static HDF5 C++ library (libhdf5_cpp) not found.])
    fi
    HDF5_STATIC_LIBS="$HDF5_LDFLAGS $LIBS"
    AC_SUBST(HDF5_STATIC_LIBS)    
  fi

  CPPFLAGS="$tmp_CPPFLAGS"
  LDFLAGS="$tmp_LDFLAGS"
  LIBS="$tmp_LIBS"
fi

#
# Check for FFTW
#
if test "x$FFTW_LIBS" == "x"; then

  tmp_CPPFLAGS="$CPPFLAGS"
  tmp_LDFLAGS="$LDFLAGS"
  tmp_LIBS="$LIBS"

  if test "$with_fftw" != "autodetect"; then
    FFTW_LDFLAGS="-L$with_fftw/lib"
    CPPFLAGS+="-I$with_fftw/include"
    LDFLAGS+="$FFTW_LDFLAGS"
  fi

  AC_CHECK_HEADER([fftw3.h], [],
    AC_MSG_ERROR([fftw headers not found. Check the --with-fftw configure option.]))
  AC_CHECK_LIB(fftw3, fftw_malloc, [],
    AC_MSG_ERROR([fftw library not found. Check the --with-fftw configure option.]))
  AC_CHECK_LIB(fftw3f, fftw_malloc, [],
    AC_MSG_ERROR([fftwf library not found. Check the --with-fftw configure option.]))
  FFTW_CFLAGS="$CPPFLAGS"
  FFTW_LIBS="$LDFLAGS $LIBS"
  AC_SUBST(FFTW_CFLAGS)
  AC_SUBST(FFTW_LIBS)

  if test "x$enable_static_tools" != "xno"; then
    $as_unset ac_cv_lib_fftw_fftw_malloc
    $as_unset ac_cv_lib_fftwf_fftw_malloc
    LDFLAGS="-static $FFTW_LDFLAGS $tmp_LDFLAGS"
    LIBS="$tmp_LIBS"
    AC_CHECK_LIB(fftw3, fftw_malloc, [],
      AC_MSG_ERROR([Static fftw library not found.]))
    AC_CHECK_LIB(fftw3f, fftw_malloc, [],
      AC_MSG_ERROR([Static fftwf library not found.]))
    FFTW_LIBS="$LDFLAGS $LIBS"
    AC_SUBST(FFTW_STATIC_LIBS)
  fi

  CPPFLAGS="$tmp_CPPFLAGS"
  LDFLAGS="$tmp_LDFLAGS"
  LIBS="$tmp_LIBS"
fi

#
# Check for OpenCV
#
if test "x$OPENCV_LIBS" == "x"; then

  tmp_CPPFLAGS="$CPPFLAGS"
  tmp_LDFLAGS="$LDFLAGS"
  tmp_LIBS="$LIBS"

  if test "$with_opencv" != "autodetect"; then
    OPENCV_LDFLAGS="-L$with_opencv/lib"
    CPPFLAGS+="-I$with_opencv/include"
    LDFLAGS+="$OPENCV_LDFLAGS"
  fi

  AC_CHECK_HEADER([opencv2/core/core.hpp], [],
    AC_MSG_ERROR([OpenCV core header (opencv2/core/core.hpp) not found. Check the --with-opencv configure option.]))
  AC_CHECK_HEADER([opencv2/imgproc/imgproc.hpp], [],
    AC_MSG_ERROR([OpenCV Imageprocessing header (opencv2/imgproc/imgproc.hpp) not found. Check the --with-opencv configure option.]))
  AC_CHECK_LIB(opencv_core, cvError, [
    LIBS="-lopencv_core $LIBS"
    AC_CHECK_LIB(opencv_imgproc, cvFitEllipse2, [],
      AC_MSG_ERROR([OpenCV Image Processing (libopencv_imgproc) library not found. Check the --with-opencv configure option.]))], [
    AC_CHECK_LIB(cxcore, cvError, [
      LIBS="-lcxcore $LIBS"
      AC_CHECK_LIB(cv, cvFitEllipse2, [], [
        AC_MSG_ERROR([OpenCV (libcv) library not found. ])])], [
      AC_MSG_ERROR([OpenCV core (libcxcore) library not found. Check the --with-opencv configure option.])])])
  OPENCV_CFLAGS="$CPPFLAGS"
  OPENCV_LIBS="$OPENCV_LDFLAGS $LIBS"
  AC_SUBST(OPENCV_CFLAGS)
  AC_SUBST(OPENCV_LIBS)

  if test "x$enable_static_tools" != "xno"; then
#    $as_unset ac_cv_lib_opencv_core_cvError
#    $as_unset ac_cv_lib_opencv_imgproc_cvFitEllipse2
#    LDFLAGS="-static $OPENCV_LDFLAGS $tmp_LDFLAGS"
#    LIBS="$tmp_LIBS"
#    AC_CHECK_LIB(opencv_core, cvError, [],
#      [AC_MSG_ERROR([Static OpenCV core (opencv_core) library not found.])])
#    AC_CHECK_LIB(opencv_imgproc, cvFitEllipse2, [],
#      [AC_MSG_ERROR([Static OpenCV Image Processing (opencv_imgproc) library not found.])])
    OPENCV_STATIC_LIBS="$OPENCV_LDFLAGS $LIBS"
    AC_SUBST(OPENCV_STATIC_LIBS)
  fi

  CPPFLAGS="$tmp_CPPFLAGS"
  LDFLAGS="$tmp_LDFLAGS"
  LIBS="$tmp_LIBS"
fi

if test "x$GLU_LIBS" == "x"; then
  tmp_CPPFLAGS="$CPPFLAGS"
  tmp_LDFLAGS="$LDFLAGS"
  tmp_LIBS="$LIBS"

  AC_CHECK_LIB(GLU,gluLookAt, [], AC_MSG_ERROR([OpenGL utility library (libGLU) not found.]))

  GLU_CFLAGS="$CPPFLAGS"
  GLU_LIBS="$LIBS"
  AC_SUBST(GLU_CFLAGS)
  AC_SUBST(GLU_LIBS)

  CPPFLAGS="$tmp_CPPFLAGS"
  LDFLAGS="$tmp_LDFLAGS"
  LIBS="$tmp_LIBS"
fi

#
# Check for QT4
#   (Linux X11 specific, for Windows make sure pkg-config does the
#    configuration!!!)
#
if test "x$QT4_LIBS" == "x"; then
  tmp_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS=""
  tmp_LDFLAGS="$LDFLAGS"
  LDFLAGS=""
  tmp_LIBS="$LIBS"
  LIBS=""
  if test "$with_qt4" != "autodetect"; then
    CPPFLAGS="-I$with_qt4/include/"
    LDFLAGS="-L$with_qt4/lib -L/usr/X11R6/lib"
    BINPATH="$with_qt4/bin"
  fi
 
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
    [$BINPATH])
  AC_PATH_PROG(QT4_UIC, uic,
    AC_MSG_ERROR(
      [Could not find $with_qt4/bin/uic. Please correct your QT4 path]),
    [$BINPATH])
  AC_PATH_PROG(QT4_RCC, rcc,
    AC_MSG_ERROR(
      [Could not find $with_qt4/bin/rcc. Please correct your QT4 path]),
    [$BINPATH])
  QT4_CFLAGS="$CPPFLAGS"
  QT4_LIBS="$LDFLAGS $LIBS"
  CPPFLAGS="$tmp_CPPFLAGS"
  LDFLAGS="$tmp_LDFLAGS"
  LIBS="$tmp_LIBS"
  AC_SUBST(QT4_CFLAGS)
  AC_SUBST(QT4_LIBS)
  AC_SUBST(QT4_MOC)
  AC_SUBST(QT4_RCC)
  AC_SUBST(QT4_UIC)
fi

LIBS+=" $OPENMP_LIBS"

if test "x$enable_static_tools" != "xno"; then
    toollinktype="static"
else
    toollinktype="dynamic"
fi

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/libcmdline/Makefile])
AC_CONFIG_FILES([src/libsvmtl/Makefile])
AC_CONFIG_FILES([src/libProgressReporter/Makefile])
AC_CONFIG_FILES([src/libProgressReporter_qt4/Makefile])
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
AC_CONFIG_FILES([src/plugins/RootSegmentation/Makefile])
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

AC_MSG_RESULT([
-------------------------------------------------
iRoCS-Toolbox $PACKAGE_VERSION configured.

Command line tools link type: $toollinktype

Using
  jpeg:                                autodetect
  JPEG_CFLAGS="$JPEG_CFLAGS"  JPEG_LIBS="$JPEG_LIBS"  JPEG_STATIC_LIBS="$JPEG_STATIC_LIBS"
  tiff:                                autodetect
  TIFF_CFLAGS="$TIFF_CFLAGS"  TIFF_LIBS="$TIFF_LIBS"  TIFF_STATIC_LIBS="$TIFF_STATIC_LIBS"
  blitz++:                             $with_blitz
  BLITZ_CFLAGS="$BLITZ_CFLAGS"  BLITZ_LIBS="$BLITZ_LIBS"  BLITZ_STATIC_LIBS="$BLITZ_STATIC_LIBS"
  GNU scientific library:              $with_gsl
  GSL_CFLAGS="$GSL_CFLAGS"  GSL_LIBS="$GSL_LIBS"  GSL_STATIC_LIBS="$GSL_STATIC_LIBS"
  hdf5:                                $with_hdf5
  HDF5_CFLAGS="$HDF5_CFLAGS"  HDF5_LIBS="$HDF5_LIBS"  HDF5_STATIC_LIBS="$HDF5_STATIC_LIBS"
  fftw:                                $with_fftw
  FFTW_CFLAGS="$FFTW_CFLAGS"  FFTW_LIBS="$FFTW_LIBS"  FFTW_STATIC_LIBS="$FFTW_STATIC_LIBS"
  OpenCV:                              $with_opencv
  OPENCV_CFLAGS="$OPENCV_CFLAGS"  OPENCV_LIBS="$OPENCV_LIBS"  OPENCV_STATIC_LIBS="$OPENCV_STATIC_LIBS"
  Qt4:                                 $with_qt4
  QT4_CFLAGS="$QT4_CFLAGS"  QT4_LIBS="$QT4_LIBS"

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
