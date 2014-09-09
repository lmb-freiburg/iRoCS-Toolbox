/**************************************************************************
**       Title: abstract command line argument class
**    $RCSfile$
**   $Revision: 716 $$Name$
**       $Date: 2005-06-09 09:48:17 +0200 (Thu, 09 Jun 2005) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.11  2005/06/09 07:46:20  tschmidt
**  - Changing enum names from bla to CmdLinebla
**
**  Revision 1.10  2003/01/31 02:52:28  mechnich
**  added XML output of all CmdLine arguments
**
**  Revision 1.9  2002/08/08 10:26:45  pigorsch
**  - changed function synopsis so that "[]" are not printed anymore in parameter descriptions
**
**  Revision 1.8  2002/06/03 15:24:44  pigorsch
**  - modified some comments
**
**  Revision 1.7  2002/05/27 15:35:49  pigorsch
**  - removed parameter "detailed" in function synopsis
**
**  Revision 1.6  2002/05/14 08:15:38  pigorsch
**  - added documenation
**
**  Revision 1.5  2002/04/22 15:56:24  pigorsch
**  - added virtual function hasDefaultValue()
**  - added virtual function getDefaultValueString()
**  - added virtual function clearFixedList()
**
**  Revision 1.4  2002/04/18 13:54:35  pigorsch
**  - made functions fixedSize() and remainingSize() virtual
**  - removed parameter fixedSize from constructors
**  - removed members pFixedSize, pRemaining
**  - renamed function clear(...) clearFlags(...) to avoid ambiguities
**
**  Revision 1.3  2002/04/10 09:53:56  pigorsch
**  - added functions to check if the argument was specified on the
**    command line and if it was modified during last parsing
**
**  Revision 1.2  2002/04/04 12:02:19  pigorsch
**  - removed "dummy" stuff
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

#ifndef CMDARG_HH
#define CMDARG_HH

#include <sstream>
#include <string>

class CmdLine;

/*======================================================================*/
/*!
 *  \class CmdArg CmdArg.hh
 *  \brief The CmdArg class is the abstract base class for all command
 *         line arguments.
 */
/*======================================================================*/
class CmdArg 
{
public:
  friend class CmdLine;

  /*====================================================================*/
  /*! 
   *   Flags that define the argument syntax
   */
  /*====================================================================*/
  enum Syntax 
  {
      isOPT       = 0x000,  ///< argument is optional
      isREQ       = 0x001,  ///< argument is required
      isVALOPT    = 0x002,  ///< argument value is optional
      isVALREQ    = 0x004,  ///< argument value is required
      isVALSEP    = 0x008,  ///< argument value must be in a separate token
      isVALSTICKY = 0x010,  ///< argument value must be in the same token
      isLIST      = 0x020,  ///< argument is a list
      isPOS       = 0x040,  ///< argument is positional
      isHIDDEN    = 0x080,  ///< argument is not to be printed in usage
      isFIXEDLIST = 0x100,  ///< argument value is a list of fixed size
      isVALTAKEN  = (isVALREQ | isVALOPT),    ///< argument takes a value
      isOPTVALOPT = (isOPT | isVALOPT),
      isOPTVALREQ = (isOPT | isVALREQ),
      isPOSVALOPT = (isPOS | isVALOPT),
      isPOSVALREQ = (isPOS | isVALREQ)
  };

  /*====================================================================*/
  /*! 
   *   Flags that say how the argument was specied on the command-line
   */
  /*====================================================================*/
  enum CmdLineFlags 
  {
      GIVEN      = 0x01,  ///< argument was given
      VALGIVEN   = 0x02,  ///< argument value was given
      OPTION     = 0x04,  ///< item was matched as an option
      KEYWORD    = 0x08,  ///< item was matched as a keyword
      POSITIONAL = 0x10,  ///< item was matched as a positional argument
      VALSEP     = 0x20,  ///< value was in a separate token
      ALLFLAGS   = 0x3F
  };

