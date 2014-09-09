/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 4820 $$Name$
**       $Date: 2011-11-08 10:57:01 +0100 (Tue, 08 Nov 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2004/09/08 14:33:22  ronneber
**  - adapted to new ParamInfo class
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#include <iostream>
#include "PrettyOptionPrinter.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void svt::PrettyOptionPrinter::printOption( const ParamInfo& param)
{
  SVM_ASSERT( param.nAlternatives() > 0);
  
  std::string itemIndentStr(_itemIndent, ' ');
  
  std::string key = std::string("-") + param.shortKey() 
      + ", --" + param.longKey() + " ";
  
  if( param.nAlternatives() == 1)
  {
    printDefListParagraph( 
        itemIndentStr + key + param.alternatives(0).value,
        param.alternatives(0).helpText);
  }
  else
  {
    /*----------------------------------------------------------
     *  print alternatives, if exist
     *
     *  calculate maximum of alternative size
     *----------------------------------------------------------*/
    size_t maxAlterSize = 0;
    for( unsigned int i = 0; i < param.nAlternatives(); ++i)
    {
      if( param.alternatives(i).value.size() > maxAlterSize)
      {
        maxAlterSize = param.alternatives(i).value.size();
      }
    }
    
    int alterIndent = _indentColumn - static_cast<int>(maxAlterSize) - 3;
    if( alterIndent < _minAlterIndent) alterIndent = _minAlterIndent;
    
    /*---------------------------------------------------------------------
     *  print single line with key
     *---------------------------------------------------------------------*/
    _os << "\033[1m" << itemIndentStr << key <<  "\033[0m\n";
    
    /*---------------------------------------------------------------------
     *  print alternatives
     *---------------------------------------------------------------------*/
    std::string alterIndentStr( alterIndent, ' ');
    for( unsigned int i = 0; i < param.nAlternatives(); ++i)
    {
      printDefListParagraph( 
          alterIndentStr + param.alternatives(i).value,
          param.alternatives(i).helpText);
    }
  }
}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  printOptions
 *  ==> see headerfile
 *=======================================================================*/
void svt::PrettyOptionPrinter::printOptions( 
    std::string header, const std::vector<ParamInfo>& params)
{
  printSubHeader( header);
  for( std::vector<ParamInfo>::const_iterator p =
           params.begin(); p != params.end(); ++p)
  {
//    if( p != params.begin()) _os << "\n";
    printOption( *p);
  }
}



/*=========================================================================
 *  DESCRIPTION OF FUNCTION:  printDefListParagraph
 *  ==> see headerfile
 *=======================================================================*/
void svt::PrettyOptionPrinter::printDefListParagraph( 
    const std::string& item, const std::string& description)
{
  _os << "\033[1m" << item << "\033[0m" << "   ";  //ensure at least two white space behind option
  
  int currentColumn = static_cast<int>(item.size()) + 3;
  
  
  std::string::size_type firstCharPos = 0;
  std::string::size_type breakCharPos = 0;
  std::string::size_type columnWidth = 78 - indentColumn();
  
  
  while( firstCharPos != std::string::npos)
  {
    breakCharPos = firstCharPos + columnWidth;
    if( breakCharPos > description.size())
    {
      breakCharPos = description.size();
    }
    else
    {
      std::string::size_type possibleBreakPos = description.rfind( ' ', breakCharPos);
      if( possibleBreakPos > firstCharPos) 
      {
        breakCharPos = possibleBreakPos;
      }
    }
    
    if( currentColumn > indentColumn())
    {
      _os << std::endl;
      currentColumn = 0;
    }
    
    for( int i = currentColumn; i < indentColumn(); ++i)
    {
      _os << " ";
    }
    _os << description.substr( firstCharPos, breakCharPos - firstCharPos)
        << std::endl;
    firstCharPos = description.find_first_not_of(' ', breakCharPos);
    currentColumn = 0;
  }
}

