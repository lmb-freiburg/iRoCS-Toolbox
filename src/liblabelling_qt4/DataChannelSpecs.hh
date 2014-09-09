/**************************************************************************
**       Title: Channel Specs for gray value data
**    $RCSfile$
**   $Revision: 3896 $ $Name$
**       $Date: 2008-07-16 15:17:47 +0200 (Wed, 16 Jul 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Channel Descriptor for one channel of multi-channel gray value data
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2008/01/17 17:05:50  tschmidt
**  - Initial revision
**
**
**************************************************************************/

#ifndef DATACHANNELSPECS_HH
#define DATACHANNELSPECS_HH

#include "ChannelSpecs.hh"

#include <libArrayToolbox/Array.hh>

class ColorControlElement;
class DoubleRangeControlElement;
class QPushButton;

class DataChannelSpecs : public ChannelSpecs
{

  Q_OBJECT

public:
  
  DataChannelSpecs(atb::Array<float,3> *data, MultiChannelModel *model = NULL);
  ~DataChannelSpecs();
  
  ChannelSpecs::ChannelType channelType() const;

  static DataChannelSpecs *load();

  blitz::TinyVector<float,3> color() const;
  void setColor(blitz::TinyVector<float,3> const &color);
  
  float gamma() const;
  void setGamma(float gamma);

  float displayMin() const;
  float displayMax() const;

  float gammaLUT(size_t index) const;

  bool showExposureProblems() const;
  void setShowExposureProblems(bool exposure);

  atb::Array<float,3> const *data() const;
  atb::Array<float,3> *data();
  float valueAt(blitz::TinyVector<double,3> const &positionUm) const;
  std::string stringValueAt(
      blitz::TinyVector<double,3> const &positionUm) const;

  ChannelSpecsRenderer *addRenderer(ViewWidget *view);

public slots:
        
  void normalize();
  void setDisplayRange(float displayMin, float displayMax);
  void setDisplayMin(float displayMin);
  void setDisplayMax(float displayMax);
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

  void updateIcon();
  void updateGammaLUT();

private:

  atb::Array<float,3> *p_data;

  std::vector<float> _gammaLUT;

  ColorControlElement *p_colorControl;
  DoubleControlElement *p_gammaControl;
  QPushButton *p_normalizeButton;
  DoubleRangeControlElement *p_displayRangeControl;
  BoolControlElement *p_showExposureProblemsControl;
  
};
    
#endif
