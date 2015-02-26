/**************************************************************************
**       Title: template command line arguments
**    $RCSfile$
**   $Revision: 3319 $$Name$
**       $Date: 2010-01-23 17:10:43 +0100 (Sat, 23 Jan 2010) $
**   Copyright: GPL $Author: emmenlau $
** Description:
**
**
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.11  2008/06/08 15:53:52  emmenlau
**   - removed arguments from (dummy) functions not using them to avoid gcc warning
**
**  Revision 1.10  2005/06/15 14:05:30  emmenlau
**   - added this->pointers to function-calls for gcc-4.0 compliance
**
**  Revision 1.9  2003/01/31 02:52:28  mechnich
**  added XML output of all CmdLine arguments
**
**  Revision 1.8  2002/10/28 13:11:33  mechnich
**  made everything compile with gcc 3.2
**  removed promptUser() from CmdLine.*
**
**  Revision 1.7  2002/05/14 08:14:50  pigorsch
**  -added documentation
**
**  Revision 1.6  2002/04/22 15:57:46  pigorsch
**  - added functions hasDefaultValue(), setDefaultValue(), getDefaultValueString()
**
**  Revision 1.5  2002/04/18 13:59:53  pigorsch
**  CmdArgs.hh
**  - splitted CmdArgTypeVector into CmdArgTypeVector and new class CmdArgTypeFixedVector
**  - adjust/verify syntax flags in all constructors
**  - added some documentation
**
**  Revision 1.4  2002/04/11 15:03:11  ronneber
**  - now ensures that in every Constructor of CmdArgTypeVector, the
**    CmdArg::isLIST flag is set
**
**  Revision 1.3  2002/04/10 09:54:55  pigorsch
**  - function parse(...) of arguments now sets the modified flag of the argument
**
**  Revision 1.2  2002/04/04 12:02:08  pigorsch
**  - added CmdArgThrow
**  - removed CmdArgUsage
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

#ifndef CMDARGS_HH
#define CMDARGS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <sstream>
#include <string>
#include <vector>

#include "CmdArg.hh"
#include "Compiler.hh"
#include "Errors.hh"


/*-------------------------------------------------------------------------
 *  Conversion functions for XML output
 *-------------------------------------------------------------------------*/
inline
std::string
typeToString( const std::string&)
{
  return "string";
}

inline
std::string
typeToString( int)
{
  return "int";
}

inline
std::string
typeToString( float)
{
  return "float";
}

inline
std::string
typeToString( double)
{
  return "double";
}

/*======================================================================*/
/*!
 *  \class CmdArgType CmdArgs.hh
 *  \brief The CmdArgType<T> class is a CmdArg that takes one value of
 *   type T.
 *
 *  The CmdArgType<T> class is a CmdArg that (optionally) takes
 *  one value of type T. For parsing it uses the Compiler<T> template
 *  class. You may specify a default value, which is returned if no value
 *  was given on the command line, by using the function.
 *  setDefaultValue
 *
 *  \par Example:
 *  \code
 *  int main(int argc, char** argv)
 *  {
 *     CmdLine cmd("fileview", "views a file");
 *     CmdArgType<std::string> argFile(
 *        'f', "file", "filename",
 *        "The name of the file that you want to view.");
 *     argFile.setDefaultValue("foo.bar");
 *
 *     ArgvIter iter(--argc, ++argv);
 *
 *     cmd.append(&argFile);
 *     cmd.parse(iter);
 *
 *     std::cout << "View file " << argFile.value() << std::endl;
 *     return 0;
 *  }
 *  \endcode
 *
 *  If you execute the program like this
 *  \verbatim fileview -f image.jpg \endverbatim
 *  or \verbatim fileview --file image.jpg \endverbatim
 *  the output is \verbatim View file image.jpg \endverbatim
 */
