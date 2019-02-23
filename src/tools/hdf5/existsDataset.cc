// g++ -Wall -O3 -g -fopenmp -DHAVE_CONFIG_H -I/home/falk/software/$HOSTGCC/include/irocs-toolbox -o existsDataset existsDataset.cc /home/falk/software/$HOSTGCC/lib/libArrayToolbox.a /home/falk/software/$HOSTGCC/lib/libBlitzHdf5.a /home/falk/software/$HOSTGCC/lib/libProgressReporter.a /home/falk/software/$HOSTGCC/lib/libcmdline.a -lblitz -lhdf5 -lgsl -lgslcblas -lm -lgomp -ldl -lpthread -lz && mv existsDataset /home/falk/software/$HOSTGCC/bin

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

  CmdLine cmd(argv[0], "Check whether an hdf5 dataset exists.");
  cmd.description(
      "This script checks for the existence of a dataset with given name in "
      "the input file. If a dataset with that name exists it returns 0 "
      "otherwise an error code is returned. This script is intended to be "
      "used in bash scripts.");

  try
  {
    cmd.append(&hdf5file);
    cmd.append(&dataset);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r  ");
    pr.setTaskProgressMin(0);
    pr.setTaskProgressMax(100);

    BlitzH5File inFile(hdf5file.value());
    if (inFile.existsDataset(dataset.value()))
    {
      std::cout << "'" << hdf5file.value() << ":" << dataset.value()
                << "' exists!" << std::endl;
      return 0;
    }       
    else
    {
      std::cout << "'" << hdf5file.value() << ":" << dataset.value()
                << "' does not exist!" << std::endl;
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
  catch (BlitzH5Error &e)
  {
    std::cerr << "BlitzH5Error during operation: " << e.what()
              << std::endl;
    return 1;
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
