/**************************************************************************
**       Title: Dummy header that uses explicit template instantiation
** Description:
**
**    WARNING: This is a generated file. Edit generate.cc, not this file!
**
**************************************************************************/
#ifndef BLITZFFTW_IMPL_HH
#define BLITZFFTW_IMPL_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// make sure we don't include the inline template definitions
// before the explicit extern template instantiations
#define BLITZFFTW_HH_NO_INCLUDE_ICC
#include <BlitzFFTW.hh>

// extern template does not conform to ISO C++, so we need to
// tell gcc to avoid warnings for this file. Else the -Werror
// option would abort compilation on this warning.
#if __GNUG__ >= 3
#pragma GCC system_header
#endif // gcc > 3

extern template void BlitzFFTW<float>::plan_forward(blitz::Array<float,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<float>::plan_forward(blitz::Array<float,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<float>::plan_forward(blitz::Array<float,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

extern template void BlitzFFTW<double>::plan_forward(blitz::Array<double,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<double>::plan_forward(blitz::Array<double,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<double>::plan_forward(blitz::Array<double,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


extern template void BlitzFFTW<float>::plan_backward(blitz::Array<std::complex<float>,1>& in, blitz::Array<float,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<float>::plan_backward(blitz::Array<std::complex<float>,2>& in, blitz::Array<float,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<float>::plan_backward(blitz::Array<std::complex<float>,3>& in, blitz::Array<float,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

extern template void BlitzFFTW<double>::plan_backward(blitz::Array<std::complex<double>,1>& in, blitz::Array<double,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<double>::plan_backward(blitz::Array<std::complex<double>,2>& in, blitz::Array<double,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<double>::plan_backward(blitz::Array<std::complex<double>,3>& in, blitz::Array<double,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


extern template void BlitzFFTW<float>::plan_forward(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<float>::plan_forward(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<float>::plan_forward(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

extern template void BlitzFFTW<double>::plan_forward(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<double>::plan_forward(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<double>::plan_forward(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


extern template void BlitzFFTW<float>::plan_backward(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<float>::plan_backward(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<float>::plan_backward(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

extern template void BlitzFFTW<double>::plan_backward(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<double>::plan_backward(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template void BlitzFFTW<double>::plan_backward(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


extern template void BlitzFFTW<float>::forward(const blitz::Array<float,1>& in, blitz::Array<std::complex<float>,1>& out) const;
extern template void BlitzFFTW<float>::forward(const blitz::Array<float,2>& in, blitz::Array<std::complex<float>,2>& out) const;
extern template void BlitzFFTW<float>::forward(const blitz::Array<float,3>& in, blitz::Array<std::complex<float>,3>& out) const;

extern template void BlitzFFTW<double>::forward(const blitz::Array<double,1>& in, blitz::Array<std::complex<double>,1>& out) const;
extern template void BlitzFFTW<double>::forward(const blitz::Array<double,2>& in, blitz::Array<std::complex<double>,2>& out) const;
extern template void BlitzFFTW<double>::forward(const blitz::Array<double,3>& in, blitz::Array<std::complex<double>,3>& out) const;


extern template void BlitzFFTW<float>::forward(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out) const;
extern template void BlitzFFTW<float>::forward(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out) const;
extern template void BlitzFFTW<float>::forward(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out) const;

extern template void BlitzFFTW<double>::forward(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out) const;
extern template void BlitzFFTW<double>::forward(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out) const;
extern template void BlitzFFTW<double>::forward(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out) const;


extern template void BlitzFFTW<float>::backward(blitz::Array<std::complex<float>,1>& in, blitz::Array<float,1>& out, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<float>::backward(blitz::Array<std::complex<float>,2>& in, blitz::Array<float,2>& out, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<float>::backward(blitz::Array<std::complex<float>,3>& in, blitz::Array<float,3>& out, const DataPreservePolicy policy) const;

extern template void BlitzFFTW<double>::backward(blitz::Array<std::complex<double>,1>& in, blitz::Array<double,1>& out, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<double>::backward(blitz::Array<std::complex<double>,2>& in, blitz::Array<double,2>& out, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<double>::backward(blitz::Array<std::complex<double>,3>& in, blitz::Array<double,3>& out, const DataPreservePolicy policy) const;


extern template void BlitzFFTW<float>::backward(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<float>::backward(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<float>::backward(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy) const;

extern template void BlitzFFTW<double>::backward(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<double>::backward(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<double>::backward(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy) const;


extern template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_forward(blitz::Array<float,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_forward(blitz::Array<float,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_forward(blitz::Array<float,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

extern template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_forward(blitz::Array<double,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_forward(blitz::Array<double,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_forward(blitz::Array<double,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


extern template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_forward(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_forward(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_forward(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

extern template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_forward(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_forward(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_forward(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


extern template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_backward(blitz::Array<std::complex<float>,1>& in, blitz::Array<float,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_backward(blitz::Array<std::complex<float>,2>& in, blitz::Array<float,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_backward(blitz::Array<std::complex<float>,3>& in, blitz::Array<float,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

extern template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_backward(blitz::Array<std::complex<double>,1>& in, blitz::Array<double,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_backward(blitz::Array<std::complex<double>,2>& in, blitz::Array<double,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_backward(blitz::Array<std::complex<double>,3>& in, blitz::Array<double,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


extern template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_backward(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_backward(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_backward(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

extern template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_backward(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_backward(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
extern template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_backward(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


extern template void BlitzFFTW<float>::exec_guru_plan(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<float>::exec_guru_plan(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<float>::exec_guru_plan(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;

extern template void BlitzFFTW<double>::exec_guru_plan(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<double>::exec_guru_plan(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<double>::exec_guru_plan(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;


extern template void BlitzFFTW<float>::exec_guru_plan_r2c(blitz::Array<float,1>& in, blitz::Array<std::complex<float>,1>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<float>::exec_guru_plan_r2c(blitz::Array<float,2>& in, blitz::Array<std::complex<float>,2>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<float>::exec_guru_plan_r2c(blitz::Array<float,3>& in, blitz::Array<std::complex<float>,3>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;

extern template void BlitzFFTW<double>::exec_guru_plan_r2c(blitz::Array<double,1>& in, blitz::Array<std::complex<double>,1>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<double>::exec_guru_plan_r2c(blitz::Array<double,2>& in, blitz::Array<std::complex<double>,2>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<double>::exec_guru_plan_r2c(blitz::Array<double,3>& in, blitz::Array<std::complex<double>,3>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;


extern template void BlitzFFTW<float>::exec_guru_plan_c2r(blitz::Array<std::complex<float>,1>& in, blitz::Array<float,1>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<float>::exec_guru_plan_c2r(blitz::Array<std::complex<float>,2>& in, blitz::Array<float,2>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<float>::exec_guru_plan_c2r(blitz::Array<std::complex<float>,3>& in, blitz::Array<float,3>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;

extern template void BlitzFFTW<double>::exec_guru_plan_c2r(blitz::Array<std::complex<double>,1>& in, blitz::Array<double,1>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<double>::exec_guru_plan_c2r(blitz::Array<std::complex<double>,2>& in, blitz::Array<double,2>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
extern template void BlitzFFTW<double>::exec_guru_plan_c2r(blitz::Array<std::complex<double>,3>& in, blitz::Array<double,3>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;



// include inline template declarations only after
// the explicit extern template instantiations
#include <BlitzFFTW.icc>

#endif

