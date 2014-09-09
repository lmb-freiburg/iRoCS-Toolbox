#ifndef FILEIOWRAPPER_HH
#define FILEIOWRAPPER_HH

#include <iostream>
#include <fstream>
#include <sstream>
#include "BlitzHdf5Light.hh"
#include <blitz/array.h>
// #include <blitz/tinyvec-et.h>

#include <libBlitzAnalyze/BlitzAnalyzeFile.hh>

// HDF5 Reader and Writer
namespace HDF5IOWrapper
{

  // n-D
  template<typename DataT, typename ElSizeT, int Dim>
  void readDataset(blitz::Array<DataT,Dim>& data,
                   blitz::TinyVector<ElSizeT,Dim>& elSize,
                   const std::string& fileName,
                   const std::string& dsName,
                   bool throwErrors = false,
                   iRoCS::ProgressReporter *pr = NULL)
  {
    try
    {
      BlitzH5File inFile(fileName);
      inFile.readDataset(data, dsName, pr);
      try
      {
        inFile.readAttribute(elSize, "element_size_um", dsName);
      }
      catch (BlitzH5Error &)
      {
        std::cerr << "Warning: Could not read element_size_um. Assuming unit "
                  << "element size" << std::endl;
        elSize = static_cast<ElSizeT>(1.0);
      }
    }
    catch (BlitzH5Error& e)
    {
      if (throwErrors) throw e;
      else
      {
        std::cerr << "Couldn't load '" << fileName << ":" << dsName << "': "
                  << e.what() << std::endl;
        exit(-1);
      }
    }
  }
  
  // n-D+t
  template<typename DataT, typename ElSizeT, int Dim>
  void readDataset(blitz::Array<DataT,Dim>& data,
                   blitz::TinyVector<ElSizeT,Dim-1>& elSize,
                   const std::string& fileName,
                   const std::string& dsName,
                   bool throwErrors = false,
                   iRoCS::ProgressReporter *pr = NULL)
  {
    try
    {
      BlitzH5File inFile(fileName);
      inFile.readDataset(data, dsName, pr);
      try
      {
        inFile.readAttribute(elSize, "element_size_um", dsName);
      }
      catch (BlitzH5Error &)
      {
        std::cerr << "Warning: Could not read element_size_um. Assuming unit "
                  << "element size" << std::endl;
        elSize = static_cast<ElSizeT>(1.0);
      }
    }
    catch (BlitzH5Error& e)
    {
      if (throwErrors) throw e;
      else
      {
        std::cerr << "Couldn't load '" << fileName << ":" << dsName << "': "
                  << e.what() << std::endl;
        exit(-1);
      }
    }
  }
  
