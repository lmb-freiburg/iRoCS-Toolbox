// g++ -Wall -O3 -g -fopenmp -DHAVE_CONFIG_H -I/home/falk/software/$HOSTGCC/include/irocs-toolbox -I/usr/include/hdf5/serial -o rotateDataset rotateDataset.cc /home/falk/software/$HOSTGCC/lib/libArrayToolbox.a /home/falk/software/$HOSTGCC/lib/libBlitzHdf5.a /home/falk/software/$HOSTGCC/lib/libProgressReporter.a /home/falk/software/$HOSTGCC/lib/libcmdline.a -lblitz -lhdf5_serial -lgsl -lgslcblas -lm -lgomp -ldl -lz -lpthread && mv rotateDataset ~/software/$HOSTGCC/bin

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

#include <libArrayToolbox/Array.hh>

enum DataType { UINT8, INT8, UINT16, INT16, UINT32, INT32, UINT64, INT64,
                FLOAT, DOUBLE };

template<typename DataT>
void rotate(
    std::string const &infile, std::string const &source,
    std::string const &outfile, std::string const &destination,
    int axisDimension, double angle, iRoCS::ProgressReporter &pr)
{
  atb::Array<DataT,3> rotated;

  if ((angle / 90.0) - std::floor(angle / 90.0) == 0)
  {
    atb::Array<DataT,3> data;
    pr.updateProgressMessage("Loading '" + infile + ":" + source + "'");
    data.load(infile, source, &pr);

    rotated.setTransformation(data.transformation());

    int n = static_cast<int>(std::floor(angle / 90.0)) % 4;
    switch (n)
    {
    case 1:
    {
      switch (axisDimension)
      {
      case 0: // 90 degrees
        pr.updateProgressMessage("Rotating by 90 degrees around the z axis");
        rotated.resize(data.extent(0), data.extent(2), data.extent(1));
        rotated.setElementSizeUm(
            blitz::TinyVector<double,3>(
                data.elementSizeUm()(0), data.elementSizeUm()(2),
                data.elementSizeUm()(1)));
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT z = 0; z < rotated.extent(0); ++z)
            for (atb::BlitzIndexT y = 0; y < rotated.extent(1); ++y)
                for (atb::BlitzIndexT x = 0; x < rotated.extent(2); ++x)
                    rotated(z, y, x) =
                        data(z, x, rotated.extent(1) - y - 1);
        break;
      case 1:
        pr.updateProgressMessage("Rotating by 90 degrees around the y axis");
        rotated.resize(data.extent(2), data.extent(1), data.extent(0));
        rotated.setElementSizeUm(
            blitz::TinyVector<double,3>(
                data.elementSizeUm()(2), data.elementSizeUm()(1),
                data.elementSizeUm()(0)));
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT z = 0; z < rotated.extent(0); ++z)
            for (atb::BlitzIndexT y = 0; y < rotated.extent(1); ++y)
                for (atb::BlitzIndexT x = 0; x < rotated.extent(2); ++x)
                    rotated(z, y, x) =
                        data(x, y, rotated.extent(0) - z - 1);
        break;
      case 2:
        pr.updateProgressMessage("Rotating by 90 degrees around the x axis");
        rotated.resize(data.extent(1), data.extent(0), data.extent(2));
        rotated.setElementSizeUm(
            blitz::TinyVector<double,3>(
                data.elementSizeUm()(1), data.elementSizeUm()(0),
                data.elementSizeUm()(2)));
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT z = 0; z < rotated.extent(0); ++z)
            for (atb::BlitzIndexT y = 0; y < rotated.extent(1); ++y)
                for (atb::BlitzIndexT x = 0; x < rotated.extent(2); ++x)
                    rotated(z, y, x) =
                        data(rotated.extent(1) - y - 1, z, x);
        break;
      default:
        throw std::runtime_error("Invalid axisDimension given.");
      }
      break;
    }
    case 2: // 180 degrees
    {
      rotated.resize(data.shape());
      rotated.setElementSizeUm(data.elementSizeUm());
      switch (axisDimension)
      {
      case 0:
        pr.updateProgressMessage("Rotating by 180 degrees around the z axis");
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT z = 0; z < rotated.extent(0); ++z)
            for (atb::BlitzIndexT y = 0; y < rotated.extent(1); ++y)
                for (atb::BlitzIndexT x = 0; x < rotated.extent(2); ++x)
                    rotated(z, y, x) =
                        data(z, rotated.extent(1) - y - 1,
                             rotated.extent(2) - x - 1);
        break;
      case 1:
        pr.updateProgressMessage("Rotating by 180 degrees around the y axis");
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT z = 0; z < rotated.extent(0); ++z)
            for (atb::BlitzIndexT y = 0; y < rotated.extent(1); ++y)
                for (atb::BlitzIndexT x = 0; x < rotated.extent(2); ++x)
                    rotated(z, y, x) =
                        data(rotated.extent(0) - z - 1, y,
                             rotated.extent(2) - x - 1);
        break;
      case 2: // 270 degrees
        pr.updateProgressMessage("Rotating by 180 degrees around the x axis");
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT z = 0; z < rotated.extent(0); ++z)
            for (atb::BlitzIndexT y = 0; y < rotated.extent(1); ++y)
                for (atb::BlitzIndexT x = 0; x < rotated.extent(2); ++x)
                    rotated(z, y, x) =
                        data(rotated.extent(0) - z - 1,
                             rotated.extent(1) - y - 1, x);
        break;
      default:
        throw std::runtime_error("Invalid axisDimension given.");
      }
      break;
    }
    case 3:
    {
      switch (axisDimension)
      {
      case 0: // 270 degrees
        pr.updateProgressMessage("Rotating by 270 degrees around the z axis");
        rotated.resize(data.extent(0), data.extent(2), data.extent(1));
        rotated.setElementSizeUm(
            blitz::TinyVector<double,3>(
                data.elementSizeUm()(0), data.elementSizeUm()(2),
                data.elementSizeUm()(1)));
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT z = 0; z < rotated.extent(0); ++z)
            for (atb::BlitzIndexT y = 0; y < rotated.extent(1); ++y)
                for (atb::BlitzIndexT x = 0; x < rotated.extent(2); ++x)
                    rotated(z, y, x) =
                        data(z, rotated.extent(2) - x - 1, y);
        break;
      case 1:
        pr.updateProgressMessage("Rotating by 270 degrees around the y axis");
        rotated.resize(data.extent(2), data.extent(1), data.extent(0));
        rotated.setElementSizeUm(
            blitz::TinyVector<double,3>(
                data.elementSizeUm()(2), data.elementSizeUm()(1),
                data.elementSizeUm()(0)));
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT z = 0; z < rotated.extent(0); ++z)
            for (atb::BlitzIndexT y = 0; y < rotated.extent(1); ++y)
                for (atb::BlitzIndexT x = 0; x < rotated.extent(2); ++x)
                    rotated(z, y, x) =
                        data(rotated.extent(2) - x - 1, y, z);
        break;
      case 2:
        pr.updateProgressMessage("Rotating by 270 degrees around the x axis");
        rotated.resize(data.extent(1), data.extent(0), data.extent(2));
        rotated.setElementSizeUm(
            blitz::TinyVector<double,3>(
                data.elementSizeUm()(1), data.elementSizeUm()(0),
                data.elementSizeUm()(2)));
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT z = 0; z < rotated.extent(0); ++z)
            for (atb::BlitzIndexT y = 0; y < rotated.extent(1); ++y)
                for (atb::BlitzIndexT x = 0; x < rotated.extent(2); ++x)
                    rotated(z, y, x) =
                        data(y, rotated.extent(0) - z - 1, x);
        break;
      default:
        throw std::runtime_error("Invalid axisDimension given.");
      }
      break;
    }
    default:
    {
      pr.updateProgressMessage("Saving '" + outfile + ":" + destination + "'");
      data.save(outfile, destination, 3, &pr);
      return;
    }
    }
  }
  else
  {
    double angleRad = angle / 180.0 * M_PI;
    blitz::TinyVector<int,2> dims;
    switch (axisDimension)
    {
    case 0:
      dims = 1, 2;
      break;
    case 1:
      dims = 0, 2;
      break;
    case 2:
      dims = 0, 1;
      break;
    default:
        throw std::runtime_error("Invalid axisDimension given.");      
    }
    blitz::TinyMatrix<double,4,4> trafo(
        atb::traits< blitz::TinyMatrix<double,4,4> >::one);
    trafo(dims(0), dims(0)) = std::cos(angleRad);
    trafo(dims(0), dims(1)) = -std::sin(angleRad);
    trafo(dims(1), dims(0)) = std::sin(angleRad);
    trafo(dims(1), dims(1)) = std::cos(angleRad);

    atb::Array<float,3> data;
    pr.updateProgressMessage("Loading '" + infile + ":" + source + "'");
    data.load(infile, source, &pr);

    data.setTransformation(
        atb::invert(atb::mmMult(trafo, atb::invert(data.transformation()))));
    blitz::TinyVector<double,3> lbUm(data.lowerBoundUm());
    blitz::TinyVector<double,3> ubUm(data.upperBoundUm());
    blitz::TinyVector<double,3> shapeUm(ubUm - lbUm);
    blitz::TinyVector<atb::BlitzIndexT,3> shapePx(
        blitz::ceil(shapeUm / data.elementSizeUm()));
    rotated.resize(shapePx);
    rotated.setElementSizeUm(data.elementSizeUm());
    trafo = 
        1, 0, 0, -lbUm(0),
        0, 1, 0, -lbUm(1),
        0, 0, 1, -lbUm(2),
        0, 0, 0, 1;
    rotated.setTransformation(trafo);

    std::stringstream msg;
    msg << "Rotating by " << angle << " degrees around the "
        << ((axisDimension == 0) ? "z" : ((axisDimension == 1) ? "y" : "x"))
        << " axis";
    pr.updateProgressMessage(msg.str());

    int p = 0;
    int pMin = pr.taskProgressMin();
    double pScale = 
        static_cast<double>(pr.taskProgressMax() - pMin) /
        static_cast<double>(rotated.size() - 1);
    int pStep = rotated.size() / 1000;
    blitz::TinyVector<atb::BlitzIndexT,3> shape(rotated.shape());
    blitz::TinyVector<double,3> elSize(rotated.elementSizeUm());
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(rotated.size()); ++i)
    {
      if (p % pStep == 0)
      {
#ifdef _OPENMP
#pragma omp critical
#endif
        pr.updateProgress(static_cast<int>(pMin + p * pScale));
      }
#ifdef _OPENMP
#pragma omp atomic
#endif
      ++p;
      blitz::TinyVector<double,3> posUm;
      ptrdiff_t tmp = i;
      for (int d = 2; d >= 0; --d)
      {
        posUm(d) = (tmp % shape(d)) * elSize(d);
        tmp /= shape(d);
      }
      posUm += lbUm;
      rotated.data()[i] = static_cast<DataT>(data.valueAt(posUm));
    }
  }
  pr.updateProgress(pr.taskProgressMax());

  pr.updateProgressMessage("Saving '" + outfile + ":" + destination + "'");
  rotated.save(outfile, destination, 3, &pr);
}

