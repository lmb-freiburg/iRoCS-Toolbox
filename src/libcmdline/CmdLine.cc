/**************************************************************************
**       Title: command line parser
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
**  Revision 1.18  2008/02/25 09:39:36  emmenlau
**   - used size_t in some places for find()-results
**
**  Revision 1.17  2007/11/15 12:48:55  tschmidt
**  - Initialized dummy variable, because later in code it is used in a
**    comparison
**
**  Revision 1.16  2005/06/09 07:52:11  tschmidt
**  - Changing enum names from bla to CmdLinebla
**  - Adding check for terminal width (I guess changed by emmenlau)
**
**  Revision 1.14  2003/02/18 22:21:13  mechnich
**  added description to usagexml output
**
**  Revision 1.13  2003/01/31 02:52:28  mechnich
**  added XML output of all CmdLine arguments
**
**  Revision 1.12  2002/10/28 13:11:33  mechnich
**  made everything compile with gcc 3.2
**  removed promptUser() from CmdLine.*
**
**  Revision 1.11  2002/08/08 10:25:57  pigorsch
**  - removed "[]" in parameter descriptions
**
**  Revision 1.10  2002/06/03 15:19:50  pigorsch
**  - removed UsageArg's short name -?
**  - optional parameters don't have []'s in descriptions anymore
**  - removed UsageLevel
**
**  Revision 1.9  2002/05/27 15:38:55  pigorsch
**  - printSynopsis uses new CmdArg::synopsis function
**  - printDescriptions uses new CmdArg::synopsis function
**  - usageHTML uses new CmdArg::synopsis function
**  - usageHTML now creates html links
**
**  Revision 1.8  2002/04/22 15:57:27  pigorsch
**  - modified function printDescriptions() to display default values
**  - modified function usageHTML() to display default values
**  - added function replace()
**  - moved function basename() from .hh to.cc
**  - in usageHTML() newlines are replaced by <br> automatically
**
**  Revision 1.7  2002/04/18 14:00:42  pigorsch
**  - use new function CmdArg::clearFlags(...)
**  - changed way of clearing flags
**
**  Revision 1.6  2002/04/10 09:46:38  pigorsch
**  - reset modified flag for all args in parse(...)
**
**  Revision 1.5  2002/04/04 12:00:51  pigorsch
**  - finished function usageHTML
**  - added longname support
**  - added default help arguments
**  - removed "dummy" stuff
**
**  Revision 1.4  2002/03/27 14:33:48  ronneber
**  - fixed minor syntax error
**
**  Revision 1.3  2002/03/27 10:32:36  pigorsch
**  - modified normal and html usage for longname
**
**  Revision 1.2  2002/03/27 10:17:50  pigorsch
**  - reordered html usage message
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

#include "ArgIter.hh"
#include "CmdArgs.hh"
#include "CmdLine.hh"

#include <cctype> // tolower(...)
#include <cstdarg> // va_arg(...), va_end(...)
#include <cstdlib> // getenv
#include <cstring> // strpbrk(...)
#include <sstream>
#ifdef linux
#include <unistd.h> // isatty(...)
#endif

CmdLine::
CmdLine(
    std::string aName,
    std::string aLongName,
    bool help)
        :pParseState(PARSE_START_STATE),
         pState(STATE_START_STATE),
         pFlags(FLAG_OPTS_FIRST),
         pStatus(STATUS_NO_ERROR),
         pNArgsParsed(0),
         pName(basename(aName)),
         pLongName(aLongName),
         pDescription(""),
         pMatchedArg(0),
         pErr(0),
         pUsageArg(0, "help", "Display usage message."),
         pUsageHTMLArg(0, "usagehtml", "Display HTML usage message."),
         pUsageXMLArg(0, "usagexml", "Display XML usage message.")
{
  if (help)
  {
    append(pUsageArg);
    append(pUsageHTMLArg);
  }
    append(pUsageXMLArg);
}

CmdLine::
CmdLine(
    std::string aName,
    std::string aLongName,
    bool help,
    CmdArg* cmdarg1 ...)
        :pParseState(PARSE_START_STATE),
         pState(STATE_START_STATE),
         pFlags(FLAG_OPTS_FIRST),
         pStatus(STATUS_NO_ERROR),
         pNArgsParsed(0),
         pName(basename(aName)),
         pLongName(aLongName),
         pMatchedArg(0),
         pErr(0),
         pUsageArg(0, "help", "Display usage message."),
         pUsageHTMLArg(0, "usagehtml", "Display HTML usage message."),
         pUsageXMLArg(0, "usagexml", "Display XML usage message.")
{
  if (help)
  {
    append(pUsageArg);
    append(pUsageHTMLArg);
  }
  append(pUsageXMLArg);
  // iterate through arguments, stop at 0
  va_list ap;
  va_start(ap, cmdarg1);
  for (CmdArg* cmdarg = cmdarg1; cmdarg != NULL; cmdarg=va_arg(ap, CmdArg*))
      append(cmdarg);
  va_end(ap);
}

CmdLine::
~CmdLine()
{}


std::ostream&
CmdLine::
usage(
    std::ostream& os) const
{
  // determine syntax to use
  UsageSyntax cmd_syntax=syntax();

  // print command-line synopsis
  unsigned int longest=printSynopsis(cmd_syntax, os);

  // print long name
  os << longname() << std::endl;

  // now print argument descriptions
  os << std::endl;
  printDescriptions(cmd_syntax, os, longest);

  // now print the command-description if there is one
  if (!pDescription.empty())
  {
    os << "\nDescription:" << std::endl;
    strindent(os, "", 0, pDescription);
  }

  return os;
}

std::ostream&
CmdLine::
usageXML(std::ostream& os) const
{
  os << "<usagexml name=\"" << name() << "\">" << std::endl;

  for (std::list<CmdArg*>::const_iterator itL=pArguments.begin();
       itL!=pArguments.end();
       ++itL)
  {

    CmdArg* cmdarg=*itL;

    os << "<option";

    std::string ln = cmdarg->longName();
    if( !ln.empty())
    {
      os << " name=\"" << ln << "\"";
    }

    os << " valuetype=\"" << cmdarg->valueTypeAsString() << "\"";

    std::string vn = cmdarg->valueName();
    if( !vn.empty())
    {
      os << " valuename=\"" << vn << "\"";
    }

    std::string val = cmdarg->valueAsString();
    if( !val.empty())
    {
      os << " value=\"" << cmdarg->valueAsString() << "\"";
    }

    if( cmdarg->hasDefaultValue())
    {
      os << " default=\"" << cmdarg->getDefaultValueString() << "\"";
    }

    os << ">\n";
    os << cmdarg->description() << std::endl;
    os << "</option>" << std::endl;
  }

  os << "<description>" << description() << "</description>" << std::endl;

  os << "</usagexml>" << std::endl;

  return os;
}

std::ostream&
CmdLine::
usageHTML(std::ostream& os) const
{
  os << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
      "<html>\n"
      "<head>\n"
      "<meta name=\"generator\" content=\"libCmdLine\">\n"
      "<title>" << name() << " - " << longname() << "</title>\n"
      "<style type='text/css'>\n"
      "<!--\n"
      "span.option { color: rgb(0,96,160);\n"
      "              font-weight: bold;}\n"
      "span.parameter { color: rgb(0,128,64);\n"
      "                 font-style: italic;}\n"
      "span.defaultvalue { color: rgb(0,128,64);}\n"
      "span.default { font-weight: bold;}\n"
      "dt { margin-top: 1em; }\n"
      "-->\n"
      "</style>\n"
      "</head>\n"
      "<body>\n"
      "<h2>NAME</h2>\n"
      "<p>" << name() << " - " << longname() << "</p>\n"
      "<h2>SYNOPSIS</h2>\n"
      "<b>" << name() << "</b>";

  std::ostringstream strOpt;

  int linkcount=0;

  for (unsigned int positionals=0;
       positionals<2;
       positionals++)
  {
    for (std::list<CmdArg*>::const_iterator itL=pArguments.begin();
         itL!=pArguments.end();
         ++itL)
    {
      CmdArg* cmdarg=*itL;

      std::string sn;
      if (cmdarg->shortName()!=0)
      {
        std::ostringstream ost;
        ost << "<span class='option'>-"
            << char(cmdarg->shortName())
            << "</span>";
        sn=ost.str();
      }

      std::string ln(cmdarg->longName());
      if (!ln.empty())
      {
        std::ostringstream ost;
        ost << "<span class='option'>--" << ln << "</span>";
        ln=ost.str();
      }

      std::string vn(cmdarg->valueName());
      if (!vn.empty())
      {
        std::ostringstream ost;
        ost << "<span class='parameter'>" << vn;
        if (cmdarg->hasSyntax(CmdArg::isFIXEDLIST))
        {
          ost << '(' << cmdarg->fixedSize() << ')';
        }
        else if (cmdarg->hasSyntax(CmdArg::isLIST))
        {
          ost << "&nbsp;...";
        }
        ost << "</span>";
        vn=ost.str();
      }

      if (cmdarg->hasSyntax(CmdArg::isHIDDEN)) continue;
      if (!positionals && cmdarg->hasSyntax(CmdArg::isPOS)) continue;
      if (positionals && !cmdarg->hasSyntax(CmdArg::isPOS)) continue;

      ++linkcount;
      os << " <a href=#link" << linkcount << '>';
      strOpt << "<dt><a name=link" << linkcount << "></a>";

      if (!cmdarg->hasSyntax(CmdArg::isREQ))
      {
        os << '[';
        strOpt << '[';
      }

      if (!cmdarg->hasSyntax(CmdArg::isPOS)
          && cmdarg->hasSyntax(CmdArg::isVALTAKEN)
          && cmdarg->hasSyntax(CmdArg::isVALSTICKY))
      {
        if (cmdarg->hasSyntax(CmdArg::isVALOPT))
        {
          if (!sn.empty())
          {
            os << sn << '[' << vn << ']';
            strOpt << sn << '[' << vn << ']';

            if (!ln.empty())
            {
              os << '|';
              strOpt << '|';
            }
          }
          if (!ln.empty())
          {
            os << ln << "[=" << vn << ']';
            strOpt << ln << "[=" << vn << ']';
          }
        }
        else
        {
          if (!sn.empty())
          {
            os << sn << vn;
            strOpt << sn << vn;
            if (!ln.empty())
            {
              os << '|';
              strOpt << '|';
            }
          }
          if (!ln.empty())
          {
            os << ln << '=' << vn;
            strOpt << ln << '=' << vn;
          }
        }

        if (!cmdarg->hasSyntax(CmdArg::isREQ))
        {
          os << ']';
          strOpt << ']';
        }
      }
      else if (!cmdarg->hasSyntax(CmdArg::isPOS))
      {
        if (!sn.empty())
        {
          os << sn;
          strOpt << sn;

          if (!ln.empty())
          {
            os << '|';
            strOpt << '|';
          }
        }
        if (!ln.empty())
        {
          os << ln;
          strOpt << ln;
        }

        if (cmdarg->hasSyntax(CmdArg::isVALTAKEN))
        {
          if (!cmdarg->hasSyntax(CmdArg::isVALSTICKY))
          {
            os << "&nbsp;";
            strOpt << "&nbsp;";
          }
        }
      }

      if (cmdarg->hasSyntax(CmdArg::isVALTAKEN))
      {
        if (!cmdarg->hasSyntax(CmdArg::isPOS) &&
            cmdarg->hasSyntax(CmdArg::isVALOPT))
        {
          os << '[';
          strOpt << '[';
        }
        if (cmdarg->hasSyntax(CmdArg::isVALSTICKY))
        {
          os << '=';
          strOpt << '=';
        }
        os << vn;
        strOpt << vn;

        if (!cmdarg->hasSyntax(CmdArg::isPOS) &&
            cmdarg->hasSyntax(CmdArg::isVALOPT))
        {
          os << ']';
          strOpt << ']';
        }
      }

      if (!cmdarg->hasSyntax(CmdArg::isREQ))
      {
        os << ']';
        strOpt << ']';
      }

      os << "</a>";
      strOpt << "</dt>\n";

      if (!(cmdarg->description().empty()))
      {
        strOpt << "<dd>" << replace(cmdarg->description(), "\n", "<br>") << "</dd>\n";
      }
      if (cmdarg->hasDefaultValue())
      {
        strOpt << "<dd><span class='default'>Default:</span> "
            "<span class='defaultvalue'>"
               << replace(cmdarg->getDefaultValueString(), " ", "&nbsp;")
               << "</span></dd>\n";
      }
    }
  }

  os << std::endl;

  if (!description().empty())
  {
    os << "<h2>DESCRIPTION</h2>\n"
        "<p>" << replace(description(), "\n", "<br>") << "</p>\n";
  }

  os << "<h2>OPTIONS/ARGUMENTS</h2>\n"
      "<dl>\n"
     << strOpt.str()
     << "</dl>\n"
      "</body>\n"
      "</html>"
     << std::flush;

  return os;
}

std::ostream&
CmdLine::
usageLATEX(std::ostream& os) const
{
  return os;
}

CmdLine::CmdLineStatus
CmdLine::
parse(
    ArgIter& argiter,
    bool autoProcessing)
{
  // reset parse-specific attributes
  pParseState=PARSE_START_STATE;
  state(CmdLine::CmdLineState(0));
  status(CmdLine::STATUS_NO_ERROR);
  pNArgsParsed=0;

  for (std::list<CmdArg*>::iterator itL=pArguments.begin();
       itL!=pArguments.end();
       ++itL)
  {
    (*itL)->modified(false);
  }

  if (autoProcessing) prologue();

  for (const char* arg=argiter.fetch();
       arg;
       arg=argiter.fetch())
  {
    parseArg(arg);
  }

  if (autoProcessing) epilogue();

  return status();
}

CmdLine::CmdLineStatus
CmdLine::
prologue()
{
  // reset parse-specific attributes
  pParseState=PARSE_START_STATE;
  state(CmdLine::CmdLineState(0));
  status(CmdLine::STATUS_NO_ERROR);
  pNArgsParsed=0;

  // reset parse-specific attributes for each argument
  for (std::list<CmdArg*>::iterator itL=pArguments.begin();
       itL!=pArguments.end();
       ++itL)
  {
    (*itL)->clearFlags();
  }

  return status();
}

CmdLine::CmdLineStatus
CmdLine::
parseArg(const char* arg)
{
  if (arg==0) return status();

  if (pParseState & PARSE_TOK_REQUIRED)
  {
    // If a required value is expected, then this argument MUST be
    // the value (even if it looks like an option)
    setStatus(parseValue(arg));
  }
  else if (isShort(arg) && (!hasState(STATE_END_OF_OPTIONS)))
  {
    // skip '-' option character
    ++arg;

    setState(STATE_SHORT_USED);
    setStatus(parseShort(arg));
  }
  else if (isLong(arg) && !hasState(STATE_END_OF_OPTIONS))

  {
    setState(STATE_LONG_USED);
    arg+=2; // skip over '--' keyword prefix

    setStatus(parseLong(arg));
  }
  else if (isEndOfOptions(arg)
           && (!hasState(STATE_END_OF_OPTIONS)))
  {
    setState(STATE_END_OF_OPTIONS);

    // see if we left an argument dangling without a value
    ck_need_val();
  }
  else
  {
    setStatus(parseValue(arg));
  }

  return status();
}


CmdLine::CmdLineStatus
CmdLine::
epilogue()
{
  // see if we left an argument dangling without a value
  ck_need_val();

  // check for any missing required arguments
  setStatus(missingArgs());


  if (hasStatus(STATUS_ARG_MISSING))
  {
    CmdLineSyntaxError se;
    se << "required argument missing";

    throw(se);
  }
  else if (status() && !(hasFlag(FLAG_NO_ABORT)))
  {
    CmdLineSyntaxError se;
    se << "parsing aborted";

    throw(se);
  }

  return status();
}

CmdLine::CmdLineMatch
CmdLine::
strmatch(
    const std::string& src,
    const std::string& attempt,
    size_t len)
{
  if (len==0 && attempt.size()>src.size())
  {
    return MATCH_NONE;
  }

  std::string::const_iterator s=src.begin();
  std::string::const_iterator a=attempt.begin();

  for (unsigned int i=0;
       ((len==0) || (i<len)) && a!=attempt.end();
       ++i)
  {
    if (tolower(*a)!=tolower(*s)) return CmdLine::MATCH_NONE;
    ++a;
    ++s;
  }

  if (s==src.end())
  {
    return CmdLine::MATCH_EXACT;
  }
  else
  {
    return CmdLine::MATCH_PARTIAL;
  }
}

void
CmdLine::
strindent(
    std::ostream& os,
    const std::string& title,
    unsigned int indent,
    const std::string& text)
{
  /*---------------------------------------------------------------
   *  Try to get current terminal width from environment
   *---------------------------------------------------------------*/
  unsigned int columns = STDCOLS;
  char* tmpColumns = std::getenv("COLUMNS");
  if( tmpColumns != NULL)
  {
    columns = std::atoi(tmpColumns);
  }
