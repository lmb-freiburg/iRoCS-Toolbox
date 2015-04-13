/**************************************************************************
 *
 * Copyright (C) 2005-2015 Olaf Ronneberger, Jörg Mechnich, Florian Pigorsch,
 *                         Mario Emmenlauer, Thorsten Schmidt
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

/**************************************************************************
**       Title: command line parser
**    $RCSfile$
**   $Revision: 4823 $$Name$
**       $Date: 2011-11-08 10:58:55 +0100 (Tue, 08 Nov 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.13  2005/06/09 07:54:33  tschmidt
**  - Changing enum names from bla to CmdLinebla
**  - Adding check for terminal width (I guess changed by emmenlau)
**
**  Revision 1.12  2003/01/31 02:52:28  mechnich
**  added XML output of all CmdLine arguments
**
**  Revision 1.11  2002/10/28 13:11:33  mechnich
**  made everything compile with gcc 3.2
**  removed promptUser() from CmdLine.*
**
**  Revision 1.10  2002/06/03 15:20:52  pigorsch
**  - removed UsageLevel
**
**  Revision 1.9  2002/05/14 08:16:10  pigorsch
**  - removed functions longPrefix() and shortPrefix()
**
**  Revision 1.8  2002/04/22 15:57:11  pigorsch
**  - added function replace()
**  - moved function basename() from .hh to.cc
**
**  Revision 1.7  2002/04/18 14:05:02  pigorsch
**  - removed #include "config.hh"
**
**  Revision 1.6  2002/04/18 14:01:00  pigorsch
**  - added some docu
**
**  Revision 1.5  2002/04/10 11:06:53  pigorsch
**  - fixed typo
**
**  Revision 1.4  2002/04/04 12:01:16  pigorsch
**  - moved error classes to Errors.hh
**  - added longname support
**  - added default help arguments
**  - added emtpy "mainpage" doxygen comment
**
**  Revision 1.3  2002/03/27 10:26:06  pigorsch
**  - fixed name / longname function headers
**
**  Revision 1.2  2002/03/27 10:24:19  pigorsch
**  - added long name support
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

/**************************************************************************
**//*!
**  \mainpage libcmdline: C++ command line parser
**  \section intro Introduction
**  "libcmdline" is used to parse command line arguments.
**
**  \section doc Documentation
**
**  \section usage Usage
***************************************************************************/

#ifndef CMDLINE_HH
#define CMDLINE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ArgIter.hh"
#include "CmdArg.hh"
#include "CmdArgs.hh"
#include "Errors.hh"

#include <iostream>
#include <list>
#include <string>
#include <cstddef>

// For ioctls to find terminal width on linux
#ifdef linux
#include <sys/ioctl.h>
#endif


#define LEFTMARGIN 5
#define STDCOLS 79
#define MAXCOLS 120

/*======================================================================*/
/*!
 *  \class CmdLine CmdLine.hh
 *  \brief The CmdLine class is a parser for command line arguments.
 *
 *
 *
 *  \todo Finish documentation.
 */
/*======================================================================*/
class CmdLine
{
public:
  /*====================================================================*/
  /*!
   *   Configuration flags for CmdLine.
   */
  /*====================================================================*/
  enum CmdLineFlags
  {
      FLAG_ANY_CASE_OPTS = 0x001, ///< Ignore character-case for short-options
      FLAG_PROMPT_USER   = 0x002, ///< Prompt the user for missing required args
      FLAG_NO_ABORT      = 0x004, ///< Don't quit upon syntax error
      FLAG_OPTS_FIRST    = 0x008, ///< No options after positional parameters
      FLAG_QUIET         = 0x080, ///< Don't print syntax error messages
      FLAG_ALL           = 0x0FF
  };


