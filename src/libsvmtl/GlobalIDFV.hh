/**************************************************************************
**       Title: GlobalIDFV
**    $RCSfile$
**   $Revision: 907 $$Name$
**       $Date: 2006-10-06 15:50:05 +0200 (Fri, 06 Oct 2006) $
**   Copyright: LGPL $Author: fehr $
** Description: globalID feature vector. Feature vector consisting of single 
**              entry namely a globalID. Reading in ASCII-Mode consists of
**              two entries per line: one label and one globalID
**              As methods like square() are missing, common SVM-kernel like 
**              Kernel_RBF will not work with this FV. The FV can work with 
**              kernels like Kernel_DS_TRIA, Kernel_DS_RBF, Kernel_FILE and 
**              other file-based kernels, where the global ID is taken
**              as 'coordinates' into these matrix-files 
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2006/10/06 13:50:05  fehr
**  linear model optimizer added
**
**  Revision 1.2  2005/07/19 13:42:59  haasdonk
**  adapted header for automatic CVS-Information extraction
**
**
**************************************************************************/

#ifndef GLOBALIDFV_HH
#define GLOBALIDFV_HH

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <cctype>
using std::isspace; // sorry gcc 2.95.4 has no isspace in namespace std

#include "adjustUniqueIDs.hh"
#include "svm_defines.hh"

namespace svt
{
  class GlobalIDFV
  {
  public:
    typedef std::vector<double>::iterator iterator;
    typedef std::vector<double>::const_iterator const_iterator;
    typedef std::vector<double>::reference reference;
    typedef std::vector<double>::const_reference const_reference;
    typedef std::vector<double>::size_type size_type;

    GlobalIDFV()
            : pLabel(0),
              _uniqueID( MAX_BELIEVABLE_UNIQUE_ID + 1), 
              pGlobalID(0)
          {};

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
    

    void setGlobalID( unsigned int uid)
          {
            pGlobalID = uid;
          }
    
    
    unsigned int getGlobalID() const
          {
            return pGlobalID;
          }
  

//     reference operator[](
//         int index)
//           {
//             pSquareValid=false;
//             return pFeatures[index];
//           };

//     const_reference operator[](
//         int index) const
//           {
//             return pFeatures[index];
//           };

//     const_iterator begin() const
//           {
//             return pFeatures.begin();
//           };
//     iterator begin()
//           {
//             pSquareValid=false;
//             return pFeatures.begin();
//           };

//     const_iterator end() const
//           {
//             return pFeatures.end();
//           };
//     iterator end()
//           {
//             pSquareValid=false;
//             return pFeatures.end();
//           };

     size_type size() const
           {
             return 0;
           };

//     void resize(
//         size_type newSize)
//           {
//             pFeatures.resize(newSize);
//             pSquareValid=false;
//           };

//     void setZero() // fixme: STL name nachgucken
//           {
//             for (iterator p=begin();
//                  p!=end();
//                  )
//             {
//               *p=0.;
//               ++p;
//             }
//             pSquareValid=false;
//           };

//     double square() const
//           {
//             if(!pSquareValid)
//             {
//               pSquare=0.;
//               for (const_iterator p=begin();
//                    p!=end();
//                    )
//               {
//                 pSquare+=*p * *p;
//                 ++p;
//               }

//               pSquareValid=true;
//             }

//             return pSquare;
//           };

//     double dotProduct(
//         const svt::GlobalIDFV& fv) const
//           {
//             double sum=0.;

//             const_iterator i=begin();
//             const_iterator j=fv.begin();
//             for (;
//                  i!=end();
//                  )
//             {
//               sum+=(*i) * (*j);
//               ++i;
//               ++j;
//             }

//             return sum;
//           };

    void readWithoutLabel( std::istream& is)
          {
            if( is.good())
            {
              //skip whitespace, stop if non-number character or newline occurs
              char c;
              do
              {
                if( is.rdbuf()->sgetc() == EOF) return;
                is.get(c);
                if( !is) return;
              } while( isspace(c) && c != '\n');
              
              is.putback(c);
              if( !isdigit(c) && c != '-' && c != '+' && c != '.') return;
              is >> pGlobalID;
            } 
          }
    

    void writeWithoutLabel( std::ostream& os) const
          {
            os << " " << pGlobalID << " ";
          }

    static const char* helpPipeFormat()
          {
            return "<label><ws><feature_0>\n"
                "where <ws> is any number of white spaces except for newline\n"
                "example:\n"
                "-1 1001";
          }
    

    bool operator==( const GlobalIDFV& fv) const
          {
            return( pGlobalID == fv.pGlobalID);
          }
    
    // this does not make sense - but a dummy is needed 
    void operator+=( const GlobalIDFV& fv)
    {}
    
    // this does not make sense - but a dummy is needed
    void operator*=( double factor)
    {}
   
    // this does not make sense - but a dummy is needed
    void setZero()
    {}

    void resize(int i)
    {}

    friend std::ostream& operator<<(std::ostream& os, const svt::GlobalIDFV& fv);
    friend std::istream& operator>>(std::istream& is, svt::GlobalIDFV& fv);

  private:
    double pLabel;
    unsigned int _uniqueID;
    unsigned int pGlobalID;
  };

  inline
  std::ostream& operator<<(std::ostream& os, const svt::GlobalIDFV& fv)
  {
    os << fv.pLabel;
    fv.writeWithoutLabel( os);
    return os;
  }

  inline
  std::istream& operator>>(std::istream& is, svt::GlobalIDFV& fv)
  {
    // read label
    double label;
    is >> label;
    fv.setLabel(label);
    fv.readWithoutLabel( is);
    return is;
  }

}

#endif