#ifdef linux
  else
  {
    winsize size;
    int stat;

    memset(&size, 0, sizeof(size));
    stat = ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    if (stat == 0)
    {
      columns = size.ws_col;
    }
  }
#endif
  if( columns < STDCOLS)
    columns = STDCOLS;
  if( columns > MAXCOLS)
    columns = MAXCOLS;

  // print the title (left-justified)
  os.width(LEFTMARGIN);
  os << "";
  os.setf(std::ios::left);
  os.width(indent);
  os << title.c_str();
  os.unsetf(std::ios::left);

  if (text.empty())
  {
    os << std::endl;
    return;
  }

  // If the title is too big, start the paragraph on a new line
  if (title.size()>indent)
  {
    os << std::endl;
    os.width(LEFTMARGIN+indent);
    os << "";
  }

  // Loop through the paragraph text witing to print until we absolutely
  // have to.
  //
  unsigned int col=LEFTMARGIN+indent+1;
  unsigned int index=0;
  unsigned int last_white=0;
  const char* p=text.c_str();

  while (p[index])
  {
    switch (p[index])
    {
      // If we have a space - just remember where it is
    case ' ':
      last_white=index;
      ++col;
      ++index;
      break;

      // If we have a tab - remember where it is and assume it
      // will take up 8 spaces in the output.
      //
    case '\t':
      last_white=index;
      col+=8;
      ++index;
      break;

      // If we have a form-feed, carriage-return, or newline, then
      // print what we have so far (including this character) and
      // start a new line.
      //
    case '\n':
    case '\r':
    case '\f':
      os.write(p, index+1);
      p+=index+1;
      col=LEFTMARGIN+indent+1;
      index=last_white=0;
      if (*p)
      {
        os.width(LEFTMARGIN+indent);
        os << "";
      }
      break;

    default:
      ++col;
      ++index;
      break;
    }

    // Are we forced to start a new line?
    if (col>columns)
    {
      // Yes - if possible, print upto the last whitespace character
      // and start the next line on a word-boundary
      if (last_white)
      {
        os.write(p, last_white);
        p+=last_white;
        while (*p==' ') ++p;
      }
      else
      {
        // No word boundary in sight - just split the line here!
        os.write(p, index);
        p+=index;
      }
      os << std::endl;

      // We just printed a newline - dont print another one right now
      while ((*p=='\n') || (*p=='\r') || (*p=='\f')) ++p;
      col=LEFTMARGIN+indent+1;
      index=last_white=0;
      if (*p)
      {
        os.width(LEFTMARGIN+indent);
        os << "";
      }
    }
    else if (index && (!p[index]))
    {
      os << p << std::endl;
    }
  }
}


