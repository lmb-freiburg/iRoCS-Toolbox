// g++ -Wall -O3 -g -fopenmp -DHAVE_CONFIG_H -I/home/falk/software/$HOSTGCC/include/irocs-toolbox -o cropDataset cropDataset.cc /home/falk/software/$HOSTGCC/lib/libArrayToolbox.a /home/falk/software/$HOSTGCC/lib/libBlitzHdf5.a /home/falk/software/$HOSTGCC/lib/libProgressReporter.a /home/falk/software/$HOSTGCC/lib/libcmdline.a -lblitz -lhdf5 -lgsl -lgslcblas -lm -lgomp && mv cropDataset ~/software/$HOSTGCC/bin

#include <config.hh>

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

#include <libArrayToolbox/Array.hh>

enum DataType { UINT8, INT8, UINT16, INT16, UINT32, INT32, UINT64, INT64,
                FLOAT, DOUBLE };

template<int OutDim, typename DataT, int InDim>
void crop(
    atb::Array<DataT,InDim> const &data,
    blitz::TinyVector<bool,InDim> const &isSingleton,
    std::string const &outfile, std::string const &destination,
    blitz::TinyVector<int,InDim> const &lbPx,
    blitz::TinyVector<int,InDim> const &ubPx,
    iRoCS::ProgressReporter *pr = NULL)
{
  int pMin = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pScale = (pr != NULL) ? (pr->taskProgressMax() - pMin) : 100;

  blitz::TinyVector<int,OutDim> outShape;
  blitz::TinyVector<double,OutDim> elSize;
  blitz::TinyMatrix<double,OutDim+1,OutDim+1> trafo;

  blitz::TinyVector<double,InDim> originOffsetUm(
      atb::homogeneousToEuclidean(
          atb::mvMult(
              atb::invert(data.transformation()), atb::euclideanToHomogeneous(
                  blitz::TinyVector<double,InDim>(
                      lbPx * data.elementSizeUm())))) -
      atb::homogeneousToEuclidean(
          atb::mvMult(
              atb::invert(data.transformation()),
              atb::euclideanToHomogeneous(
                  blitz::TinyVector<double,InDim>(0.0)))));

  blitz::TinyMatrix<double,InDim+1,InDim+1> translation(
      atb::traits<blitz::TinyMatrix<double,InDim+1,InDim+1> >::one);
  for (int d = 0; d < InDim; ++d) translation(d, InDim) = originOffsetUm(d);
  translation = atb::invert(atb::mmMult(atb::invert(data.transformation()),
                                        translation));

  int row = 0;
  for (int r = 0; r < InDim; ++r)
  {
    if (!isSingleton(r))
    {
      outShape(row) = ubPx(r) - lbPx(r) + 1;
      elSize(row) = data.elementSizeUm()(r);
      int col = 0;
      for (int c = 0; c < InDim; ++c)
      {
        if (!isSingleton(c))
        {
          trafo(row, col) = translation(r, c);
          col++;
        }
      }
      trafo(r, OutDim) = translation(r, InDim);
      row++;
    }
  }
  int col = 0;
  for (int c = 0; c < InDim; ++c)
  {
    if (!isSingleton(c))
    {
      trafo(OutDim, col) = translation(InDim, col);
      col++;
    }
  }
  trafo(OutDim, OutDim) = translation(InDim, InDim);

  if (pr != NULL && ! pr->updateProgressMessage("Cropping data")) return;
  atb::Array<DataT,OutDim> out(outShape, elSize, trafo);
  int p = 0;
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(out.size()); ++i)
  {
    if (pr != NULL)
    {
      if (pr->isAborted()) continue;
      int progress;
#ifdef _OPENMP
#pragma omp critical
#endif
      {
        progress = p;
        ++p;
      }
      if (progress % std::min(1, static_cast<int>(out.size() / 100)) == 0)
      {
        pr->updateProgress(
            static_cast<int>(
                pMin + pScale * static_cast<double>(progress) /
                static_cast<double>(out.size() - 1)));
      }
    }
    ptrdiff_t tmp = i;
    blitz::TinyVector<int,InDim> rdPos;
    int d2 = 0;
    for (int d = InDim - 1; d >= 0; --d)
    {
      if (isSingleton(d)) rdPos(d) = lbPx(d);
      else
      {
        rdPos(d) = lbPx(d) + (tmp % out.extent(d2));
        tmp /= out.extent(d2);
        d2++;
      }
    }
    out.data()[i] = data(rdPos);
  }

  if (pr != NULL && ! pr->updateProgressMessage(
          "Saving '" + outfile + ":" + destination + "'")) return;
  out.save(outfile, destination, 3, pr);

  if (pr != NULL) pr->updateProgress(pMin + pScale);
}

