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
**       Title: kernel wrapper that scales the features
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
**  Revision 1.10  2005/07/19 13:03:59  haasdonk
**  removed redundant cout-messages, erroneous filename and added a new function
**  for computing a training-kernel matrix in BasicSVMAdapter*
**
**  Revision 1.9  2005/06/24 07:23:55  fehr
**  removed static for thread-safety
**
**  Revision 1.8  2005/06/06 21:23:31  haasdonk
**  added updateCache() with two FV-lists, required for classification with precomputed kernel-matrices
**
**  Revision 1.7  2005/03/29 17:56:53  ronneber
**  - added clearCache()
**
**  Revision 1.6  2005/02/11 09:26:53  ronneber
**  - fixed stupid bug in k_function() when chache is no up to date
**
**  Revision 1.5  2004/10/11 17:04:55  ronneber
**  added missing include
**
**  Revision 1.4  2004/09/13 10:12:08  ronneber
**  - corrected some typos in svt_check Macros
**
**  Revision 1.3  2004/09/13 10:04:04  ronneber
**  - documentation update
**
**  Revision 1.2  2004/09/08 14:30:22  ronneber
**  - adapted to new ParamInfo class
**
**  Revision 1.1  2004/09/03 07:11:20  ronneber
**  inisital revison
**
**
**
**************************************************************************/

#ifndef KERNEL_SCALE_HH
#define KERNEL_SCALE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// std includes
#include <map>
#include <set>
#include <string>
#include <vector>

// libsvmtl includes
#include "svm_defines.hh"
#include "SVMError.hh"
#include "ProgressReporter.hh"

// requirements of template parameters
#include "svt_check/RequireHelpers.hh"
#include "svt_check/RequireStData.hh"
#include "svt_check/RequireLoadSaveParameters.hh"
#include "svt_check/RequireNameDescriptionParamInfos.hh"
#include "svt_check/RequireFeatureVectorUniqueID.hh"
#include "svt_check/RequireKernel_k_function.hh"

namespace svt
{

  /*-----------------------------------------------------------------------
   *  Error classes for Kernel_SCALE
   *-----------------------------------------------------------------------*/
  class UnknownScaleAlgorithm : public SVMError {};
  class ScaleComponentsMismatch :  public SVMError {};

  /*======================================================================*/
  /*!
   *  \class Kernel_SCALE
   *  \ingroup kernel
   *  \brief The Kernel_SCALE class provides a wrapper for other
   *         kernel classes , that scales the feature vectors, before
   *         they are passed to the wrapped kernel
   *
   *  For calculation the scale factors, a minmax and a stddev
   *  algorithm is available. The scaled feature vectors are cached
   *  (using their uniqueID()), so there is no performance
   *  drawback. The scale factors and offsets are saved like any
   *  other kernel parameters. So, when using this kernel for
   *  classification the given raw test data is scaled one-the-fly in
   *  the same way as the training data was scaled.
   *
   *  \param KERNEL kernel class that should be wrapped (see \ref
   *         kernel for available kernels). KERNEL must fullfil the
   *         following interfaces
   *         - svt_check::RequireKernel_k_function
   *         - svt_check::RequireLoadSaveParameters
   *         - svt_check::RequireNameDescriptionParamInfos
   */
  /*======================================================================*/
  template< typename KERNEL>
  class Kernel_SCALE
  {
    CHECK_CLASS_TEMPLATE2( svt_check::RequireLoadSaveParameters<KERNEL>)
    CHECK_CLASS_TEMPLATE3( svt_check::RequireNameDescriptionParamInfos<KERNEL>)

  public:
    Kernel_SCALE()
            :_cacheIsUpToDate(false)
          {}


    Kernel_SCALE( const KERNEL& kernel)
            : _kernel(kernel),
              _cacheIsUpToDate(false)
          {}

    /*======================================================================*/
    /*!
     *   set algorithm for data scaling
     *
     *   \param algorithm algortihm as string
     *
     */
    /*======================================================================*/
    void setAlgorithm( const std::string& algorithm)
          {
            _algorithm = algorithm;
          }

    /*======================================================================*/
    /*!
     *   return selected algorithm (default is "")
     *
     *   \return selected algorithm (default is "")
     */
    /*======================================================================*/
    const std::string& algorithm() const
          {
            return _algorithm;
          }

