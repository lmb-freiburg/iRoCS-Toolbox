/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 538 $$Name$
**       $Date: 2004-09-13 12:04:04 +0200 (Mon, 13 Sep 2004) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.2  2004/09/08 14:33:53  ronneber
**  - adapted to new ParamInfo class
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef PRETTYOPTIONPRINTER_HH
#define PRETTYOPTIONPRINTER_HH

#include <iostream>
#include <map>
#include "HelpExtractor.hh"
#include "ParamInfo.hh"
#include "SVMError.hh"

namespace svt
{
  /*======================================================================*/
  /*!
   *  \class PrettyOptionPrinter
   *  \ingroup commandline
   *  \brief The PrettyOptionPrinter class contains mehtods for pretty
   *  printing of commandline options  
   *
   *  
   */
  /*======================================================================*/
  class PrettyOptionPrinter
  {
  public:
    
    PrettyOptionPrinter( std::ostream& os, int indentColumn = 30)
            : _indentColumn( indentColumn),
              _itemIndent( 4),
              _minAlterIndent( 12),
              _os(os)
          {}
    

    /*======================================================================*/ 
    /*! 
     *   pretty print command line option. Description is written as a
     *   block starting from prettyPrintDescrColumn()
     *
     *   \param param infos from the parameter
     */
    /*======================================================================*/
    void printOption( const ParamInfo& param);
    

    /*======================================================================*/
    /*! 
     *   print subHeader and all options contained in the vector.
     *
     *   \param header  header passed to printSubHeader
     *   \param params  vector of ParamInfos to be printed
     */
    /*======================================================================*/
    void printOptions( std::string header,
                       const std::vector<ParamInfo>& params);
     
                
    
    /*======================================================================*/
    /*! 
     *   print one paragraph of a definition list. 
     *
     *   \param item  item to be defined, e.g. "--gamma value"
     *   \param description  text, for this item. Will be intented at
     *                indentColumn() and wrapped at column 78  
     */
    /*======================================================================*/
    void printDefListParagraph( const std::string& item, 
                                const std::string& description);
    

    
    void printHeader( const std::string& header)
          {
            _os << header << "\n";
          }
    


    void printSubHeader( const std::string& header)
          {
            _os << "\n"
                << header << "\n"
                << std::string( header.size(), '-') << "\n";
          }
    


    /*======================================================================*/
    /*! 
     *   get / set start column for pretty printing of command line options
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    int indentColumn() const
          {
            return _indentColumn;
          }
    
    void setIndentColumn( int column)
          {
            _indentColumn = column;
          }
    
#if 0
    template< typename LIST>
    void printTypeHelp( std::string key) const
          {
            std::map<std::string, std::string> nameDescriptionMap;
            
            HelpExtractor<LIST>::getNamesDescriptions( nameDescriptionMap);
            for( std::map<std::string,std::string>::const_iterator p = 
                     nameDescriptionMap.begin(); 
                 p != nameDescriptionMap.end(); ++p)
            {
              printOption( key + " " + p->first, p->second);
            }
          }
#endif
  private:    
    int _indentColumn;
    int _itemIndent;
    int _minAlterIndent;
    std::ostream& _os;
    
  };
  
}



#endif
