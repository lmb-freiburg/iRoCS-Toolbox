/**************************************************************************
**       Title: Channel Specs for iRoCS coordinate system visualization
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Channel Descriptor for one channel containing a continuous iRoCS model
**
**************************************************************************/

#ifndef IROCSCHANNELSPECS_HH
#define IROCSCHANNELSPECS_HH

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