  /*====================================================================*/
  /*!
   *   Status flags for CmdLine.
   */
  /*====================================================================*/
  enum CmdLineStatus
  {
      STATUS_NO_ERROR      = 0x000,  ///< No problems.
      STATUS_ARG_MISSING   = 0x001,  ///< A required argument was not specified.
      STATUS_VAL_MISSING   = 0x002,  ///< A required argument value was not specified.
      STATUS_VAL_NOTSTICKY = 0x004,  ///< Value needs to be in same token.
      STATUS_VAL_NOTSEP    = 0x008,  ///< Value needs to be in separate token.
      STATUS_KWD_AMBIGUOUS = 0x010,  ///< An ambiguous keyword prefix was specified.
      STATUS_BAD_OPTION    = 0x020,  ///< An invalid option was specified.
      STATUS_BAD_KEYWORD   = 0x040,  ///< An invalid keyword was specified.
      STATUS_BAD_VALUE     = 0x080,  ///< An invalid value was specified for an arg.
      STATUS_TOO_MANY_ARGS = 0x100   ///< Too many positional args were specified.
  };


  /*====================================================================*/
  /*!
   *   State flags for CmdLine.
   */
  /*====================================================================*/
  enum CmdLineState
  {
      STATE_START_STATE    = 0x00, ///<
      STATE_END_OF_OPTIONS = 0x01, ///< End of options (--) occured.
      STATE_SHORT_USED     = 0x02, ///< Short options were used.
      STATE_LONG_USED      = 0x04, ///< Long options were used.
      STATE_ALL            = 0x07
  };


  /*====================================================================*/
  /*!
   *   Parsing state flags for CmdLine.
   */
  /*====================================================================*/
  enum CmdLineParseState
  {
      PARSE_START_STATE  = 0x00,  ///< Parsing of a CmdArg is completely finished.
      PARSE_TOK_REQUIRED = 0x01,  ///< The next token is required.
      PARSE_WANT_VAL     = 0x02,  ///< The last parsed CmdArg may optionally get a value.
      PARSE_NEED_VAL     = (PARSE_WANT_VAL | PARSE_TOK_REQUIRED), ///< The last parsed CmdArg requires a value.
      PARSE_ALL          = 0x07
  };


  /*====================================================================*/
  /*!
   *   Constructor.
   *   Sets the program name and longname.
   *
   *   \param name program name
   *   \param longname program long name
   *   \param help automatically add default help arguments?
   */
  /*====================================================================*/
  CmdLine(
      std::string name,
      std::string longname,
      bool help=true);

  /*====================================================================*/
  /*!
   *   Constructor.
   *   Sets the program name and longname.
   *   Adds given CmdArgs.
   *   ATTENTION: the last element in the CmdArg list must be NULL!
   *
   *   \param name program name
   *   \param longname program long name
   *   \param help automatically add default help arguments?
   *   \param cmdarg1 ... CmdArgs to add
   *
   *   \exception CmdLineDublicateArgError
   */
  /*====================================================================*/
  CmdLine(
      std::string name,
      std::string longname,
      bool help,
      CmdArg* cmdarg1 ...);


  /*====================================================================*/
  /*!
   *   Destructor.
   */
  /*====================================================================*/
  virtual
  ~CmdLine();


  /*====================================================================*/
  /*!
   *   Return previously specified program name.
   *
   *   \return program name
   */
  /*====================================================================*/
  std::string
  name() const;


  /*====================================================================*/
  /*!
   *   Specify program name.
   *
   *   \param name program name
   */
  /*====================================================================*/
  void
  name(const std::string& value);


  /*====================================================================*/
  /*!
   *   Return previously specified program long name.
   *
   *   \return program long name
   */
  /*====================================================================*/
  std::string
  longname() const;


  /*====================================================================*/
  /*!
   *   Specify program long name.
   *
   *   \param name program long name
   */
  /*====================================================================*/
  void
  longname(const std::string& value);


  /*====================================================================*/
  /*!
   *   Return previously specified program description.
   *
   *   \return program description
   */
  /*====================================================================*/
  std::string
  description() const;


  /*====================================================================*/
  /*!
   *   Specify program description.
   *
   *   \param description program description
   */
  /*====================================================================*/
  void
  description(const std::string& description);


