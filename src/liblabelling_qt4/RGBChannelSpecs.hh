/**************************************************************************
**       Title: Channel Specs for RGB data
**    $RCSfile$
**   $Revision: 3896 $ $Name$
**       $Date: 2008-07-16 15:17:47 +0200 (Wed, 16 Jul 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Channel Descriptor for one channel of multi-channel RGB data
**
**************************************************************************/

#ifndef RGBCHANNELSPECS_HH
#define RGBCHANNELSPECS_HH

#include "ChannelSpecs.hh"

#include <libArrayToolbox/Array.hh>

class RGBChannelSpecs : public ChannelSpecs
{

  Q_OBJECT

public:
  
  RGBChannelSpecs(atb::Array<blitz::TinyVector<float,3>,3> *data,
                  MultiChannelModel *model = NULL);
  ~RGBChannelSpecs();
  
  ChannelSpecs::ChannelType channelType() const;

  static RGBChannelSpecs *load();

  float gamma() const;
  void setGamma(float gamma);

  bool normalize() const;
  void setNormalize(bool normalize);
  float minValue() const;
  float maxValue() const;
  float gammaLUT(size_t index) const;

  bool showExposureProblems() const;
  void setShowExposureProblems(bool exposure);

  atb::Array<blitz::TinyVector<float,3>,3> const *data() const;
  atb::Array<blitz::TinyVector<float,3>,3> *data();
  float valueAt(blitz::TinyVector<double,3> const &positionUm) const;
  std::string stringValueAt(
      blitz::TinyVector<double,3> const &positionUm) const;
  
  ChannelSpecsRenderer *addRenderer(ViewWidget *view);

public slots:
  
  void setTransformation(
      blitz::TinyMatrix<double,4,4> const &transformation);

/*======================================================================*/
/*! 
 *   Reset the channel shape, element size, transformation and bounding box
 *   from the underlying data. This method must be called whenever the
 *   underlying data were programmatically changed using the direct data
 *   access methods. If updates are enabled it will also do cache updates.
 */
/*======================================================================*/
  void revalidate();

private slots:

  void updateNorm(bool normalize);
  void updateGammaLUT();

private:

  atb::Array<blitz::TinyVector<float,3>,3> *p_data;

  float _minValue, _maxValue;
  std::vector<float> _gammaLUT;

  DoubleControlElement *p_gammaControl;
  BoolControlElement *p_normalizeControl;
  BoolControlElement *p_showExposureProblemsControl;
  
};
    
#endif
