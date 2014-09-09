/**************************************************************************
**       Title: Test suite for functions of BlitzFFTW
**    $RCSfile$
**   $Revision: 4591 $ $Name$
**       $Date: 2011-02-08 17:17:52 +0100 (Tue, 08 Feb 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.5  2007/11/27 17:35:55  fehr
**  complex2complex added, new interface: plan->plan_forward/backward, execute->forward/backward
**
**  Revision 1.4  2007/11/06 10:51:02  tschmidt
**  - Added basic unit tests for 1D and 2D, it should work for n-D but is
**    not tested yet
**  - Minor bug fixes
**
**  Revision 1.3  2007/11/05 17:11:27  tschmidt
**  - Added DataPreservalPolicy. The user can decide whether the given Arrays
**    may be overwritten or whether local copies are used where necessary to
**    preserve the original data.
**
**  Revision 1.2  2007/11/05 10:55:11  tschmidt
**  - Added getPaddedShape method, to rapidly find out which shape a padded
**    array would have
**  - Added nextBestFFTSize(int)... has been forgotten
**
**  Revision 1.1.1.1  2007/11/03 01:41:53  tschmidt
**  - Initial revision
**
**
**************************************************************************/

#include "lmbunit.hh"
#include <libBlitzFFTW/BlitzFFTW.hh>

static void testSingleton() 
{
  BlitzFFTW<double>* fftProc = BlitzFFTW<double>::instance();
  LMBUNIT_ASSERT_EQUAL(BlitzFFTW<double>::instance(), fftProc);

  BlitzFFTW<double>* fftDoubleProc = BlitzFFTW<double>::instance();
  LMBUNIT_ASSERT_EQUAL(BlitzFFTW<double>::instance(), fftDoubleProc);
}

static void testGetPaddedShape() 
{
  blitz::TinyVector<int,1> shape1D(7);
  LMBUNIT_ASSERT_EQUAL(
      BlitzFFTW<double>::instance()->getPaddedShape(shape1D)(0), 8);
  blitz::TinyVector<int,2> shape2D(1, 25);
  LMBUNIT_ASSERT_EQUAL(
      BlitzFFTW<double>::instance()->getPaddedShape(shape2D)(0), 2);
  LMBUNIT_ASSERT_EQUAL(
      BlitzFFTW<double>::instance()->getPaddedShape(shape2D)(1), 30);
  blitz::TinyVector<int,3> shape3D(15, 27, 13);
  LMBUNIT_ASSERT_EQUAL(
      BlitzFFTW<double>::instance()->getPaddedShape(shape3D)(0), 16);
  LMBUNIT_ASSERT_EQUAL(
      BlitzFFTW<double>::instance()->getPaddedShape(shape3D)(1), 30);
  LMBUNIT_ASSERT_EQUAL(
      BlitzFFTW<double>::instance()->getPaddedShape(shape3D)(2), 16);
}

