/**************************************************************************
 **       Title: 2-D color viewer widget
 **    $RCSfile$
 **   $Revision: 3880 $ $Name$
 **       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
 **   Copyright: GPL $Author: tschmidt $
 ** Description:
 **
 **    Widget for displaying 2-D RGB data
 **
 **************************************************************************/

#ifndef ORTHOVIEWPLANE_HH
#define ORTHOVIEWPLANE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QWidget>

#include <blitz/array.h>

#include <map>
#include <list>

#include <libProgressReporter/ProgressReporter.hh>

class QWheelEvent;
class QMouseEvent;
class QXmlStreamWriter;

class OrthoViewWidget;
class OrthoViewOverlay;
class ChannelSpecs;
class ChannelSpecsOrthoViewRenderer;

class OrthoViewPlane : public QWidget 
{

  Q_OBJECT

  public:
  
  OrthoViewPlane(
      OrthoViewWidget* orthoView, int orthogonalDimension,
      QWidget* parent = NULL);
  ~OrthoViewPlane();
  
  void addOverlay(OrthoViewOverlay* overlay);
  void removeOverlay(OrthoViewOverlay* overlay);
  blitz::TinyVector<double,3> mousePositionUm(int x, int y) const;
  blitz::TinyVector<double,3> mousePositionPx(int x, int y) const;

  OrthoViewWidget *orthoView();
  blitz::TinyVector<int,2> const &dimensions() const;
  int orthogonalDimension() const;
  std::string viewId() const;

  void paint(QPainter &painter);
  void paint(
      QXmlStreamWriter &svgStream,
      blitz::TinyVector<double,3> const &shapePx,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      std::string const &font, double fontSizePt,
      iRoCS::ProgressReporter *pr = NULL);

public slots:
  
  void adjustSize();

/*======================================================================*/
/*! 
 *   Repaint the RGB Array containing the fused information of all Data, 
 *   Visualization and RGBChannels. This must be called before a view update,
 *   if the cache of one of those channels changed.
 */
/*======================================================================*/
  void updateData();

private:
  
  void paintEvent(QPaintEvent*);
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void wheelEvent(QWheelEvent* e);

  OrthoViewWidget* p_orthoView;
  blitz::Array<blitz::TinyVector<unsigned char,4>,2> _data;
  blitz::TinyVector<int,2> _dimensions;
  int _orthogonalDimension;

  bool _dragVertical, _dragHorizontal, _isDragging;

  std::list<OrthoViewOverlay*> _overlays;

};

#endif
