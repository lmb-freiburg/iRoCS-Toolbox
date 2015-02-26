/**************************************************************************
**       Title: SparseFV.hh
**    $RCSfile$
**   $Revision: 907 $$Name$
**       $Date: 2006-10-06 15:50:05 +0200 (Fri, 06 Oct 2006) $
**   Copyright: LGPL $Author: fehr $
** Description: Implementation of a sparse feature vector
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2006/10/06 13:50:05  fehr
**  linear model optimizer added
**
**  Revision 1.2  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**  Revision 1.5  2002/09/04 10:29:57  pigorsch
**  - initial revision, copied from libpollenpp and modified
**
**  Revision 1.4  2002/06/27 10:59:17  pigorsch
**  - added function cleanup
**
**  Revision 1.3  2002/05/23 11:54:44  ronneber
**  - inlined operator<< and operator>>
**
**  Revision 1.2  2002/05/14 10:15:54  ronneber
**  - made interface more similar to BasicFV to make them more easily exchangeable
**
**  Revision 1.1  2002/03/26 12:44:02  ronneber
**  restructured for autoconf
**
**  Revision 1.2  2002/01/28 13:42:15  pigorsch
**  * added functions Read-/WriteWithoutLabel
**
**  Revision 1.1  2002/01/14 13:54:04  pigorsch
**  * initial revision
**
**
**
**************************************************************************/

#ifndef SPARSEFV_HH
#define SPARSEFV_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <iostream>
#include <list>
#include <cctype>

#include "svm_defines.hh"
#include "SVMError.hh"

using std::isspace;


namespace svt
{
  /*======================================================================*/
  /*!
   *  \class SparseItem
   *  \brief The SparseItem class specifies one element in a SpareFV 
   *
   */
  /*======================================================================*/
  class SparseItem
  {
  public:
    SparseItem(
        unsigned int index,
        double value)
            :pIndex(index),
             pValue(value)
          {};

    unsigned int index() const
          {
            return pIndex;
          };
    
    const double& value() const
          {
            return pValue;
          };

    double& value() 
          {
            return pValue;
          };

    
  protected:
    unsigned int pIndex;
    double pValue;
  };
  
  /*======================================================================*/
  /*!
   *  \class SparseFV
   *  \ingroup lowlevel
   *  \brief The SparseFV class specifies a vector with sparse storage
   *
   *  only non-zero elements of thethe vector are stored in a list
   */
  /*======================================================================*/
  class SparseFV
  {
  public:
    typedef std::list<svt::SparseItem>::iterator       iterator;
    typedef std::list<svt::SparseItem>::const_iterator const_iterator;
    typedef double&                                    reference;
    typedef const double&                              const_reference;
    typedef unsigned int                               size_type;

    /*====================================================================*/
    /*! 
     *   Constructor. Creates empty FV
     */
    /*====================================================================*/
    SparseFV()
            : pLabel(0),
              _uniqueID( MAX_BELIEVABLE_UNIQUE_ID+1),
              pSquareValid(true),
             pSquare(0.),
             _size( 0)
          {};

    /*====================================================================*/
    /*! 
     *   Destructor.
     */
    /*====================================================================*/
    ~SparseFV()
          {};

    /*====================================================================*/
    /*! 
     *   Returns label of FV
     *
     *   \return label
     */
    /*====================================================================*/
    double getLabel() const
          {
            return pLabel;
          };

    /*====================================================================*/
    /*! 
     *   Set FV's label
     *
     *   \param value new label value 
     *
     */
    /*====================================================================*/
    void setLabel(double value)
          {
            pLabel=value;
          };

    void setUniqueID( unsigned int uid)
          {
            _uniqueID = uid;
          }
    
    
    unsigned int uniqueID() const
          {
            return _uniqueID;
          }

    /*====================================================================*/
    /*! 
     *   Clear FV's data
     *
     */
    /*====================================================================*/
    void clear()
          {
            pFeatures.clear();
            pSquareValid=false;
          };
        
    const_iterator begin() const
          {
            return pFeatures.begin();
          };
  
    iterator begin()
          {
            pSquareValid=false;
            return pFeatures.begin();
          };
  
    const_iterator end() const
          {
            return pFeatures.end();
          };
    
    iterator end()
          {
            pSquareValid=false;
            return pFeatures.end();
          };

    /*====================================================================*/
    /*! 
     *   Return const reference to value data item with specified index
     *
     *   \param index
     *
     *   \return value of data[index]
     */
    /*====================================================================*/
    double operator[](size_type index) const
          {
            const_iterator p = begin();
            while( p != end() && p->index() < index)
            { 
              ++p;
            }
            if (p->index() == index)
            {
              return p->value();
            }
            return 0.;
          };
  
    /*====================================================================*/
    /*! 
     *   Return reference to value data item with specified index
     *
     *   \param index
     *
     *   \return reference to value of data[index]
     */
    /*====================================================================*/
    reference operator[]( size_type index)
          {
            pSquareValid=false;
          
            // search insertion position
            iterator p = begin();
            while( p != end() && p->index() < index)
            { 
              ++p;
            }
            if (p != end() && p->index() == index)
            {
              return p->value();
            }
            
            // there is no entry, create one
            iterator newItem = pFeatures.insert(p, svt::SparseItem(index, 0));
            return newItem->value();
            
          };
    