  // n-D vectorial
  template<typename DataT, typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      const blitz::Array<blitz::TinyVector<DataT,InnerDim>,OuterDim>& data,
      const blitz::TinyVector<ElSizeT,OuterDim>& elSize,
      const std::string& fileName,
      const std::string& dsName,
      DataT displayMin, DataT displayMax, bool throwErrors = false,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      const std::string& dim_interpretation = "")
  {
    try
    {
      BlitzH5File outFile(fileName, BlitzH5File::WriteOrNew);
      outFile.writeDataset(data, dsName, compression, pr);
      outFile.writeAttribute(elSize, "element_size_um", dsName);
      if (dim_interpretation == "")
      {
        switch (OuterDim)
        {
        case 1 :
          outFile.writeAttribute(
              std::string("xc"), "dim_interpretation", dsName);
          break;
        case 2 :
          outFile.writeAttribute(
              std::string("yxc"), "dim_interpretation", dsName);
          break;
        case 3 :
          outFile.writeAttribute(
              std::string("zyxc"), "dim_interpretation", dsName);
          break;
        default :
          std::cerr << "Could not write attribute 'dim_interpretation'. No "
                    << "default specification for vectorial " << OuterDim
                    << "-D data available. You should give an explicit "
                    << "interpretation string if possible" << std::endl;
          break;
        }
      }
      else outFile.writeAttribute(
          dim_interpretation, "dim_interpretation", dsName);
      outFile.writeAttribute(
          std::string("vector"), "value_interpretation", dsName);
      outFile.writeAttribute(displayMin, "display_min", dsName);
      outFile.writeAttribute(displayMax, "display_max", dsName);      
    }
    catch (BlitzH5Error& e)
    {
      if (throwErrors) throw e;
      else
      {
        std::cerr << "Couldn't save '" << fileName << ":" << dsName << "': "
                  << e.what() << std::endl;
        exit(-1);
      }
    }
  }
  
  // n-D vectorial
  template<typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      const blitz::Array<blitz::TinyVector<unsigned char,InnerDim>,OuterDim>&
      data,
      const blitz::TinyVector<ElSizeT,OuterDim>& elSize,
      const std::string& fileName,
      const std::string& dsName, bool throwErrors = false, int compression = 0, 
      iRoCS::ProgressReporter *pr = NULL,
      const std::string& dim_interpretation = "")
  {
    writeDataset(data, elSize, fileName, dsName,
                 static_cast<unsigned char>(0),
                 static_cast<unsigned char>(255),
                 throwErrors, compression, pr, dim_interpretation);
  }

  // n-D vectorial
  template<typename DataT, typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      const blitz::Array<blitz::TinyVector<DataT,InnerDim>,OuterDim>& data,
      const blitz::TinyVector<ElSizeT,OuterDim>& elSize,
      const std::string& fileName,
      const std::string& dsName, bool throwErrors = false, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      const std::string& dim_interpretation = "")
  {
    DataT minValue = blitz::min(data[0]);
    for (int d = 1; d < InnerDim; ++d)
    {
      DataT value = blitz::min(data[d]);
      if (value < minValue) minValue = value;
    }
    if (minValue > 0) minValue = 0;
    DataT maxValue = blitz::max(data[0]);
    for (int d = 1; d < InnerDim; ++d)
    {
      DataT value = blitz::max(data[d]);
      if (value > maxValue) maxValue = value;
    }
    writeDataset(data, elSize, fileName, dsName, minValue, maxValue,
                 throwErrors, compression, pr, dim_interpretation);
  }


  // n-D+t vectorial
  template<typename DataT, typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      const blitz::Array<blitz::TinyVector<DataT,InnerDim>,OuterDim>& data,
      const blitz::TinyVector<ElSizeT,OuterDim-1>& elSize,
      const std::string& fileName,
      const std::string& dsName,
      DataT displayMin, DataT displayMax, bool throwErrors = false,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      const std::string& dim_interpretation = "")
  {
    try
    {
      BlitzH5File outFile(fileName, BlitzH5File::WriteOrNew);
      outFile.writeDataset(data, dsName, compression, pr);
      outFile.writeAttribute(elSize, "element_size_um", dsName);
      if (dim_interpretation == "")
      {
        switch (OuterDim)
        {
        case 2 :
          outFile.writeAttribute(
              std::string("txc"), "dim_interpretation", dsName);
          break;
        case 3 :
          outFile.writeAttribute(
              std::string("tyxc"), "dim_interpretation", dsName);
          break;
        case 4 :
          outFile.writeAttribute(
              std::string("tzyxc"), "dim_interpretation", dsName);
          break;
        default :
          std::cerr << "Could not write attribute 'dim_interpretation'. No "
                    << "default specification for vectorial " << OuterDim
                    << "-D data available. You should give an explicit "
                    << "interpretation string if possible" << std::endl;
          break;
        }
      }
      else outFile.writeAttribute(
          dim_interpretation, "dim_interpretation", dsName);
      outFile.writeAttribute(
          std::string("vector"), "value_interpretation", dsName);
      outFile.writeAttribute(displayMin, "display_min", dsName);
      outFile.writeAttribute(displayMax, "display_max", dsName);      
    }
    catch (BlitzH5Error& e)
    {
      if (throwErrors) throw e;
      else
      {
        std::cerr << "Couldn't save '" << fileName << ":" << dsName << "': "
                  << e.what() << std::endl;
        exit(-1);
      }
    }
  }
      
  // n-D+t vectorial
  template<typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      const blitz::Array<blitz::TinyVector<unsigned char,InnerDim>,OuterDim>&
      data,
      const blitz::TinyVector<ElSizeT,OuterDim-1>& elSize,
      const std::string& fileName,
      const std::string& dsName, bool throwErrors = false,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      const std::string& dim_interpretation = "")
  {
    writeDataset(data, elSize, fileName, dsName,
                 0, 255, throwErrors, compression, pr, dim_interpretation);
  }

  // n-D+t vectorial
  template<typename DataT, typename ElSizeT, int InnerDim, int OuterDim>
  void writeDataset(
      const blitz::Array<blitz::TinyVector<DataT,InnerDim>,OuterDim>& data,
      const blitz::TinyVector<ElSizeT,OuterDim-1>& elSize,
      const std::string& fileName,
      const std::string& dsName, bool throwErrors = false,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      const std::string& dim_interpretation = "")
  {
    DataT minValue = blitz::min(data[0]);
    for (int d = 1; d < InnerDim; ++d)
    {
      DataT value = blitz::min(data[d]);
      if (value < minValue) minValue = value;
    }
    if (minValue > 0) minValue = 0;
    DataT maxValue = blitz::max(data[0]);
    for (int d = 1; d < InnerDim; ++d)
    {
      DataT value = blitz::max(data[d]);
      if (value > maxValue) maxValue = value;
    }
    writeDataset(data, elSize, fileName, dsName, minValue, maxValue,
                 throwErrors, compression, pr, dim_interpretation);
  }

  // n-D intensity
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(const blitz::Array<DataT,Dim>& data,
                    const blitz::TinyVector<ElSizeT,Dim>& elSize,
                    const std::string& fileName,
                    const std::string& dsName,
                    DataT displayMin, DataT displayMax,
                    bool throwErrors = false, int compression = 0,
                    iRoCS::ProgressReporter *pr = NULL,
                    const std::string& dim_interpretation = "")
  {
    try
    {
      BlitzH5File outFile(fileName, BlitzH5File::WriteOrNew);
      outFile.writeDataset(data, dsName, compression, pr);
      outFile.writeAttribute(elSize, "element_size_um", dsName);
      if (dim_interpretation == "")
      {
        switch (Dim)
        {
        case 1 :
          outFile.writeAttribute(
              std::string("x"), "dim_interpretation", dsName);
          break;
        case 2 :
          outFile.writeAttribute(
              std::string("yx"), "dim_interpretation", dsName);
          break;
        case 3 :
          outFile.writeAttribute(
              std::string("zyx"), "dim_interpretation", dsName);
          break;
        default :
          std::cerr << "Could not write attribute 'dim_interpretation'. No "
                    << "default specification for "  << Dim
                    << "-D data available. You should give an explicit "
                    << "interpretation string if possible" << std::endl;
          break;
        }
      }
      else outFile.writeAttribute(
          dim_interpretation, "dim_interpretation", dsName);
      outFile.writeAttribute(
          std::string("intensity"), "value_interpretation", dsName);
      outFile.writeAttribute(displayMin, "display_min", dsName);
      outFile.writeAttribute(displayMax, "display_max", dsName);      
    }
    catch (BlitzH5Error& e)
    {
      if (throwErrors) throw e;
      else
      {
        std::cerr << "Couldn't save '" << fileName << ":" << dsName << "': "
                  << e.what() << std::endl;
        exit(-1);
      }
    }
  }
  
  // n-D intensity
  template<typename ElSizeT, int Dim>
  void writeDataset(
      const blitz::Array<unsigned char,Dim>& data,
      const blitz::TinyVector<ElSizeT,Dim>& elSize,
      const std::string& fileName,
      const std::string& dsName, bool throwErrors = false, int compression = 0,
      iRoCS::ProgressReporter *pr = NULL,
      const std::string& dim_interpretation = "")
  {
    writeDataset(data, elSize, fileName, dsName,
                 static_cast<unsigned char>(0),
                 static_cast<unsigned char>(255),
                 throwErrors, compression, pr, dim_interpretation);
  }

  // n-D intensity
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
      const blitz::Array<DataT,Dim>& data,
      const blitz::TinyVector<ElSizeT,Dim>& elSize,
      const std::string& fileName,
      const std::string& dsName, bool throwErrors = false,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      const std::string& dim_interpretation = "")
  {
    DataT minValue = blitz::min(data);
    if (minValue > 0) minValue = 0;
    DataT maxValue = blitz::max(data);
    writeDataset(data, elSize, fileName, dsName, minValue, maxValue,
                 throwErrors, compression, pr, dim_interpretation);
  }

  // n-D complex intensity
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
      const blitz::Array<std::complex<DataT>,Dim>& data,
      const blitz::TinyVector<ElSizeT,Dim>& elSize,
      const std::string& fileName,
      const std::string& dsName, bool throwErrors = false,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      const std::string& dim_interpretation = "")
  {
    std::complex<DataT> minValue =
        std::complex<DataT>(std::max(DataT(0), blitz::min(blitz::real(data))),
                            std::max(DataT(0), blitz::min(blitz::imag(data))));
    std::complex<DataT> maxValue = 
        std::complex<DataT>(blitz::max(blitz::real(data)),
                            blitz::max(blitz::imag(data)));
    writeDataset(data, elSize, fileName, dsName, minValue, maxValue,
                 throwErrors, compression, pr, dim_interpretation);
  }

  // n-D+t intensity
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(const blitz::Array<DataT,Dim>& data,
                    const blitz::TinyVector<ElSizeT,Dim-1>& elSize,
                    const std::string& fileName,
                    const std::string& dsName,
                    DataT displayMin, DataT displayMax,
                    bool throwErrors = false,
                    int compression = 0,
                    iRoCS::ProgressReporter *pr = NULL,
                    const std::string& dim_interpretation = "")
  {
    try
    {
      BlitzH5File outFile(fileName, BlitzH5File::WriteOrNew);
      outFile.writeDataset(data, dsName, compression, pr);
      outFile.writeAttribute(elSize, "element_size_um", dsName);
      if (dim_interpretation == "")
      {
        switch (Dim)
        {
        case 2 :
          outFile.writeAttribute(
              std::string("tx"), "dim_interpretation", dsName);
          break;
        case 3 :
          outFile.writeAttribute(
              std::string("tyx"), "dim_interpretation", dsName);
          break;
        case 4 :
          outFile.writeAttribute(
              std::string("tzyx"), "dim_interpretation", dsName);
          break;
        default :
          std::cerr << "Could not write attribute 'dim_interpretation'. No "
                    << "default specification for "  << Dim
                    << "-D data available. You should give an explicit "
                    << "interpretation string if possible" << std::endl;
          break;
        }
      }
      else outFile.writeAttribute(
          dim_interpretation, "dim_interpretation", dsName);
      outFile.writeAttribute(
          std::string("intensity"), "value_interpretation", dsName);
      outFile.writeAttribute(displayMin, "display_min", dsName);
      outFile.writeAttribute(displayMax, "display_max", dsName);      
    }
    catch (BlitzH5Error& e)
    {
      if (throwErrors) throw e;
      else
      {
        std::cerr << "Couldn't save '" << fileName << ":" << dsName << "': "
                  << e.what() << std::endl;
        exit(-1);
      }
    }
  }
  
  // n-D+t intensity
  template<typename ElSizeT, int Dim>
  void writeDataset(
      const blitz::Array<unsigned char,Dim>& data,
      const blitz::TinyVector<ElSizeT,Dim-1>& elSize,
      const std::string& fileName,
      const std::string& dsName, bool throwErrors = false,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      const std::string& dim_interpretation = "")
  {
    writeDataset(data, elSize, fileName, dsName,
                 0, 255, throwErrors, compression, pr, dim_interpretation);
  }

  // n-D+t complex intensity
  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(
      const blitz::Array<std::complex<DataT>,Dim>& data,
      const blitz::TinyVector<ElSizeT,Dim-1>& elSize,
      const std::string& fileName,
      const std::string& dsName, bool throwErrors = false,
      int compression = 0, iRoCS::ProgressReporter *pr = NULL,
      const std::string& dim_interpretation = "")
  {
    std::complex<DataT> minValue =
        std::complex<DataT>(std::max(DataT(0), blitz::min(blitz::real(data))),
                            std::max(DataT(0), blitz::min(blitz::imag(data))));
    std::complex<DataT> maxValue = 
        std::complex<DataT>(blitz::max(blitz::real(data)),
                            blitz::max(blitz::imag(data)));
    writeDataset(data, elSize, fileName, dsName, minValue, maxValue,
                 throwErrors, compression, pr, dim_interpretation);
  }

}