  /*====================================================================*/
  /*! 
   *   Create an argument that takes a value.
   *   The default flags are to assume that the argument is optional
   *   and that the value is required.
   *   If you don't want the argument to have a short name, set 
   *   shortName=0.
   *   If you don't want the argument to have a long name, set 
   *   longName="".
   *
   *   \param shortName the short name of the argument
   *   \param longName the long name of the argument 
   *   \param valueName the name of the value
   *   \param description the argument's description
   *   \param syntax_flags
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdArg(
      char shortName,
      const std::string longName,
      const std::string valueName,
      const std::string description,
      CmdArg::Syntax syntax_flags=isOPTVALREQ);
  
  /*====================================================================*/
  /*! 
   *   Create an argument that takes no value.
   *   The default flags are to assume that the argument is optional.
   *   If you don't want the argument to have a short name, set 
   *   shortName=0.
   *   If you don't want the argument to have a long name, set 
   *   longName="". 
   *
   *   \param shortName the short name of the argument
   *   \param longName the long name of the argument 
   *   \param description the argument's description
   *   \param syntax_flags
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdArg(
      char shortName,
      const std::string longName,
      const std::string description,
      CmdArg::Syntax syntax_flags=isOPT);
  
  /*====================================================================*/
  /*! 
   *   Create a positional argument (no short and long names).
   *   The default flags are to assume that the argument is positional
   *   and that the value is required.
   *
   *   \param valueName the name of the value
   *   \param description the argument's description
   *   \param syntax_flags
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdArg(
      const std::string value,
      const std::string description,
      CmdArg::Syntax syntax=isPOSVALREQ);

  /*====================================================================*/
  /*! 
   *   Destructor.
   */
  /*====================================================================*/  
  virtual 
  ~CmdArg();

  /*====================================================================*/
  /*! 
   *   Return fixed size if this CmdArg is a fixed list, return 0
   *   otherwise.
   *
   *   \return fixed size
   */
  /*====================================================================*/
  virtual
  unsigned int
  fixedSize() const;
  
  /*====================================================================*/
  /*! 
   *   Return number of remaining values if this CmdArg is a fixed list,
   *   return 0 otherwise.
   *
   *   \return remaining number of values
   */
  /*====================================================================*/
  virtual
  unsigned int 
  remainingSize() const;
  
  /*====================================================================*/
  /*! 
   *   Parse a value string. 
   *   If this argument does not take a value, or the
   *   value is optional and was NOT supplied, then "" is passed as arg.
   *   If arg was completely accepted, endptr is set to NULL. Otherwise
   *   it points to the first unparsed character in arg.
   *   If the parsing was successful, true is returned.
   *
   *   \param arg string to parse
   *   \param endptr pointer to the first unused char of arg, or NULL 
   *          if all chars were parsed successfully
   *   \param cmd reference to the CmdLine object that called the function
   *
   *   \return true if parsing was successful
   */
  /*====================================================================*/
  virtual bool
  parse(
      const char* arg,
      const char*& endptr,
      CmdLine& cmd)=0;
  
  /*====================================================================*/
  /*! 
   *   Retrieve the syntax flags for this argument.
   */
  /*====================================================================*/
  CmdArg::Syntax
  syntax() const;

  /*====================================================================*/
  /*! 
   *   Returns true if the argument's syntax flags contain s.
   *
   *   \param s syntax flag to be checked
   *
   *   \return true if the argument's syntax flags contain s
   */
  /*====================================================================*/  
  bool
  hasSyntax(CmdArg::Syntax s) const;
  
  /*====================================================================*/
  /*! 
   *   Retrieve the flags that say how this argument was specified.
   */
  /*====================================================================*/
  CmdArg::CmdLineFlags
  flags() const;

  /*====================================================================*/
  /*! 
   *   Returns true if the argument's flags contain f.
   *
   *   \param s flag to be checked
   *
   *   \return true if the argument's flags contain f
   */
  /*====================================================================*/ 
  bool
  hasFlag(CmdArg::CmdLineFlags f) const;
  
  /*====================================================================*/
  /*! 
   *   Get the sequence number corresponding to the last
   *   time this argument was matched on the command-line.
   *   If this argument was not matched, the sequence number
   *   will be zero, otherwise it will be 'N' where the last
   *   time this argument was matched, it was the 'N'th argument
   *   encountered.
   *
   *   \return sequence number
   */
  /*====================================================================*/
  unsigned int
  sequence() const;

  /*====================================================================*/
  /*! 
   *   Returns the argument's short name. Return's 0 if it has none.
   *
   *   \return short name
   */
  /*====================================================================*/
  char
  shortName() const;
 
