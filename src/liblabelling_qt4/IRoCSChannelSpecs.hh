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

#ifndef IROCSCHANNELSPECS_HH
#define IROCSCHANNELSPECS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecs.hh"

#include <libArrayToolbox/iRoCS.hh>

class ColorControlElement;

class IRoCSChannelSpecs : public ChannelSpecs
{

  Q_OBJECT

public:
  
  IRoCSChannelSpecs(atb::IRoCS *rct, MultiChannelModel *model = NULL);
  ~IRoCSChannelSpecs();
  
  ChannelSpecs::ChannelType channelType() const;

  blitz::TinyVector<float,3> color() const;

  float valueAt(blitz::TinyVector<double,3> const &positionUm) const;
  std::string stringValueAt(
      blitz::TinyVector<double,3> const &positionUm) const;

  ChannelSpecsRenderer *addRenderer(ViewWidget *view);

  blitz::TinyVector<double,3> const &lowerBoundUm() const;
  blitz::TinyVector<double,3> const &upperBoundUm() const;
  blitz::TinyVector<double,3> const &elementSizeUm() const;

  void load(const std::string &fileName, const std::string &group);
  void save(const std::string &fileName, const std::string &group);

  atb::IRoCS *rct();

public slots:
  
  void setColor(blitz::TinyVector<float,3> const &color);
  void updateChannelColor();

private:
  
  QColor qColor();

  ColorControlElement *p_colorControl;

  atb::IRoCS *p_rct;

  friend class IRoCSChannelSpecsOrthoViewRenderer;

};
    
#endif
