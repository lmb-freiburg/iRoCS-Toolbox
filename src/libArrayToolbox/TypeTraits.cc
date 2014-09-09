/**************************************************************************
**       Title: General type traits
**    $RCSfile$
**   $Revision: 3896 $ $Name$
**       $Date: 2008-07-16 15:17:47 +0200 (Wed, 16 Jul 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Templates to obtain specific information about used types
**
**************************************************************************/

#include "TypeTraits.hh"

namespace atb
{
  
  bool const traits<bool>::smallest = false;
  bool const traits<bool>::greatest = true;
  bool const traits<bool>::zero = false;
  bool const traits<bool>::one = true;
  bool const traits<bool>::saturated = true;
  bool const traits<bool>::isComplex = false;


  unsigned char const traits<unsigned char>::smallest = 0;
  unsigned char const traits<unsigned char>::greatest = 255;
  unsigned char const traits<unsigned char>::zero = 0;
  unsigned char const traits<unsigned char>::one = 1;
  unsigned char const traits<unsigned char>::saturated = 255;
  bool const traits<unsigned char>::isComplex = false;


  char const traits<char>::smallest = -128;
  char const traits<char>::greatest = 127;
  char const traits<char>::zero = 0;
  char const traits<char>::one = 1;
  char const traits<char>::saturated = 127;
  bool const traits<char>::isComplex = false;


  unsigned short const traits<unsigned short>::smallest = 0;
  unsigned short const traits<unsigned short>::greatest = 65535;
  unsigned short const traits<unsigned short>::zero = 0;
  unsigned short const traits<unsigned short>::one = 1;
  unsigned short const traits<unsigned short>::saturated = 65535;
  bool const traits<unsigned short>::isComplex = false;


  short const traits<short>::smallest = -32768;
  short const traits<short>::greatest = 32767;
  short const traits<short>::zero = 0;
  short const traits<short>::one = 1;
  short const traits<short>::saturated = 32767;
  bool const traits<short>::isComplex = false;


  unsigned int const traits<unsigned int>::smallest = 0;
  unsigned int const traits<unsigned int>::greatest =
      std::numeric_limits<unsigned int>::max();
  unsigned int const traits<unsigned int>::zero = 0;
  unsigned int const traits<unsigned int>::one = 1;
  unsigned int const traits<unsigned int>::saturated = 65535;
  bool const traits<unsigned int>::isComplex = false;


  int const traits<int>::smallest = std::numeric_limits<int>::min();
  int const traits<int>::greatest = std::numeric_limits<int>::max();
  int const traits<int>::zero = 0;
  int const traits<int>::one = 1;
  int const traits<int>::saturated = 65535;
  bool const traits<int>::isComplex = false;


  unsigned long const traits<unsigned long>::smallest = 0;
  unsigned long const traits<unsigned long>::greatest =
      std::numeric_limits<unsigned long>::max();
  unsigned long const traits<unsigned long>::zero = 0;
  unsigned long const traits<unsigned long>::one = 1;
  unsigned long const traits<unsigned long>::saturated = 65535;
  bool const traits<unsigned long>::isComplex = false;


  long const traits<long>::smallest = std::numeric_limits<long>::min();
  long const traits<long>::greatest = std::numeric_limits<long>::max();
  long const traits<long>::zero = 0;
  long const traits<long>::one = 1;
  long const traits<long>::saturated = 65535;
  bool const traits<long>::isComplex = false;


  unsigned long long const traits<unsigned long long>::smallest = 0;
  unsigned long long const traits<unsigned long long>::greatest =
      std::numeric_limits<unsigned long long>::max();
  unsigned long long const traits<unsigned long long>::zero = 0ll;
  unsigned long long const traits<unsigned long long>::one = 1ll;
  unsigned long long const traits<unsigned long long>::saturated = 65535ll;
  bool const traits<unsigned long long>::isComplex = false;


  long long const traits<long long>::smallest =
      std::numeric_limits<unsigned long long>::min();
  long long const traits<long long>::greatest =
      std::numeric_limits<unsigned long long>::max();
  long long const traits<long long>::zero = 0ll;
  long long const traits<long long>::one = 1ll;
  long long const traits<long long>::saturated = 65535ll;
  bool const traits<long long>::isComplex = false;


  float const traits<float>::smallest = -std::numeric_limits<float>::infinity();
  float const traits<float>::greatest = std::numeric_limits<float>::infinity();
  float const traits<float>::zero = 0.0f;
  float const traits<float>::one = 1.0f;
  float const traits<float>::saturated = 1.0f;
  bool const traits<float>::isComplex = false;


  double const traits<double>::smallest =
      -std::numeric_limits<double>::infinity();
  double const traits<double>::greatest =
      std::numeric_limits<double>::infinity();
  double const traits<double>::zero = 0.0;
  double const traits<double>::one = 1.0;
  double const traits<double>::saturated = 1.0;
  bool const traits<double>::isComplex = false;


  long double const traits<long double>::smallest =
      -std::numeric_limits<long double>::infinity();
  long double const traits<long double>::greatest =
      std::numeric_limits<long double>::infinity();
  long double const traits<long double>::zero = 0.0;
  long double const traits<long double>::one = 1.0;
  long double const traits<long double>::saturated = 1.0;
  bool const traits<long double>::isComplex = false;


  std::complex<float> const traits< std::complex<float> >::smallest =
      std::complex<float>(-std::numeric_limits<float>::infinity(), 0.0f);
  std::complex<float> const traits< std::complex<float> >::greatest =
      std::complex<float>(std::numeric_limits<float>::infinity(), 0.0f);
  std::complex<float> const traits< std::complex<float> >::zero =
      std::complex<float>(traits<float>::zero, traits<float>::zero);
  std::complex<float> const traits< std::complex<float> >::one =
      std::complex<float>(traits<float>::one, traits<float>::zero);
  std::complex<float> const traits< std::complex<float> >::saturated =
      std::complex<float>(traits<float>::saturated, traits<float>::zero);
  bool const traits< std::complex<float> >::isComplex = true;


  std::complex<double> const traits< std::complex<double> >::smallest =
      std::complex<double>(-std::numeric_limits<double>::infinity(), 0.0);
  std::complex<double> const traits< std::complex<double> >::greatest =
      std::complex<double>(std::numeric_limits<double>::infinity(), 0.0);
  std::complex<double> const traits< std::complex<double> >::zero =
      std::complex<double>(traits<double>::zero, traits<double>::zero);
  std::complex<double> const traits< std::complex<double> >::one =
      std::complex<double>(traits<double>::one, traits<double>::zero);
  std::complex<double> const traits< std::complex<double> >::saturated =
      std::complex<double>(traits<double>::saturated, traits<double>::zero);
  bool const traits< std::complex<double> >::isComplex = true;


  std::complex<long double> const
  traits< std::complex<long double> >::smallest =
      std::complex<long double>(
          -std::numeric_limits<long double>::infinity(), 0.0);
  std::complex<long double> const
  traits< std::complex<long double> >::greatest =
      std::complex<long double>(
          std::numeric_limits<long double>::infinity(), 0.0);
  std::complex<long double> const traits< std::complex<long double> >::zero =
      std::complex<long double>(
          traits<long double>::zero, traits<long double>::zero);
  std::complex<long double> const traits< std::complex<long double> >::one =
      std::complex<long double>(
          traits<long double>::one, traits<long double>::zero);
  std::complex<long double> const
  traits< std::complex<long double> >::saturated =
      std::complex<long double>(
          traits<long double>::saturated, traits<long double>::zero);
  bool const traits< std::complex<long double> >::isComplex = true;

}