static void testPadUnpad() 
{
  blitz::Array<double,1> data1D(7);

  for (int i = 0; i < 7; ++i)
      data1D(i) = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);

  blitz::Array<double,1> padded1D;
  blitz::TinyVector<int,1> lb1D;
  blitz::TinyVector<int,1> ub1D;

  BlitzFFTW<double>::instance()->pad(data1D, padded1D, lb1D, ub1D);
  
  LMBUNIT_ASSERT_EQUAL(padded1D.extent(0), 8);
  LMBUNIT_ASSERT_EQUAL(lb1D(0), 1);
  LMBUNIT_ASSERT_EQUAL(ub1D(0), 7);
  LMBUNIT_ASSERT_EQUAL(padded1D(0), 0);
  for (int i = lb1D(0); i <= ub1D(0); ++i)
      LMBUNIT_ASSERT_EQUAL(data1D(i - lb1D(0)), padded1D(i));
  
  blitz::Array<double,1> unpadded1D;
  BlitzFFTW<double>::instance()->unpad(padded1D, unpadded1D, lb1D, ub1D);
  
  LMBUNIT_ASSERT_EQUAL(unpadded1D.extent(0), 7);
  for (int i = 0; i < 7; ++i)
      LMBUNIT_ASSERT_EQUAL(data1D(i), unpadded1D(i));

  blitz::Array<double,2> data2D(1, 25);

  for (int i = 0; i < 25; ++i) 
      data2D(0, i) = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);

  blitz::Array<double,2> padded2D;
  blitz::TinyVector<ptrdiff_t,2> lb2D;
  blitz::TinyVector<ptrdiff_t,2> ub2D;
  blitz::TinyVector<ptrdiff_t,2> minShape2(data2D.shape());

  BlitzFFTW<double>::instance()->pad(data2D, padded2D, lb2D, ub2D,
                                     minShape2, BlitzFFTW<double>::VALUE, 42.0);
  
  LMBUNIT_ASSERT_EQUAL(padded2D.extent(0), 2);
  LMBUNIT_ASSERT_EQUAL(padded2D.extent(1), 30);
  LMBUNIT_ASSERT_EQUAL(lb2D(0), 1);
  LMBUNIT_ASSERT_EQUAL(lb2D(1), 3);
  LMBUNIT_ASSERT_EQUAL(ub2D(0), 1);
  LMBUNIT_ASSERT_EQUAL(ub2D(1), 27);
  blitz::TinyVector<int,2> pos2D;
  for (pos2D(0) = 0; pos2D(0) < padded2D.extent(0); ++pos2D(0)) 
  {
    for (pos2D(1) = 0; pos2D(1) < padded2D.extent(1); ++pos2D(1)) 
    {
      blitz::TinyVector<int,2> srcPos;
      srcPos = pos2D - lb2D;
      if (pos2D(0) >= lb2D(0) && pos2D(0) <= ub2D(0) &&
          pos2D(1) >= lb2D(1) && pos2D(1) <= ub2D(1)) 
      {
        LMBUNIT_ASSERT_EQUAL(padded2D(pos2D), data2D(srcPos));
      }
      else
      {
        LMBUNIT_ASSERT_EQUAL(padded2D(pos2D), 42);
      }
    }
  }

  blitz::Array<double,2> unpadded2D;
  BlitzFFTW<double>::instance()->unpad(padded2D, unpadded2D, lb2D, ub2D);
  
  LMBUNIT_ASSERT_EQUAL(unpadded2D.extent(0), 1);
  LMBUNIT_ASSERT_EQUAL(unpadded2D.extent(1), 25);
  for (pos2D(0) = 0; pos2D(0) < unpadded2D.extent(0); ++pos2D(0)) 
  {
    for (pos2D(1) = 0; pos2D(1) < unpadded2D.extent(1); ++pos2D(1)) 
    {
      LMBUNIT_ASSERT_EQUAL(data2D(pos2D), unpadded2D(pos2D));
    }
  }

  blitz::Array<double,3> data3D(15, 27, 13);

  for (blitz::Array<double,3>::iterator it = data3D.begin();
       it != data3D.end(); ++it) 
  {
    *it = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
  }
  
  blitz::Array<double,3> padded3D;
  blitz::TinyVector<ptrdiff_t,3> lb3D;
  blitz::TinyVector<ptrdiff_t,3> ub3D;

  blitz::TinyVector<ptrdiff_t,3> minShape = (3 * data3D.shape()) / 2;

  BlitzFFTW<double>::instance()->pad(data3D, padded3D, lb3D, ub3D,
                                    minShape,
                                    BlitzFFTW<double>::REPEATBORDER);
  
  LMBUNIT_ASSERT_EQUAL(padded3D.extent(0), 24);
  LMBUNIT_ASSERT_EQUAL(padded3D.extent(1), 40);
  LMBUNIT_ASSERT_EQUAL(padded3D.extent(2), 20);

  LMBUNIT_ASSERT_EQUAL(lb3D(0), 5);
  LMBUNIT_ASSERT_EQUAL(lb3D(1), 7);
  LMBUNIT_ASSERT_EQUAL(lb3D(2), 4);
  LMBUNIT_ASSERT_EQUAL(ub3D(0), 19);
  LMBUNIT_ASSERT_EQUAL(ub3D(1), 33);
  LMBUNIT_ASSERT_EQUAL(ub3D(2), 16);
  
  blitz::TinyVector<int,3> pos3D;
  for (pos3D(0) = 0; pos3D(0) < padded3D.extent(0); ++pos3D(0)) 
  {
    for (pos3D(1) = 0; pos3D(1) < padded3D.extent(1); ++pos3D(1)) 
    {
      for (pos3D(2) = 0; pos3D(2) < padded3D.extent(2); ++pos3D(2)) 
      {
        blitz::TinyVector<int,3> srcPos;
        srcPos = pos3D - lb3D;
        for (int d = 0; d < 3; ++d) 
        {
          if (srcPos(d) < 0) srcPos(d) = 0;
          if (srcPos(d) >= data3D.extent(d)) srcPos(d) = data3D.extent(d) - 1;
        }
        LMBUNIT_ASSERT_EQUAL(data3D(srcPos), padded3D(pos3D));
      }
    }
  }
  
  BlitzFFTW<double>::instance()->pad(data3D, padded3D, lb3D, ub3D,
                                    minShape,
                                    BlitzFFTW<double>::MIRRORBORDER);
  
  LMBUNIT_ASSERT_EQUAL(padded3D.extent(0), 24);
  LMBUNIT_ASSERT_EQUAL(padded3D.extent(1), 40);
  LMBUNIT_ASSERT_EQUAL(padded3D.extent(2), 20);

  LMBUNIT_ASSERT_EQUAL(lb3D(0), 5);
  LMBUNIT_ASSERT_EQUAL(lb3D(1), 7);
  LMBUNIT_ASSERT_EQUAL(lb3D(2), 4);
  LMBUNIT_ASSERT_EQUAL(ub3D(0), 19);
  LMBUNIT_ASSERT_EQUAL(ub3D(1), 33);
  LMBUNIT_ASSERT_EQUAL(ub3D(2), 16);
  
  // Some picked values
  LMBUNIT_ASSERT_EQUAL(padded3D(lb3D), data3D(0, 0, 0));
  LMBUNIT_ASSERT_EQUAL(
      padded3D(blitz::TinyVector<ptrdiff_t,3>(lb3D - 1)), data3D(1, 1, 1));  
  LMBUNIT_ASSERT_EQUAL(
      padded3D(ub3D),
      data3D(blitz::TinyVector<ptrdiff_t,3>(data3D.shape() - 1)));
  LMBUNIT_ASSERT_EQUAL(
      padded3D(blitz::TinyVector<ptrdiff_t,3>(ub3D + 1)),
      data3D(blitz::TinyVector<ptrdiff_t,3>(data3D.shape() - 2)));  

  for (pos3D(0) = 0; pos3D(0) < padded3D.extent(0); ++pos3D(0)) 
  {
    for (pos3D(1) = 0; pos3D(1) < padded3D.extent(1); ++pos3D(1)) 
    {
      for (pos3D(2) = 0; pos3D(2) < padded3D.extent(2); ++pos3D(2)) 
      {
        blitz::TinyVector<int,3> srcPos;
        srcPos = pos3D - lb3D;
        for (int d = 0; d < 3; ++d) 
        {
          if (srcPos(d) < 0) srcPos(d) = -srcPos(d);
          ptrdiff_t n = srcPos(d) / (data3D.extent(d) - 1);
          bool inc = (n % 2 == 0);
          if (inc) srcPos(d) -= n * (data3D.extent(d) - 1);
          else srcPos(d) = (n + 1) * (data3D.extent(d) - 1) - srcPos(d);
        }
        LMBUNIT_ASSERT_EQUAL(data3D(srcPos), padded3D(pos3D));
      }
    }
  }
  
  blitz::Array<double,3> unpadded3D;
  BlitzFFTW<double>::instance()->unpad(padded3D, unpadded3D, lb3D, ub3D);
  
  for (pos3D(0) = 0; pos3D(0) < unpadded3D.extent(0); ++pos3D(0)) 
  {
    for (pos3D(1) = 0; pos3D(1) < unpadded3D.extent(1); ++pos3D(1)) 
    {
      for (pos3D(2) = 0; pos3D(2) < unpadded3D.extent(2); ++pos3D(2)) 
      {
        LMBUNIT_ASSERT_EQUAL(data3D(pos3D), unpadded3D(pos3D));
      }
    }
  }
}