bool
CmdLine::
handleArg(
    CmdArg* cmdarg,
    const char* arg,
    const char*& endptr)
{
  // update the number of parsed args
  ++pNArgsParsed;

  // call the argument compiler
  bool ok=cmdarg->parse(arg, endptr, *this);
  if (ok)
  {
    cmdarg->setFlags(CmdArg::GIVEN);
    cmdarg->sequence(pNArgsParsed);

    if (arg!=endptr)
    {
      cmdarg->setFlags(CmdArg::VALGIVEN);
    }
  }

  // if we were waiting for a value - we just got it
  if (arg!=endptr)
  {
    if (cmdarg==pMatchedArg) pParseState=PARSE_START_STATE;
  }

  // if this is a list - optional values may follow the one given
  if ((cmdarg->hasSyntax(CmdArg::isLIST) && arg!=endptr)
      || (cmdarg->hasSyntax(CmdArg::isFIXEDLIST) && cmdarg->remainingSize()>0 && arg!=endptr))
  {
    pMatchedArg=cmdarg;
    pParseState=PARSE_WANT_VAL;
  }

  return ok;
}

void
CmdLine::
ck_need_val()
{
  if (pParseState==PARSE_WANT_VAL)
  {
    // argument was given but optional value was not
    pMatchedArg->setFlags(CmdArg::GIVEN);
    if (!pMatchedArg->hasSyntax(CmdArg::isLIST)
        || !pMatchedArg->hasFlag(CmdArg::VALGIVEN))
    {
      const char* dummy;
      handleArg(pMatchedArg, 0, dummy);
    }
    pParseState=PARSE_START_STATE;
  }
  else if (pParseState==PARSE_NEED_VAL)
  {
    // argument was given but required value was not
#ifndef CONFIG_BREAK_ON_ERROR
    if (!hasFlag(CmdLine::FLAG_QUIET))
    {
      argError("value required for", pMatchedArg) << "." << std::endl;
    }
    setStatus(STATUS_VAL_MISSING);
    pParseState=PARSE_START_STATE;
#else
    CmdLineSyntaxError se;
    se << "required value missing";
    throw(se);
#endif
  }
}