/*======================================================================*/
template<typename T>
class CmdArgType: public CmdArg
{
public:
  /*====================================================================*/
  /*!
   *   Constructor. Creates a CmdArgType with short and long name.
   *   By default the syntax flags are set so that the argument is
   *   optional and its value is required.
   *
   *   \param aShortName short name of the argument
   *   \param aLongName long name of the argument
   *   \param aValueName name of the agrument's value
   *   \param aDescription description of the argument
   *   \param aSyntax syntax flags
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdArgType(
      char aShortName,
      const std::string aLongName,
      const std::string aValueName,
      const std::string aDescription,
      Syntax aSyntax=isOPTVALREQ)
          :CmdArg(aShortName, aLongName, aValueName, aDescription,
                  Syntax(aSyntax & ~isLIST & ~isFIXEDLIST & ~isPOS)),
           pHasDefault(false)
        {};

  /*====================================================================*/
  /*!
   *   Constructor. Creates a positional CmdArgType.
   *   By default the syntax flags are set so that the argument is
   *   optional and its value is required.
   *
   *   \param aValueName name of the agrument's value
   *   \param aDescription description of the argument
   *   \param aSyntax syntax flags
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdArgType(
      const std::string aValueName,
      const std::string aDescription,
      Syntax aSyntax=isPOSVALREQ)
          :CmdArg(aValueName, aDescription,
                  Syntax((aSyntax|isPOS) & ~isLIST & ~isFIXEDLIST)),
           pHasDefault(false)
        {};

  /*====================================================================*/
  /*!
   *   Destructor.
   */
  /*====================================================================*/
  virtual ~CmdArgType()
        {};

  /*====================================================================*/
  /*!
   *   Parse value string.
   *   If the given arg could not be parsed correctly a CmdLineSyntaxError
   *   exception is thrown.
   *
   *   \param arg the string that is to be parsed
   *   \param endptr is set to the first unparsed character of arg
   *   \param cmd reference to the CmdLine object that initiated the parsing
   *   (is ignored)
   *
   *   \exception CmdLineSyntaxError
   *
   *   \return true if parsing was successful
   */
  /*====================================================================*/
  virtual bool
  parse(
      const char* arg,
      const char*& endptr,
      CmdLine& )
        {
          bool temp=pCompiler.compile(arg, endptr, pValue);
          if (temp==false)
          {
            CmdLineSyntaxError se;
            se << "parsing error: " << pCompiler.errorMsg();
            throw(se);
          }
          modified(true);
          return temp;
        };

  /*====================================================================*/
  /*!
   *   Return the reference to the value of the argument.
   *
   *   \return reference to the value of the argument
   */
  /*====================================================================*/
  virtual T&
  value()
        {
          return pValue;
        };

  /*====================================================================*/
  /*!
   *   Specify default value.
   *
   *   \param v default value
   */
  /*====================================================================*/
  virtual
  void
  setDefaultValue(const T& v)
        {
          pHasDefault=true;
          pDefaultValue=v;
          pValue=v;
        };

  /*====================================================================*/
  /*!
   *   Returns true if a default value was specified.
   *
   *   \return true if a default value was specified
   */
  /*====================================================================*/
  virtual
  bool
  hasDefaultValue() const
        {
          return pHasDefault;
        };

  /*====================================================================*/
  /*!
   *   Returns the default value as a string.
   *
   *   \return default value as a string
   */
  /*====================================================================*/
  virtual
  std::string
  getDefaultValueString() const
        {
          std::ostringstream os;
          os << pDefaultValue;
          return os.str();
        };

  /*====================================================================*/
  /*!
   *   Return the const reference to the value of the argument.
   *
   *   \return const reference to the value of the argument
   */
  /*====================================================================*/
  virtual const T&
  value() const
        {
          return pValue;
        };

  /*====================================================================*/
  /*!
   *   Return the value type as a std::string.
   *
   *   \return value type as a std::string
   */
  /*====================================================================*/
  virtual std::string
  valueTypeAsString() const
        {
          return typeToString( T());
        }

  /*====================================================================*/
  /*!
   *   Return the value as a std::string.
   *
   *   \return value as a std::string
   */
  /*====================================================================*/
  virtual std::string
  valueAsString() const
        {
          std::stringstream ret;
          ret << value();
          return ret.str();
        }

private:
  Compiler<T> pCompiler;
  T pValue, pDefaultValue;
  bool pHasDefault;
};