// Analyze Reader and Writer
namespace AnalyzeIOWrapper
{

  template<typename DataT, typename ElSizeT, int Dim>
  void readDataset(blitz::Array<DataT,Dim>& /*data*/,
                   blitz::TinyVector<ElSizeT,Dim>& /*elSize*/,
                   const std::string& /*fileName*/,
                   const std::string& /*dsName*/)
  {
    std::cerr << "AnalyzeIOWrapper: ERROR: Sorry, reading of Analyze files "
              << "not implemented yet" << std::endl;
  }
  
  template<typename ElSizeT>
  void writeDataset(const blitz::Array<unsigned char,3>& data,
                    const blitz::TinyVector<ElSizeT,3>& elSize,
                    const std::string& fileName,
                    const std::string& /*dsName*/)
  {
    BlitzAnalyzeFile outFile(fileName);
    outFile.saveArrayWithElemSize(data, blitz::TinyVector<float,3>(elSize));
  }

  template<typename ElSizeT>
  void writeDataset(const blitz::Array<short,3>& data,
                    const blitz::TinyVector<ElSizeT,3>& elSize,
                    const std::string& fileName,
                    const std::string& /*dsName*/)
  {
    BlitzAnalyzeFile outFile(fileName);
    outFile.saveArrayWithElemSize(data, blitz::TinyVector<float,3>(elSize));
  }

