/**************************************************************************
**       Title: basic feature vector
**    $RCSfile$
**   $Revision: 5057 $$Name$
**       $Date: 2013-04-05 13:34:53 +0200 (Fri, 05 Apr 2013) $
**   Copyright: LGPL $Author: tschmidt $
** Description:
**
**
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2006/04/10 15:16:42  ronneber
**  - added Constructor from std::vector<double>
**  - added operators +=, *=, +, -
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**  Revision 1.7  2003/05/19 10:49:17  ronneber
**  - added readWithoutLabel()
**
**  Revision 1.6  2002/06/06 07:00:20  ronneber
**  - made again compilable for gcc-2.95.4 (has no std::isspace ...)
**
**  Revision 1.5  2002/06/05 17:18:31  mechnich
**  Made minor corrections for compilation under Win32
**
**  Revision 1.4  2002/05/14 10:11:19  ronneber
**  - added writeWithoutLabel()
**
**  Revision 1.3  2002/05/13 16:15:00  ronneber
**  - removed unused tmpLabel stuff
**
**  Revision 1.2  2002/04/25 06:27:44  pigorsch
**  apapted to new directory structure
**
**  Revision 1.1  2002/03/26 12:44:02  ronneber
**  restructured for autoconf
**
**  Revision 1.6  2002/03/13 14:20:32  pigorsch
**  *** empty log message ***
**
**  Revision 1.5  2002/03/11 17:15:47  mechnich
**  modified some expressions to increase standard conformity (for compilation under Win32)
**
**  Revision 1.4  2002/01/21 13:47:36  pigorsch
**  * changed type of label from double to int
**  * fixed input function
**
**  Revision 1.3  2002/01/17 09:27:41  pigorsch
**  * fixed input/output functions
**
**  Revision 1.2  2002/01/17 09:20:07  pigorsch
**  * renamed p_features to pFeatures
**            p_tmplabel to pTmpLabel
**            p_label to pLabel
**            p_squareValid to pSquareValid
**            p_square to pSquare
**
**  Revision 1.1  2002/01/17 09:06:53  pigorsch
**  * initial revision
**  * BasicFeatureVector is renamed to BasicFV
**  * added input/output support
**
**
**
**
**
**************************************************************************/

#ifndef BASICFV_HH
#define BASICFV_HH

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <cctype>
using std::isspace; // sorry gcc 2.95.4 has no isspace in namespace std

