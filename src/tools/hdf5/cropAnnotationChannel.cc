// g++ -Wall -O3 -g -fopenmp -DHAVE_CONFIG_H -I/home/falk/software/$HOSTGCC/include/irocs-toolbox -o cropAnnotationChannel cropAnnotationChannel.cc /home/falk/software/$HOSTGCC/lib/libArrayToolbox.a /home/falk/software/$HOSTGCC/lib/libBlitzHdf5.a /home/falk/software/$HOSTGCC/lib/libProgressReporter.a /home/falk/software/$HOSTGCC/lib/libcmdline.a -lblitz -lhdf5 -lgsl -lgslcblas -lm -lgomp && mv cropAnnotationChannel ~/software/$HOSTGCC/bin

#include <config.hh>

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>

#include <libProgressReporter/ProgressReporterStream.hh>

#include <libArrayToolbox/ATBNucleus.hh>

int main(int argc, char** argv)
{
  CmdArgType<std::string> infile(
      "<input hdf5 file>",
      "The input file to crop the cells from", CmdArg::isREQ);
  CmdArgType<std::string> source(
      's', "source", "<hdf5 group>",
      "The annotation group to crop.", CmdArg::isREQ);
  CmdArgType<std::string> outfile(
      'o', "outfile", "<string>", "The output file name.", CmdArg::isREQ);
  CmdArgType<std::string> destination(
      'd', "destination", "<hdf5 dataset>", "The output dataset to write the "
      "cropped annotation channel to.", CmdArg::isREQ);
  CmdArgTypeVector<int> lowerBoundUm(
      'l', "lowerBoundUm", "<double vector>", "The lower bound of the cropped "
      "region in micrometers. If not given, it will be set to -infinity.");
  CmdArgTypeVector<int> upperBoundUm(
      'u', "upperBoundUm", "<double vector>", "The upper bound of the cropped "
      "region in micrometers. If not given it will be set to infinity");

  CmdLine cmd(argv[0], "Crop the annotation channel using the given bounds.");
  cmd.description(
      "The selected annotation channel is cropped using the given voxel "
      "bounds. Markers with radius attribute are removed if they touch one "
      "of the bounds.");

  try
  {
    cmd.append(&infile);
    cmd.append(&source);
    cmd.append(&outfile);
    cmd.append(&destination);
    cmd.append(&lowerBoundUm);
    cmd.append(&upperBoundUm);

    ArgvIter argvIter(--argc, ++argv);
    cmd.parse(argvIter);

    blitz::TinyVector<double,3> lbUm, ubUm;
    for (size_t i = 0; i < 3; ++i) {
      lbUm(i) = (i < lowerBoundUm.size()) ?
          lowerBoundUm[i] : -std::numeric_limits<double>::infinity();
      ubUm(i) = (i < upperBoundUm.size()) ?
          upperBoundUm[i] : std::numeric_limits<double>::infinity();
    }

    std::cout << "  Cropping range: " << lbUm << " - " << ubUm << std::endl;

    std::vector<atb::Nucleus> srcMarkers, trgMarkers;
    std::cout << "Loading markers from " << infile.value() << ":"
              << source.value() << std::endl;
    atb::Nucleus::loadList(srcMarkers, infile.value(), source.value());
    for (size_t i = 0; i < srcMarkers.size(); ++i) {
      if (blitz::all(
              srcMarkers[i].positionUm() + srcMarkers[i].radiusUm() >= lbUm) &&
          blitz::all(
              srcMarkers[i].positionUm() - srcMarkers[i].radiusUm() <= ubUm))
          trgMarkers.push_back(srcMarkers[i]);
    }
    std::cout << "Saving markers to " << outfile.value() << ":"
              << destination.value() << std::endl;
    atb::Nucleus::saveList(trgMarkers, outfile.value(), destination.value());
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