CmdLine::UsageSyntax
CmdLine::
syntax() const
{
  if (hasState(STATE_SHORT_USED)
      && hasState(STATE_LONG_USED))
  {
    return USAGE_BOTH;
  }
  else if (hasState(STATE_LONG_USED))
  {
    return USAGE_LONG_ONLY;
  }
  else
  {
    return USAGE_SHORT_ONLY;
  }
}

CmdLine::CmdLineStatus
CmdLine::
missingArgs()
{
  for (std::list<CmdArg*>::iterator itL=pArguments.begin();
       itL!=pArguments.end();
       ++itL)
  {
    CmdArg* ca=*itL;

    if (ca->hasSyntax(CmdArg::isREQ) && !ca->hasFlag(CmdArg::GIVEN))
    {
      if (!hasFlag(CmdLine::FLAG_QUIET))
      {
        error() << ca->synopsis(false) << " required.\n";
      }
      if (hasStatus(STATUS_ARG_MISSING))
      {
        // user didnt supply the missing argument
        return status();
      }
      else if (!hasFlag(FLAG_NO_ABORT) && status())
      {
        // other problems
        return status();
      }
      else
      {
        setStatus(STATUS_ARG_MISSING);
      }
    }
  }
  return status();
}

CmdArg*
CmdLine::
matchShort(char optchar) const
{
  for (std::list<CmdArg*>::const_iterator itL=pArguments.begin();
       itL!=pArguments.end();
       ++itL)
  {
    CmdArg* cmdarg=*itL;

    // skip arguments without shortName
    if (cmdarg->shortName()==0) continue;

    if (optchar==cmdarg->shortName())
    {
      // exact match
      return cmdarg;
    }
    else if (hasFlag(FLAG_ANY_CASE_OPTS))
    {
      // case-insensitive match
      if (tolower(optchar)==tolower(cmdarg->shortName()))
      {
        return cmdarg;
      }
    }
  }

  return 0;
}

