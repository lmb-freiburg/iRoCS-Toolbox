/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 2825 $$Name$
**       $Date: 2009-09-15 17:04:15 +0200 (Tue, 15 Sep 2009) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2005/01/14 16:17:38  fehr
**  64-Bit Port and netCDF upgrade 3.5->3.6
**
**  Revision 1.2  2004/09/08 14:25:56  ronneber
**  - parseString no longer accepts :t: and :f: for kernel changes, because
**    it will be automatically extracted from Kernel now
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/

#include "SVMError.hh"
#include "GridAxis.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  parseString
 *  ==> see headerfile
 *=======================================================================*/
void svt::GridAxis::parseString( const std::string& params)
{

  /*-----------------------------------------------------------------------
   *  read keyname
   *-----------------------------------------------------------------------*/
  std::string::size_type colonPos = params.find(':');
  if( colonPos == std::string::npos)
  {
    ParseError err;
    err << "Parameter string '" << params << "' does not contain a colon "
        "after key name!\n";
    throw err;
  }
  
  _keyName = params.substr( 0, colonPos);

//  /*-----------------------------------------------------------------------
//   *  read changesKernel flag
//   *-----------------------------------------------------------------------*/
//  if( colonPos+2 >= params.size())
//  {
//    ParseError err;
//    err << "Parameter string '" << params << "' does not contain a "
//        "changesKernel flag after key name\n";
//    throw err;
//  }
//  
//  if( params[colonPos+1] == 't')
//  {
//    setChangesKernel( true);
//  }
//  else if( params[colonPos+1] == 'f')
//  {
//    setChangesKernel( false);
//  }
//  else
//  {
//    ParseError err;
//    err << "Wrong ChangesKernel flag '" <<params[colonPos+1] << "' in '" 
//        << params << "': must be 't' or 'f'\n";
//    throw err;
//  }
//    
//  if(params[colonPos+2] != ':')
//  {
//    ParseError err;
//    err << "ChangesKernel flag in '"  << params << "': must be followed by"
//        " a colon ':'\n";
//    throw err;
//  }
//  
  
  /*-----------------------------------------------------------------------
   *  split remaining string at commas
   *-----------------------------------------------------------------------*/
  std::vector<std::string> words;
  std::string::size_type currentPos = colonPos+1;
  while( currentPos < params.size())
  {
    std::string::size_type splitPos = params.find(',', currentPos);
    if( splitPos == std::string::npos)
    {
      splitPos = params.size();
    }
    words.push_back( params.substr( currentPos, splitPos-currentPos));
    currentPos = splitPos+1;
  }
  
  /*-----------------------------------------------------------------------
   *  now create appropriate list of doubles from given strings
   *-----------------------------------------------------------------------*/
  _values.clear();
  std::string::size_type  wordIndex = 0;
  while( wordIndex < words.size())
  {
    if( words[wordIndex].substr(0,3) == "add")
    {
      double startVal = 0;
      if( _values.size() > 0)
      {
        startVal = _values.back();
      }
      double increment = stringToDouble( words[wordIndex].substr(3), params);
      if( increment <= 0)
      {
        ParseError err;
        err << "missing increment or increment<=0 after 'add' in given string '" 
            << params << "'\n";
        throw err;
      }

      if( wordIndex+1 >= words.size())
      {
        ParseError err;
        err << "missing range end after 'add..' in given string '" 
            << params << "'\n";
        throw err;
      }
      double endVal = stringToDouble( words[wordIndex+1], params);
      for( /*unsigned*/  int i = 1; startVal + i*increment < endVal + 1e-10; ++i)
      {
        _values.push_back( startVal + i*increment);
      }
      wordIndex += 2;
    }
    else if( words[wordIndex].substr(0,3) == "mul")
    {
      double startVal = 0;
      if( _values.size() > 0)
      {
        startVal = _values.back();
      }

      double factor = stringToDouble( words[wordIndex].substr(3), params);
      if( factor <= 1)
      {
        ParseError err;
        err << "missing factor or factor<=1.0 after 'mul' in given string '" 
            << params << "'\n";
        throw err;
      }

      if( wordIndex+1 >= words.size())
      {
        ParseError err;
        err << "missing range end after 'mul..' in given string '" 
            << params << "'\n";
        throw err;
      }
      double endVal = stringToDouble( words[wordIndex+1],params);
     
      
      for( double value = startVal*factor; value < endVal + 1e-10; value *= factor)
      {
        _values.push_back( value);
      }
      wordIndex += 2;
    }
    else
    {
      _values.push_back( stringToDouble( words[wordIndex], params));
      ++wordIndex;
    }
  }
        
}

