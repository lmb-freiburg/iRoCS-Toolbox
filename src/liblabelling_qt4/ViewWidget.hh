/**************************************************************************
**       Title: Abstract View Widget
**    $RCSfile$
**   $Revision: 3880 $ $Name$
**       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    Abstract view widget containing the common interface for all views
**
**************************************************************************/

#ifndef VIEWWIDGET_HH
#define VIEWWIDGET_HH

#include <QtGui/QWidget>

#include <blitz/array.h>
#include <map>

class MultiChannelModel;
class ChannelSpecs;
class ChannelSpecsRenderer;

class ViewWidget : public QWidget
{

Q_OBJECT

public:
  
  enum ViewType { OrthoView, OpenGlRenderingView };

  ViewWidget(MultiChannelModel* model, QWidget* parent = 0,
             Qt::WindowFlags f = 0);
  virtual ~ViewWidget();

  virtual ViewWidget::ViewType viewType() const = 0;
  virtual void addChannel(ChannelSpecs* channel) = 0;
  virtual void removeChannel(ChannelSpecs* channel) = 0;

  bool snapToVoxelGrid() const;
  bool showHelperLines() const;
  blitz::TinyVector<double,3> positionUm() const;

  MultiChannelModel* model() const;

signals:

  void positionChanged(const blitz::TinyVector<double,3>& positionUm);

public slots:

/*======================================================================*/
/*! 
 *   This function can be implemented by ViewWidget instances to
 *   send update requests to the actual low level views. It is called whenever
 *   the model changes in a way that may require a view update. The default
 *   implementation just calls update().
 */
/*======================================================================*/
  virtual void redraw();
  virtual void setSnapToVoxelGrid(bool snap);
  virtual void setShowHelperLines(bool show);
  virtual void setPositionUm(const blitz::TinyVector<double,3>& positionUm);

protected:
  
  MultiChannelModel* p_model;

private:
  
  bool _snapToVoxelGrid, _showHelperLines;
  blitz::TinyVector<double,3> _positionUm;

};

#endif