  /*====================================================================*/
  /*!
   *   Append specified CmdArg to the list of CmdArgs.
   *
   *   \param cmdarg pointer to CmdArg to append
   *
   *   \exception CmdLineDublicateArgError
   *
   *   \return reference to changed CmdLine object
   */
  /*====================================================================*/
  CmdLine&
  append(CmdArg* cmdarg);


  /*====================================================================*/
  /*!
   *   Append specified CmdArg to the list of CmdArgs.
   *
   *   \param cmdarg reference to CmdArg to append
   *
   *   \exception CmdLineDublicateArgError
   *
   *   \return reference to modified CmdLine object
   */
  /*====================================================================*/
  CmdLine&
  append(CmdArg& cmdarg);


  /*====================================================================*/
  /*!
   *   Print usage message on the given output stream.
   *
   *   \param os    output stream
   *
   *   \return modified output stream
   */
  /*====================================================================*/
  std::ostream&
  usage(std::ostream& os) const;


  /*====================================================================*/
  /*!
   *   Print usage message on CmdLine's error output stream.
   *
   *   \return modified output stream
   */
  /*====================================================================*/
  std::ostream&
  usage() const;


  /*====================================================================*/
  /*!
   *   Print XML usage information on the given output stream.
   *
   *   \param os output stream
   *
   *   \return modified output stream
   */
  /*====================================================================*/
  std::ostream&
  usageXML(std::ostream& os) const;


  /*====================================================================*/
  /*!
   *   Print HTML usage message on the given output stream.
   *
   *   \param os output stream
   *
   *   \return modified output stream
   */
  /*====================================================================*/
  std::ostream&
  usageHTML(std::ostream& os) const;


  /*====================================================================*/
  /*!
   *   Print LATEX usage message on the given output stream.
   *
   *   \param os output stream
   *
   *   \return modified output stream
   *
   *   \todo Implement function.
   */
  /*====================================================================*/
  std::ostream&
  usageLATEX(std::ostream& os) const;


  /*====================================================================*/
  /*!
   *   Return CmdLine's status.
   *
   *   \return status
   */
  /*====================================================================*/
  CmdLine::CmdLineStatus
  status() const;


  /*====================================================================*/
  /*!
   *   Specify new set of status flags.
   *
   *   \param s status to set
   */
  /*====================================================================*/
  void
  status(CmdLine::CmdLineStatus s);


  /*====================================================================*/
  /*!
   *   Set only given status flags. Other status flags are not modified.
   *
   *   \param s status to set
   */
  /*====================================================================*/
  void
  setStatus(CmdLine::CmdLineStatus s);


  /*====================================================================*/
  /*!
   *   Checks if the given status flags are set.
   *
   *   \param s status to check
   *
   *   \return true if the given status flags are set
   */
  /*====================================================================*/
  bool
  hasStatus(CmdLine::CmdLineStatus s) const;


  /*====================================================================*/
  /*!
   *   Return CmdLine's state.
   *
   *   \return state
   */
  /*====================================================================*/
  CmdLine::CmdLineState
  state() const;


  /*====================================================================*/
  /*!
   *   Specify new set of state flags.
   *
   *   \param s state to set
   */
  /*====================================================================*/
  void
  state(CmdLine::CmdLineState s);


  /*====================================================================*/
  /*!
   *   Set only given state flags. Other state flags are not modified.
   *
   *   \param s state to set
   */
  /*====================================================================*/
  void
  setState(CmdLine::CmdLineState s);


  /*====================================================================*/
  /*!
   *   Clear only the given state flags. Other state flags are not
   *   modified.
   *
   *   \param s state to clear
   */
  /*====================================================================*/
  void
  clearState(CmdLine::CmdLineState s=STATE_ALL);


  /*====================================================================*/
  /*!
   *   Checks if the given state flags are set.
   *
   *   \param s state to check
   *
   *   \return true if the given state flags are set
   */
  /*====================================================================*/
  bool hasState(CmdLine::CmdLineState s) const;


  /*====================================================================*/
  /*!
   *   Return CmdLine's flags.
   *
   *   \return flags
   */
  /*====================================================================*/
  CmdLine::CmdLineFlags
  flags() const;


