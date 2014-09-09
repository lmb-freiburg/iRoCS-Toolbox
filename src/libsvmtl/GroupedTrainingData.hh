/**************************************************************************
**       Title: Container for grouped training data for Multiclass SVM's
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
**  Revision 1.5  2005/03/29 17:52:50  ronneber
**  - added FV_begin() and FV_end()
**
**  Revision 1.4  2004/09/13 10:02:25  ronneber
**  - documentation update
**
**  Revision 1.3  2004/09/03 07:08:20  ronneber
**  - now using Accessor in create() arguments
**
**  Revision 1.2  2004/09/01 14:43:36  ronneber
**  changed IterToPointerTraits stuff to
**  DirectAccessor and DereferencingAccessor, to make code more
**  intuitive understandable
**
**  Revision 1.1  2004/08/26 08:36:58  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef GROUPEDTRAININGDATA
#define GROUPEDTRAININGDATA

#include <algorithm>

#include "SVMError.hh"
#include "SVM_Problem.hh"

// requirements of template parameters
#include "svt_check/RequireFeatureVectorUniqueID.hh"

namespace svt
{
  
  /*======================================================================*/
  /*!
   *  \class GroupedTrainingData
   *  \ingroup lowlevel
   *  \brief The GroupedTrainingData class is a container for feature
   *         vectors. They are grouped internally by their label   
   *
   *  The grouping is done by a std::stable_sort, which ensures
   *  maximal performance for allready correct grouped data. The order
   *  within one groups is preserved.
   * 
   *  This class is used in multi-class SVM Algorithms and improves
   *  performance if a multi-class classifier is called multiple times
   *  (e.g. during a cross validation)
   *
   *  \param FV a feature vector class (e.g. BasicFV, SparseFV, or
   *            your own class)
   */
  /*======================================================================*/
  template<typename FV>
  class GroupedTrainingData
  {
    CHECK_CLASS_TEMPLATE1( svt_check::RequireFeatureVectorUniqueID<FV>)
  public:
    GroupedTrainingData()
            :_labelsStoredInExtraArray( false)
          {}
    
    
    GroupedTrainingData(const SVM_Problem<FV>& problem)
            :_labelsStoredInExtraArray( false)
          {
            create( problem);
          }
    
    template< typename ForwardIter, typename Accessor>
    GroupedTrainingData( ForwardIter FV_begin,  const ForwardIter& FV_end,
                         Accessor accessor)
            :_labelsStoredInExtraArray( false)
          {
            create( FV_begin, FV_end, accessor);
          }
    
    void create( const SVM_Problem<FV>& problem);
    
    template< typename ForwardIter, typename Accessor>
    void create( ForwardIter FV_begin,  const ForwardIter& FV_end,
                 Accessor accessor);
    
    unsigned int nClasses() const
          {
            return static_cast<unsigned int>(_classIndexToLabel.size());
          }
    
    double classIndexToLabel( int classIndex) const
          {
            return _classIndexToLabel[classIndex];
          }
    
    const std::vector<double>& classIndizesToLabels() const
          {
            return _classIndexToLabel;
          }
    
    unsigned int classStartIndex( int classIndex) const
          {
            return _classStartIndizes[classIndex];
          }
    
    size_t nFeatureVectors() const
          {
            return _featureVectors.size();
          }
    
    
    
    FV* featureVector( unsigned int i) const 
          {
            return _featureVectors[i];
          }
    
    
    double label( unsigned int fvIndex) const
          {
            if( _labelsStoredInExtraArray)
            {
              SVM_ASSERT( fvIndex < _labels.size());
              return _labels[fvIndex];
            }
            else
            {
              SVM_ASSERT( fvIndex < _featureVectors.size());
              return _featureVectors[fvIndex]->getLabel();
            }
          }
    
    
    std::vector<FV*>& allFeatureVectors()
          {
            return _featureVectors;
          }
    
    const std::vector<FV*>& allFeatureVectors() const
          {
            return _featureVectors;
          }
    
    typename std::vector<FV*>::const_iterator FV_begin() const
          {
            return _featureVectors.begin();
          }
    
    typename std::vector<FV*>::const_iterator FV_end() const
          {
            return _featureVectors.end();
          }
    
    
  private:
    std::vector<FV*>           _featureVectors;
    std::vector<unsigned int>  _classStartIndizes;
    std::vector<double>        _classIndexToLabel;
    bool                       _labelsStoredInExtraArray;
    std::vector<double>        _labels;
    
  };
  
}

#include "GroupedTrainingData.icc"

#endif