CmdArg*
CmdLine::
matchLong(
    const std::string& kwd,
    ptrdiff_t len,
    bool& is_ambiguous) const
{
  CmdArg* partialMatch=0;
  is_ambiguous=false;

  for (std::list<CmdArg*>::const_iterator itL=pArguments.begin();
       itL!=pArguments.end();
       ++itL)
  {
    CmdArg* cmdarg=*itL;

    const std::string source=cmdarg->longName();

    // skip arguments without longName
    if (source.empty()) continue;

    // try to match
    CmdLineMatch result=strmatch(source, kwd, len);

    if (result==MATCH_EXACT) // we found our candidate :-)
    {
      is_ambiguous=false;
      return cmdarg;
    }
    else if (result==MATCH_PARTIAL) // partial match
    {
      if (partialMatch!=0) // was there a partial match before?
      {
        is_ambiguous=true;
      }
      partialMatch=cmdarg;
    }
  }

  if (!is_ambiguous) // there was only one partial match :-)
  {
    return partialMatch;
  }
  else // there were at least two partial matches :-(
  {
    return 0;
  }
}

CmdArg*
CmdLine::
matchPos() const
{
  CmdArg* last_pos_list=0;

  for (std::list<CmdArg*>::const_iterator itL=pArguments.begin();
       itL!=pArguments.end();
       ++itL)
  {
    CmdArg* cmdarg=*itL;

    if (cmdarg->hasSyntax(CmdArg::isPOS))
    {
      if (!cmdarg->hasFlag(CmdArg::GIVEN))
      {
        return cmdarg;
      }
      else if (cmdarg->hasSyntax(CmdArg::isLIST)
               || cmdarg->hasSyntax(CmdArg::isFIXEDLIST))
      {
        last_pos_list=cmdarg;
      }
    }
  }

  return last_pos_list;
}

CmdLine::CmdLineStatus
CmdLine::
parseShort(
    const char* arg)
{
  const char* endptr=arg;
  CmdArg::CmdLineFlags save_flags=CmdArg::CmdLineFlags(0);
  CmdLine::CmdLineStatus rc=CmdLine::CmdLineStatus(0);
  CmdArg* cmdarg=0;
  bool val_ok;

  // see if we left an argument dangling without a value
  ck_need_val();

  do
  { // loop over bundled options
    cmdarg=matchShort(*arg);
    if (cmdarg==0)
    {
#ifndef CONFIG_BREAK_ON_ERROR
      if (!hasFlag(CmdLine::FLAG_QUIET))
      {
        error() << "unknown option \"" << '-' << char(*arg)
                << "\"." << std::endl;
      }
      rc=CmdLine::CmdLineStatus(rc | STATUS_BAD_OPTION);

      ++arg; // skip bad option
      continue;
#else
      CmdLineSyntaxError se;
      se << "unknown option \"" << '-' << char(*arg) << "\"";
      throw(se);
#endif
    }

    // skip past option character
    ++arg;

    //>PG
    if (cmdarg->hasSyntax(CmdArg::isFIXEDLIST))
    {
      cmdarg->clearFixedList();
    }
    //<PG

    save_flags=cmdarg->flags();
    cmdarg->clearFlags();
    cmdarg->setFlags(CmdArg::OPTION);
    if (*arg==0 && cmdarg->hasSyntax(CmdArg::isVALTAKEN))
    {
      // End of string -- value must be in next arg
      // Save this cmdarg-pointer for later and set the parse_state to
      // indicate that we are expecting a value.
      //
      if (cmdarg->hasSyntax(CmdArg::isVALSTICKY))
      {
        // If this argument is sticky we already missed our chance
        // at seeing a value.
        //
        if (cmdarg->hasSyntax(CmdArg::isVALREQ))
        {
#ifndef CONFIG_BREAK_ON_ERROR
          if (!hasFlag(CmdLine::FLAG_QUIET))
          {
            error() << "value required in same argument for "
                    << '-' << char(cmdarg->shortName())
                    << " option." << std::endl;
          }
          rc=CmdLine::CmdLineStatus(rc | STATUS_VAL_MISSING | STATUS_VAL_NOTSTICKY);
          cmdarg->flags(save_flags);
#else
          CmdLineSyntaxError se;
          se << "value required in same argument for "
             << '-' << char(cmdarg->shortName())
             << " option";
          throw(se);
#endif
        }
        else
        {
          // The value is optional - set the GIVEN flag and call
          // handle_arg with NULL (and check the result).
          //
          cmdarg->setFlags(CmdArg::GIVEN);
          pParseState=PARSE_START_STATE;

          const char* dummy;
          val_ok=handleArg(cmdarg, 0, dummy);
          if (!val_ok)
          {
#ifndef CONFIG_BREAK_ON_ERROR
            if (!hasFlag(CmdLine::FLAG_QUIET))
            {
              argError("bad value for", cmdarg) << "." << std::endl;
            }
            rc=CmdLine::CmdLineStatus(rc | STATUS_BAD_VALUE);
            cmdarg->flags(save_flags);
#else
            CmdLineSyntaxError se;
            se << "bad value for ...:\n" << cmdarg->errorMsg();
            throw(se);
#endif
          }
        }
      }
      else // not sticky
      {
        // Wait for the value to show up next time around
        cmdarg->setFlags(CmdArg::GIVEN);
        pMatchedArg=cmdarg;
        pParseState=PARSE_WANT_VAL;

        if (cmdarg->hasSyntax(CmdArg::isVALREQ))
        {
          pParseState=CmdLine::CmdLineParseState(pParseState | PARSE_TOK_REQUIRED);
        }
      }
      return rc;
    }

    // If this option is an isVALSEP and "arg" is not-empty then we
    // have an error.
    //
    if (cmdarg->hasSyntax(CmdArg::isVALTAKEN)
        && cmdarg->hasSyntax(CmdArg::isVALSEP))
    {
#ifndef CONFIG_BREAK_ON_ERROR
      if (!hasFlag(CmdLine::FLAG_QUIET))
      {
        error() << "value required in separate argument for "
                << '-' << char(cmdarg->shortName())
                << " option." << std::endl;
      }
      rc=CmdLine::CmdLineStatus(rc | STATUS_VAL_MISSING | STATUS_VAL_NOTSEP);
      cmdarg->flags(save_flags);
      return rc;
#else
      CmdLineSyntaxError se;
      se << "value required in separate argument for "
         << '-' << char(cmdarg->shortName())
         << " option";
      throw(se);
#endif
    }
    else
    {
      // handle the option
      endptr=arg;
      val_ok=handleArg(cmdarg, arg, endptr);
      if (!val_ok)
      {
#ifndef CONFIG_BREAK_ON_ERROR
        if (!hasFlag(CmdLine::FLAG_QUIET))
        {
          argError("bad value for", cmdarg) << '.' << std::endl;
        }
        rc=CmdLine::CmdLineStatus(rc | STATUS_BAD_VALUE);

        cmdarg->flags(save_flags);
#else
        CmdLineSyntaxError se;
        se << "bad value for ...:\n" << cmdarg->errorMsg();
        throw(se);
#endif
      }
      cmdarg->setFlags(CmdArg::GIVEN);
      if (arg!=endptr) cmdarg->setFlags(CmdArg::VALGIVEN);

      arg=endptr;

      cmdarg->setFlags(CmdArg::VALGIVEN);
    }
  } while (arg && *arg);

  return rc;
}