    /*======================================================================*/
    /*!
     *   calc scale factor  and offset.
     *   ATTENTION: ensure that all feature Vectors return the same
     *   size() -- even for SparseFV.
     *
     *   \param fvBegin   iterator to begin of feature vector list
     *   \param fvEnd     iterator to end of  feature vector list
     *   \param accessor accessor for Feature vector. Use
     *                    svt::DirectAccessor (default) if your
     *                    feature vectors are stored directly in the
     *                    container, e.g. std::vector<FV>. Otherwise
     *                    use svt::DereferencingAccessor,
     *                    if your container has only pointers to the
     *                    feature vectors, e.g. std::vector<FV*>
     *
     */
    /*======================================================================*/
    template< typename ForwardIter, typename Accessor>
    void calcScaleAndOffset( const ForwardIter& fvBegin,
                             const ForwardIter& fvEnd,
                             Accessor accessor) const;


    /*======================================================================*/
    /*!
     *   scale single feature vector with internal offset and scale
     *   factor. fvIn and fvOut must already have same sizes and must
     *   both implement size() and operator[]
     *
     *   \param fvIn  input feature vector
     *   \param fvOut (output) scaled feature Vector. fvOut must
     *                already have the same size as fvIn
     */
    /*======================================================================*/
    template< typename FV_IN, typename FV_OUT>
    void scaleSingleFV( const FV_IN& fvIn, FV_OUT& fvOut) const
          {
            for( unsigned int i = 0; i < fvIn.size(); ++i)
            {
              fvOut[i] = (fvIn[i] + _offset[i]) * _scaleFactor[i];
            }
          }


    /*======================================================================*/
    /*!
     *   update internal cache with scaled feature vectors, and (if
     *   algorithm is not empty) updates offsets and scaleFactors
     *   according to dataset and requested algorithm
     *   (e.g. "minmax"). This means: for training data, specify an
     *   algorithm, and for test data setAlgorithm() to empty string,
     *   so that offset and scaleFactors from training data are
     *   reused.
     *
     *   \param fvBegin  iterator to begin of feature vector container
     *   \param fvEnd    iterator to end of feature vector container
     *   \param accessor accessor to acces Feature vector in container
     *                   (e.g., svt::DirectAccessor of
     *                   svt::DereferencingAccessor)
     *
     *   \exception ScaleComponentsMismatch number of scale factors
     *                                      and number of feature
     *                                      vectors components don't
     *                                      match
     */
    /*======================================================================*/
    template< typename ForwardIter, typename Accessor>
    void updateCache( const ForwardIter& fvBegin,
                      const ForwardIter& fvEnd,
                      Accessor accessor,
                      ProgressReporter* pr = 0) const;

    // new updateCache-Syntax:
    template< typename ForwardIter1, typename Accessor1,
              typename ForwardIter2, typename Accessor2 >
    void updateCache( const ForwardIter1& /*fvBegin1*/,
                      const ForwardIter1& /*fvEnd1*/,
                      Accessor1 /*accessor1*/,
                      const ForwardIter2& /*fvBegin2*/,
                      const ForwardIter2& /*fvEnd2*/,
                      Accessor2 /*accessor2*/,
                      ProgressReporter* /*pr*/ = NULL) const
          {
            // nothing to be done or can be implemented later
          }

    void clearCache() const
          {
            _scaledFVsByUID.clear();
            _cacheIsUpToDate = false;

            /*--------------------------------------------------------------
             *  clear cache of underlying kernel
             *--------------------------------------------------------------*/
            _kernel.clearCache();
          }



    /*======================================================================*/
    /*!
     *   kernel function. Passes scaled feature vectors to underlying
     *   Kernel function (that was specified as KERNEL template
     *   parameter). If updateCache() has been called before, the
     *   scaled feature vectors are taken from the cache, otherwise
     *   they will be calculated on the fly
     *
     *   \param x  first feature vector
     *   \param y  second feature vector
     *
     *   \return  result from underlying Kernel function with scaled
     *            feature vectors
     */
    /*======================================================================*/
    template< typename FV>
    double k_function( const FV& x, const FV& y) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireFeatureVectorUniqueID<FV>);
            CHECK_MEMBER_TEMPLATE_2PARAM( svt_check::RequireKernel_k_function<KERNEL,FV>);
            if( _cacheIsUpToDate == false)
            {
              if( x.size() != nComponents() || y.size() != nComponents())
              {
                ScaleComponentsMismatch err;
                err << "number of scale factors (" << nComponents()
                    << ") and number of feature vectors components ("
                    << x.size() << ") don't match. "
                    "Maybe you forgot to specify the scale_algorithm?";

                throw err;
              }

              BasicFV xScaled;
              BasicFV yScaled;
              xScaled.resize( x.size());
              yScaled.resize( y.size());
              scaleSingleFV( x, xScaled);
              scaleSingleFV( y, yScaled);
              xScaled.setUniqueID( x.uniqueID());
              yScaled.setUniqueID( y.uniqueID());
              return _kernel.k_function( xScaled, yScaled);
            }

