/**************************************************************************
 *
 * This file belongs to the iRoCS Toolbox.
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
