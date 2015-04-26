/**************************************************************************
 *
 * Copyright (C) 2010 Thorsten Falk, Mario Emmenlauer
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

/**************************************************************************
**       Title: Dummy implementation, explicitly instantiates templates
** Description:
**
**    WARNING: This is a generated file. Edit generate.cc, not this file!
**
**************************************************************************/
#include <BlitzFFTW.hh>

template void BlitzFFTW<float>::plan_forward(blitz::Array<float,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<float>::plan_forward(blitz::Array<float,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<float>::plan_forward(blitz::Array<float,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

template void BlitzFFTW<double>::plan_forward(blitz::Array<double,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<double>::plan_forward(blitz::Array<double,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<double>::plan_forward(blitz::Array<double,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


template void BlitzFFTW<float>::plan_backward(blitz::Array<std::complex<float>,1>& in, blitz::Array<float,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<float>::plan_backward(blitz::Array<std::complex<float>,2>& in, blitz::Array<float,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<float>::plan_backward(blitz::Array<std::complex<float>,3>& in, blitz::Array<float,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

template void BlitzFFTW<double>::plan_backward(blitz::Array<std::complex<double>,1>& in, blitz::Array<double,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<double>::plan_backward(blitz::Array<std::complex<double>,2>& in, blitz::Array<double,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<double>::plan_backward(blitz::Array<std::complex<double>,3>& in, blitz::Array<double,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


template void BlitzFFTW<float>::plan_forward(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<float>::plan_forward(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<float>::plan_forward(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

template void BlitzFFTW<double>::plan_forward(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<double>::plan_forward(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<double>::plan_forward(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


template void BlitzFFTW<float>::plan_backward(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<float>::plan_backward(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<float>::plan_backward(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

template void BlitzFFTW<double>::plan_backward(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<double>::plan_backward(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template void BlitzFFTW<double>::plan_backward(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


template void BlitzFFTW<float>::forward(const blitz::Array<float,1>& in, blitz::Array<std::complex<float>,1>& out) const;
template void BlitzFFTW<float>::forward(const blitz::Array<float,2>& in, blitz::Array<std::complex<float>,2>& out) const;
template void BlitzFFTW<float>::forward(const blitz::Array<float,3>& in, blitz::Array<std::complex<float>,3>& out) const;

template void BlitzFFTW<double>::forward(const blitz::Array<double,1>& in, blitz::Array<std::complex<double>,1>& out) const;
template void BlitzFFTW<double>::forward(const blitz::Array<double,2>& in, blitz::Array<std::complex<double>,2>& out) const;
template void BlitzFFTW<double>::forward(const blitz::Array<double,3>& in, blitz::Array<std::complex<double>,3>& out) const;


template void BlitzFFTW<float>::forward(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out) const;
template void BlitzFFTW<float>::forward(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out) const;
template void BlitzFFTW<float>::forward(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out) const;

template void BlitzFFTW<double>::forward(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out) const;
template void BlitzFFTW<double>::forward(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out) const;
template void BlitzFFTW<double>::forward(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out) const;


template void BlitzFFTW<float>::backward(blitz::Array<std::complex<float>,1>& in, blitz::Array<float,1>& out, const DataPreservePolicy policy) const;
template void BlitzFFTW<float>::backward(blitz::Array<std::complex<float>,2>& in, blitz::Array<float,2>& out, const DataPreservePolicy policy) const;
template void BlitzFFTW<float>::backward(blitz::Array<std::complex<float>,3>& in, blitz::Array<float,3>& out, const DataPreservePolicy policy) const;

template void BlitzFFTW<double>::backward(blitz::Array<std::complex<double>,1>& in, blitz::Array<double,1>& out, const DataPreservePolicy policy) const;
template void BlitzFFTW<double>::backward(blitz::Array<std::complex<double>,2>& in, blitz::Array<double,2>& out, const DataPreservePolicy policy) const;
template void BlitzFFTW<double>::backward(blitz::Array<std::complex<double>,3>& in, blitz::Array<double,3>& out, const DataPreservePolicy policy) const;


template void BlitzFFTW<float>::backward(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy) const;
template void BlitzFFTW<float>::backward(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy) const;
template void BlitzFFTW<float>::backward(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy) const;

template void BlitzFFTW<double>::backward(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy) const;
template void BlitzFFTW<double>::backward(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy) const;
template void BlitzFFTW<double>::backward(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy) const;


template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_forward(blitz::Array<float,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_forward(blitz::Array<float,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_forward(blitz::Array<float,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_forward(blitz::Array<double,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_forward(blitz::Array<double,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_forward(blitz::Array<double,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_forward(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_forward(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_forward(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_forward(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_forward(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_forward(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_backward(blitz::Array<std::complex<float>,1>& in, blitz::Array<float,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_backward(blitz::Array<std::complex<float>,2>& in, blitz::Array<float,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_backward(blitz::Array<std::complex<float>,3>& in, blitz::Array<float,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_backward(blitz::Array<std::complex<double>,1>& in, blitz::Array<double,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_backward(blitz::Array<std::complex<double>,2>& in, blitz::Array<double,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_backward(blitz::Array<std::complex<double>,3>& in, blitz::Array<double,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_backward(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_backward(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<float>::blitz_fftw_plan BlitzFFTW<float>::get_plan_backward(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;

template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_backward(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_backward(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;
template BlitzFFTW<double>::blitz_fftw_plan BlitzFFTW<double>::get_plan_backward(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;


template void BlitzFFTW<float>::exec_guru_plan(blitz::Array<std::complex<float>,1>& in, blitz::Array<std::complex<float>,1>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
template void BlitzFFTW<float>::exec_guru_plan(blitz::Array<std::complex<float>,2>& in, blitz::Array<std::complex<float>,2>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
template void BlitzFFTW<float>::exec_guru_plan(blitz::Array<std::complex<float>,3>& in, blitz::Array<std::complex<float>,3>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;

template void BlitzFFTW<double>::exec_guru_plan(blitz::Array<std::complex<double>,1>& in, blitz::Array<std::complex<double>,1>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
template void BlitzFFTW<double>::exec_guru_plan(blitz::Array<std::complex<double>,2>& in, blitz::Array<std::complex<double>,2>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
template void BlitzFFTW<double>::exec_guru_plan(blitz::Array<std::complex<double>,3>& in, blitz::Array<std::complex<double>,3>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;


template void BlitzFFTW<float>::exec_guru_plan_r2c(blitz::Array<float,1>& in, blitz::Array<std::complex<float>,1>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
template void BlitzFFTW<float>::exec_guru_plan_r2c(blitz::Array<float,2>& in, blitz::Array<std::complex<float>,2>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
template void BlitzFFTW<float>::exec_guru_plan_r2c(blitz::Array<float,3>& in, blitz::Array<std::complex<float>,3>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;

template void BlitzFFTW<double>::exec_guru_plan_r2c(blitz::Array<double,1>& in, blitz::Array<std::complex<double>,1>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
template void BlitzFFTW<double>::exec_guru_plan_r2c(blitz::Array<double,2>& in, blitz::Array<std::complex<double>,2>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
template void BlitzFFTW<double>::exec_guru_plan_r2c(blitz::Array<double,3>& in, blitz::Array<std::complex<double>,3>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;


template void BlitzFFTW<float>::exec_guru_plan_c2r(blitz::Array<std::complex<float>,1>& in, blitz::Array<float,1>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
template void BlitzFFTW<float>::exec_guru_plan_c2r(blitz::Array<std::complex<float>,2>& in, blitz::Array<float,2>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
template void BlitzFFTW<float>::exec_guru_plan_c2r(blitz::Array<std::complex<float>,3>& in, blitz::Array<float,3>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;

template void BlitzFFTW<double>::exec_guru_plan_c2r(blitz::Array<std::complex<double>,1>& in, blitz::Array<double,1>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
template void BlitzFFTW<double>::exec_guru_plan_c2r(blitz::Array<std::complex<double>,2>& in, blitz::Array<double,2>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;
template void BlitzFFTW<double>::exec_guru_plan_c2r(blitz::Array<std::complex<double>,3>& in, blitz::Array<double,3>& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;