#include "adjustUniqueIDs.hh"
#include "svm_defines.hh"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace svt
{
  class BasicFV
  {
  public:
    typedef std::vector<double>::iterator iterator;
    typedef std::vector<double>::const_iterator const_iterator;
    typedef std::vector<double>::reference reference;
    typedef std::vector<double>::const_reference const_reference;
    typedef std::vector<double>::size_type size_type;

    BasicFV()
            : pLabel(0),
              _uniqueID( MAX_BELIEVABLE_UNIQUE_ID + 1),  /* ensure that user specifies 
                                                  unique ID before it is used */
              pSquareValid(false),
              pSquare(0)
          {
#ifdef _OPENMP
            omp_init_lock(&_pSquareMutexLock);
#endif
          };

    BasicFV( std::vector<double> features, 
             double label = 0, 
             int uniqueID = MAX_BELIEVABLE_UNIQUE_ID + 1)
            : pFeatures( features),
              pLabel( label),
              _uniqueID( uniqueID),
              pSquareValid( false),
              pSquare(0)
          {
#ifdef _OPENMP
            omp_init_lock(&_pSquareMutexLock);
#endif
          }
    
    BasicFV(BasicFV const &fv)
            : pFeatures(fv.pFeatures),
              pLabel(fv.pLabel),
              _uniqueID(fv._uniqueID),
              pSquareValid(fv.pSquareValid),
              pSquare(fv.pSquare)
          {
#ifdef _OPENMP
            omp_init_lock(&_pSquareMutexLock);
#endif
          };

    BasicFV &operator=(BasicFV const &fv)
          {
#ifdef _OPENMP
            omp_set_lock(&_pSquareMutexLock);
#endif            
            pFeatures = fv.pFeatures;
            pLabel = fv.pLabel;
            _uniqueID = fv._uniqueID;
            pSquareValid = fv.pSquareValid;
            pSquare = fv.pSquare;
#ifdef _OPENMP
            omp_unset_lock(&_pSquareMutexLock);
#endif         
            return *this;
          };

    ~BasicFV()
          {
#ifdef _OPENMP
            omp_destroy_lock(&_pSquareMutexLock);
#endif
          }

    void setLabel( double value)
          {
            pLabel=value;
          };

    double getLabel() const
          {
            return pLabel;
          };

    void setUniqueID( unsigned int uid)
          {
            _uniqueID = uid;
          }
    
    
    unsigned int uniqueID() const
          {
            return _uniqueID;
          }
    

    reference operator[](
        int index)
          {
#ifdef _OPENMP
            omp_set_lock(&_pSquareMutexLock);
#endif
            pSquareValid=false;
#ifdef _OPENMP
            omp_unset_lock(&_pSquareMutexLock);
#endif
            return pFeatures[index];
          };

    const_reference operator[](
        int index) const
          {
            return pFeatures[index];
          };

    const_iterator begin() const
          {
            return pFeatures.begin();
          };
    iterator begin()
          {
#ifdef _OPENMP
            omp_set_lock(&_pSquareMutexLock);
#endif
            pSquareValid=false;
#ifdef _OPENMP
            omp_unset_lock(&_pSquareMutexLock);
#endif
            return pFeatures.begin();
          };

    const_iterator end() const
          {
            return pFeatures.end();
          };
    iterator end()
          {
#ifdef _OPENMP
            omp_set_lock(&_pSquareMutexLock);
#endif
            pSquareValid=false;
#ifdef _OPENMP
            omp_unset_lock(&_pSquareMutexLock);
#endif
            return pFeatures.end();
          };

    size_type size() const
          {
            return pFeatures.size();
          };

    void resize(
        size_type newSize)
          {
#ifdef _OPENMP
            omp_set_lock(&_pSquareMutexLock);
#endif
            pFeatures.resize(newSize);
            pSquareValid=false;
#ifdef _OPENMP
            omp_unset_lock(&_pSquareMutexLock);
#endif
          };

    void setZero() // fixme: STL name nachgucken
          {
#ifdef _OPENMP
            omp_set_lock(&_pSquareMutexLock);
#endif
            for (iterator p=begin(); p!=end(); )
            {
              *p=0.;
              ++p;
            }
            pSquareValid=false;
#ifdef _OPENMP
            omp_unset_lock(&_pSquareMutexLock);
#endif
          };

    double square() const
          {
#ifdef _OPENMP
            omp_set_lock(&_pSquareMutexLock);
#endif
            if(!pSquareValid)
            {
              pSquare=0.;
              for (const_iterator p=begin();
                   p!=end();
                   )
              {
                pSquare+=*p * *p;
                ++p;
              }

              pSquareValid=true;
            }
#ifdef _OPENMP
            omp_unset_lock(&_pSquareMutexLock);
#endif

            return pSquare;
          };

    double dotProduct(
        const svt::BasicFV& fv) const
          {
            double sum=0.;

            const_iterator i=begin();
            const_iterator j=fv.begin();
            for (; i!=end(); )
            {
              sum+=(*i) * (*j);
              ++i;
              ++j;
            }

            return sum;
          };

    void readWithoutLabel( std::istream& is)
          {
#ifdef _OPENMP
            omp_set_lock(&_pSquareMutexLock);
#endif
            pFeatures.clear();
            pSquareValid=false;
            
            while( is.good())
            {
              //skip whitespace, stop if non-number character or newline occurs
              char c;
              do
              {
                if( is.rdbuf()->sgetc() == EOF)
                {
#ifdef _OPENMP
                  omp_unset_lock(&_pSquareMutexLock);
#endif
                  return;
                }
                is.get(c);
                if( !is)
                {
#ifdef _OPENMP
                  omp_unset_lock(&_pSquareMutexLock);
#endif
                  return;
                }
              } while( isspace(c) && c != '\n');
              
              is.putback(c);
              if( !isdigit(c) && c != '-' && c != '+' && c != '.')
              {
#ifdef _OPENMP
                omp_unset_lock(&_pSquareMutexLock);
#endif
                return;
              }
              double value;
              is >> value;
              pFeatures.push_back(value);
            } 
#ifdef _OPENMP
            omp_unset_lock(&_pSquareMutexLock);
#endif
          }
    

    void writeWithoutLabel( std::ostream& os) const
          {
            for (svt::BasicFV::const_iterator p = pFeatures.begin();
                 p != pFeatures.end(); ++p )
            {
              os << " " << *p;
            }
          }

    static const char* helpPipeFormat()
          {
            return "<label><ws><feature_0><ws><feature_1>...<ws><feature_n>\n"
                "where <ws> is any number of white spaces except for newline\n"
                "example:\n"
                "4 0.123 2.432 42.0 137.0815 24.35";
          }
    

    bool operator==( const BasicFV& fv) const
          {
            return( pFeatures == fv.pFeatures);
          }
    
    void operator+=( const BasicFV& fv)
          {
#ifdef _OPENMP
            omp_set_lock(&_pSquareMutexLock);
#endif
            size_t n = size();
            BasicFV::iterator resultP = begin();
            BasicFV::const_iterator fvP = fv.begin();
            for( size_t i = 0; i < n; ++i)
            {
              *resultP += *fvP;
              ++resultP;
              ++fvP;
            }
            pSquareValid=false;
#ifdef _OPENMP
            omp_unset_lock(&_pSquareMutexLock);
#endif
          }
    
    void operator*=( double factor)
          {
#ifdef _OPENMP
            omp_set_lock(&_pSquareMutexLock);
#endif
            size_t n = size();
            BasicFV::iterator resultP = begin();
            for( size_t i = 0; i < n; ++i)
            {
              *resultP *= factor;
              ++resultP;
            }
#ifdef _OPENMP
            omp_unset_lock(&_pSquareMutexLock);
#endif
          }
    
            

    friend std::ostream& operator<<(std::ostream& os, const svt::BasicFV& fv);
    friend std::istream& operator>>(std::istream& is, svt::BasicFV& fv);
  private:
    std::vector<double> pFeatures;
    double pLabel;
    unsigned int _uniqueID;
    
#ifdef _OPENMP
    mutable omp_lock_t _pSquareMutexLock;
#endif
    mutable bool pSquareValid;
    mutable double pSquare;
  };

  inline
  std::ostream& operator<<(std::ostream& os, const svt::BasicFV& fv)
  {
    os << fv.pLabel;
    fv.writeWithoutLabel( os);
//    os << "\n";
    return os;
  }

  inline
  std::istream& operator>>(std::istream& is, svt::BasicFV& fv)
  {
    // read label
    double label;
    is >> label;
    fv.setLabel(label);
    fv.readWithoutLabel( is);
    return is;
  }

  inline BasicFV operator+( const BasicFV& fv1, const BasicFV& fv2)
  {
    BasicFV result;
    size_t n = fv1.size();
    result.resize( n);
    BasicFV::iterator resultP = result.begin();
    BasicFV::const_iterator fv1P = fv1.begin();
    BasicFV::const_iterator fv2P = fv2.begin();
    for( size_t i = 0; i < n; ++i)
    {
      *resultP = *fv1P + *fv2P;
      ++resultP;
      ++fv1P;
      ++fv2P;
    }
    return result;
  }
  
  inline BasicFV operator-( const BasicFV& fv1, const BasicFV& fv2)
  {
    BasicFV result;
    size_t n = fv1.size();
    result.resize( n);
    BasicFV::iterator resultP = result.begin();
    BasicFV::const_iterator fv1P = fv1.begin();
    BasicFV::const_iterator fv2P = fv2.begin();
    for( size_t i = 0; i < n; ++i)
    {
      *resultP = *fv1P - *fv2P;
      ++resultP;
      ++fv1P;
      ++fv2P;
    }
    return result;
  }
  
  inline BasicFV operator*( double factor, const BasicFV& fv)
  {
    BasicFV result;
    size_t n = fv.size();
    result.resize( n);
    BasicFV::iterator resultP = result.begin();
    BasicFV::const_iterator fvP = fv.begin();
    for( size_t i = 0; i < n; ++i)
    {
      *resultP = factor * *fvP;
      ++resultP;
      ++fvP;
    }
    return result;
  }
  
  inline BasicFV operator*( const BasicFV& fv, double factor)
  {
    return operator*( factor, fv);
  }
  
  


}

#endif