    /*======================================================================*/
    /*! 
     *   resizing sparse vector has no effect on the vector. Just
     *   remebering this value to return it, if someone asks for
     *   size() (just for compatibility to other containters)
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    void resize( size_type s)
          {
            _size = s;
          }
    


    /*======================================================================*/
    /*! 
     *   size of sparse vector is just the value of the last resize()
     *
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    size_type size() const
          {
            return _size;
          }
    

    /*======================================================================*/
    /*! 
     *   cleanup function to delete items with value 0 (maybe
     *   someone needs this) 
     *   \todo optimize
     *   \param 
     *
     *   \exception 
     *
     *   \return
     */
    /*======================================================================*/
    void cleanup()
          {
            bool foundZero=true;
            while (foundZero)
            {
              foundZero=false;
              for (iterator p=begin();
                   p!=end();
                   ++p)
              {
                if (p->value()==0)
                {
                  pFeatures.erase(p);
                  foundZero=true;
                  break;
                }
              }
            }
          }
    

    /*====================================================================*/
    /*! 
     *   Calculate and return square of FV
     *
     *   \return square of FV
     */
    /*====================================================================*/
    double square() const
          {
            if (!pSquareValid)
            {
              pSquare=0.;
              for (const_iterator p=begin();
                   p!=end(); 
                   )
              {
                pSquare+=p->value() * p->value();
                ++p;
              }
            
              pSquareValid=true;
            }

            return pSquare;
          };
  
    /*====================================================================*/
    /*! 
     *   Calculate and return dot product of *this and specified FV
     *
     *   \param fv
     *
     *   \return dot product of *this and fv
     */
    /*====================================================================*/
    double dotProduct(
        const svt::SparseFV& fv) const
          {
            double sum=0.;
          
            const_iterator i=begin();
            const_iterator j=fv.begin();
            for (;
                 i!=end() && j!=fv.end();
                 )
            {
              if (i->index()==j->index())
              {
                sum+=i->value() * j->value();
                ++i;
                ++j;
              }
              else if (i->index()>j->index())
              {
                ++j;
              }
              else
              {
                ++i;
              }
            }

            return sum;
          };

    /*======================================================================*/
    /*! 
     *   comparison
     *
     *   \param fv
     *
     *   \return true or false
     */
    /*======================================================================*/
    bool operator==( const svt::SparseFV& fv) const
          {
            if( pFeatures.size() != fv.pFeatures.size()) return false;
            const_iterator p1 = pFeatures.begin();
            const_iterator p2= fv.pFeatures.begin();
            for( ;  p1 != pFeatures.end();  ++p1, ++p2)
            {
              if( p1->index() != p2->index()
                  || p1->value() != p2->value())
              {
                return false;
              }
            }
            return true;
          }
    
    void operator+=( const svt::SparseFV& fv)
    {
	svt::SparseError err;
	if( pFeatures.size() == fv.pFeatures.size())
	{
	    SparseFV::iterator p1 = pFeatures.begin();
	    SparseFV::const_iterator p2 = fv.begin();
	    for( ;  p1 != pFeatures.end();  ++p1, ++p2)
	    {
		if( p1->index() == p2->index())
		{
		    p1->value() += p2->value();
		}
		else
		{
		    err << "can not calculate with sparse vectors of different shape !\n";
		    throw err;
		}
	    }

	}
	else
	{
	    err<<"can not calculate with sparse vectors of differnt size !/n";
	    throw err;
	}
    }

    void operator*=( double factor)
    {
	SparseFV::iterator p1 = pFeatures.begin();
	for( ;  p1 != pFeatures.end();  ++p1)
	{
	    p1->value() *= factor;
	}
    }

    void setZero()
    {
	SparseFV::iterator p1 = pFeatures.begin();
	for( ;  p1 != pFeatures.end();  ++p1)
	{
	    p1->value() = 0;
	}
    }
    /*====================================================================*/
    /*! 
     *   Read FV from input stream without label
     *
     *   \param is
     *
     */
    /*====================================================================*/
    void readWithoutLabel(
        std::istream& is)
          {
            clear();
            
            while (is.good())
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
              if( !isdigit(c) ) return;
              // read index
              unsigned int index;
              is >> index;
              // read ":"
              is >> c;
              // read value
              double value;
              is >> value;
              
              // add it to fv
              operator[](index) = value;
            }
          };
    
    /*====================================================================*/
    /*! 
     *   Write FV to output stream without label
     *
     *   \param os
     *
     */
    /*====================================================================*/
    void writeWithoutLabel(
        std::ostream& os) const
          {
            for (const_iterator p=begin(); p!=end(); ++p )
            {
              if( p != begin()) os << " ";
              os << p->index() << ":" << p->value();
            }
          };

    static const char* helpPipeFormat()
          {
            return "<label><ws><index>:<feature><ws><index>:<feature>...\n"
                "where <ws> is any number of white spaces except for newline\n"
                "First <index> is 1.\n"
                "example:\n"
                "4 1:0.123 13:2.432 235:42.0";
          }
    
    
    friend std::ostream& operator<<(std::ostream& os, const svt::SparseFV& fv);
    friend std::istream& operator>>(std::istream& is, svt::SparseFV& fv);

  protected:
    std::list<svt::SparseItem> pFeatures;
    double pLabel;
    unsigned int _uniqueID;
    
    mutable bool pSquareValid;
    mutable double pSquare;
    size_type _size;
  };


  inline 
  std::ostream& operator<<(std::ostream& os, const svt::SparseFV& fv)
  {
    os << fv.pLabel << " ";
    fv.writeWithoutLabel( os);
    return os;
  }
  
  inline 
  std::istream& operator>>(std::istream& is, svt::SparseFV& fv)
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
