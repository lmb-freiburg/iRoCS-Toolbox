// g++ -Wall -Wextra -pedantic -Wno-long-long -O3 -std=c++11 -g -fopenmp -DHAVE_CONFIG_H -I/home/falk/software/$HOSTGCC/include/irocs-toolbox -o scaleDataset scaleDataset.cc /home/falk/software/$HOSTGCC/lib/libArrayToolbox.a /home/falk/software/$HOSTGCC/lib/libBlitzHdf5.a /home/falk/software/$HOSTGCC/lib/libProgressReporter.a /home/falk/software/$HOSTGCC/lib/libcmdline.a -lblitz -lhdf5 -lgsl -lgslcblas -lm -lgomp -ldl -lpthread -lz && mv scaleDataset /home/falk/software/$HOSTGCC/bin

#include <config.hh>
#include <cstdint>

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

#include <libArrayToolbox/Array.hh>

enum DataType { UINT8, INT8, UINT16, INT16, UINT32, INT32, UINT64, INT64,
                FLOAT, DOUBLE };

template<typename DataT, int Dim>
void rescale(std::string const &ifName, std::string const &source,
             std::string const &ofName, std::string const &target,
             std::vector<double> const &elSizeVec, bool disablePresmoothing,
             atb::InterpolationType interp, iRoCS::ProgressReporter &pr)
{
  bool vectorial = (Dim == static_cast<int>(elSizeVec.size() + 1));
  if (!vectorial && Dim != static_cast<int>(elSizeVec.size()))
  {
    std::stringstream msg;
    msg << "Dataset is " << Dim << "-D but you provided "
        << elSizeVec.size() << " element size components.";
    throw std::runtime_error(msg.str().c_str());
  }

  if (vectorial)
  {
    blitz::Array<DataT,Dim> in;
    blitz::TinyVector<double,Dim-1> elSizeIn;
    blitz::TinyMatrix<double,Dim,Dim> trafo(
        atb::traits< blitz::TinyMatrix<double,Dim,Dim> >::one);
    {
      BlitzH5File inFile(ifName);
      pr.updateProgressMessage("Loading '" + ifName + ":" + source + "'");
      inFile.readDataset(in, source, &pr);
      inFile.readAttribute(elSizeIn, "element_size_um", source);
      try
      {
        inFile.readAttribute(trafo, "transformation", source);
      }
      catch (BlitzH5Error &e)
      {
        std::cerr << "Warning: '" << ifName << ":" << source
                  << "' does not contain a transformation matrix, assuming "
                  << "identity." << std::endl;
      }
    }
    
    blitz::TinyVector<atb::BlitzIndexT,Dim-1> channelShapeIn;
    for (int d = 0; d < Dim - 1; ++d) channelShapeIn(d) = in.extent(d);
    atb::BlitzIndexT numelChannelIn = blitz::product(channelShapeIn),
        numelChannelOut = 1;
    int nChannels = in.extent(Dim - 1);

    blitz::TinyVector<double,Dim-1> elSizeOut;
    std::memcpy(
        elSizeOut.dataFirst(), elSizeVec.data(), (Dim - 1) * sizeof(double));
    atb::Array<DataT,Dim-1> *channel = new atb::Array<DataT,Dim-1>();
    blitz::Array<DataT,Dim> out;
    for (atb::BlitzIndexT c = 0; c < nChannels; ++c)
    {
      channel->resize(channelShapeIn);
      channel->setElementSizeUm(elSizeIn);
      channel->setTransformation(trafo);
      if (interp == atb::NearestIP)
          channel->setInterpolator(
              atb::NearestInterpolator<DataT,Dim-1>(atb::RepeatBT));
      else if (interp == atb::CubicIP)
          channel->setInterpolator(
              atb::CubicInterpolator<DataT,Dim-1>(atb::RepeatBT));
      else channel->setInterpolator(
          atb::LinearInterpolator<DataT,Dim-1>(atb::RepeatBT));
      atb::Interpolator<DataT,Dim-1> const &ip = channel->interpolator();
      for (atb::BlitzIndexT i = 0; i < numelChannelIn; ++i)
          channel->dataFirst()[i] = in.dataFirst()[i * nChannels + c];
      std::stringstream msg;
      msg << "Rescaling channel " << c;
      pr.updateProgressMessage(msg.str());
      if (disablePresmoothing)
      {
        blitz::TinyVector<atb::BlitzIndexT,Dim-1> channelShapeOut(
            channel->shape() * elSizeIn / elSizeOut);
        atb::Array<DataT,Dim-1> *channelOut = new atb::Array<DataT,Dim-1>(
            channelShapeOut, elSizeOut, trafo);
        for (size_t i = 0; i < channelOut->size(); ++i)
        {
          blitz::TinyVector<double,Dim-1> posPx;
          size_t tmp = i;
          for (int d = Dim - 2; d >= 0; --d)
          {
            posPx(d) = tmp % channelOut->extent(d);
            tmp /= channelOut->extent(d);
          }
          posPx = posPx * elSizeOut / elSizeIn;
          channelOut->dataFirst()[i] = ip.get(*channel, posPx);
        }
        delete channel;
        channel = channelOut;
      }
      else channel->rescale(elSizeOut, &pr);
      if (out.size() == 0)
      {
        blitz::TinyVector<atb::BlitzIndexT,Dim> outShape;
        for (int d = 0; d < Dim - 1; ++d)
        {
          outShape(d) = channel->extent(d);
          numelChannelOut *= outShape(d);
        }
        outShape(Dim - 1) = nChannels;
        out.resize(outShape);
      }
      for (atb::BlitzIndexT i = 0; i < numelChannelOut; ++i)
          out.dataFirst()[i * nChannels + c] = channel->dataFirst()[i];
    }
    {
      BlitzH5File outFile(ofName, BlitzH5File::WriteOrNew);
      pr.updateProgressMessage("Saving '" + ofName + ":" + target + "'");
      outFile.writeDataset(out, target, 3, &pr);
      outFile.writeAttribute(elSizeOut, "element_size_um", target);
      outFile.writeAttribute(trafo, "transformation", target);
    }
  }
  else
  {
    blitz::TinyVector<double,Dim> elSize;
    std::memcpy(elSize.dataFirst(), elSizeVec.data(), Dim * sizeof(double));
    atb::Array<DataT,Dim> data;
    pr.updateProgressMessage("Loading '" + ifName + ":" + source + "'");
    data.load(ifName, source, &pr);
    if (interp == atb::NearestIP)
        data.setInterpolator(
            atb::NearestInterpolator<DataT,Dim>(atb::RepeatBT));
    else if (interp == atb::CubicIP)
        data.setInterpolator(
            atb::CubicInterpolator<DataT,Dim>(atb::RepeatBT));
    else data.setInterpolator(
            atb::LinearInterpolator<DataT,Dim>(atb::RepeatBT));
    pr.updateProgressMessage("Rescaling...");
    if (disablePresmoothing)
    {
      blitz::TinyVector<atb::BlitzIndexT,Dim> outShape(
          data.shape() * data.elementSizeUm() / elSize);
      atb::Array<DataT,Dim> out(outShape, elSize, data.transformation());
      for (size_t i = 0; i < out.size(); ++i)
      {
        blitz::TinyVector<double,Dim> posPx;
        size_t tmp = i;
        for (int d = Dim - 1; d >= 0; --d)
        {
          posPx(d) = tmp % out.extent(d);
          tmp /= out.extent(d);
        }
        posPx = posPx * out.elementSizeUm() / data.elementSizeUm();
        out.dataFirst()[i] = data.interpolator().get(data, posPx);
      }
      data.resize(out.shape());
      data.setElementSizeUm(out.elementSizeUm());
      data.setTransformation(out.transformation());
      std::memcpy(
          data.dataFirst(), out.dataFirst(), data.size() * sizeof(DataT));
    }
    else data.rescale(elSize, &pr);
    pr.updateProgressMessage("Saving '" + ofName + ":" + target + "'");
    data.save(ofName, target, 3, &pr);
  }
}

