/**************************************************************************
**       Title: Multi-Channel Model
**    $RCSfile$
**   $Revision: 3883 $ $Name$
**       $Date: 2008-01-21 14:28:13 +0100 (Mon, 21 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    This class holds all channel data
**
**************************************************************************/

#ifndef MULTICHANNELMODEL_HH
#define MULTICHANNELMODEL_HH

#include <QtCore/QObject>

#include <libArrayToolbox/Array.hh>
#include <libArrayToolbox/ATBSpline.hh>
#include <libArrayToolbox/iRoCS.hh>
#include <libArrayToolbox/algo/ShellCoordinateTransform.hh>

#include "Marker.hh"
#include "ChannelSpecs.hh"

class QWidget;
class QComboBox;
class QStackedWidget;
class QLineEdit;
class QToolButton;

class DataChannelSpecs;
class RGBChannelSpecs;
class AnnotationChannelSpecs;
class VisualizationChannelSpecs;
class SplineCurveChannelSpecs;
class IRoCSChannelSpecs;
class IRoCSSCTChannelSpecs;

class ViewWidget;

class ColorMapEditorWidget;
class ChannelSelectionControlElement;

class MultiChannelModel : public QObject
{

Q_OBJECT

public:

  typedef std::vector<ChannelSpecs*>::iterator iterator;

  MultiChannelModel();
  ~MultiChannelModel();

  blitz::TinyVector<double,3> const &lowerBoundUm() const;
  blitz::TinyVector<double,3> const &upperBoundUm() const;
  blitz::TinyVector<double,3> boundingBoxShapeUm() const;
  blitz::TinyVector<double,3> const &elementSizeUm() const;
  void setElementSizeUm(blitz::TinyVector<double,3> const &elementSizeUm);

  QWidget* channelEditorWidget() const;
  QWidget* infoWidget() const;

  DataChannelSpecs* addDataChannel(atb::Array<float,3>* channel);
  RGBChannelSpecs* addRGBChannel(
      atb::Array<blitz::TinyVector<float,3>,3>* channel);
  VisualizationChannelSpecs *addVisualizationChannel(
      atb::Array<int,3> *channel = NULL);
  AnnotationChannelSpecs* addAnnotationChannel(
      Marker::MarkerType markerType = Marker::Point);
  SplineCurveChannelSpecs* addSplineCurveChannel(
      atb::BSpline< blitz::TinyVector<double,3> > *spline);
  IRoCSChannelSpecs* addIRoCSChannel(atb::IRoCS *rct);
  IRoCSSCTChannelSpecs* addIRoCSSCTChannel(ShellCoordinateTransform *sct);
  ChannelSpecs *addChannel(ChannelSpecs *channel);
  void removeChannel(ChannelSpecs* channel, bool freeMemory = true);
  void removeChannel(size_t index);
  void removeChannel(const std::string& name);
  
  size_t nChannels(
      ChannelSpecs::ChannelTypes types = ChannelSpecs::Any) const;
  
  ChannelSpecs* selectedChannel();

  ChannelSpecs* channel(size_t index);
  ChannelSpecs* channel(const std::string& name);
  iterator begin();
  iterator end();

  void addView(ViewWidget* view);
  void removeView(ViewWidget* view);
  std::vector<ViewWidget*>::iterator viewsBegin();
  std::vector<ViewWidget*>::iterator viewsEnd();

  void updateInfoWidget(const blitz::TinyVector<double,3>& positionUm);

  bool modified();
  void setModified(bool modified);

signals:
        
  void modifiedFlagChanged(bool);
  void isEmpty(bool);
  void containsAnnotationsChanged(bool);
  void boundingBoxChanged(
      blitz::TinyVector<double,3> const &, blitz::TinyVector<double,3> const &);
  void elementSizeChanged(blitz::TinyVector<double,3> const &);
  void channelAdded(ChannelSpecs *);
  void channelNamesOrIconsChanged();
  // Warning: The passed pointer is already released!!! Only use it as unique
  // channel identifier to remove associated ressources
  void channelRemoved(ChannelSpecs *);
  void channelSelectionChanged(int);
  void annotationChannelSelected(bool);

public slots:
   
  void setBoundingBoxUm(
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm);
  void setActiveChannel(int index);
  void deleteActiveChannel();

private slots:

  void setActiveChannelName();
  void updateViews();

private:

  void setLowerBoundUm(blitz::TinyVector<double,3> const &lowerBoundUm);
  void setUpperBoundUm(blitz::TinyVector<double,3> const &upperBoundUm);
  std::string newChannelName();
  bool channelNamesUnique();
  void addChannelSpecs(ChannelSpecs* specs);

  std::vector<ChannelSpecs*> _channels;
  std::vector<ViewWidget*> _views;

  blitz::TinyVector<double,3> _lowerBoundUm, _upperBoundUm;
  blitz::TinyVector<double,3> _elementSizeUm;

  QWidget* p_channelEditorWidget;
  ChannelSelectionControlElement *p_channelSelectionControl;
  QStackedWidget* p_channelControlStack;
  QToolButton* p_deleteChannelButton;

  QWidget* p_infoWidget;
  QLineEdit* p_positionVxXLineEdit;
  QLineEdit* p_positionVxYLineEdit;
  QLineEdit* p_positionVxZLineEdit;
  QLineEdit* p_positionUmXLineEdit;
  QLineEdit* p_positionUmYLineEdit;
  QLineEdit* p_positionUmZLineEdit;
  QLineEdit* p_valueLineEdit;

  bool _modified;
};

#endif