static void testFFT1DDoubleWithDataPreserval() 
{
  int baseSize = 12;

  BlitzFFTW<double>* fftProc = BlitzFFTW<double>::instance();
  
  blitz::Array<double,1> data(baseSize);
  blitz::Array<double,1> dataCopy(baseSize);
  blitz::Array<std::complex<double>,1> fft(baseSize / 2 + 1);
  blitz::Array<std::complex<double>,1> fftCopy(baseSize / 2 + 1);

  for (int i = 0; i < baseSize; ++i)
      data(i) = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
  dataCopy = data;

  for (int i = 0; i < baseSize / 2 + 1; ++i)
      fft(i) = std::complex<double>(static_cast<double>(rand()) / static_cast<double>(RAND_MAX),
                                   static_cast<double>(rand()) / static_cast<double>(RAND_MAX));
  fftCopy = fft;

  fftProc->plan_forward(data, fft);

  for (int i = 0; i < baseSize; ++i)
      LMBUNIT_ASSERT_EQUAL(data(i), dataCopy(i));
  for (int i = 0; i < baseSize / 2 + 1; ++i)
      LMBUNIT_ASSERT_EQUAL(fft(i), fftCopy(i));

  fftProc->plan_backward(fft, data);
  
  for (int i = 0; i < baseSize; ++i)
      LMBUNIT_ASSERT_EQUAL(data(i), dataCopy(i));
  for (int i = 0; i < baseSize / 2 + 1; ++i)
      LMBUNIT_ASSERT_EQUAL(fft(i), fftCopy(i));

  dataCopy = 1;

  fftProc->forward(dataCopy, fftCopy);
  fft = fftCopy;

  for (int i = 0; i < baseSize; ++i)
      LMBUNIT_ASSERT_EQUAL(dataCopy(i), 1);

  LMBUNIT_ASSERT_EQUAL_DELTA(fftCopy(0).real(), baseSize, 1e-10f);
  LMBUNIT_ASSERT_EQUAL_DELTA(fftCopy(0).imag(), 0.0f, 1e-10f);
  for (int i = 1; i < baseSize / 2 + 1; ++i) 
  {
    LMBUNIT_ASSERT_EQUAL_DELTA(fftCopy(i).real(), 0.0f, 1e-10f);
    LMBUNIT_ASSERT_EQUAL_DELTA(fftCopy(i).imag(), 0.0f, 1e-10f);
  }

  fftProc->backward(fftCopy, dataCopy);

  dataCopy /= dataCopy.size();
  
  for (int i = 0; i < baseSize; ++i)
      LMBUNIT_ASSERT_EQUAL_DELTA(dataCopy(i), 1, 1e-10);

  for (int i = 0; i < baseSize / 2 + 1; ++i)
      LMBUNIT_ASSERT_EQUAL(fftCopy(i), fft(i));
}