CmdLine::CmdLineStatus
CmdLine::
parseLong(
    const char* arg)
{
  CmdLine::CmdLineStatus rc=CmdLine::CmdLineStatus(0);
  CmdArg* cmdarg=NULL;
  bool ambiguous=false;
  ptrdiff_t len=-1;
  bool val_ok;

  // see if we left an argument dangling without a value
  ck_need_val();

  // If there is a value with this argument, get it now!
  const char* val=strpbrk(arg, ":=");
  if (val)
  {
    len=val-arg;
    ++val;
  }

  cmdarg=matchLong(arg, len, ambiguous);
  if (cmdarg==0)
  {
#ifndef CONFIG_BREAK_ON_ERROR
    if (!hasFlag(CmdLine::FLAG_QUIET))
    {
      error() << ((ambiguous) ? "ambiguous" : "unknown") << " option "
              << "\"--" << arg << "\"." << std::endl;
    }
    rc=CmdLine::CmdLineStatus(rc | ((ambiguous) ? STATUS_KWD_AMBIGUOUS : STATUS_BAD_KEYWORD));
    return rc;
#else
    CmdLineSyntaxError se;
    se << ((ambiguous) ? "ambiguous" : "unknown") << " option "
       << "\"--" << arg << "\"";
    throw(se);
#endif
  }

  //>PG
  if (cmdarg->hasSyntax(CmdArg::isFIXEDLIST))
  {
    cmdarg->clearFixedList();
  }
  //<PG

  CmdArg::CmdLineFlags save_flags=cmdarg->flags();
  cmdarg->clearFlags();
  cmdarg->setFlags(CmdArg::KEYWORD);
  if (cmdarg->hasSyntax(CmdArg::isVALTAKEN) && val==0)
  {
    // Value must be in the next argument.
    // Save this cmdarg for later and indicate that we are
    // expecting a value.
    //
    if (cmdarg->hasSyntax(CmdArg::isVALSTICKY))
    {
      // If this argument is sticky we already missed our chance
      // at seeing a value.
      //
      if (cmdarg->hasSyntax(CmdArg::isVALREQ))
      {
#ifndef CONFIG_BREAK_ON_ERROR
        if (!hasFlag(CmdLine::FLAG_QUIET))
        {
          error() << "value required in same argument for --"
                  << cmdarg->longName() << " option." << std::endl;
        }
        rc=CmdLine::CmdLineStatus(rc | STATUS_VAL_MISSING | STATUS_VAL_NOTSTICKY);
        cmdarg->flags(save_flags);
#else
        CmdLineSyntaxError se;
        se << "value required in same argument for --"
           << cmdarg->longName() << " option";
        throw(se);
#endif
      }
      else
      {
        // The value is optional - set the GIVEN flag and call
        // handleArg with NULL (and check the result).
        //
        cmdarg->setFlags(CmdArg::GIVEN);
        pParseState=PARSE_START_STATE;

        const char* dummy=0;
        val_ok=handleArg(cmdarg, 0, dummy);
        if (!val_ok)
        {
#ifndef CONFIG_BREAK_ON_ERROR
          if (!hasFlag(CmdLine::FLAG_QUIET))
          {
            argError("bad value for", cmdarg) << "." << std::endl;
          }
          rc=CmdLine::CmdLineStatus(rc | STATUS_BAD_VALUE);

          cmdarg->flags(save_flags);
#else
          CmdLineSyntaxError se;
          se << "bad value for ...:\n" << cmdarg->errorMsg();
          throw(se);
#endif
        }
      }
    }
    else
    {
      // Wait for the value to show up next time around
      cmdarg->setFlags(CmdArg::GIVEN);
      pMatchedArg=cmdarg;
      pParseState=PARSE_WANT_VAL;
      if (cmdarg->hasSyntax(CmdArg::isVALREQ))
      {
        pParseState=CmdLine::CmdLineParseState(pParseState | PARSE_TOK_REQUIRED);
      }
    }
    return rc;
  }

  // If this option is an isVALSEP and "val" is not-NULL then we
  // have an error.
  //
  if (val && cmdarg->hasSyntax(CmdArg::isVALTAKEN)
      && cmdarg->hasSyntax(CmdArg::isVALSEP))
  {
#ifndef CONFIG_BREAK_ON_ERROR
    if (!hasFlag(CmdLine::FLAG_QUIET))
    {
      error() << "value required in separate argument for --"
              << cmdarg->longName() << " option." << std::endl;
    }
    rc=CmdLine::CmdLineStatus(rc | STATUS_VAL_MISSING | STATUS_VAL_NOTSEP);
    cmdarg->flags(save_flags);
    return rc;
#else
    CmdLineSyntaxError se;
    se << "value required in separate argument for --"
       << cmdarg->longName() << " option";
    throw(se);
#endif
  }

  // handle the keyword
  const char* dummy = 0;
  val_ok=handleArg(cmdarg, val, dummy);
  if (!val_ok)
  {
#ifndef CONFIG_BREAK_ON_ERROR
    if (!hasFlag(CmdLine::FLAG_QUIET))
    {
      argError("bad value for", cmdarg) << "." << std::endl;
    }
    rc=CmdLine::CmdLineStatus(rc | STATUS_BAD_VALUE);
    cmdarg->flags(save_flags);
#else
    CmdLineSyntaxError se;
    se << "bad value for ...:\n" << cmdarg->errorMsg();
    throw(se);
#endif
  }

  return rc;
}