int main(int argc, char** argv)
{
  CmdArgType<std::string> infile(
      "<input hdf5 file>",
      "The input file containng the datset to rotate", CmdArg::isREQ);
  CmdArgType<std::string> source(
      's', "source", "<hdf5 dataset>", "The dataset to rotate.", CmdArg::isREQ);
  CmdArgType<std::string> outfile(
      'o', "outfile", "<string>", "The output file name.", CmdArg::isREQ);
  CmdArgType<std::string> destination(
      'd', "destination", "<hdf5 dataset>", "The output dataset to write the "
      "rotated data to.", CmdArg::isREQ);
  CmdArgType<int> axisDimension(
      0, "axisDimension", "<int>", "The index of the axis to rotate around. "
      "0=z, 1=y, 2=x");
  axisDimension.setDefaultValue(0);
  CmdArgType<double> angle(
      'a', "angle", "<double>", "The rotation angle in degrees.");
  angle.setDefaultValue(90);

  CmdLine cmd(
      argv[0], "Rotate the given 3-D dataset dataset by the given angle arond "
      "the axis pointing along the specified dimension.");
  cmd.description(
      "Multiples of 90 degrees will be fast and memory efficient. Other "
      "angles will require conversion to floating point values for "
      "interpolation and will consume quite a lot of memory. This tool is "
      "dataset aware. The output datatype will be the same as the input "
      "datatype.");

  try
  {
    cmd.append(&infile);
    cmd.append(&source);
    cmd.append(&outfile);
    cmd.append(&destination);
    cmd.append(&axisDimension);
    cmd.append(&angle);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r  ");
    pr.setTaskProgressMin(0);
    pr.setTaskProgressMax(100);

    DataType dataType = DOUBLE;
    {
      BlitzH5File inFile(infile.value());
      hid_t datatypeId = inFile.getDatasetType(source.value());
      if (H5Tget_class(datatypeId) == H5T_INTEGER)
      {
        if (H5Tget_sign(datatypeId) == H5T_SGN_2)
        {
          if (H5Tget_size(datatypeId) == 1) dataType = INT8;
          else if (H5Tget_size(datatypeId) == 2) dataType = INT16;
          else if (H5Tget_size(datatypeId) == 4) dataType = INT32;
          else if (H5Tget_size(datatypeId) == 8) dataType = INT64;
          else dataType = DOUBLE;
        }
        else
        {
          if (H5Tget_size(datatypeId) == 1) dataType = UINT8;
          else if (H5Tget_size(datatypeId) == 2) dataType = UINT16;
          else if (H5Tget_size(datatypeId) == 4) dataType = UINT32;
          else if (H5Tget_size(datatypeId) == 8) dataType = UINT64;
          else dataType = DOUBLE;
        }
      }
      H5Tclose(datatypeId);
    }

    switch (dataType)
    {
    case UINT8 :
      rotate<unsigned char>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          axisDimension.value(), angle.value(), pr);
      break;
    case INT8 :
      rotate<char>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          axisDimension.value(), angle.value(), pr);
      break;
    case UINT16 :
      rotate<unsigned short>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          axisDimension.value(), angle.value(), pr);
      break;
    case INT16 :
      rotate<short>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          axisDimension.value(), angle.value(), pr);
      break;
    case UINT32 :
      rotate<unsigned int>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          axisDimension.value(), angle.value(), pr);
      break;
    case INT32 :
      rotate<int>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          axisDimension.value(), angle.value(), pr);
      break;
    case UINT64 :
      rotate<unsigned long long>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          axisDimension.value(), angle.value(), pr);
      break;
    case INT64 :
      rotate<long long>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          axisDimension.value(), angle.value(), pr);
      break;
    case FLOAT :
      rotate<float>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          axisDimension.value(), angle.value(), pr);
      break;
    case DOUBLE :
      rotate<double>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          axisDimension.value(), angle.value(), pr);
      break;
    }  
  }
  catch (CmdLineUsageError &e)
  {
    cmd.usage();
    return -1;
  }
  catch (CmdLineUsageHTMLError &e)
  {
    cmd.usageHTML(std::cout);
    return -1;
  }
  catch (CmdLineUsageXMLError &e)
  {
    cmd.usageXML(std::cout);
    return -1;
  }
  catch (CmdLineSyntaxError &e)
  {
    cmd.error() << e.str() << std::endl;
    cmd.usage(std::cerr);
    return -2;
  }    
  catch (BlitzH5Error &e)
  {
    std::cerr << "BlitzH5Error during operation: " << e.what()
              << std::endl;
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception during execution: " << e.what() << std::endl;
    return -3;
  }
  // catch (...)
  // {
  //   std::cerr << "Unknown error during execution" << std::endl;
  //   return -4;
  // }
  return 0;
}