  /*====================================================================*/
  /*!
   *   Specify new set of flags.
   *
   *   \param f Flags to set
   */
  /*====================================================================*/
  void
  flags(CmdLine::CmdLineFlags f);


  /*====================================================================*/
  /*!
   *   Set only the given flags. Other flags are not modified.
   *
   *   \param f Flags to set
   */
  /*====================================================================*/
  void
  setFlags(CmdLine::CmdLineFlags f);


  /*====================================================================*/
  /*!
   *   Clear only the given flags. Other flags are not modified.
   *
   *   \param f Flags to clear
   */
  /*====================================================================*/
  void
  clearFlags(CmdLine::CmdLineFlags f=FLAG_ALL);


  /*====================================================================*/
  /*!
   *   Checks if the given flags are set.
   *
   *   \param f flags to check
   *
   *   \return true if the given flags are set
   */
  /*====================================================================*/
  bool hasFlag(CmdLine::CmdLineFlags f) const;


  /*====================================================================*/
  /*!
   *   Return CmdLine's error output stream.
   *
   *   \return error output stream
   */
  /*====================================================================*/
  std::ostream&
  error() const;


  /*====================================================================*/
  /*!
   *   Specify CmdLine's error output stream.
   *
   *   \param os error output stream
   */
  /*====================================================================*/
  void
  error(std::ostream& os);


  /*====================================================================*/
  /*!
   *   Parse a set of arguments. Pre- and post-processing (resetting
   *   arguments, ...) is automatically performed (by calling prologue()
   *   and epilogue()) if autoProcessing is true.
   *
   *   \param argiter ArgIter that provides the arguments
   *   \param autoProcessing if true perform pre- and post-processing
   *
   *   \return resultant status
   */
  /*====================================================================*/
  CmdLine::CmdLineStatus
  parse(
      ArgIter& argiter,
      bool autoProcessing=true);


  /*====================================================================*/
  /*!
   *   Perform the necessary pre-processing.
   *
   *   \return resultant status
   */
  /*====================================================================*/
  CmdLine::CmdLineStatus
  prologue();


  /*====================================================================*/
  /*!
   *   Parse a single argument.
   *
   *   \param arg argument to be processed
   *
   *   \return resultant status
   */
  /*====================================================================*/
  CmdLine::CmdLineStatus
  parseArg(const char* arg);


  /*====================================================================*/
  /*!
   *   Perform the necessary post-processing.
   *
   *   \exception CmdLineSyntaxError
   *
   *   \return resultant status
   */
  /*====================================================================*/
  CmdLine::CmdLineStatus
  epilogue();


  /*====================================================================*/
  /*!
   *   Find out the number of arguments parsed so far.
   *
   *   \return number of arguments parsed
   */
  /*====================================================================*/
  unsigned int
  nargsParsed() const;


  /*====================================================================*/
  /*!
   *   Retrieve an argument based on its short name. Returns NULL if no
   *   argument matches the given character.
   *
   *   \param shortName short name to find
   *
   *   \return pointer to matching CmdArg
   */
  /*====================================================================*/
  CmdArg*
  getArg(char shortName) const;


  /*====================================================================*/
  /*!
   *   Retrieve an argument based on its long name. Returns NULL if no
   *   argument matches the given character.
   *
   *   \param longName long name to find
   *
   *   \return pointer to matching CmdArg
   */
  /*====================================================================*/
  CmdArg*
  getArg(const std::string& keyword) const;


private:
  CmdLine::CmdLineParseState pParseState;
  CmdLine::CmdLineState pState;
  CmdLine::CmdLineFlags pFlags;
  CmdLine::CmdLineStatus pStatus;
  unsigned int pNArgsParsed;
  std::string pName;
  std::string pLongName;
  std::string pDescription;
  CmdArg* pMatchedArg;
  std::list<CmdArg*> pArguments;
  std::ostream* pErr;
  CmdArgThrow<CmdLineUsageError> pUsageArg;
  CmdArgThrow<CmdLineUsageHTMLError> pUsageHTMLArg;
  CmdArgThrow<CmdLineUsageXMLError> pUsageXMLArg;


