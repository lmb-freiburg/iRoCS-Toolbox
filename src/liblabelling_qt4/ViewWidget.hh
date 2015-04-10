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

#ifndef VIEWWIDGET_HH
#define VIEWWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