template<typename DataT, int Dim>
void crop(
    std::string const &infile, std::string const &source,
    std::string const &outfile, std::string const &destination,
    blitz::TinyVector<int,Dim> const &lbPx,
    blitz::TinyVector<int,Dim> const &ubPx,
    bool keepSingletonDimensions, iRoCS::ProgressReporter *pr = NULL)
{
  int pMin = (pr != NULL) ? pr->taskProgressMin() : 0;
  int pScale = (pr != NULL) ? (pr->taskProgressMax() - pMin) : 100;

  if (pr != NULL && !pr->updateProgressMessage(
          "Loading '" + infile + ":" + source + "'")) return;
  atb::Array<DataT,Dim> data;
  data.load(infile, source, pr);
  
  int outDim = 0;
  blitz::TinyVector<bool,Dim> isSingleton(true);
  for (int d = 0; d < Dim; ++d)
  {
    if (ubPx(d) > lbPx(d))
    {
      outDim++;
      isSingleton(d) = false;
    }
  }

  if (keepSingletonDimensions || outDim == Dim)
  {
    blitz::TinyVector<int,Dim> outShape(ubPx - lbPx + 1);
    blitz::TinyVector<double,Dim> elSize(data.elementSizeUm());
    blitz::TinyMatrix<double,Dim+1,Dim+1> trafo(data.transformation());

    blitz::TinyVector<double,Dim> originOffsetUm(
        atb::homogeneousToEuclidean(
            atb::mvMult(
                atb::invert(trafo), atb::euclideanToHomogeneous(
                    blitz::TinyVector<double,Dim>(
                        lbPx * data.elementSizeUm())))) -
        atb::homogeneousToEuclidean(
            atb::mvMult(
                atb::invert(trafo), atb::euclideanToHomogeneous(
                    blitz::TinyVector<double,Dim>(0.0)))));

    blitz::TinyMatrix<double,Dim+1,Dim+1> translation(
        atb::traits<blitz::TinyMatrix<double,Dim+1,Dim+1> >::one);
    for (int d = 0; d < Dim; ++d) translation(d, Dim) = originOffsetUm(d);
    trafo = atb::invert(atb::mmMult(atb::invert(trafo), translation));

    if (pr != NULL && ! pr->updateProgressMessage("Cropping data")) return;
    atb::Array<DataT,Dim> out(outShape, elSize, trafo);
    int p = 0;
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(out.size()); ++i)
    {
      if (pr != NULL)
      {
        if (pr->isAborted()) continue;
        int progress;
#ifdef _OPENMP
#pragma omp critical
#endif
        {
          progress = p;
          ++p;
        }
        if (progress % std::min(1, static_cast<int>(out.size() / 100)) == 0)
        {
          pr->updateProgress(
              static_cast<int>(
                  pMin + pScale * static_cast<double>(progress) /
                  static_cast<double>(out.size() - 1)));
        }
      }
      ptrdiff_t tmp = i;
      blitz::TinyVector<int,Dim> rdPos;
      for (int d = Dim - 1; d >= 0; --d)
      {
        rdPos(d) = lbPx(d) + (tmp % out.extent(d));
        tmp /= out.extent(d);
      }
      out.data()[i] = data(rdPos);
    }

    if (pr != NULL && ! pr->updateProgressMessage(
            "Saving '" + outfile + ":" + destination + "'")) return;
    out.save(outfile, destination, 3, pr);
  }
  else
  {
    switch (outDim)
    {
    case 1:
      crop<1>(data, isSingleton, outfile, destination, lbPx, ubPx, pr);
      break;
    case 2:
      crop<2>(data, isSingleton, outfile, destination, lbPx, ubPx, pr);
      break;
    case 3:
      crop<3>(data, isSingleton, outfile, destination, lbPx, ubPx, pr);
      break;
    case 4:
      crop<4>(data, isSingleton, outfile, destination, lbPx, ubPx, pr);
      break;
    default:
      std::cerr << "Reduction to " << outDim << "-D not implemented."
                << std::endl;
      exit(-1);
    }
  }

  if (pr != NULL) pr->updateProgress(pMin + pScale);
}