/*======================================================================*/
/*!
 *  \class CmdArgTypeVector CmdArgs.hh
 *  \brief The CmdArgTypeVector<T> class is a CmdArg that takes a list
 *  of values of type T. Since it is derived from std::vector<T> you can
 *  use it like a std::vector<T>.
 *
 *  The CmdArgTypeVector<T> class is a CmdArg that (optionally) takes
 *  a list of values of type T. For parsing it uses the Compiler<T> template
 *  class. Since it is derived from std::vector<T> you can
 *  use it like a std::vector<T>.
 *
 *  \par Example:
 *  \code
 *  int main(int argc, char** argv)
 *  {
 *     CmdLine cmd("sum", "sums all specified integers");
 *     CmdArgTypeVector<int> argInts(
 *        "integers",
 *        "All the integers you want to sum.");
 *
 *     ArgvIter iter(--argc, ++argv);
 *
 *     cmd.append(&argInts);
 *     cmd.parse(iter);
 *
 *     for (std::vector<int>::const_iterator p=argInts.begin();
 *          p!=argInts.end();
 *          ++p)
 *     {
 *        sum+=*p;
 *     }
 *     std::cout << sum << std::endl;
 *
 *     return 0;
 *  }
 *  \endcode
 *
 *  If you execute the program like this
 *  \verbatim sum 1 2 3 \endverbatim
 *  the output is \verbatim 6 \endverbatim
 */
/*======================================================================*/
template<typename T>
class CmdArgTypeVector: public CmdArg, public std::vector<T>
{
public:

  typedef typename CmdArgTypeVector<T>::const_iterator const_iterator;

  /*====================================================================*/
  /*!
   *   Constructor. Creates a CmdArgTypeVector with short and long name.
   *   By default the syntax flags are set so that the argument is
   *   optional and its values are required.
   *
   *   \param aShortName short name of the argument
   *   \param aLongName long name of the argument
   *   \param aValueName name of the agrument's value
   *   \param aDescription description of the argument
   *   \param aSyntax syntax flags
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdArgTypeVector(
      char aShortName,
      const std::string aLongName,
      const std::string aValueName,
      const std::string aDescription,
      Syntax aSyntax=Syntax(isOPTVALREQ|isLIST))
          :CmdArg(aShortName, aLongName, aValueName,
                  aDescription,
                  Syntax((aSyntax|isLIST) & ~isFIXEDLIST & ~isPOS)),
           pHasDefault(false)
        {};

  /*====================================================================*/
  /*!
   *   Constructor. Creates a positional CmdArgTypeVector.
   *   By default the syntax flags are set so that the argument's
   *   values are required.
   *
   *   \param aValueName name of the agrument's value
   *   \param aDescription description of the argument
   *   \param aSyntax syntax flags
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdArgTypeVector(
      const std::string aValueName,
      const std::string aDescription,
      Syntax aSyntax=Syntax(isPOSVALREQ|isLIST))
          :CmdArg(aValueName, aDescription,
                  Syntax((aSyntax|isLIST|isPOS) & ~isFIXEDLIST)),
           pHasDefault(false)
        {};

  /*====================================================================*/
  /*!
   *   Destructor.
   */
  /*====================================================================*/
  virtual ~CmdArgTypeVector()
        {};

  /*====================================================================*/
  /*!
   *   Specify default value.
   *
   *   \param v default value
   */
  /*====================================================================*/
  virtual
  void
  setDefaultValue(const std::vector<T>& v)
        {
          pHasDefault=true;
          pDefaultValue=v;
          std::vector<T>::operator=(v);
        };

  /*====================================================================*/
  /*!
   *   Returns true if a default value was specified.
   *
   *   \return true if a default value was specified
   */
  /*====================================================================*/
  virtual
  bool
  hasDefaultValue() const
        {
          return pHasDefault;
        };

  /*====================================================================*/
  /*!
   *   Returns the default value as a string.
   *
   *   \return default value as a string
   */
  /*====================================================================*/
  virtual
  std::string
  getDefaultValueString() const
        {
          std::ostringstream os;
          os << "[ ";
          for (typename std::vector<T>::const_iterator p=pDefaultValue.begin();
               p!=pDefaultValue.end();
               ++p)
          {
            os << (*p) << " ";
          }
          os << "]";

          return os.str();
        };

  /*====================================================================*/
  /*!
   *   Parse value string and append it to the vector.
   *   If the given arg could not be parsed correctly a CmdLineSyntaxError
   *   exception is thrown.
   *
   *   \param arg the string that is to be parsed
   *   \param endptr is set to the first unparsed character of arg
   *   \param cmd reference to the CmdLine object that initiated the parsing
   *   (is ignored)
   *
   *   \exception CmdLineSyntaxError
   *
   *   \return true if parsing was successful
   */
  /*====================================================================*/
  virtual bool
  parse(
      const char* arg,
      const char*& endptr,
      CmdLine& )
        {
          T value;
          bool temp=pCompiler.compile(arg, endptr, value);
          if (temp==false)
          {
            CmdLineSyntaxError se;
            se << "parsing error: " << pCompiler.errorMsg();
            throw(se);
          }
          else
          {
            if (!modified() && hasDefaultValue() && *this==pDefaultValue)
            {
              std::vector<T>::clear();
            }
            this->push_back(value);
          }
          modified(true);
          return temp;
        };

