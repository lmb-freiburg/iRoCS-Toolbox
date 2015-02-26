/**************************************************************************
**       Title: 
**    $RCSfile$
**   $Revision: 2824 $$Name$
**       $Date: 2009-09-14 09:30:46 +0200 (Mon, 14 Sep 2009) $
**   Copyright: LGPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/03/29 18:03:55  ronneber
**  - added FV_begin() and FV_end()
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef SVM_PROBLEM_HH
#define SVM_PROBLEM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

namespace svt
{
  template< typename FV>
  struct SVM_Problem
  {
    SVM_Problem()
            : l(0),
              y(0),
              x(0)
          {}

    SVM_Problem( int size)
            : l(0),
              y(0),
              x(0)
          {
            resize( size);
          }

  private:
    // forbid copying of SVM_Problem
    SVM_Problem( const SVM_Problem<FV>& orig) {}
    void operator=( const SVM_Problem<FV>& orig) {}
  public:
    

    ~SVM_Problem()
          {
            free_memory();
          }
    

    void resize( int size)
          {
            if( y != 0 || x != 0)
            {
              free_memory();
            }
            y = new double[ size];
            x = new FV*[size];
            l = size;
          }

    void free_memory()
          {
            if( y != 0 || x != 0)
            {
              delete[] x;
              delete[] y;
            }
            
            l = 0;
            x = 0;
            y = 0;
          }

    unsigned int nFeatureVectors() const
          {
            return l;
          }
    
    FV* featureVector( unsigned int i) const
          {
            return x[i];
          }
    
    double label( unsigned int i) const
          {
            return y[i];
          }
    
    FV** FV_begin() const
          {
            return x;
          }
    
    FV** FV_end() const
          {
            return x + l;
          }
    


    int l;
    double* y;  // array of y's (labels)
    FV**    x;  // array of pointers to Feature Vectors

  };
  
  
}
  
#endif
