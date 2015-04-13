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
**       Title: fftw Wrapper for fast fftw Calculation using blitz++ Arrays
**    $RCSfile$
**   $Revision: 1619 $$Name$
**       $Date: 2008-05-02 15:00:40 +0200 (Fri, 02 May 2008) $
**   Copyright: GPL $Author: emmenlau $
** Description:
**
**
**************************************************************************/
#include <vector>
#include <string>
#include <iostream>
#include <fstream>


int main()
{
  std::vector<std::string> DataT;
  DataT.push_back("float");
  DataT.push_back("double");

  std::vector<std::string> Dim;
  Dim.push_back("1");
  Dim.push_back("2");
  Dim.push_back("3");

  std::ofstream genHeader("BlitzFFTW-Impl.hh", std::ios_base::trunc | std::ios_base::binary);
  std::ofstream genImpl("BlitzFFTW-Impl.cc", std::ios_base::trunc | std::ios_base::binary);

  genHeader << "/**************************************************************************\n";
  genHeader << "**       Title: Dummy header that uses explicit template instantiation\n";
  genHeader << "** Description:\n";
  genHeader << "**\n";
  genHeader << "**    WARNING: This is a generated file. Edit generate.cc, not this file!\n";
  genHeader << "**\n";
  genHeader << "**************************************************************************/\n";
  genHeader << "#ifndef BLITZFFTW_IMPL_HH\n";
  genHeader << "#define BLITZFFTW_IMPL_HH\n";
  genHeader << '\n';
  genHeader << "// make sure we don't include the inline template definitions\n";
  genHeader << "// before the explicit extern template instantiations\n";
  genHeader << "#define BLITZFFTW_HH_NO_INCLUDE_ICC\n";
  genHeader << "#include <BlitzFFTW.hh>\n";
  genHeader << '\n';
  genHeader << "// extern template does not conform to ISO C++, so we need to\n";
  genHeader << "// tell gcc to avoid warnings for this file. Else the -Werror\n";
  genHeader << "// option would abort compilation on this warning.\n";
  genHeader << "#if __GNUG__ >= 3\n";
  genHeader << "#pragma GCC system_header\n";
  genHeader << "#endif // gcc > 3\n";
  genHeader << '\n';

  genImpl << "/**************************************************************************\n";
  genImpl << "**       Title: Dummy implementation, explicitly instantiates templates\n";
  genImpl << "** Description:\n";
  genImpl << "**\n";
  genImpl << "**    WARNING: This is a generated file. Edit generate.cc, not this file!\n";
  genImpl << "**\n";
  genImpl << "**************************************************************************/\n";
  genImpl << "#include <BlitzFFTW.hh>\n";
  genImpl << '\n';


  /*
  // gcc did not like the explicit class instantiation in combination
  // with the specialization of BlitzFFTW<DataT>::instance(), so we leave
  // it out. Its not so critical anyways.
  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    std::string tmp("template class BlitzFFTW<"+DataT[DataTIdx]+">;");
    genHeader << "extern " << tmp << '\n';
    genImpl << tmp << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';
  */

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template void BlitzFFTW<"+DataT[DataTIdx]+">::plan_forward(blitz::Array<"+DataT[DataTIdx]+","+Dim[DimIdx]+">& in, blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template void BlitzFFTW<"+DataT[DataTIdx]+">::plan_backward(blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& in, blitz::Array<"+DataT[DataTIdx]+","+Dim[DimIdx]+">& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template void BlitzFFTW<"+DataT[DataTIdx]+">::plan_forward(blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& in, blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template void BlitzFFTW<"+DataT[DataTIdx]+">::plan_backward(blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& in, blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template void BlitzFFTW<"+DataT[DataTIdx]+">::forward(const blitz::Array<"+DataT[DataTIdx]+","+Dim[DimIdx]+">& in, blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& out) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template void BlitzFFTW<"+DataT[DataTIdx]+">::forward(blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& in, blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& out) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template void BlitzFFTW<"+DataT[DataTIdx]+">::backward(blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& in, blitz::Array<"+DataT[DataTIdx]+","+Dim[DimIdx]+">& out, const DataPreservePolicy policy) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template void BlitzFFTW<"+DataT[DataTIdx]+">::backward(blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& in, blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& out, const DataPreservePolicy policy) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template BlitzFFTW<"+DataT[DataTIdx]+">::blitz_fftw_plan BlitzFFTW<"+DataT[DataTIdx]+">::get_plan_forward(blitz::Array<"+DataT[DataTIdx]+","+Dim[DimIdx]+">& in, blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template BlitzFFTW<"+DataT[DataTIdx]+">::blitz_fftw_plan BlitzFFTW<"+DataT[DataTIdx]+">::get_plan_forward(blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& in, blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template BlitzFFTW<"+DataT[DataTIdx]+">::blitz_fftw_plan BlitzFFTW<"+DataT[DataTIdx]+">::get_plan_backward(blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& in, blitz::Array<"+DataT[DataTIdx]+","+Dim[DimIdx]+">& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template BlitzFFTW<"+DataT[DataTIdx]+">::blitz_fftw_plan BlitzFFTW<"+DataT[DataTIdx]+">::get_plan_backward(blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& in, blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& out, const DataPreservePolicy policy, const unsigned int plan_flags) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template void BlitzFFTW<"+DataT[DataTIdx]+">::exec_guru_plan(blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& in, blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template void BlitzFFTW<"+DataT[DataTIdx]+">::exec_guru_plan_r2c(blitz::Array<"+DataT[DataTIdx]+","+Dim[DimIdx]+">& in, blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';

  for(size_t DataTIdx = 0; DataTIdx < DataT.size(); ++DataTIdx) {
    for(size_t DimIdx = 0; DimIdx < Dim.size(); ++DimIdx) {
      std::string tmp("template void BlitzFFTW<"+DataT[DataTIdx]+">::exec_guru_plan_c2r(blitz::Array<std::complex<"+DataT[DataTIdx]+">,"+Dim[DimIdx]+">& in, blitz::Array<"+DataT[DataTIdx]+","+Dim[DimIdx]+">& out, blitz_fftw_plan plan, const DataPreservePolicy policy) const;");
      genHeader << "extern " << tmp << '\n';
      genImpl << tmp << '\n';
    }
    genHeader << '\n';
    genImpl << '\n';
  }
  genHeader << '\n';
  genImpl << '\n';


  // finished template instantiations
  genHeader << '\n';
  genHeader << "// include inline template declarations only after\n";
  genHeader << "// the explicit extern template instantiations\n";
  genHeader << "#include <BlitzFFTW.icc>\n";
  genHeader << '\n';
  genHeader << "#endif\n";
  genHeader << '\n';

  genHeader.close();
  genImpl.close();

  return 0;
}
