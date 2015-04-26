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
**       Title: basic command line argument
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
**  Revision 1.7  2005/06/09 07:57:12  tschmidt
**  - Changed enum names from bla to CmdLinebla
**
**  Revision 1.6  2002/06/03 15:23:52  pigorsch
**  - optional parameters don't have []'s in descriptions anymore
**
**  Revision 1.5  2002/05/27 15:36:41  pigorsch
**  - removed parameter "detailed" in function synopsis
**
**  Revision 1.4  2002/04/22 15:56:57  pigorsch
**  - removed some comments
**
**  Revision 1.3  2002/04/18 13:55:03  pigorsch
**  - removed parameter fixedSize from constructors
**  - changed way of clearing flags
**
**  Revision 1.2  2002/04/10 09:54:20  pigorsch
**  - constructors set pModified=false
**  - adjustSyntax now throws syntax error exception on error
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

#include "CmdArg.hh"
#include "CmdLine.hh"

CmdArg::
CmdArg(
    char aShortName,
    const std::string aLongName,
    const std::string aValueName,
    const std::string aDescription,
    CmdArg::Syntax aSyntax)
        :pFlags((CmdArg::CmdLineFlags)0),
         pSyntax(aSyntax),
         pSequence(0),
         pShortName(aShortName),
         pLongName(aLongName),
         pValueName(aValueName),
         pDescription(aDescription),
         pModified(false)
{
  adjustSyntax();
}

CmdArg::
CmdArg(
    char aShortName,
    const std::string aLongName,
    const std::string aDescription,
    CmdArg::Syntax aSyntax)
        :pFlags((CmdArg::CmdLineFlags)0),
         pSyntax(aSyntax),
         pSequence(0),
         pShortName(aShortName),
         pLongName(aLongName),
         pValueName(),
         pDescription(aDescription),
         pModified(false)
{
  adjustSyntax();
}

CmdArg::
CmdArg(
    const std::string aValueName,
    const std::string aDescription,
    CmdArg::Syntax aSyntax)
        :pFlags((CmdArg::CmdLineFlags)0),
         pSyntax(aSyntax),
         pSequence(0),
         pShortName(0),
         pLongName(),
         pValueName(aValueName),
         pDescription(aDescription),
         pModified(false)
{

  adjustSyntax();
}

CmdArg::
~CmdArg()
{}

void
CmdArg::
adjustSyntax()
{
  // If the value is specified as both OPTIONAL and REQUIRED
  // then assume it is required.
  //
  if (hasSyntax(isVALREQ) && hasSyntax(isVALOPT))
  {
    pSyntax=CmdArg::Syntax(syntax() & (~isVALOPT));
  }

  // If they said the argument was both STICKY and SEPARATE then
  // I dont know what to think just just ignore both of them.
  //
  if (hasSyntax(isVALSTICKY) && hasSyntax(isVALSEP))
  {
    pSyntax=CmdArg::Syntax(syntax() & (~(isVALSTICKY | isVALSEP)));
  }

  // If a non-NULL, non-empty value-name was given but we werent
  // told that the argument takes a value, then assume that it
  // does take a value and that the value is required.
  //
  if (!pValueName.empty() && (!hasSyntax(isVALTAKEN)))
  {
    pSyntax=CmdArg::Syntax(syntax() | isVALREQ);
  }

  // If a value is taken and the argument is positional, then
  // we need to make isREQ and isOPT consistent with isVALREQ
  // and isVALOPT
  //
  if (hasSyntax(isVALTAKEN) && hasSyntax(isPOS))
  {
    pSyntax=CmdArg::Syntax(syntax() & (~(isREQ | isOPT)));
    if (hasSyntax(isVALREQ)) pSyntax=CmdArg::Syntax(syntax() | isREQ);
    if (hasSyntax(isVALOPT)) pSyntax=CmdArg::Syntax(syntax() | isOPT);
  }

  // If a value is taken but no value name was given,
  // then default the value name.
  //
  if (hasSyntax(isVALTAKEN) && (pValueName.empty()))
  {
    pValueName="value";
  }

  // If argument has fixed size>0 then it must me a fixed list.
  if (fixedSize()>0)
  {
    pSyntax=CmdArg::Syntax(syntax() | isFIXEDLIST);

    // if an argument is a fixed list, it cant be a normal list.
    pSyntax=CmdArg::Syntax(syntax() & (~isLIST));
  }
  else
  {
    pSyntax=CmdArg::Syntax(syntax() & (~isFIXEDLIST));
  }

  // If no keyword name or character name was given, then the
  // argument had better take a value and it must be positional
  //
  if ((!pShortName) && (pLongName.empty())
      && (!hasSyntax(isPOS)))
  {
    if (hasSyntax(isVALTAKEN))
    {
      pSyntax=CmdArg::Syntax(syntax() | isPOS);
    }
    else
    {
      CmdLineSyntaxError se;
      se << "syntax definition error: non-positional CmdArg has no short or long name";
      throw(se);
    }
  }
}

