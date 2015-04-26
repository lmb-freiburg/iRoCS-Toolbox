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

#ifndef SPLINECURVECHANNELSPECS_HH
#define SPLINECURVECHANNELSPECS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecs.hh"

#include <libArrayToolbox/ATBSpline.hh>

class QColorDialog;

class Double3ControlElement;

class SplineCurveChannelSpecs : public ChannelSpecs
{

  Q_OBJECT

public:
  
  SplineCurveChannelSpecs(atb::BSpline< blitz::TinyVector<double,3> > *spline,
                          MultiChannelModel *model = NULL);
  ~SplineCurveChannelSpecs();
  
  ChannelSpecs::ChannelType channelType() const;

  blitz::TinyVector<float,3> color() const;

  float valueAt(blitz::TinyVector<double,3> const &positionUm) const;
  std::string stringValueAt(
      blitz::TinyVector<double,3> const &positionUm) const;

  ChannelSpecsRenderer *addRenderer(ViewWidget *view);

  blitz::TinyVector<double,3> const &lowerBoundUm() const;
  blitz::TinyVector<double,3> const &upperBoundUm() const;
  blitz::TinyVector<double,3> const &elementSizeUm() const;

  void load(const std::string &fileName, const std::string &group,
            bool throwErrors = false);
  void save(const std::string &fileName, const std::string &group,
            bool throwErrors = false);

  atb::BSpline< blitz::TinyVector<double,3> > const *spline() const;
                                                                   
public slots:
  
  void setColor(blitz::TinyVector<float,3> const &color);
  void updateChannelColor();
  void chooseColor();

private:
  
  void init();
  QColor qColor();

  Double3ControlElement *p_channelColor;

  atb::BSpline< blitz::TinyVector<double,3> > *p_spline;

  friend class SplineCurveChannelSpecsOrthoViewRenderer;

};
    
#endif
