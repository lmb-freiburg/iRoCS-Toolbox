// g++ -Wall -O2 -g -fopenmp -DHAVE_CONFIG_H -I/home/falk/software/$HOSTGCC/include/irocs-toolbox -o existsAttribute existsAttribute.cc /home/falk/software/$HOSTGCC/lib/libBlitzHdf5.a /home/falk/software/$HOSTGCC/lib/libcmdline.a -lhdf5 -lgomp -ldl -lpthread -lz

#include <config.hh>

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

int main(int argc, char** argv)
{
  CmdArgType<std::string> hdf5file(
      "<hdf5 file>",
      "The file containing the dataset for which the element size should be "
      "set.", CmdArg::isREQ);
  CmdArgType<std::string> object(
      'o', "object", "<hdf5 object>",
      "The object the attribute belongs to (hdf5 group or dataset).");
  object.setDefaultValue("/");
  CmdArgType<std::string> attribute(
      'a', "attribute", "<attribute name>",
      "The name of the attribute.", CmdArg::isREQ);

  CmdLine cmd(argv[0], "Check whether an hdf5 attribute exists.");
  cmd.description(
      "This script checks for the existence of an attribute attached to "
      "the given object in the input file. If such an attribute exists the "
      "command returns 0 otherwise an error. This script is intended to be "
      "used in bash scripts.");

  try
  {
    cmd.append(&hdf5file);
    cmd.append(&object);
    cmd.append(&attribute);

    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

    BlitzH5File inFile(hdf5file.value());
    return (inFile.existsAttribute(attribute.value(), object.value())) ? 0 : 1;
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
