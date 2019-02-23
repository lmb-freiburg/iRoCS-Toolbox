// g++ -Wall -O3 -g -fopenmp -DBZ_THREADSAFE -I/home/falk/software/$HOSTGCC/include/irocs-toolbox -o setTransformationMatrix setTransformationMatrix.cc /home/falk/software/$HOSTGCC/lib/libIRoCS.a /home/falk/software/$HOSTGCC/lib/libArrayToolbox.a /home/falk/software/$HOSTGCC/lib/libBlitzHdf5.a /home/falk/software/$HOSTGCC/lib/libBlitzFFTW.a /home/falk/software/$HOSTGCC/lib/libProgressReporter.a /home/falk/software/$HOSTGCC/lib/libBaseFunctions.a /home/falk/software/$HOSTGCC/lib/libcmdline.a -lblitz -lhdf5 -lfftw3 -lfftw3f -lgsl -lgslcblas -lm -lgomp -lz && mv setTransformationMatrix /home/falk/software/$HOSTGCC/bin

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include <stdexcept>
#include <blitz/array.h>

int main(int argc, char** argv)
{
  CmdArgType<std::string> inFileName(
      "<hdf5 input file>", "The input file containing the dataset to transform",
      CmdArg::isREQ);
  CmdArgType<std::string> dataset(
      'd', "dataset", "<hdf5 dataset path>", "The dataset to transform.",
      CmdArg::isREQ);
  CmdArgTypeFixedVector<double> transformation(
      't', "transformation", "<16 x double>", "The homogeneous transformation "
      "matrix to apply. The oredering is row major, i.e. the matrix entries "
      "are given in order A_11 A_12 A_13 ... A_43 A_44", 16);
  CmdArgType<std::string> referenceFileName(
      0, "referenceFile", "<hdf5 file>", "The name of the hdf5 file containing "
      "the reference dataset containing the transformation to apply.");
  CmdArgType<std::string> referenceDataset(
      0, "referenceDataset", "<hdf5 dataset path>", "The name of the dataset "
      "containing the transformation to apply.");
  CmdArgSwitch relative(
      'r', "relative", "Chain the transformation to the transformation of the "
      "dataset insted of replacing it.");

  CmdLine cmd(argv[0], "Set the transformation matrix of the dataset.");
  cmd.description(
      "The transformation can be either given directly on the command line "
      "via the -t (--transformation) parameter or extracted from a reference "
      "dataset which is given using the --referenceFile and --referenceDataset "
      "parameters. Usually the transformation matrix is replaced, but you can "
      "also apply the given transformation to the transformation already "
      "contained in the dataset when using the -r (--relative) switch.");

  try
  {
    cmd.append(&inFileName);
    cmd.append(&dataset);
    cmd.append(&transformation);
    cmd.append(&referenceFileName);
    cmd.append(&referenceDataset);
    cmd.append(&relative);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r  ");
    pr.setTaskProgressMin(0);
    pr.setTaskProgressMax(100);

    if (!(transformation.given() ||
          (referenceFileName.given() && referenceDataset.given())))
        throw std::runtime_error(
            "Either -t (--transformation) or a reference dataset specified by "
            "--referenceFile and -referenceDataset must be given.");

    blitz::TinyMatrix<double,4,4> trafo;
    if (transformation.given())
        std::memcpy(trafo.data(), transformation.data(), 16 * sizeof(double));
    else
    {
      BlitzH5File referenceFile(referenceFileName.value());
      referenceFile.readAttribute(
          trafo, "transformation", referenceDataset.value());
    }

    BlitzH5File outfile(inFileName.value(), BlitzH5File::Write);
    if (outfile.existsDataset(dataset.value()) ||
        outfile.existsGroup(dataset.value()))
    {
      if (relative.given())
      {
        blitz::TinyMatrix<double,4,4> trafoOld;
        outfile.readAttribute(trafoOld, "transformation", dataset.value());
        trafo = trafoOld * trafo;
      }
      outfile.writeAttribute(trafo, "transformation", dataset.value());
    }
    else
    {
      std::cerr << "No such dataset or group '" << inFileName.value() << ":"
                << dataset.value() << "'" << std::endl;
      return 1;
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
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    return -3;
  }
  catch (...)
  {
    std::cerr << "Unknown error during execution" << std::endl;
    return -4;
  }
  return 0;
}
