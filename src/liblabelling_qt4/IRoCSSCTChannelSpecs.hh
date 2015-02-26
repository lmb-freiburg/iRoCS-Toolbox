/**************************************************************************
**       Title: Channel Specs for iRoCS coordinate system visualization with
**              elliptic root profiles
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Channel Descriptor for one channel containing a continuous iRoCS model
**    with elliptic root profile as used in the cell-segmentation pipeline
**
**************************************************************************/

#ifndef IROCSSCTCHANNELSPECS_HH
#define IROCSSCTCHANNELSPECS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecs.hh"

#include <libArrayToolbox/algo/ShellCoordinateTransform.hh>

class ColorControlElement;

class IRoCSSCTChannelSpecs : public ChannelSpecs
{

  Q_OBJECT

public:
  
  IRoCSSCTChannelSpecs(
      ShellCoordinateTransform *sct, MultiChannelModel *model = NULL);
  ~IRoCSSCTChannelSpecs();
  
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

  ShellCoordinateTransform *sct();

public slots:
  
  void setColor(blitz::TinyVector<float,3> const &color);
  void updateChannelColor();

private:
  
  QColor qColor();

  ColorControlElement *p_colorControl;

  ShellCoordinateTransform *p_sct;

  friend class IRoCSSCTChannelSpecsOrthoViewRenderer;

};
    
#endif
