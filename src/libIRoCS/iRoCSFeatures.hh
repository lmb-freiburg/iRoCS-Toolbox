/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
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

#ifndef IROCSFEATURES_HH
#define IROCSFEATURES_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <libArrayToolbox/ATBDataSynthesis.hh>
#include <libArrayToolbox/SeparableConvolutionFilter.hh>
#include <libArrayToolbox/LaplacianFilter.hh>
#include <libArrayToolbox/HoughTransform.hh>
#include <libArrayToolbox/Normalization.hh>
#include <libArrayToolbox/SphericalTensor.hh>
#include <libArrayToolbox/iRoCS.hh>

#include <liblabelling_qt4/AnnotationChannelSpecs.hh>

#include <libsvmtl/BasicFV.hh>

#define DEBUG_MEMORY

namespace iRoCS
{

  class Features 
  {

  public:
  
#ifdef DEBUG_MEMORY
  struct __Memblock
  {
    std::string allocCodeLine;
    std::string command;
  };
  std::map<void*,__Memblock> __allocatedMemblocks;
#endif

    enum NormalizationType
    {
        None = 0x0000,
        FeatureZeroMeanStddev = 0x0001,
        UnitNorm = 0x0002,
        Combined = 0x0003,
        Invalid = 0xffff
    };

    static std::string normalizationTypeToString(NormalizationType type);
    static NormalizationType stringToNormalizationType(
        std::string const &normTypeString);

    static const int PositiveMagnitude;
    static const int NegativeMagnitude;
    static const int PositiveRadius;
    static const int NegativeRadius;

    Features(
        blitz::TinyVector<double,3> const &featureElementSizeUm = 1.0,
        iRoCS::ProgressReporter *progress = NULL);
    ~Features();

    blitz::TinyVector<double,3> const &elementSizeUm() const;

/*======================================================================*/
/*! 
 *   Adds a new feature name to the specified group. If the group does 
 *   not exist it is created on the fly. On group creation its normalization
 *   flag will be set to no normalization. The mean and stddev Arrays
 *   will be left unchanged! This is necessary for the normalizeFeatures()
 *   method which decides based on the length of those Arrays whether
 *   the feature normalization parameters are loaded or have to be computed.
 *
 *   \param groupName   The name of the group to append a feature to
 *   \param featureName The name of the feature to append
 */
/*======================================================================*/
    void addFeatureToGroup(
        std::string const &groupName, std::string const &featureName);

    void setGroupNormalization(
        std::string const &groupName, NormalizationType normType);

    template<typename DataT>
    atb::Array<double,3>& dataScaled(
        atb::Array<DataT,3> const &data, std::string const &cacheFileName);

    std::string sdFeatureName(atb::SDMagFeatureIndex const &index) const;

    template<typename DataT>
    atb::Array<double,3>& sdFeature(
        atb::Array<DataT,3> const &data, atb::SDMagFeatureIndex const &index,
        const int maxBand, std::string const &cacheFileName);

    std::string houghFeatureName(const int state) const;

    template<typename DataT>
    atb::Array<double,3>& houghFeature(
        atb::Array<DataT,3> const &data, const int state,
        std::string const &cacheFileName);

    template<typename DataT>
    atb::Array<blitz::TinyVector<double,3>,3>& intrinsicCoordinates(
        atb::Array<DataT,3> const &data, atb::IRoCS const &rct,
        std::string const &cacheFileName);

    void deleteFeature(atb::SDMagFeatureIndex const &index);

    void deleteFeature(const int state);

    void generateRandomSamples(
        std::vector< blitz::TinyVector<double,3> > &markers,
        blitz::TinyVector<double,3> const &upperBoundUm,
        int nInRootSamples, int nOutRootSamples);

    void normalizeFeatures(std::vector<svt::BasicFV> &samples);

    void saveNormalizationParameters(std::string const &modelFileName);

    void loadNormalizationParameters(std::string const &modelFileName);

    void trainTwoClassSVM(
        std::vector<svt::BasicFV> &trainVectors,
        std::string const &modelFileName,
        float cost, float gamma);

    void classifyTwoClassSVM(
        std::vector<svt::BasicFV>& testVectors,
        std::string const &modelFileName);
    
    void trainMultiClassSVM(
        std::vector<svt::BasicFV> &trainVectors,
        std::string const &modelFileName,
        float cost, float gamma);

    void classifyMultiClassSVM(
        std::vector<svt::BasicFV>& testVectors,
        std::string const &modelFileName);

    static std::string h5GroupName(const std::string& rawGroup);

  private:
  
    iRoCS::ProgressReporter *p_progress;

    std::map<int,std::string> _houghDsNames;

    atb::Array<double,3> _dataScaled;
    std::map<atb::SDMagFeatureIndex,atb::Array<double,3>*> _sdFeatures;
    std::map<int, atb::Array<double,3>*> _houghFeatures;
    atb::Array<blitz::TinyVector<double,3>,3> _intrinsicCoordinates;

    std::string _featureBaseGroup;
    std::vector<std::string> _featureGroups;
    std::vector<NormalizationType> _normalize;
    std::vector<std::vector<std::string>*> _featureNames;
    std::vector<std::vector<double>*> _means, _stddevs;

  };

}

#include "iRoCSFeatures.icc"

#endif