CmdLine::CmdLineStatus
CmdLine::
parseValue(
    const char* arg)
{
  CmdArg::CmdLineFlags save_flags=CmdArg::CmdLineFlags(0);
  CmdLine::CmdLineStatus rc=CmdLine::CmdLineStatus(0);
  CmdArg* cmdarg=0;

  if (pParseState & PARSE_WANT_VAL)
  {
    if (pMatchedArg==0)
    {
      CmdLineInternalError ie;
      ie << "parse-state is inconsistent with last-matched-arg";
      throw(ie);
    }
    // get back the cmdarg that we saved for later
    // - here is the value it was expecting
    //
    cmdarg=pMatchedArg;
    save_flags=cmdarg->flags();
  }
  else
  {
    // argument is positional - find out which one it is
    cmdarg=matchPos();
    if (cmdarg==0)
    {
#ifndef CONFIG_BREAK_ON_ERROR
      if (!hasFlag(CmdLine::FLAG_QUIET))
      {
        error() << "too many arguments given." << std::endl;
      }
      rc=CmdLine::CmdLineStatus(rc | STATUS_TOO_MANY_ARGS);
      return rc;
#else
      CmdLineSyntaxError se;
      se << "too many arguments given (" << arg << ")";
      throw(se);
#endif
    }
    save_flags=cmdarg->flags();
    cmdarg->clearFlags();
    cmdarg->setFlags(CmdArg::POSITIONAL);
    if (hasFlag(FLAG_OPTS_FIRST))
    {
      setState(STATE_END_OF_OPTIONS);
    }
  }

  // handle this value
  cmdarg->setFlags(CmdArg::VALSEP);

  const char* dummy;
  bool val_ok=handleArg(cmdarg, arg, dummy);
  if (!val_ok)
  {
#ifndef CONFIG_BREAK_ON_ERROR
    if (!hasFlag(CmdLine::FLAG_QUIET))
    {
      argError("bad value for", cmdarg) << '.' << std::endl;
    }
    rc=CmdLine::CmdLineStatus(rc | STATUS_BAD_VALUE);
    cmdarg->flags(save_flags);
    if (!cmdarg->hasSyntax(CmdArg::isLIST))
    {
      pParseState=PARSE_START_STATE;
    }
#else
    CmdLineSyntaxError se;
    se << "bad value for ...:\n" << cmdarg->errorMsg();
    throw(se);
#endif
  }

  // If the value was okay and we were requiring a value, then
  // a value is no longer required.
  if (val_ok)
  {
    if (cmdarg->hasSyntax(CmdArg::isLIST))
    {
      pParseState=CmdLine::CmdLineParseState(
          pParseState & ~PARSE_TOK_REQUIRED);
    }
    else if (cmdarg->hasSyntax(CmdArg::isFIXEDLIST))
    {
      //we got all the values we needed.
      if (cmdarg->remainingSize()==0)
      {
        pParseState=PARSE_START_STATE;
      }
      else //we still need some values
      {
        pParseState=PARSE_NEED_VAL;
      }
    }
  }

  return rc;
}

std::ostream&
CmdLine::
argError(
    const std::string& error_str,
    const CmdArg* cmdarg) const
{
  std::ostream& os=error() << error_str << ' ';

  if (cmdarg->hasFlag(CmdArg::GIVEN))
  {
    if (cmdarg->hasFlag(CmdArg::KEYWORD))
    {
      os << "--" << cmdarg->longName() << " option";
    }
    else if (cmdarg->hasFlag(CmdArg::OPTION))
    {
      os << '-' << (char)cmdarg->shortName() << " option";
    }
    else
    {
      os << cmdarg->valueName() << " argument";
    }
  }
  else
  {
    if (cmdarg->hasSyntax(CmdArg::isPOS))
    {
      os << cmdarg->valueName() << " argument";
    }
    else
    {
      os << '-' << (char)cmdarg->shortName() << " option";
    }
  }
  return os;
}