  /*====================================================================*/
  /*! 
   *   Returns the argument's long name. Return's "" if it has none.
   *
   *   \return long name
   */
  /*====================================================================*/ 
  const std::string
  longName() const;
  
  /*====================================================================*/
  /*! 
   *   Returns the argument's value name. Return's "" if it takes no value.
   *
   *   \return value name
   */
  /*====================================================================*/ 
  const std::string
  valueName() const;
  
  /*====================================================================*/
  /*! 
   *   Returns the argument's value type. 
   *
   *   \return value type
   */
  /*====================================================================*/ 
  virtual std::string
  valueTypeAsString() const=0;
  
  /*====================================================================*/
  /*! 
   *   Returns the argument's value. 
   *
   *   \return value
   */
  /*====================================================================*/ 
  virtual std::string
  valueAsString() const=0;
  
  /*====================================================================*/
  /*! 
   *   Get the description (help-message) of this argument.
   *
   *   \return description
   */
  /*====================================================================*/
  const std::string
  description() const;

  /*====================================================================*/
  /*!
   *   Generates and returns the arguments synopsis.
   *
   *   \param displayShort if true print short name
   *   \param displayLong if true display long name
   *
   *   \return synopsis string
   */
  /*====================================================================*/
  const std::string
  synopsis(
      bool displayShort=true,
      bool displayLong=true) const;
  
  /*====================================================================*/
  /*! 
   *   Returns error message of last parsing.
   *
   *   \return last error message
   */
  /*====================================================================*/
  const std::string
  errorMsg() const;

  /*====================================================================*/
  /*! 
   *   Clears the error message.
   */
  /*====================================================================*/
  void
  clearErrorMsg();
  
  /*====================================================================*/
  /*! 
   *   Sets the error message.
   *
   *   \param msg new error message.
   */
  /*====================================================================*/
  void
  setErrorMsg(const std::string& msg);

  /*====================================================================*/
  /*! 
   *   Returns true if the argument was parsed at the last parsing run.
   *
   *   \return if the argument was parsed
   */
  /*====================================================================*/
  bool 
  modified() const;  

  /*====================================================================*/
  /*! 
   *   Sets the modified flag.
   *
   *   \param m
   */
  /*====================================================================*/
  void 
  modified(bool m);

  /*====================================================================*/
  /*! 
   *   Returns true if the argument was parsed in any of the parsing runs.
   *
   *   \return if the argument was ever parsed
   */
  /*====================================================================*/
  bool 
  given() const;

  /*====================================================================*/
  /*! 
   *   Returns true if a default value was specified.
   *
   *   \return true if a default value was specified
   */
  /*====================================================================*/ 
  virtual 
  bool
  hasDefaultValue() const;
  
  /*====================================================================*/
  /*! 
   *   Returns the default value as a string.
   *
   *   \return default value as a string
   */
  /*====================================================================*/
  virtual
  std::string
  getDefaultValueString() const;

  /*====================================================================*/
  /*! 
   *   If the argument is a fixed list, this function clears all its values.
   */
  /*====================================================================*/
  virtual
  void
  clearFixedList();
  
protected:
  /*====================================================================*/
  /*! 
   *   Adjust and correct specified syntax flags.
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  void
  adjustSyntax();

  /*====================================================================*/
  /*! 
   *   Sets the argument's flags.
   *
   *   \param f new flags
   */
  /*====================================================================*/
  void
  flags(CmdArg::CmdLineFlags f);

  /*====================================================================*/
  /*! 
   *   Toggles specified flags
   *
   *   \param f flags to toggle
   */
  /*====================================================================*/
  void
  setFlags(CmdArg::CmdLineFlags f);

  /*====================================================================*/
  /*! 
   *   Clears the specified flags
   *
   *   \param f flags to clear
   */
  /*====================================================================*/  
  void
  clearFlags(CmdArg::CmdLineFlags f=ALLFLAGS);

  /*====================================================================*/
  /*! 
   *   Sets the sequence number.
   *
   *   \param num new sequence number
   */
  /*====================================================================*/  
  void
  sequence(unsigned int num);
  

protected:
  CmdArg::CmdLineFlags pFlags;
  CmdArg::Syntax pSyntax;

  unsigned int pSequence;
  char pShortName;
  std::string pLongName;
  std::string pValueName;
  std::string pDescription;

  std::string pErrorMessage;

  bool pModified;
};

#include "CmdArg.icc"

#endif
