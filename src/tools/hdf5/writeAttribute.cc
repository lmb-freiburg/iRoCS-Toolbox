// g++ -Wall -O3 -g -fopenmp -DBZ_THREADSAFE -I/home/falk/software/$HOSTGCC/include/irocs-toolbox -o writeAttribute writeAttribute.cc /home/falk/software/$HOSTGCC/lib/libIRoCS.a /home/falk/software/$HOSTGCC/lib/libArrayToolbox.a /home/falk/software/$HOSTGCC/lib/libBlitzHdf5.a /home/falk/software/$HOSTGCC/lib/libBlitzFFTW.a /home/falk/software/$HOSTGCC/lib/libProgressReporter.a /home/falk/software/$HOSTGCC/lib/libBaseFunctions.a /home/falk/software/$HOSTGCC/lib/libcmdline.a -lblitz -lhdf5 -lfftw3 -lfftw3f -lgsl -lgslcblas -lm -lgomp -lz

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include <stdexcept>
#include <blitz/array.h>

int main(int argc, char** argv)
{
  CmdArgType<std::string> outFileName(
      "<hdf5 file>", "The file to write a string attribute to", CmdArg::isREQ);
  CmdArgType<std::string> objectName(
      'o', "object", "<hdf5 object path>", "The object to write the attribute "
      "to. This may ba a group or a dataset.");
  objectName.setDefaultValue("/");
  CmdArgType<std::string> attributeName(
      'n', "name", "<string>", "The attribute's name", CmdArg::isREQ);
  CmdArgType<std::string> attributeValue(
      'v', "value", "<string>", "The value of the attribute", CmdArg::isREQ);

  CmdLine cmd(argv[0], "Add a string attribute to the given hdf5 object.");
  cmd.description(
      "The attribute is appended to the given hdf5 object, replacing an "
      "existing attribute with same name");

  try
  {
    cmd.append(&outFileName);
    cmd.append(&objectName);
    cmd.append(&attributeName);
    cmd.append(&attributeValue);
    
    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

    iRoCS::ProgressReporterStream pr(std::cout, 0, 0, 100, "\r  ");
    pr.setTaskProgressMin(0);
    pr.setTaskProgressMax(100);

    BlitzH5File outfile(outFileName.value(), BlitzH5File::Write);
    outfile.writeAttribute(
        attributeValue.value(), attributeName.value(), objectName.value());
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
