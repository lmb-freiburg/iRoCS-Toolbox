// g++ -Wall -O3 -g -fopenmp -DBZ_THREADSAFE -I${HOME}/software/$HOSTGCC/include/irocs-toolbox -o h5rm h5rm.cc ${HOME}/software/$HOSTGCC/lib/libBlitzHdf5.a ${HOME}/software/$HOSTGCC/lib/libProgressReporter.a ${HOME}/software/$HOSTGCC/lib/libcmdline.a -lblitz -lhdf5 -lz -lpthread -lgomp && cp h5rm ${HOME}/software/$HOSTGCC/bin

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

int main(int argc, char** argv)
{
  std::string cmdlineString = argv[0];
  for (int i = 1; i < argc; ++i) cmdlineString += std::string(" ") + argv[i];

  CmdArgTypeVector<std::string> h5objects(
      'o', "objects", "<hdf5 datasets and/or groups>",
      "The hdf5 objects to remove.", CmdArg::isREQ);
  CmdArgType<std::string> infile(
      "<hdf5 input file>", "The input file containing the objects to remove.",
      CmdArg::isREQ);

  CmdLine cmd(argv[0], "Recursively remove the given objects and its "
              "descendents from the input file");
  cmd.description(
      "The given hdf5 object and all sub-groups, datasets and attributes are "
      "removed from the input file.");

  try
  {
    cmd.append(&h5objects);
    cmd.append(&infile);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

    try
    {
      BlitzH5File inFile(infile.value(), BlitzH5File::Write);
      for (size_t i = 0; i < h5objects.size(); ++i)
      {
        if (inFile.existsGroup(h5objects[i])) inFile.deleteGroup(h5objects[i]);
        if (inFile.existsDataset(h5objects[i]))
            inFile.deleteDataset(h5objects[i]);
      }
    }
    catch(BlitzH5Error& e)
    {
      std::cerr << "h5rm failed: " << e.what() << std::endl;
      exit(-1);
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