static void testFFT1DDoubleWithoutDataPreserval() 
{
  int baseSize = 12;

  BlitzFFTW<double>* fftProc = BlitzFFTW<double>::instance();
  
  blitz::Array<double,1> data(baseSize);
  blitz::Array<std::complex<double>,1> fft(baseSize / 2 + 1);

  fftProc->plan_forward(data, fft);
  fftProc->plan_backward(fft, data);

  data = 1;

  fftProc->forward(data, fft);

  for (int i = 0; i < baseSize; ++i)
      LMBUNIT_ASSERT_EQUAL(data(i), 1);

  LMBUNIT_ASSERT_EQUAL_DELTA(fft(0).real(), baseSize, 1e-10f);
  LMBUNIT_ASSERT_EQUAL_DELTA(fft(0).imag(), 0.0f, 1e-10f);
  for (int i = 1; i < baseSize / 2 + 1; ++i) 
  {
    LMBUNIT_ASSERT_EQUAL_DELTA(fft(i).real(), 0.0f, 1e-10f);
    LMBUNIT_ASSERT_EQUAL_DELTA(fft(i).imag(), 0.0f, 1e-10f);
  }

  fftProc->backward(fft, data);

  data /= data.size();
  
  for (int i = 0; i < baseSize; ++i)
      LMBUNIT_ASSERT_EQUAL_DELTA(data(i), 1, 1e-10);
}

