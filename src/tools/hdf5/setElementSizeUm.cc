// g++ -Wall -O3 -g -fopenmp -DHAVE_CONFIG_H -I/home/falk/software/$HOSTGCC/include/irocs-toolbox -o setElementSizeUm setElementSizeUm.cc /home/falk/software/$HOSTGCC/lib/libArrayToolbox.a /home/falk/software/$HOSTGCC/lib/libBlitzHdf5.a /home/falk/software/$HOSTGCC/lib/libProgressReporter.a /home/falk/software/$HOSTGCC/lib/libcmdline.a -lblitz -lhdf5 -lgsl -lgslcblas -lm -lgomp -ldl -lpthread -lz && mv setElementSizeUm /home/falk/software/$HOSTGCC/bin

#include <config.hh>

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

#include <libArrayToolbox/Array.hh>

int main(int argc, char** argv)
{
  CmdArgType<std::string> hdf5file(
      "<hdf5 file>",
      "The file containing the dataset for which the element size should be "
      "set.", CmdArg::isREQ);
  CmdArgType<std::string> dataset(
      'd', "dataset", "<hdf5 dataset>", "The dataset for which the element "
      "size will be changed.", CmdArg::isREQ);
  CmdArgTypeVector<double> elementSizeUm(
      'e', "elementSizeUm", "<double>[ <double>]*", "The new element size of "
      "the dataset in (z y x) ordering. Give as many numbers as your dataset "
      "contains dimensions otherwise the behavior of tools reading the dataset "
      "is undefined.");
  CmdArgType<std::string> referenceFileName(
      0, "referenceFile", "<hdf5 file>", "The name of the hdf5 file containing "
      "the reference dataset containing the element size to apply.");
  CmdArgType<std::string> referenceDataset(
      0, "referenceDataset", "<hdf5 dataset path>", "The name of the dataset "
      "containing the element size to apply.");

  CmdLine cmd(argv[0], "Set the element size of the given dataset.");
  cmd.description(
      "The element size in micrometers of the dataset is set to the provided "
      "vector or copied from the given dataset.");

  try
  {
    cmd.append(&hdf5file);
    cmd.append(&dataset);
    cmd.append(&referenceFileName);
    cmd.append(&referenceDataset);
    cmd.append(&elementSizeUm);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r  ");
    pr.setTaskProgressMin(0);
    pr.setTaskProgressMax(100);

    if (!(elementSizeUm.given() ||
          (referenceFileName.given() && referenceDataset.given())))
        throw std::runtime_error(
            "Either -e (--elementSizeUm) or a reference dataset specified by "
            "--referenceFile and -referenceDataset must be given.");

    std::vector<double> elSize(elementSizeUm.size(), 1.0);
    if (elementSizeUm.given())
        std::memcpy(elSize.data(), elementSizeUm.data(),
                    elementSizeUm.size() * sizeof(double));
    else
    {
      BlitzH5File referenceFile(referenceFileName.value());
      referenceFile.readAttribute(
          elSize, "element_size_um", referenceDataset.value());
    }

    try
    {
      BlitzH5File outFile(hdf5file.value(), BlitzH5File::Write);
      if (outFile.existsDataset(dataset.value()) ||
          outFile.existsGroup(dataset.value()))
          outFile.writeAttribute(elSize, "element_size_um", dataset.value());
      else
      {
        std::cerr << "No such dataset or group '" << hdf5file.value() << ":"
                  << dataset.value() << "'" << std::endl;
        return 1;
      }
    }
    catch (BlitzH5Error &e)
    {
      std::cerr << "Could not set element size of dataset: " << e.what()
                << std::endl;
      return -1;
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