const std::string
CmdArg::
synopsis(
    bool displayShort,
    bool displayLong) const
{
  char sn=shortName();
  std::string ln=longName();
  std::string vn=valueName();

  std::ostringstream ost;

  if (!hasSyntax(CmdArg::isPOS))
  {
    if (ln.empty())
    {
      displayLong=false;
    }
    if (sn==0)
    {
      displayShort=false;
    }

    if (!displayShort && !displayLong)
    {
      if (sn!=0)
      {
        displayShort=true;
      }
      else if (!ln.empty())
      {
        displayLong=true;
      }
    }
  }

  //if (!hasSyntax(CmdArg::isREQ)) ost << '[';

  if (displayShort && displayLong
      && !hasSyntax(CmdArg::isPOS)
      && hasSyntax(CmdArg::isVALTAKEN)
      && hasSyntax(CmdArg::isVALSTICKY))
  {
    if (hasSyntax(CmdArg::isVALOPT))
    {
      ost << '-' << char(sn) << '[' << vn << "]|--"
          << ln << "[=" << vn << ']';
    }
    else
    {
      ost << '-' << sn << vn << "|--" << ln << '=' << vn;
    }

    if (hasSyntax(CmdArg::isFIXEDLIST))
    {
      ost << '(' << fixedSize() << ')';
    }

    if (hasSyntax(CmdArg::isLIST))
    {
      ost << " ...";
    }

    //if (!hasSyntax(CmdArg::isREQ))
    //{
    //  ost << ']';
    //}

    return ost.str();
  }

  if (!hasSyntax(CmdArg::isPOS))
  {
    if (displayShort)
    {
      ost << '-' << char(sn);
      if (displayLong)
      {
        ost << '|';
      }
    }
    if (displayLong)
    {
      ost << "--" << ln;
    }

    if (hasSyntax(CmdArg::isVALTAKEN))
    {
      if (!hasSyntax(CmdArg::isVALSTICKY))
      {
        ost << ' ';
      }
    }
  }

  // If the argument takes a value then print the value
  if (hasSyntax(CmdArg::isVALTAKEN))
  {
    if (!hasSyntax(CmdArg::isPOS) &&
        hasSyntax(CmdArg::isVALOPT))
    {
      ost << '[';
    }
    if (hasSyntax(CmdArg::isVALSTICKY))
    {
      if (!displayShort && displayLong) ost << '=';
    }
    ost << vn;

    if (hasSyntax(CmdArg::isFIXEDLIST))
    {
      ost << '(' << fixedSize() << ')';
    }

    if (hasSyntax(CmdArg::isLIST))
    {
      ost << " ...";
    }
    if (!hasSyntax(CmdArg::isPOS) &&
        hasSyntax(CmdArg::isVALOPT))
    {
      ost << ']';
    }
  }

  //if (!hasSyntax(CmdArg::isREQ))
  //{
  //  ost << ']';
  //}

  return ost.str();
}