unsigned int
CmdLine::
printSynopsis(
    CmdLine::UsageSyntax aSyntax,
    std::ostream& os) const
{
  /*---------------------------------------------------------------
   *  Try to get current terminal width from environment
   *---------------------------------------------------------------*/
  unsigned int columns = STDCOLS;
  char* tmpColumns = std::getenv("COLUMNS");
  if( tmpColumns != NULL)
  {
    columns = std::atoi(tmpColumns);
  }
#ifdef linux
  else
  {
    winsize size;
    int stat;

    memset(&size, 0, sizeof(size));
    stat = ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    if (stat == 0)
    {
      columns = size.ws_col;
    }
  }
#endif
  if( columns < STDCOLS)
    columns = STDCOLS;
  if( columns > MAXCOLS)
    columns = MAXCOLS;

  // first print the command name
  os << "Usage: " << name();

  size_t curPos=pName.size()+7;

  // print option-syntax followed by positional parameters
  std::string buf;
  size_t longest=0;

  for (unsigned int positionals=0;
       positionals<2;
       positionals++)
  {
    for (std::list<CmdArg*>::const_iterator itL=pArguments.begin();
         itL!=pArguments.end();
         ++itL)
    {
      CmdArg* cmdarg=*itL;

      // don't display hidden arguments
      if (cmdarg->hasSyntax(CmdArg::isHIDDEN)) continue;
      if (!positionals && cmdarg->hasSyntax(CmdArg::isPOS)) continue;
      if (positionals && !cmdarg->hasSyntax(CmdArg::isPOS)) continue;

      // figure out how wide this parameter is (for printing)
      size_t len, pl;
      buf=cmdarg->synopsis(
//          true,
          aSyntax==USAGE_SHORT_ONLY || aSyntax==USAGE_BOTH,
          aSyntax==USAGE_LONG_ONLY || aSyntax==USAGE_BOTH);
      len=pl=buf.size();

      if (!len) continue;


//      if (cmdarg->hasSyntax(CmdArg::isOPT)) pl=-2;
//XXX      if (cmdarg->hasSyntax(CmdArg::isLIST)) pl-=4; // " ..."
      if (!cmdarg->hasSyntax(CmdArg::isREQ))
      {
        buf="[" + buf + "]";
        len+=2;
      }

      if (pl>longest) longest=pl;

      // Will this fit
      if (curPos+len+1>columns)
      {
        os << '\n';
        os.width(LEFTMARGIN);
        os << ""; // No - start a new line;
        curPos=LEFTMARGIN;
      }
      else
      {
        os << ' ';
        ++curPos;
      }
      curPos+=len;
      os << buf;
    }
  }
  os << std::endl;

  return (unsigned int)longest;
}


void
CmdLine::
printDescriptions(
    CmdLine::UsageSyntax aSyntax,
    std::ostream& os,
    unsigned int longest) const
{
  bool titleDisplayed=false;
  std::string buf;

  for (unsigned int positionals=0;
       positionals<2;
       positionals++)
  {
    for (std::list<CmdArg*>::const_iterator itL=pArguments.begin();
         itL!=pArguments.end();
         ++itL)
    {
      CmdArg* cmdarg=*itL;

      // don't display hidden arguments
      if (cmdarg->hasSyntax(CmdArg::isHIDDEN)) continue;
      if (!positionals && cmdarg->hasSyntax(CmdArg::isPOS)) continue;
      if (positionals && !cmdarg->hasSyntax(CmdArg::isPOS)) continue;


      if (!titleDisplayed)
      {
        titleDisplayed=true;
        os << "Options/Arguments:\n";
      }

      buf=cmdarg->synopsis(
//          false,
          aSyntax==USAGE_SHORT_ONLY || aSyntax==USAGE_BOTH,
          aSyntax==USAGE_LONG_ONLY || aSyntax==USAGE_BOTH);
      if (buf.size()==0)
      {
        continue;
      }
      if (cmdarg->hasDefaultValue())
      {
        strindent(os, buf, longest+2,
                  cmdarg->description() +
                  " Default: "+cmdarg->getDefaultValueString());
      }
      else
      {
        strindent(os, buf, longest+2, cmdarg->description());
      }
    }
  }
}

CmdLine&
CmdLine::
append(CmdArg* cmdarg)
{
  for (std::list<CmdArg*>::const_iterator p=pArguments.begin();
       p!=pArguments.end();
       ++p)
  {
    if ((cmdarg->shortName()!=0 && (*p)->shortName()==cmdarg->shortName())
        || (cmdarg->longName()!=""
            && strmatch((*p)->longName(), cmdarg->longName())==MATCH_EXACT))
    {
      CmdLineDublicateArgError e;
      e << "duplicate command line argument defined";
      throw(e);
    }
  }
  pArguments.push_back(cmdarg);
  return *this;
}

std::string
CmdLine::
basename(const std::string& filename)
{
  // find last slash in filename
  size_t slash=filename.rfind('/');
  if (slash==std::string::npos) // slash not found
  {
    return filename;
  }
  else
  {
    return filename.substr(slash+1);
  }
}

std::string
CmdLine::
replace(
    std::string s,
    const std::string& replacedText,
    const std::string& replacedBy)
{
  // that would result in an infinite loop!
  if (replacedBy.find(replacedText)!=std::string::npos)
  {
    return s;
  }

  for (size_t i=s.find(replacedText);
       i!=std::string::npos;
       i=s.find(replacedText))
  {
    s.replace(i, replacedText.size(), replacedBy);
  }
  return s;
}