static void testFFT2DDoubleWithDataPreserval() 
{
  int baseSize = 12;

  BlitzFFTW<double>* fftProc = BlitzFFTW<double>::instance();
  
  blitz::Array<double,2> data(baseSize, baseSize);
  blitz::Array<double,2> dataCopy(baseSize, baseSize);
  blitz::Array<std::complex<double>,2> fft(baseSize, baseSize / 2 + 1);
  blitz::Array<std::complex<double>,2> fftCopy(baseSize, baseSize / 2 + 1);

  for (blitz::Array<double,2>::iterator it = data.begin();
       it != data.end(); ++it)
      *it = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
  dataCopy = data;

  for (blitz::Array<std::complex<double>,2>::iterator it = fft.begin();
       it != fft.end(); ++it)
      *it = std::complex<double>(
          static_cast<double>(rand()) / static_cast<double>(RAND_MAX),
          static_cast<double>(rand()) / static_cast<double>(RAND_MAX));
  fftCopy = fft;

  fftProc->plan_forward(data, fft);

  blitz::TinyVector<int,2> p;
  for (p(0) = 0; p(0) < data.extent(0); ++p(0)) 
  {
    for (p(1) = 0; p(1) < data.extent(1); ++p(1)) 
    {
      LMBUNIT_ASSERT_EQUAL(data(p), dataCopy(p));
    }
  }
  for (p(0) = 0; p(0) < fft.extent(0); ++p(0)) 
  {
    for (p(1) = 0; p(1) < fft.extent(1); ++p(1)) 
    {
      LMBUNIT_ASSERT_EQUAL(fft(p), fftCopy(p));
    }
  }
  
  fftProc->plan_backward(fft, data);
  
  for (p(0) = 0; p(0) < data.extent(0); ++p(0)) 
  {
    for (p(1) = 0; p(1) < data.extent(1); ++p(1)) 
    {
      LMBUNIT_ASSERT_EQUAL(data(p), dataCopy(p));
    }
  }
  for (p(0) = 0; p(0) < fft.extent(0); ++p(0)) 
  {
    for (p(1) = 0; p(1) < fft.extent(1); ++p(1)) 
    {
      LMBUNIT_ASSERT_EQUAL(fft(p), fftCopy(p));
    }
  }
  
  dataCopy = 1;

  fftProc->forward(dataCopy, fftCopy);
  fft = fftCopy;

  for (p(0) = 0; p(0) < dataCopy.extent(0); ++p(0)) 
  {
    for (p(1) = 0; p(1) < dataCopy.extent(1); ++p(1)) 
    {
      LMBUNIT_ASSERT_EQUAL(dataCopy(p), 1);
    }
  }
  for (p(0) = 0; p(0) < fft.extent(0); ++p(0)) 
  {
    for (p(1) = 0; p(1) < fft.extent(1); ++p(1)) 
    {
      if (p(0) == 0 && p(1) == 0) 
      {
        LMBUNIT_ASSERT_EQUAL_DELTA(fftCopy(p).real(),
                                   baseSize * baseSize, 1e-10f);
        LMBUNIT_ASSERT_EQUAL_DELTA(fftCopy(p).imag(), 0.0f, 1e-10f);
      }
      else 
      {
        LMBUNIT_ASSERT_EQUAL_DELTA(fftCopy(p).real(), 0.0f, 1e-10f);
        LMBUNIT_ASSERT_EQUAL_DELTA(fftCopy(p).imag(), 0.0f, 1e-10f);
      }
    }
  }

  fftProc->backward(fftCopy, data);

  data /= data.size();
  
  for (p(0) = 0; p(0) < data.extent(0); ++p(0)) 
  {
    for (p(1) = 0; p(1) < data.extent(1); ++p(1)) 
    {
      LMBUNIT_ASSERT_EQUAL_DELTA(data(p), 1.0f, 1e-10f);
    }
  }
  for (p(0) = 0; p(0) < fft.extent(0); ++p(0)) 
  {
    for (p(1) = 0; p(1) < fft.extent(1); ++p(1)) 
    {
      LMBUNIT_ASSERT_EQUAL(fft(p), fftCopy(p));
    }
  }
}

