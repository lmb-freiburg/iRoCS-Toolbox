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
**************************************************************************/

#ifndef VISUALIZATIONCHANNELSPECS_HH
#define VISUALIZATIONCHANNELSPECS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecs.hh"

#include "../libArrayToolbox/Array.hh"

class ColorMap;
class ColorMapEditorWidget;

class QComboBox;

class VisualizationChannelSpecs : public ChannelSpecs
{

  Q_OBJECT

public:
  
  VisualizationChannelSpecs(
      atb::Array<int,3> *data, MultiChannelModel *model = NULL,
      bool sign = false, int bitDepth = 16);
  ~VisualizationChannelSpecs();
  
  ChannelSpecs::ChannelType channelType() const;

  static VisualizationChannelSpecs *load();

  atb::Array<int,3> const *data() const;
  atb::Array<int,3> *data();
  float valueAt(blitz::TinyVector<double,3> const &positionUm) const;
  std::string stringValueAt(
      blitz::TinyVector<double,3> const &positionUm) const;
  int& labelAt(blitz::TinyVector<double,3> const &positionUm);
  int& labelAtVoxel(blitz::TinyVector<ptrdiff_t,3> const &pos);
  
  ColorMap const &colorMap() const;
  ColorMap &colorMap();

  void setSign(bool sign);
  bool sign() const;

  void setBitDepth(int bitDepth);
  int bitDepth() const;

  void hasNewData();
  
  ChannelSpecsRenderer *addRenderer(ViewWidget *view);

public slots:

  void normalizeIndexRange();
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

private:

  atb::Array<int,3> *p_data;
  bool _sign;
  int _bitDepth;

  ColorMap *p_colorMap;

  ColorMapEditorWidget *p_colorMapEditor;
  
};
    
#endif
