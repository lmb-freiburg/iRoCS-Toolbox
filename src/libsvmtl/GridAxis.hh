/**************************************************************************
**       Title: one axis for grid search
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
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef GRIDAXIS_HH
#define GRIDAXIS_HH

#include <string>
#include <vector>
#include <stdlib.h>
#include <cstddef>

#include "SVMError.hh"

namespace svt
{
  class GridAxis
  {
  public:
    GridAxis()
            :_changesKernel(true)
          {}
    
    GridAxis( const std::string& params)
          {
            parseString( params);
          }
    
    
    /*======================================================================*/
    /*! 
     *   parse specification for grid axis from string. Syntax is
     *   <keyname>:<changesKernel>:<valuespec>
     *
     *   with <keyname> the name of the key, e.g. "cost"
     *   <changesKernel> flag, wether this parametrer changes the
     *                   kernel, e.g. "t" or "f" 
     *   <valuespec>     specificaion of values, which can be
     *
     *   linear, e.g.,      "1,add1,5"  --> values: 1,2,3,4,5
     *   exponential, e.g., "1,mul2,16" --> values: 1,2,4,8,16
     *   list, e.g.,        "-2,5,42,3" --> values: -2,5,42,3
     *
     *   \param params string containing grid axis specification 
     *
     *   \exception ParseError params-string has incorrect format
     */
    /*======================================================================*/
    void parseString( const std::string& params);
    

    /*======================================================================*/
    /*!  
     *   Name of key belonging to this axis
     *
     *   \return key name
     */
    /*======================================================================*/
    const std::string& keyName() const
          {
            return _keyName;
          }
    
    /*======================================================================*/
    /*!  
     *   number of values that belong to this axis
     *
     *   \return key name
     */
    /*======================================================================*/
    size_t nValues() const
          {
            return _values.size();
          }
    
    /*======================================================================*/
    /*! 
     *   value[index]
     *
     *   \param index index
     *
     *   \return value[index]
     */
    /*======================================================================*/
    double value( size_t index) const
          {
            return _values[index];
          }
    
    /*======================================================================*/
    /*! 
     *   specify, wether kernel is affected from this key or not. This info is 
     *   required in Grid-search, to decide wether to recalculate the kernel
     *   matrix or not. E.g. changing "gamma" affects the kernel matrix, while
     *   changing "cost" does only affect the training algorithm
     *
     *   \param f new value for flag
     */
    /*======================================================================*/
    void setChangesKernel( bool f)
          {
            _changesKernel = f;
          }
    
    /*======================================================================*/
    /*! 
     *   see setChangesKernel() for Details
     *
     *   \return changesKernel, Default is true
     */
    /*======================================================================*/
    bool changesKernel() const
          {
            return _changesKernel;
          }
    

  private:
    double stringToDouble( const std::string& s, const std::string& param)
          {
            char* endptr;
            double v = strtod( s.c_str(), &endptr);
            size_t nInterpretedChars = static_cast<size_t>(endptr - s.c_str());
            if(nInterpretedChars < s.size())
            {
              ParseError err;
              err << "Could not convert '" << s << "' in '" << param << "' to double number\n";
              throw err;
            }
            return v;
            
          }
    
              
              
            

    std::string         _keyName;
    std::vector<double> _values;
    bool                _changesKernel;
    
  };
}

            
#endif