  /*====================================================================*/
  /*!
   *   Return the value type as a std::string.
   *
   *   \return value type as a std::string
   */
  /*====================================================================*/
  virtual std::string
  valueTypeAsString() const
        {
          return "vector_" + typeToString( T());
        }

  /*====================================================================*/
  /*!
   *   Return the value as a std::string.
   *
   *   \return value as a std::string
   */
  /*====================================================================*/
  virtual std::string
  valueAsString() const
        {
          std::stringstream ret;

          const_iterator it = this->begin();
          if( it != this->end())
          {
            ret << *it;;
            ++it;
          }

          for( ; it != this->end(); ++it)
          {
            ret << " " << *it;
          }

          return ret.str();
        }

private:
  Compiler<T> pCompiler;
  std::vector<T> pDefaultValue;
  bool pHasDefault;
};

/*======================================================================*/
/*!
 *  \class CmdArgTypeFixedVector CmdArgs.hh
 *  \brief The CmdArgTypeFixedVector<T> class is a CmdArg that takes a
 *  fixed size list of values of type T. Since it is derived from
 *  std::vector<T> you can use it like a std::vector<T>.
 */
/*======================================================================*/
template<typename T>
class CmdArgTypeFixedVector: public CmdArg, public std::vector<T>
{
public:

  typedef typename CmdArgTypeFixedVector<T>::const_iterator const_iterator;

  /*====================================================================*/
  /*!
   *   Constructor. Creates a CmdArgTypeFixedVector with short and long name.
   *   By default the syntax flags are set so that the argument is
   *   optional and its values are required.
   *
   *   \param aShortName short name of the argument
   *   \param aLongName long name of the argument
   *   \param aValueName name of the agrument's value
   *   \param aDescription description of the argument
   *   \param aFixedSize size of the vector. must not be 0.
   *   \param aSyntax syntax flags
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdArgTypeFixedVector(
      char aShortName,
      const std::string aLongName,
      const std::string aValueName,
      const std::string aDescription,
      unsigned int aFixedSize,
      Syntax aSyntax=Syntax(isOPTVALREQ|isFIXEDLIST))
          :CmdArg(aShortName, aLongName, aValueName,
                  aDescription,
                  Syntax((aSyntax|isFIXEDLIST) & ~isLIST & ~isPOS)),
           pFixedSize(aFixedSize),
           pHasDefault(false)
        {
          if (aFixedSize==0)
          {
            CmdLineSyntaxError se;
            se << "syntax error: size of fixed vector is 0";
            throw(se);
          }

          adjustSyntax();
        };

  /*====================================================================*/
  /*!
   *   Constructor. Creates a positional CmdArgTypeFixedVector.
   *   By default the syntax flags are set so that the argument's
   *   values are required.
   *
   *   \param aValueName name of the agrument's value
   *   \param aDescription description of the argument
   *   \param aFixedSize size of the vector. must not be 0.
   *   \param aSyntax syntax flags
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdArgTypeFixedVector(
      const std::string aValueName,
      const std::string aDescription,
      unsigned int aFixedSize,
      Syntax aSyntax=Syntax(isPOSVALREQ|isFIXEDLIST))
          :CmdArg(aValueName, aDescription,
                  Syntax((aSyntax|isFIXEDLIST|isPOS) & ~isLIST)),
           pFixedSize(aFixedSize),
           pHasDefault(false)
        {
          if (aFixedSize==0)
          {
            CmdLineSyntaxError se;
            se << "syntax error: size of fixed vector is 0";
            throw(se);
          }

          adjustSyntax();
        };

  /*====================================================================*/
  /*!
   *   Destructor.
   */
  /*====================================================================*/
  virtual ~CmdArgTypeFixedVector()
        {};

  /*====================================================================*/
  /*!
   *   Specify default value.
   *
   *   \param v default value
   */
  /*====================================================================*/
  virtual
  void
  setDefaultValue(const std::vector<T>& v)
        {
          if (pFixedSize!=v.size())
          {
            CmdLineSyntaxError se;
            se << "syntax error: size of default vector does not match fixed size";
            throw(se);
          }
          pHasDefault=true;
          pDefaultValue=v;
          std::vector<T>::operator=(v);
        };