  template<typename ElSizeT>
  void writeDataset(
      const blitz::Array<blitz::TinyVector<unsigned char,3>,3>& data,
      const blitz::TinyVector<ElSizeT,3>& elSize,
      const std::string& fileName,
      const std::string& /*dsName*/)
  {
    BlitzAnalyzeFile outFile(fileName);
    outFile.saveArrayWithElemSize(data, blitz::TinyVector<float,3>(elSize));
  }

  template<typename DataT, typename ElSizeT>
  void writeDataset(const blitz::Array<DataT,3>& data,
                    const blitz::TinyVector<ElSizeT,3>& elSize,
                    const std::string& fileName,
                    const std::string& /*dsName*/)
  {
    // Some standard type, that is hopefully castable to short
    if (std::numeric_limits<DataT>::is_specialized)
    {
      double dataMin = blitz::min(data);
      double dataMax = blitz::max(data);
      blitz::Array<short,3> dataOut(data.shape());
      dataOut = blitz::cast<short>(
          32767 * blitz::cast<double>(data - dataMin) /
          static_cast<double>(dataMax - dataMin));
      BlitzAnalyzeFile outFile(fileName);
      outFile.saveArrayWithElemSize(
          dataOut, blitz::TinyVector<float,3>(elSize));
    }
  }

