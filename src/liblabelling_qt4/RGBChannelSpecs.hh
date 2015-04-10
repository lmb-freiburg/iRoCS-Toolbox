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

#ifndef RGBCHANNELSPECS_HH
#define RGBCHANNELSPECS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