  /*====================================================================*/
  /*!
   *   Returns true if a default value was specified.
   *
   *   \return true if a default value was specified
   */
  /*====================================================================*/
  virtual
  bool
  hasDefaultValue() const
        {
          return pHasDefault;
        };

  /*====================================================================*/
  /*!
   *   Returns the default value as a string.
   *
   *   \return default value as a string
   */
  /*====================================================================*/
  virtual
  std::string
  getDefaultValueString() const
        {
          std::ostringstream os;
          os << "[ ";
          for (typename std::vector<T>::const_iterator p=pDefaultValue.begin();
               p!=pDefaultValue.end();
               ++p)
          {
            os << (*p) << " ";
          }
          os << "]";

          return os.str();
        };

  /*====================================================================*/
  /*!
   *   Parse value string and append it to the vector.
   *   If the given arg could not be parsed correctly a
   *   CmdLineSyntaxError exception is thrown.
   *
   *   \param arg the string that is to be parsed
   *   \param endptr is set to the first unparsed character of arg
   *   \param cmd reference to the CmdLine object that initiated the parsing
   *   (is ignored)
   *
   *   \exception CmdLineSyntaxError
   *
   *   \return true if parsing was successful
   */
  /*====================================================================*/
  virtual bool
  parse(
      const char* arg,
      const char*& endptr,
      CmdLine& )
        {
          T value;
          bool temp=pCompiler.compile(arg, endptr, value);
          if (temp==false)
          {
            CmdLineSyntaxError se;
            se << "parsing error: " << pCompiler.errorMsg();
            throw(se);
          }
          else
          {
/*
            // erase given default list
            if (!modified())
            {
              std::vector<T>::clear();
            }
*/
            // check remaining size
            if (remainingSize()==0)
            {
              //clear();
              CmdLineSyntaxError se;
              se << "syntax error: too many values for fixed vector";
              throw(se);
            }

            this->push_back(value);
          }
          modified(true);
          return temp;
        };

  /*====================================================================*/
  /*!
   *   Return the fixed size of the vector.
   *
   *   \return fixed size
   */
  /*====================================================================*/
  virtual
  unsigned int
  fixedSize() const
        {
          return pFixedSize;
        };

  /*====================================================================*/
  /*!
   *   Return the number of remaining values.
   *
   *   \return number of remaining values
   */
  /*====================================================================*/
  virtual
  unsigned int
  remainingSize() const
        {
          return fixedSize() - this->size();
        };

  /*====================================================================*/
  /*!
   *   Clear all values.
   */
  /*====================================================================*/
  virtual
  void
  clearFixedList()
        {
          this->clear();
        };

  /*====================================================================*/
  /*!
   *   Return the value type as a std::string.
   *
   *   \return value type as a std::string
   */
  /*====================================================================*/
  virtual std::string
  valueTypeAsString() const
        {
          return "vector_" + typeToString( T());
        }

  /*====================================================================*/
  /*!
   *   Return the value as a std::string.
   *
   *   \return value as a std::string
   */
  /*====================================================================*/
  virtual std::string
  valueAsString() const
        {
          std::stringstream ret;

          const_iterator it = this->begin();
          if( it != this->end())
          {
            ret << *it;;
            ++it;
          }

          for( ; it != this->end(); ++it)
          {
            ret << " " << *it;
          }

          return ret.str();
        }

private:
  Compiler<T> pCompiler;
  unsigned int pFixedSize;
  std::vector<T> pDefaultValue;
  bool pHasDefault;
};

/*======================================================================*/
/*!
 *  \class CmdArgSwitch CmdArgs.hh
 *  \brief The CmdArgSwitch class is a CmdArg that behaves like a switch:
 *  if it was specified on the command line its value is true, if it was
 *  not specified its value is false.
 */
/*======================================================================*/
class CmdArgSwitch: public CmdArg
{
public:
  /*====================================================================*/
  /*!
   *   Constructor. Creates a CmdArgSwitch with short and long name.
   *   By default the syntax flags are set so that the argument is
   *   optional.
   *
   *   \param aShortName short name of the argument
   *   \param aLongName long name of the argument
   *   \param aDescription description of the argument
   *   \param aSyntax syntax flags
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdArgSwitch(
      char aShortName,
      const std::string aLongName,
      const std::string aDescription,
      Syntax aSyntax=Syntax(isOPT))
          :CmdArg(aShortName, aLongName, aDescription,
                  Syntax(aSyntax & ~isVALTAKEN & ~isLIST & ~isFIXEDLIST & ~isPOS)),
           pValue(false)
        {};

  /*====================================================================*/
  /*!
   *   Destructor.
   */
  /*====================================================================*/
  virtual ~CmdArgSwitch()
        {};