static void testFFT2DDoubleWithoutDataPreserval() 
{
  int baseSize = 12;

  BlitzFFTW<double>* fftProc = BlitzFFTW<double>::instance();
  
  blitz::Array<double,2> data(baseSize, baseSize);
  blitz::Array<std::complex<double>,2> fft(baseSize, baseSize / 2 + 1);

  fftProc->plan_forward(data, fft);
  fftProc->plan_backward(fft, data);

  data = 1;

  fftProc->forward(data, fft);

  blitz::TinyVector<int,2> p;
  for (p(0) = 0; p(0) < data.extent(0); ++p(0)) 
  {
    for (p(1) = 0; p(1) < data.extent(1); ++p(1)) 
    {
      LMBUNIT_ASSERT_EQUAL(data(p), 1);
    }
  }
  for (p(0) = 0; p(0) < fft.extent(0); ++p(0)) 
  {
    for (p(1) = 0; p(1) < fft.extent(1); ++p(1)) 
    {
      if (p(0) == 0 && p(1) == 0) 
      {
        LMBUNIT_ASSERT_EQUAL_DELTA(fft(p).real(),
                                   baseSize * baseSize, 1e-10f);
        LMBUNIT_ASSERT_EQUAL_DELTA(fft(p).imag(), 0.0f, 1e-10f);
      }
      else 
      {
        LMBUNIT_ASSERT_EQUAL_DELTA(fft(p).real(), 0.0f, 1e-10f);
        LMBUNIT_ASSERT_EQUAL_DELTA(fft(p).imag(), 0.0f, 1e-10f);
      }
    }
  }

  fftProc->backward(fft, data);

  data /= data.size();
  
  for (p(0) = 0; p(0) < data.extent(0); ++p(0)) 
  {
    for (p(1) = 0; p(1) < data.extent(1); ++p(1)) 
    {
      LMBUNIT_ASSERT_EQUAL_DELTA(data(p), 1.0f, 1e-10f);
    }
  }
}

static void testComplex2ComplexDoubleWithoutDataPreserval()
{
  int baseSize = 12;

  BlitzFFTW<double>* fftProc = BlitzFFTW<double>::instance();

  blitz::Array<std::complex<double>,2> data(baseSize, baseSize);
  blitz::Array<std::complex<double>,2> fft(baseSize, baseSize)
;

  fftProc->plan_forward(data, fft);
  fftProc->plan_backward(fft, data);

  data = 1;

  fftProc->forward(data, fft);

  blitz::TinyVector<int,2> p;
  for (p(0) = 0; p(0) < data.extent(0); ++p(0))
  {
    for (p(1) = 0; p(1) < data.extent(1); ++p(1))
    {
      LMBUNIT_ASSERT_EQUAL(data(p), (std::complex<double>)1);
    }
  }
  for (p(0) = 0; p(0) < fft.extent(0); ++p(0))
  {
    for (p(1) = 0; p(1) < fft.extent(1); ++p(1))
    {
      if (p(0) == 0 && p(1) == 0)
      {
        LMBUNIT_ASSERT_EQUAL_DELTA(fft(p).real(),
                                   baseSize * baseSize, 1e-10f);
        LMBUNIT_ASSERT_EQUAL_DELTA(fft(p).imag(), 0.0f, 1e-10f);
      }
      else
      {
        LMBUNIT_ASSERT_EQUAL_DELTA(fft(p).real(), 0.0f, 1e-10f);
        LMBUNIT_ASSERT_EQUAL_DELTA(fft(p).imag(), 0.0f, 1e-10f);
      }
    }
  }

  fftProc->backward(fft, data);

  data /= data.size();

  for (p(0) = 0; p(0) < data.extent(0); ++p(0))
  {
    for (p(1) = 0; p(1) < data.extent(1); ++p(1))
    {
      LMBUNIT_ASSERT_EQUAL_DELTA(data(p).real(), 1.0f, 1e-10f);
      LMBUNIT_ASSERT_EQUAL_DELTA(data(p).imag(), 0.0f, 1e-10f);
    }
  }
}


static void testUnShuffle() 
{
  blitz::Array<double,3> data(10, 10, 10);
  data = 0;
  data(blitz::TinyVector<ptrdiff_t,3>(data.shape() / 2)) = 1;
  BlitzFFTW<double>::instance()->unShuffle(data, data);
  LMBUNIT_ASSERT_EQUAL(data(0, 0, 0), 1);
}


int main() 
{
  
  LMBUNIT_WRITE_HEADER();
  
  LMBUNIT_RUN_TEST(testSingleton());
  LMBUNIT_RUN_TEST(testGetPaddedShape());
  LMBUNIT_RUN_TEST(testPadUnpad());
  LMBUNIT_RUN_TEST(testFFT1DDoubleWithDataPreserval());
  LMBUNIT_RUN_TEST(testFFT1DDoubleWithoutDataPreserval());
  LMBUNIT_RUN_TEST(testFFT2DDoubleWithDataPreserval());
  LMBUNIT_RUN_TEST(testFFT2DDoubleWithoutDataPreserval());
  LMBUNIT_RUN_TEST(testComplex2ComplexDoubleWithoutDataPreserval());
  LMBUNIT_RUN_TEST(testUnShuffle());
  LMBUNIT_WRITE_STATISTICS();
  
  return _nFails;
}