  /*====================================================================*/
  /*!
   *   Result of a string match.
   */
  /*====================================================================*/
  enum CmdLineMatch
  {
      MATCH_NONE,    ///< No match.
      MATCH_PARTIAL, ///< Partial match.
      MATCH_EXACT    ///< Exact match.
  };

  /*====================================================================*/
  /*!
   *   Try to match "attempt" against "src", if "len" is not 0 then
   *   only the first "len" characters are compared.
   *   Returns MATCH_EXACT for an exact match, MATCH_PARTIAL for a partial
   *   match and MATCH_NONE otherwise.
   *
   *   \param src
   *   \param attempt
   *   \param len     number of characters to compare
   *
   *   \return result of matching
   */
  /*====================================================================*/
  CmdLineMatch
  static strmatch(
      const std::string& src,
      const std::string& attempt,
      size_t len=0);


  /*====================================================================*/
  /*!
   *   Print a hanging indented paragraph on an outstream. Long lines
   *   are broken at word boundaries and are wrapped to line up with
   *   the rest of the paragraph.  The format looks like the following:
   *
   *   \verbatim
   *   <------------------------79---------------------------->
   *   <---5---><---indent--->
   *            title         This is the first sentence.  This
   *                          is the second sentence. etc ...
   *   \endverbatim
   *
   *   \param os print on this ostream
   *   \param title title to print
   *   \param indent distance between start of title and start of text
   *   \param test text to print
   *
   */
  /*====================================================================*/
  static void
  strindent(
      std::ostream& os,
      const std::string& title,
      unsigned int indent,
      const std::string& text);


  /*====================================================================*/
  /*!
   *   Return the basename of a given filename.
   *
   *   \param filename
   *
   *   \return basename
   */
  /*====================================================================*/
  static std::string
  basename(const std::string& filename);


  /*====================================================================*/
  /*!
   *   Copy constructor. Not allowed!
   */
  /*====================================================================*/
  CmdLine(const CmdLine&)
        :pParseState(PARSE_START_STATE),
         pState(STATE_START_STATE),
         pFlags(FLAG_OPTS_FIRST),
         pStatus(STATUS_NO_ERROR),
         pNArgsParsed(0),
         pName(""),
         pLongName(""),
         pDescription(""),
         pMatchedArg(0),
         pErr(0),
         pUsageArg(0, "help", "Display usage message."),
         pUsageHTMLArg(0, "usagehtml", "Display HTML usage message."),
         pUsageXMLArg(0, "usagexml", "Display XML usage message.")
        {};


  /*====================================================================*/
  /*!
   *   Assignment operator. Not allowed!
   */
  /*====================================================================*/
  CmdLine&
  operator=(const CmdLine&)
        {
          return *this;
        };


  /*====================================================================*/
  /*!
   *   Usage syntax flags.
   */
  /*====================================================================*/
  enum UsageSyntax
  {
      USAGE_SHORT_ONLY = 0, ///< Only display short options.
      USAGE_LONG_ONLY  = 1, ///< Only display long options.
      USAGE_BOTH = 2        ///< Display both short and long options.
  };


  /*====================================================================*/
  /*!
   *   Let the given CmdArg process the given string.
   *
   *   \param cmdarg Pointer to a CmdArg
   *   \param arg    String that is to be handled.
   *   \param endptr Pointer to the first unhandled character in arg.
   *
   *   \return True if cmdarg could successfully handle arg.
   */
  /*====================================================================*/
  bool
  handleArg(
      CmdArg* cmdarg,
      const char* arg,
      const char*& endptr);


  /*====================================================================*/
  /*!
   *   Checks if all required arguments/values are given and sets flags.
   */
  /*====================================================================*/
  void
  ck_need_val();


  /*====================================================================*/
  /*!
   *   Checks which type of syntax to use.
   *
   *   \return type of syntax
   */
  /*====================================================================*/
  UsageSyntax
  syntax() const;


