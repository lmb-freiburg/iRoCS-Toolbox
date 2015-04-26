/**************************************************************************
**       Title: test program for libcmdline
**    $RCSfile$
**   $Revision: 3287 $$Name$
**       $Date: 2010-01-14 23:08:53 +0100 (Thu, 14 Jan 2010) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.6  2002/04/22 15:58:36  pigorsch
**  - uses "deafult" functions
**
**  Revision 1.5  2002/04/11 08:45:26  ronneber
**  - adapted to new position of lmbunit.h
**
**  Revision 1.4  2002/04/10 11:08:09  pigorsch
**  - testing IStreamIter :-)
**
**  Revision 1.3  2002/04/04 12:36:41  pigorsch
**  - constructor changed again
**
**  Revision 1.2  2002/03/27 14:36:07  ronneber
**  - adapted to new CmdLine constructor
**
**  Revision 1.1  2002/03/26 07:45:29  ronneber
**  restructuring for autoconf
**
**  Revision 1.1  2002/03/26 07:36:28  ronneber
**  restructuring for autoconf
**
**  Revision 1.1.1.1  2002/03/22 13:45:07  pigorsch
**  moved from polsoft repository
**
**
**
**************************************************************************/

#include <iostream>
#include <cstdlib>

#include <libcmdline/IStreamIter.hh>
#include <libcmdline/ArgvIter.hh>
#include <libcmdline/CmdArgs.hh>
#include <libcmdline/CmdLine.hh>
#include <libcmdline/Compiler.hh>
#include <libcmdline/Errors.hh>

int main(
    int argc,
    char** argv)
{
  CmdArgSwitch toggle1('1', "toggle1", "This toggles feature 1");
  
  CmdArgSwitch toggle2('2', "toggle2", "This activates feature 2");
  
  CmdArgType<int> count(
      'c', "count", "number", "Number of hurzes.");
  count.setDefaultValue(0);
  
  CmdArgTypeVector<std::string> files(
      'f', "files", "filenames", "Files to delete.");
  
  CmdArgTypeFixedVector<double> position(
      'p', "position", "coordinates", "The initial 3dim position", 3);
  std::vector<double> def;
  def.push_back(1);
  def.push_back(2);
  def.push_back(3);
  position.setDefaultValue(def);
  
  CmdArgType<std::string> image(
      'i', "image", "filename", 
      "Image to show. The specified image file must exist and "
      "must be accessible by the current user. This sentence is "
      "so long because we want to known if line breaking works.");
  image.setDefaultValue("hurz.png");
  
  CmdArgType<int> answer(
      0, "answer", "life_universe_everything", 
      "Should be 42.\nWow - Look at that indentation...");
  answer.setDefaultValue(42);
  
  CmdArgTypeVector<std::string> hurzes(
      "hurzes", "All the hurzes we need. This is used for "
      "checking if vectors work...",
//      CmdArg::Syntax(CmdArg::isOPT|CmdArg::isVALOPT));
      CmdArg::isREQ);
  
  
  CmdLine cmd(argv[0], "just a test program", true);

  cmd.description(
      "Once you realize that documentation should be laughed at, peed upon, "
      "put on fire, and just ridiculed in general, THEN, and only then, have "
      "you reached the level where you can safely read it and try to use it "
      "to actually implement a driver.\n\n"
      "	                                 -- Linus Torvalds");
  
  ArgvIter iter(--argc, ++argv);
  //IStreamIter iter(std::cin);

  try
  { 
    cmd.append(&toggle1);
    cmd.append(&toggle2);
    cmd.append(&count);
    cmd.append(&files);
    cmd.append(&position);
    cmd.append(&image);
    cmd.append(&answer);
    cmd.append(&hurzes);
    
    cmd.parse(iter);
  }
  catch (CmdLineUsageError e)
  {
    cmd.usage();
    exit(0);
  }
  catch (CmdLineUsageHTMLError e)
  {
    cmd.usageHTML(std::cerr);
    exit(0);
  }
  catch (CmdLineSyntaxError e)
  {
    cmd.error() << "Oops: you seem to have given invalid command line arguments...\n"
                << e.str() << std::endl;
    cmd.usage();
    exit(-1);
  }
  catch (CmdLineError e)
  {
    cmd.error() << "Oops: an CmdLine error occured. That's bad...\n"
                << e.str() << std::endl;
    exit(-2);
  }
  catch (...)
  {
    cmd.error() << "Oops: an unknown error occured. That's really bad...\n";
    exit(-3);
  }

  std::cerr << "count   =" << count.value() << std::endl
       << "toggle1 =" << (toggle1.value() ? "true" : "false") << std::endl
       << "toggle2 =" << (toggle2.value() ? "true" : "false") << std::endl
       << "image   =" << image.value() << std::endl
       << "answer  =" << answer.value() << std::endl
       << "position=[";

  for (CmdArgTypeVector<double>::const_iterator p=position.begin();
       p!=position.end();
       ++p)
  {
    std::cerr << *p << " ";
  }
  std::cerr << "]" << std::endl;

  std::cerr << "hurzes  =[";
  
  for (CmdArgTypeVector<std::string>::const_iterator h=hurzes.begin();
       h!=hurzes.end();
       ++h)
  {
    std::cerr << *h << " ";
  }
  std::cerr << "]" << std::endl;  
}
