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

#ifndef DATACHANNELSPECS_HH
#define DATACHANNELSPECS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