template<int Dim>
void rescale(std::string const &ifName, std::string const &source,
             std::string const &ofName, std::string const &target,
             std::vector<double> const &elSizeVec, bool disablePresmoothing,
             atb::InterpolationType interp, iRoCS::ProgressReporter &pr)
{
  DataType dataType = DOUBLE;
  {
    BlitzH5File inFile(ifName);
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
  switch (dataType)
  {
  case UINT8 :
    rescale<uint8_t,Dim>(
        ifName, source, ofName, target, elSizeVec, disablePresmoothing,
        interp, pr);
    break;
  case INT8 :
    rescale<int8_t,Dim>(
        ifName, source, ofName, target, elSizeVec, disablePresmoothing,
        interp, pr);
    break;
  case UINT16 :
    rescale<uint16_t,Dim>(
        ifName, source, ofName, target, elSizeVec, disablePresmoothing,
        interp, pr);
    break;
  case INT16 :
    rescale<int16_t,Dim>(
        ifName, source, ofName, target, elSizeVec, disablePresmoothing,
        interp, pr);
    break;
  case UINT32 :
    rescale<uint32_t,Dim>(
        ifName, source, ofName, target, elSizeVec, disablePresmoothing,
        interp, pr);
    break;
  case INT32 :
    rescale<int32_t,Dim>(
        ifName, source, ofName, target, elSizeVec, disablePresmoothing,
        interp, pr);
    break;
  case UINT64 :
    rescale<uint64_t,Dim>(
        ifName, source, ofName, target, elSizeVec, disablePresmoothing,
        interp, pr);
    break;
  case INT64 :
    rescale<int64_t,Dim>(
        ifName, source, ofName, target, elSizeVec, disablePresmoothing,
        interp, pr);
    break;
  case FLOAT :
    rescale<float,Dim>(
        ifName, source, ofName, target, elSizeVec, disablePresmoothing,
        interp, pr);
    break;
  case DOUBLE :
    rescale<double,Dim>(
        ifName, source, ofName, target, elSizeVec, disablePresmoothing,
        interp, pr);
    break;
  }
}

int main(int argc, char** argv)
{
  CmdArgType<std::string> infile(
      "<hdf5 file>",
      "The file containing the dataset to rescale.", CmdArg::isREQ);
  CmdArgType<std::string> source(
      's', "source", "<hdf5 dataset>", "The dataset to rescale.",
      CmdArg::isREQ);
  CmdArgType<std::string> outfile(
      'o', "outfile", "<hdf5 file>", "The output file. If not given, output "
      "will be written back to the input file");
  CmdArgType<std::string> target(
      't', "target", "<hdf5 dataset>", "The name of the rescaled dataset.");
  CmdArgTypeVector<double> elementSizeUm(
      'e', "elementSizeUm", "<double>[ <double>]*", "The new element size of "
      "the dataset in (z y x) ordering. Give as many numbers as your dataset "
      "contains dimensions.");
  CmdArgType<std::string> referenceFileName(
      0, "referenceFile", "<hdf5 file>", "If given in conjunction with "
      "<referenceDataset>, <dataset> is rescaled so that its final element "
      "size matches the element size of the dataset specified by "
      "<referenceFile>:<referenceDataset>.");
  CmdArgType<std::string> referenceDataset(
      0, "referenceDataset", "<hdf5 dataset path>", "If given in conjunction "
      "with <referenceFile>, <dataset> is rescaled so that its final element "
      "size matches the element size of the dataset specified by "
      "<referenceFile>:<referenceDataset>.");
  CmdArgSwitch disablePresmoothing(
      0, "disable_presmoothing", "If given the dataset is just scaled without "
      "pre-smoothing. May lead to aliasing.");
  CmdArgSwitch nearestInterpolation(
      'n', "nearest_interpolation", "Use nearest neighbor instead of linear "
      "interpolation.");
  CmdArgSwitch cubicInterpolation(
      'c', "cubic_interpolation", "Use cubic instead of linear interpolation.");

  CmdLine cmd(argv[0], "Rescale dataset to given target element size.");
  cmd.description(
      "The target element size in micrometers of the dataset is set to the "
      "provided vector or copied from the given dataset.");

  try
  {
    cmd.append(&infile);
    cmd.append(&source);
    cmd.append(&outfile);
    cmd.append(&target);
    cmd.append(&elementSizeUm);
    cmd.append(&referenceFileName);
    cmd.append(&referenceDataset);
    cmd.append(&disablePresmoothing);
    cmd.append(&nearestInterpolation);
    cmd.append(&cubicInterpolation);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r  ");
    pr.setTaskProgressMin(0);
    pr.setTaskProgressMax(100);

    if (!(elementSizeUm.given() ||
          (referenceFileName.given() && referenceDataset.given())))
        throw std::runtime_error(
            "Either '-e' (--elementSizeUm) or a reference dataset specified by "
            "--referenceFile and --referenceDataset must be given.");

    std::vector<double> elSizeVec;
    if (!elementSizeUm.given())
    {
      BlitzH5File inFile(referenceFileName.value());
      inFile.readAttribute(
          elSizeVec, referenceDataset.value(), "element_size_um");
    }
    else
    {
      elSizeVec.resize(elementSizeUm.size());
      std::memcpy(
          elSizeVec.data(), &elementSizeUm[0],
          elementSizeUm.size() * sizeof(double));
    }

    if (nearestInterpolation.given() && cubicInterpolation.given())
        throw std::runtime_error(
            "You cannot specify both, nearest and cubic interpolation.");
    
    std::string ofName = outfile.given() ? outfile.value() : infile.value();
    std::string odsName = target.given() ? target.value() : source.value();

    int nDims;
    {
      BlitzH5File inFile(infile.value());
      nDims = inFile.getDatasetShape(source.value()).size();
    }
    
    atb::InterpolationType interp = atb::LinearIP;
    if (nearestInterpolation.given()) interp = atb::NearestIP;
    if (cubicInterpolation.given()) interp = atb::CubicIP;

    switch (nDims)
    {
    case 2:
      rescale<2>(
          infile.value(), source.value(), ofName, odsName, elSizeVec,
          disablePresmoothing.given(), interp, pr);
      break;
    case 3:
      rescale<3>(
          infile.value(), source.value(), ofName, odsName, elSizeVec, 
          disablePresmoothing.given(), interp, pr);
      break;
    case 4:
      rescale<4>(
          infile.value(), source.value(), ofName, odsName, elSizeVec, 
          disablePresmoothing.given(), interp, pr);
      break;
    default:
    {
      std::stringstream msg;
      msg << "Rescaling of " << nDims << "-D Arrays not supported";
      throw std::runtime_error(msg.str().c_str());
    }
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