  /*====================================================================*/
  /*!
   *   Set the value to true.
   *
   *   \param arg the string that is to be parsed (is ignored)
   *   \param endptr is set to the first unparsed character of arg (is ignored)
   *   \param cmd reference to the CmdLine object that initiated the parsing
   *   (is ignored)
   *
   *   \return true
   */
  /*====================================================================*/
  virtual bool
  parse(
      const char* ,
      const char*& ,
      CmdLine& )
        {
          pValue=true;
          modified(true);
          return true;
        };

  /*====================================================================*/
  /*!
   *   Return the reference to the value of the argument.
   *
   *   \return reference to the value of the argument
   */
  /*====================================================================*/
  bool&
  value()
        {
          return pValue;
        };

  /*====================================================================*/
  /*!
   *   Return the const reference to the value of the argument.
   *
   *   \return const reference to the value of the argument
   */
  /*====================================================================*/
  const bool&
  value() const
        {
          return pValue;
        };

  /*====================================================================*/
  /*!
   *   Returns true if the argument was specified on the command line.
   *
   *   \return true if the argument was specified on the command line
   */
  /*====================================================================*/
  bool
  toggled() const
        {
          return pValue;
        };

  /*====================================================================*/
  /*!
   *   Reset status (set value to false).
   */
  /*====================================================================*/
  void
  reset()
        {
          pValue=false;
        };


  /*====================================================================*/
  /*!
   *   Return the value type as a std::string.
   *
   *   \return value type as a std::string
   */
  /*====================================================================*/
  virtual std::string
  valueTypeAsString() const
        {
          return "bool";
        }

  /*====================================================================*/
  /*!
   *   Return the value as a std::string.
   *
   *   \return value as a std::string
   */
  /*====================================================================*/
  virtual std::string
  valueAsString() const
        {
          if( pValue)
              return "1";
          else
              return "0";
        }

private:
  bool pValue;
};


/*======================================================================*/
/*!
 *  \class CmdArgThrow CmdArgs.hh
 *  \brief The CmdArgThrow<E> class is a CmdArg that throws an exception
 *  of type E when specified on the command line.
 */
/*======================================================================*/
template<typename E>
class CmdArgThrow: public CmdArg
{
public:
  /*====================================================================*/
  /*!
   *   Constructor. Creates a CmdArgThrow with short and long name.
   *   By default the syntax flags are set so that the argument is
   *   optional.
   *
   *   \param aShortName short name of the argument
   *   \param aLongName long name of the argument
   *   \param aDescription description of the argument
   *   \param aSyntax syntax flags
   *
   *   \exception CmdLineSyntaxError
   */
  /*====================================================================*/
  CmdArgThrow(
      char aShortName,
      const std::string aLongName,
      const std::string aDescription,
      Syntax aSyntax=isOPT)
          :CmdArg(aShortName, aLongName, aDescription,
                  Syntax((aSyntax|isOPT) & ~isVALTAKEN & ~isLIST & ~isFIXEDLIST & ~isPOS))
        {};

  /*====================================================================*/
  /*!
   *   Destructor.
   */
  /*====================================================================*/
  virtual ~CmdArgThrow()
        {};


  /*====================================================================*/
  /*!
   *   Throw exception E.
   *
   *   \param arg the string that is to be parsed (is ignored)
   *   \param endptr is set to the first unparsed character of arg (is ignored)
   *   \param cmd reference to the CmdLine object that initiated the parsing
   *   (is ignored)
   *
   *   \exception E
   *
   *   \return true
   */
  /*====================================================================*/
  virtual bool
  parse(
      const char* ,
      const char*& ,
      CmdLine& )
        {
          E e;
          throw(e);
          return true;
        };

  /*====================================================================*/
  /*!
   *   Return the value type as a std::string.
   *
   *   \return value type as a std::string
   */
  /*====================================================================*/
  virtual std::string
  valueTypeAsString() const
        {
          return "throw";
        }

  /*====================================================================*/
  /*!
   *   Return the value as a std::string.
   *
   *   \return value as a std::string
   */
  /*====================================================================*/
  virtual std::string
  valueAsString() const
        {
          return "";
        }

};

#endif
