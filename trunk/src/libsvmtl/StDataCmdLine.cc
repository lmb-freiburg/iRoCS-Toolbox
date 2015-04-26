/**************************************************************************
 *
 * Copyright (C) 2004-2015 Olaf Ronneberger, Florian Pigorsch, Jörg Mechnich,
 *                         Thorsten Falk
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
**       Title: 
**    $RCSfile$
**   $Revision: 795 $$Name$
**       $Date: 2006-01-11 18:57:36 +0100 (Wed, 11 Jan 2006) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2006/01/11 17:57:18  ronneber
**  - moved intepretation of single '-' for filenames from stdin to
**    StDataCmdLine, now it is available in all parts of the programs
**
**  Revision 1.2  2004/09/08 14:38:50  ronneber
**  - added updateShortCutTable() and translateShortKeys() for short key, e.g.,
**    "-g" processing
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/


#include "StDataCmdLine.hh"
#include "ParamInfo.hh"


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  parseCommandline
 *  ==> see headerfile
 *=======================================================================*/
void svt::StDataCmdLine::parseCommandline( int argc, const char** argv)
{
  /*-----------------------------------------------------------------------
   *  special treatment for '--help' and '-h'
   *-----------------------------------------------------------------------*/
  if( argc > 1 
      && ( std::string(argv[1]) == "--help" 
           || std::string( argv[1]) == "-h"))
  {
    setValue( "--help", "");
    _helpRequested = true;
    
    return;
  }
  
  
  /*-----------------------------------------------------------------------
   *  get mode (e.g. train, classify crossval, ...)
   *-----------------------------------------------------------------------*/
  if( argc == 1)
  {
    ParseCmdLineError err;
    err << "no mode specified!";
    throw err;
  }
  
  _mode = argv[1];
  
  /*-----------------------------------------------------------------------
   *  break parameters into key-value pairs (each option must have
   *  exacty one following parameter)
   *-----------------------------------------------------------------------*/
  int i = 2;
  while( i < argc)
  {
    std::string currentArg = argv[i];
    if( currentArg.size() < 2 || currentArg[0] != '-')
    {
      break;
    }
    
    if( currentArg[1] != '-')
    {
      /*-------------------------------------------------------------------
       *  single minus arguments are stored unprocessed
       *-------------------------------------------------------------------*/
      std::string shortKey;
      
      std::string value;
      if( i < argc-1)
      {
        setValue( currentArg, argv[i+1]);
        i += 2;
      }
      else
      {
        ParseCmdLineError err;
        err << "missing parameter for argument `" << currentArg << "'";
        throw err;
      }
    }
    else if( currentArg[1] == '-')
    {
      /*-----------------------------------------------------------------
       *  extended argument (must be two words), e.g. "--gamma 0.42"
       *-----------------------------------------------------------------*/
      if( i < argc-1)
      {
        std::string key = currentArg.substr(2);
        setValue( key, argv[i+1]);
        i += 2;
      }
      else if( currentArg == "--help")
      {
        setValue( "--help", "");
        _helpRequested = true;
        
        i += 1;
      }
      else
      {
        ParseCmdLineError err;
        err << "missing parameter for argument `"<< currentArg << "'";
        throw err;
      }
    }
    else
    {
      ParseCmdLineError err;
      err << "error while parsing argument `" << currentArg << "'";
      throw err;
    }
  }

  /*-----------------------------------------------------------------------
   *  remaining arguments must be filenames
   *
   *  if filenames is single '-' read filenames from stdin
   *-----------------------------------------------------------------------*/
  if( argc-i == 1 && std::string(argv[i]) == "-")
  {
    // read file names from stdin
    std::cerr << "reading filenames from stdin..\n";
    std::string name;
    while( std::cin >> name)
    {
      _filenames.push_back( name);
    }
    std::cerr << "OK, got " << _filenames.size() << " file names.\n";
  }
  else
  {
    _filenames.resize( argc-i);
    std::copy( argv+i, argv+argc, _filenames.begin());
  }

  return;
}



/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void svt::StDataCmdLine::updateShortCutTable( const std::vector<ParamInfo>& params)
{
  /*-----------------------------------------------------------------------
   *  iterate through all elements and add their shorKey and LongKey to 
   *  internal translation map
   *-----------------------------------------------------------------------*/
  for( std::vector<ParamInfo>::const_iterator 
           p = params.begin(); p != params.end(); ++p)
  {
    /*---------------------------------------------------------------------
     *  search, if there is already an entry for this shortcut  
     *---------------------------------------------------------------------*/
    std::map<std::string, std::string>::iterator q = 
        _shortToLongKeyMap.find( p->shortKey());
    
    if( q == _shortToLongKeyMap.end())
    {
      /*-------------------------------------------------------------------
       *  mapping does not exist yet. Just add it
       *-------------------------------------------------------------------*/
      _shortToLongKeyMap[p->shortKey()] = p->longKey();
    }
    else if( q->second != p->longKey())
    {
      /*-------------------------------------------------------------------
       *  ambiguous mapping: all longKey's for this shortKey are 
       *  concatenated in the value string with '--' before each value, 
       *  e.g. "--cost --coef --custom" 
       *-------------------------------------------------------------------*/
      if( q->second != "" && q->second[0] != '-')
      {
        q->second = std::string( "--") + q->second;
      }
      q->second += " --" + p->longKey();
    }
  }
  
}





/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  translateShortKeys
 *  ==> see headerfile
 *=======================================================================*/
void svt::StDataCmdLine::translateShortKeys()
{
  /*-----------------------------------------------------------------------
   *  copy short key arguments to extra table and remove them from _map
   *-----------------------------------------------------------------------*/
  std::vector<std::pair<std::string, std::string> > shortKeyArguments;
  
  std::map<std::string, std::string>::iterator p = _map.begin(); 
  while( p != end())
  {
    if( p->first[0] == '-')
    {
      shortKeyArguments.push_back( *p);
      std::map<std::string, std::string>::iterator p_erase = p;
      ++p;
      _map.erase( p_erase);
    }
    else
    {
      ++p;
    }
  }

  /*-----------------------------------------------------------------------
   *  translate the extracted short key argumens
   *-----------------------------------------------------------------------*/
  for( std::vector<std::pair<std::string, std::string> >::const_iterator 
           p = shortKeyArguments.begin(); p != shortKeyArguments.end(); ++p)
  {
    std::string key = p->first;

    /*-------------------------------------------------------------------
     *  search short key in translation map
     *-------------------------------------------------------------------*/
    std::map<std::string, std::string>::const_iterator q
        = _shortToLongKeyMap.find( key.substr(1));
    
    /*-------------------------------------------------------------------
     *  check for unknown short key
     *-------------------------------------------------------------------*/
    if( q == _shortToLongKeyMap.end() || q->second == "")
    {
      ParseCmdLineError err;
      err << "unknown argument `" << key << "'";
      throw err;
    }
    
    /*-------------------------------------------------------------------
     *  check for ambiguous short key
     *-------------------------------------------------------------------*/
    if( (q->second)[0] == '-')
    {
      ParseCmdLineError err;
      err << "ambiguous short key '" << key << "' . Please use "
          "appropriate long key (" << q->second << ") instead";
      throw err;
    }
    
    /*-------------------------------------------------------------------
     *  insert appropriate longkey/value pair 
     *-------------------------------------------------------------------*/
    setValue( q->second, p->second);
  }

}