            unsigned int xUid = x.uniqueID();
            unsigned int yUid = y.uniqueID();

            SVM_ASSERT( xUid < _scaledFVsByUID.size());
            SVM_ASSERT( yUid < _scaledFVsByUID.size());

            return _kernel.k_function( _scaledFVsByUID[xUid],
                                       _scaledFVsByUID[yUid]);
          }


    /*======================================================================*/
    /*!
     *   number of components (features) in scale factor and offset
     *
     *   \return number of components (features) in scale factor and offset
     */
    /*======================================================================*/
    unsigned int nComponents() const
          {
            return static_cast<unsigned int>(_scaleFactor.size());
          }



    /*======================================================================*/
    /*!
     *   return scale factor for the nth component of the feature vector
     *
     *   \return scale factor for the nth component of the feature vector
     */
    /*======================================================================*/
    double scaleFactor( unsigned int index) const
          {
            return _scaleFactor[ index];
          }


    /*======================================================================*/
    /*!
     *   return offset for the nth component of the feature vector
     *
     *   \return offset for the nth component of the feature vector
     */
    /*======================================================================*/
    double offset( unsigned int index) const
          {
             return _offset[ index];
          }

    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            if( stData.valueExists( "scale_factor"))
            {
              _scaleFactor.resize( stData.getArraySize( "scale_factor"));
              stData.getArray( "scale_factor", _scaleFactor.begin(),
                               static_cast<int>(_scaleFactor.size()));
              _offset.resize( stData.getArraySize( "scale_offset"));
              stData.getArray( "scale_offset", _offset.begin(),
                               static_cast<int>(_offset.size()));
              if( _scaleFactor.size() != _offset.size())
              {
                LoadError err;
                err << "sizes of 'scale_factor' array ("
                    << _scaleFactor.size() << ") and 'scale_offset' "
                    "array (" << _offset.size() << ") mismatch.";
                throw err;
              }
            }
            stData.getValue( "scale_algorithm", _algorithm);
            _kernel.loadParameters( stData);
          }

    /*======================================================================*/
    /*!
     *   save scale_factor and scale_offset.
     *   ATTENTION: the algorithm is saved to 'scale_used_algorithm',
     *   which is another key as in loadParameters (there it expects
     *   the algorithm in 'scale_algorithm').
     *   This ensures, that when parameters are loaded from a model,
     *   the _algorithm keeps empty and therefore the scale_offset and
     *   scale_factor from training data set are reused as it should
     *   be.
     *
     *   \param stData structured data, where to save the parameters
     *
     */
    /*======================================================================*/
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            _kernel.saveParameters( stData);
            if( _algorithm != "")
            {
              stData.setValue( "scale_used_algorithm", _algorithm);
            }
            if( _scaleFactor.size() != 0)
            {
              stData.setArray( "scale_factor", _scaleFactor.begin(),
                               _scaleFactor.size());
              stData.setArray( "scale_offset", _offset.begin(),
                               _offset.size());
            }


          }

      static std::string name()
          {
            return std::string( "scaled_") + KERNEL::name();
          }

    static std::string description()
          {
            return std::string( "scaled feature vectors passed to ") + KERNEL::description();
          }

    /*======================================================================*/
    /*!
     *   get information about the parameters, that are used in
     *   loadParameters() and saveParameters(). The Infos are appended
     *   to the passed array
     *
     *   \param p  array of ParamInfos. The Infos will be appended to
     *             this array
     */
    /*======================================================================*/
    static void getParamInfos( std::vector<ParamInfo>& p)
          {
            KERNEL::getParamInfos( p);
            p.push_back(
                ParamInfo( "scale_algorithm", "sa"));
            p.back().addAlternative( "minmax",
                                     "scale each feature that min becomes -1 "
                                     "and max becomes +1");
            p.back().addAlternative( "stddev",
                                     "scale each feature that mean becomes 0 "
                                     "and standard deviation becomes 1");
            p.push_back(
                ParamInfo( "scale_factor", "sf", "array",
                           "array containing scale factors for each "
                           "feature -- usually you don't want to specify "
                           "this manually"));
            p.push_back(
                ParamInfo( "scale_offset", "so", "array",
                           "array containing offsets for each feature "
                           "-- usually you don't want to specify this "
                           "manually"));
          }

  private:
    KERNEL                       _kernel;
    std::string                  _algorithm;
    mutable std::vector<double>  _offset;
    mutable std::vector<double>  _scaleFactor;
    mutable bool                 _cacheIsUpToDate;
    mutable std::vector<BasicFV> _scaledFVsByUID;

  };

}

#include "Kernel_SCALE.icc"

#endif
