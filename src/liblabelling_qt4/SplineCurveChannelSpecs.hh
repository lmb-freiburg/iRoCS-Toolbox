/**************************************************************************
**       Title: Channel Specs for Spline curve visualization and editing
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Channel Descriptor for one channel containing an editable spline
**    curve
**
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
