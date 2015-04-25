/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

#include <liblabelling_qt4/ExceptionAwareApplication.hh>
#include <iostream>

#include <liblabelling_qt4/LabellingMainWidget.hh>

#include <libcmdline/CmdLine.hh>
#include <libcmdline/ArgvIter.hh>


#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
// For sysconf
#include <unistd.h>
// Print stacktrace after segmentation fault
#include <execinfo.h>
#include <signal.h>
#endif

#if !defined(_WIN32) && !defined(_WIN64)
void segfault_handler(int)
{
  void *array[10];
  size_t size;
  
  // get void*'s for all entries on the stack
  size = backtrace(array, 10);
  
  // print out all the frames to stderr
  std::cerr << "Segmentation fault! Backtrace follows" << std::endl;
  backtrace_symbols_fd(array, size, 2);
  exit(-1);
}
#endif

int main(int argc, char **argv )
{
#if !defined(_WIN32) && !defined(_WIN64)
  signal(SIGSEGV, segfault_handler);
#endif

  ExceptionAwareApplication a(argc, argv);
  
  /*---------------------------------------------------------------------
   *  Specify command line arguments and descriptions
   *---------------------------------------------------------------------*/
  CmdArgType<std::string> memoryLimit(
      'm', "memory-limit", "[0-9]*[kKmMgG]", 
      "Set the maximum memory usage of labelling plugins");

  CmdArgType<std::string> inFileName(
      "infile",
      "input file (HDF5). If the file contains a labelling project it will be "
      "opened, if not a selection dialog let's the user choose data sets and "
      "annotations to load. The import mode can be forced by giving the "
      "-i (--interactive) command line flag", CmdArg::isPOSVALOPT);
  CmdArgSwitch forceImport(
      'i', "interactive", "Disregard any project information contained in the "
      "input file and let the user select chennels using an import channel "
      "dialog.");
  
  CmdLine cmd(argv[0], "Annotate 3D volumes from hdf5 datasets in an "
              "orthographic view");

  try
  {
    cmd.append(&memoryLimit);
    cmd.append(&inFileName);
    cmd.append(&forceImport);
    
    cmd.description( 
        "labelling is an interactive tool for the annotation of 3D microscopic "
        "data sets. It's specifically useful for the annotation of cell "
        "nucleus positions of different tissues. Its plugin interface allows "
        "to arbitrarily increase its capabilities.");
    
    /*---------------------------------------------------------------------
     *  Parse command line
     *---------------------------------------------------------------------*/
    ArgvIter arg_iter(--argc, ++argv);
    cmd.parse(arg_iter);

#if defined(_WIN32) || defined(_WIN64)
    std::freopen("stderr.txt", "w", stderr);
#endif
    
    size_t mem = 0;
    if (memoryLimit.given())
    {
      if (memoryLimit.value().find_last_of("0123456789") ==
          memoryLimit.value().size() - 1)
          mem = atoi(memoryLimit.value().c_str());
      else
      {
        mem = atoi(memoryLimit.value().substr(
                       0, memoryLimit.value().size() - 1).c_str());
        char unit = memoryLimit.value()[memoryLimit.value().size() - 1];
        switch(unit){
        case 'k' :
        case 'K' :
          mem *= 1024;
          break;
        case 'm' :
        case 'M' :
          mem *= 1024 * 1024;
          break;
        case 'g' :
        case 'G' :
          mem *= 1024 * 1024 * 1024;
          break;
        default :
          break;
        }
      }
    }
    
    if (mem == 0)
    {
      // Set memory limit to 80% system memory installed
#if defined(_WIN32) || defined(_WIN64)
      MEMORYSTATUSEX statex;
      statex.dwLength = sizeof(statex);
      GlobalMemoryStatusEx(&statex);
      mem = static_cast<size_t>(0.8 * statex.ullTotalPhys);
#else
      mem = static_cast<size_t>(
          0.8 * static_cast<double>(sysconf(_SC_PHYS_PAGES)) *
          static_cast<double>(sysconf(_SC_PAGE_SIZE)));
#endif
    }
    
    // std::cout << "Using " << mem << " Bytes RAM" << std::endl;
    
    LabellingMainWidget *mw = NULL;
    if (inFileName.given())
        mw = new LabellingMainWidget(
            mem, inFileName.value(), forceImport.given());
    else mw = new LabellingMainWidget(mem);
    
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    
    int status = a.exec();

    delete mw;
    
    return status;
  }
  catch (CmdLineUsageError e)
  {
    cmd.usage();
    exit(0);
  }
  catch (CmdLineUsageHTMLError e)
  {
    cmd.usageHTML( std::cout);
    exit(0);
  }
  catch (CmdLineUsageXMLError e)
  {
    cmd.usageXML( std::cout);
    exit(0);
  }
  catch (CmdLineSyntaxError e)
  {
    cmd.error() << e.str() << std::endl;
    cmd.usage( std::cerr);
    exit(-1);
  }
  catch (std::exception& e) 
  {
    std::cerr << "Error while parsing command line arguments: " << e.what()
              << std::endl;
    cmd.usage();
    exit(-1);
  }
  catch(...)
  {
    std::cerr << "Error during command line parsing" << std::endl;
    cmd.usage();
    exit(-1);
  }
}
