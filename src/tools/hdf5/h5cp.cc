// g++ -Wall -O3 -g -fopenmp -DBZ_THREADSAFE -I${HOME}/software/$HOSTGCC/include/irocs-toolbox -o h5cp h5cp.cc ${HOME}/software/$HOSTGCC/lib/libBlitzHdf5.a ${HOME}/software/$HOSTGCC/lib/libProgressReporter.a ${HOME}/software/$HOSTGCC/lib/libcmdline.a -lblitz -lhdf5 -lz -lpthread -lgomp && cp h5cp ${HOME}/software/$HOSTGCC/bin

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

int main(int argc, char** argv)
{
  std::string cmdlineString = argv[0];
  for (int i = 1; i < argc; ++i) cmdlineString += std::string(" ") + argv[i];

  CmdArgType<std::string> outfile(
      'o', "outfile", "<hdf5 file name>",
      "The output file to recursively copy the given hdf5 object to.",
      CmdArg::isREQ);
  CmdArgTypeVector<std::string> h5objects(
      's', "sourceObjects", "<hdf5 datasets and/or groups>",
      "The hdf5 objects to copy.", CmdArg::isREQ);
  CmdArgType<std::string> infile(
      "<hdf5 input file>", "The input file containing the object to copy.",
      CmdArg::isREQ);
  CmdArgSwitch forceFlag(
      'f', "force", "Force copying the objects. This means objects that "
      "already exist in the output file will be overwritten!");

  CmdLine cmd(argv[0], "Recursively copy the given object and its descendents "
              "from the input file to the output file");
  cmd.description(
      "The given hdf5 object and all sub-groups, datasets and attributes are "
      "copied from the input file to the output file. The names will not "
      "change.");

  try
  {
    cmd.append(&outfile);
    cmd.append(&h5objects);
    cmd.append(&infile);
    cmd.append(&forceFlag);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

    try
    {
      BlitzH5File inFile(infile.value());
      BlitzH5File outFile(outfile.value(), BlitzH5File::WriteOrNew);
      for (size_t i = 0; i < h5objects.size(); ++i)
      {
        if (forceFlag.given())
        {
          if (outFile.existsGroup(h5objects[i]))
              outFile.deleteGroup(h5objects[i]);
          if (outFile.existsDataset(h5objects[i]))
              outFile.deleteDataset(h5objects[i]);
        }
        inFile.copyObject(h5objects[i], outFile, 3);
      }
    }
    catch(BlitzH5Error& e)
    {
      std::cerr << "h5cp failed: " << e.what() << std::endl;
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