  template<typename DataT, typename ElSizeT>
  void writeDataset(const blitz::Array<blitz::TinyVector<DataT,3>,3>& data,
                    const blitz::TinyVector<ElSizeT,3>& elSize,
                    const std::string& fileName,
                    const std::string& /*dsName*/)
  {
    // Some standard type, that is hopefully castable to short
    if (std::numeric_limits<DataT>::is_specialized)
    {
      double dataMin = blitz::min(data[0]);
      double dataMax = blitz::max(data[0]);
      for (int d = 1; d < 3; ++d)
      {
        double tmp = blitz::min(data[d]);
        if (tmp < dataMin) dataMin = tmp;
        tmp = blitz::max(data[d]);
        if (tmp > dataMax) dataMax = tmp;
      }
      
      blitz::Array<blitz::TinyVector<unsigned char,3>,3> dataOut(data.shape());
      for (int d = 0; d < 3; ++d)
          dataOut[d] = blitz::cast<unsigned char>(
              255 * blitz::cast<double>(data[d] - dataMin) /
              static_cast<double>(dataMax - dataMin));
      BlitzAnalyzeFile outFile(fileName);
      outFile.saveArrayWithElemSize(
          dataOut, blitz::TinyVector<float,3>(elSize));
    }
  }

  template<typename DataT, typename ElSizeT, int Dim>
  void writeDataset(const blitz::Array<DataT,Dim>& /*data*/,
                    const blitz::TinyVector<ElSizeT,Dim>& /*elSize*/,
                    const std::string& /*fileName*/,
                    const std::string& /*dsName*/)
  {
    std::cerr << "AnalyzeIOWrapper: ERROR: The datatype and/or dimension of "
              << "the provided array can not be written to an Analyze file. "
              << "Natively Supported types are: blitz::Array<unsigned char,3>, "
              << "blitz::Array<short,3> and "
              << "blitz::Array<blitz::TinyVector<unsigned char,3>,3>."
              << std::endl
              << "Other scalar types are cast to double, then rescaled "
              << "to [0, 32767] and stored as shorts, "
              << "vectorial types are rescaled to the range [0, 255] and "
              << "stored as unsigned characters" << std::endl;
  }
  
}
  

#endif