template<int Dim>
void crop(
    std::string const &infile, std::string const &source,
    std::string const &outfile, std::string const &destination,
    std::vector<int> const &lbPx, std::vector<int> const &ubPx,
    bool keepSingletonDimensions, iRoCS::ProgressReporter *pr = NULL)
{
  DataType dataType = DOUBLE;
  {
    BlitzH5File inFile(infile);
    hid_t datatypeId = inFile.getDatasetType(source);
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

  blitz::TinyVector<int,Dim> lbPxV, ubPxV;
  for (size_t d = 0; d < Dim; ++d)
  {
    lbPxV(d) = lbPx[d];
    ubPxV(d) = ubPx[d];
  }

  switch (dataType)
  {
  case UINT8 :
    crop<unsigned char>(
        infile, source, outfile, destination,
        lbPxV, ubPxV, keepSingletonDimensions, pr);
    break;
  case INT8 :
    crop<char>(
        infile, source, outfile, destination,
        lbPxV, ubPxV, keepSingletonDimensions, pr);
    break;
  case UINT16 :
    crop<unsigned short>(
        infile, source, outfile, destination,
        lbPxV, ubPxV, keepSingletonDimensions, pr);
    break;
  case INT16 :
    crop<short>(
        infile, source, outfile, destination,
        lbPxV, ubPxV, keepSingletonDimensions, pr);
    break;
  case UINT32 :
    crop<unsigned int>(
        infile, source, outfile, destination,
        lbPxV, ubPxV, keepSingletonDimensions, pr);
    break;
  case INT32 :
    crop<int>(
        infile, source, outfile, destination,
        lbPxV, ubPxV, keepSingletonDimensions, pr);
    break;
  case UINT64 :
    crop<unsigned long long>(
        infile, source, outfile, destination,
        lbPxV, ubPxV, keepSingletonDimensions, pr);
    break;
  case INT64 :
    crop<long long>(
        infile, source, outfile, destination,
        lbPxV, ubPxV, keepSingletonDimensions, pr);
    break;
  case FLOAT :
    crop<float>(
        infile, source, outfile, destination,
        lbPxV, ubPxV, keepSingletonDimensions, pr);
    break;
  case DOUBLE :
    crop<double>(
        infile, source, outfile, destination,
        lbPxV, ubPxV, keepSingletonDimensions, pr);
    break;
  }  
}

int main(int argc, char** argv)
{
  CmdArgType<std::string> infile(
      "<input hdf5 file>",
      "The input file to crop the cells from", CmdArg::isREQ);
  CmdArgType<std::string> source(
      's', "source", "<hdf5 dataset>", "The dataset to crop.", CmdArg::isREQ);
  CmdArgType<std::string> outfile(
      'o', "outfile", "<string>", "The output file name.", CmdArg::isREQ);
  CmdArgType<std::string> destination(
      'd', "destination", "<hdf5 dataset>", "The output dataset to write the "
      "cropped data to.", CmdArg::isREQ);
  CmdArgTypeVector<int> lowerBoundPx(
      'l', "lowerBoundPx", "<int vector>", "The lower bound of the cropped "
      "region in pixels. If not given, it will be set to zero.");
  CmdArgTypeVector<int> upperBoundPx(
      'u', "upperBoundPx", "<int vector>", "The upper bound of the cropped "
      "region in pixels. If not given it will be set to the maximum "
      "position within the dataset");  
  CmdArgSwitch keepSingletonDimensions(
      'k', "keepSingletonDimensions", "If given, the output dataset will have "
      "the same dimensionality as the input dataset, even if output dimensions "
      "will become singleton. In normal operation singleton dimensions will be "
      "removed and thus the output dataset's dimensionality reduced.");

  CmdLine cmd(argv[0], "Crop the dataset using the given bounds.");
  cmd.description(
      "The selected dataset is cropped using the given voxel bounds. If not "
      "explicitely forbidden by the user, singleton dimensions will be removed. "
      "This tool is datatype aware!");

  try
  {
    cmd.append(&infile);
    cmd.append(&source);
    cmd.append(&outfile);
    cmd.append(&destination);
    cmd.append(&lowerBoundPx);
    cmd.append(&upperBoundPx);
    cmd.append(&keepSingletonDimensions);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

    std::vector<int> lbPx, ubPx;
    {
      BlitzH5File inFile(infile.value());
      std::vector<hsize_t> dims(inFile.getDatasetShape(source.value()));
      lbPx.resize(dims.size());
      for (size_t i = 0; i < lbPx.size(); ++i)
          lbPx[i] = (i < lowerBoundPx.size() && lowerBoundPx[i] >= 0) ?
              lowerBoundPx[i] : 0;
      ubPx.resize(dims.size());
      for (size_t i = 0; i < ubPx.size(); ++i)
          ubPx[i] = (i < upperBoundPx.size() && upperBoundPx[i] >= 0 &&
                     upperBoundPx[i] < static_cast<int>(dims[i])) ?
              upperBoundPx[i] : (static_cast<int>(dims[i] - 1));
    }
    std::cout << "  Cropping range: " << std::flush;
    for (size_t i = 0; i < lbPx.size(); ++i)
        std::cout << lbPx[i] << " " << std::flush;
    std::cout << "- " << std::flush;
    for (size_t i = 0; i < lbPx.size(); ++i)
        std::cout << ubPx[i] << " " << std::flush;
    std::cout << std::endl;
    
    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r  ");
    pr.setTaskProgressMin(0);
    pr.setTaskProgressMax(100);

    switch (lbPx.size())
    {
    case 1:
      crop<1>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          lbPx, ubPx, keepSingletonDimensions.given(), &pr);
      break;
    case 2:
      crop<2>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          lbPx, ubPx, keepSingletonDimensions.given(), &pr);
      break;
    case 3:
      crop<3>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          lbPx, ubPx, keepSingletonDimensions.given(), &pr);
      break;
    case 4:
      crop<4>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          lbPx, ubPx, keepSingletonDimensions.given(), &pr);
      break;
    case 5:
      crop<5>(
          infile.value(), source.value(), outfile.value(), destination.value(),
          lbPx, ubPx, keepSingletonDimensions.given(), &pr);
      break;
    default:
      throw CmdLineError() << lbPx.size() << "-D data not supported.";
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
  catch (...)
  {
    std::cerr << "Unknown error during execution" << std::endl;
    return -4;
  }
  return 0;
}