  /*====================================================================*/
  /*!
   *   Checks if all required arguments were specified and calls
   *   promptUser if necessary.
   *
   *   \return status
   */
  /*====================================================================*/
  CmdLine::CmdLineStatus
  missingArgs();


  /*====================================================================*/
  /*!
   *   Returns the matching CmdArg.
   *
   *   \param optchar short name to match
   *
   *   \return matching CmdArg, NULL if nothing matched
   */
  /*====================================================================*/
  CmdArg*
  matchShort(char optchar) const;


  /*====================================================================*/
  /*!
   *   Returns the matching CmdArg.
   *
   *   \param kwd long name to match
   *   \param len how many chars must match, 0 for all
   *   \param is_ambiguous is the match ambigous?
   *
   *   \return matching CmdArg, NULL if nothing matched
   */
  /*====================================================================*/
  CmdArg*
  matchLong(
      const std::string& kwd,
      ptrdiff_t len,
      bool& is_ambiguous) const;


  /*====================================================================*/
  /*!
   *   Try to match positional argument.
   *
   *   \return matching positional CmdArg, NULL if nothing matched
   */
  /*====================================================================*/
  CmdArg*
  matchPos() const;

  /*====================================================================*/
  /*!
   *   Checks if string is a short name.
   *
   *   \param s string to check
   *
   *   \return true if it is a short name
   */
  /*====================================================================*/
  static bool
  isShort(const char* s);


  /*====================================================================*/
  /*!
   *   Checks if string is a long name.
   *
   *   \param s string to check
   *
   *   \return true if it is a long name
   */
  /*====================================================================*/
  static bool
  isLong(const char* s);


  /*====================================================================*/
  /*!
   *   Checks if string is EndOfOptions ("--") argument.
   *
   *   \param s string to check
   *
   *   \return true if it is EndOfOptions ("--") argument
   */
  /*====================================================================*/
  static bool
  isEndOfOptions(const char* s);


  /*====================================================================*/
  /*!
   *   Parse argument given by short name.
   *
   *   \param arg string to parse
   *
   *   \return status
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdLine::CmdLineStatus
  parseShort(const char* arg);


  /*====================================================================*/
  /*!
   *   Parse argument given by long name.
   *
   *   \param arg string to parse
   *
   *   \return status
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdLine::CmdLineStatus
  parseLong(const char* arg);


  /*====================================================================*/
  /*!
   *   Parse a value.
   *
   *   \param arg string to parse
   *
   *   \return status
   *
   *   \exception CmdLineInternalError
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdLine::CmdLineStatus
  parseValue(const char* arg);


  /*====================================================================*/
  /*!
   *   Print formatted error message.
   *
   *   \param error_str error message
   *   \param cmdarg CmdArg that caused the error
   *
   *   \return modified ostream
   */
  /*====================================================================*/
  std::ostream&
  argError(
      const std::string& error_str,
      const CmdArg* cmdarg) const;


  /*====================================================================*/
  /*!
   *   Print synopsis.
   *
   *   \param syntax syntax type
   *   \param os ostream to print to
   *
   *   \return length of longest argument string
   */
  /*====================================================================*/
  unsigned int
  printSynopsis(
      UsageSyntax syntax,
      std::ostream& os) const;


  /*====================================================================*/
  /*!
   *   Print descriptions of all arguments.
   *
   *   \param syntax syntax type
   *   \param os ostream to print to
   *   \param longest length of longest argument string
   */
  /*====================================================================*/
  void
  printDescriptions(
      UsageSyntax syntax,
      std::ostream& os,
      unsigned int longest) const;

  /*====================================================================*/
  /*!
   *   Replace all occurances of replacedText in s by replacedBy.
   *
   *   \param s string in which text is to be replaced
   *   \param replacedText string that is to be replaced
   *   \param replacedBy string that replaces replacedText
   *
   *   \return modified string
   */
  /*====================================================================*/
  static
  std::string
  replace(
      std::string s,
      const std::string& replacedText,
      const std::string& replacedBy);

};

#include "CmdLine.icc"

#endif
