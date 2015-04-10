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

#include "ViewWidget.hh"

#include "ChannelSpecsRenderer.hh"
#include "MultiChannelModel.hh"

ViewWidget::ViewWidget(MultiChannelModel* model, QWidget* parent,
                       Qt::WindowFlags f)
        : QWidget(parent, f), p_model(model), _snapToVoxelGrid(false),
          _showHelperLines(false), _positionUm(0.0f)
{
  connect(this, SIGNAL(positionChanged(const blitz::TinyVector<double,3>&)),
          SLOT(update()));
}

ViewWidget::~ViewWidget() 
{}

bool ViewWidget::snapToVoxelGrid() const
{
  return _snapToVoxelGrid;
}

bool ViewWidget::showHelperLines() const
{
  return _showHelperLines;
}

blitz::TinyVector<double,3> ViewWidget::positionUm() const
{
  return _positionUm;
}

MultiChannelModel* ViewWidget::model() const
{
  return p_model;
}

void ViewWidget::redraw()
{
  update();
}

void ViewWidget::setSnapToVoxelGrid(bool snap)
{
  _snapToVoxelGrid = snap;
}

void ViewWidget::setShowHelperLines(bool show)
{
  _showHelperLines = show;
}

void ViewWidget::setPositionUm(blitz::TinyVector<double,3> const &positionUm)
{
  blitz::TinyVector<double,3> newPos(positionUm);

  // Clip position to model bounding box
  for (int d = 0; d < 3; ++d)
  {
    if (newPos(d) < p_model->lowerBoundUm()(d))
        newPos(d) = p_model->lowerBoundUm()(d);
    if (newPos(d) > p_model->upperBoundUm()(d))
        newPos(d) = p_model->upperBoundUm()(d);
  }

  if (_snapToVoxelGrid)
  {
    // Move to closest voxel position
    if (blitz::all(p_model->elementSizeUm() !=
                   std::numeric_limits<double>::infinity()))
        newPos = blitz::floor(newPos / p_model->elementSizeUm() + 0.5)
            * p_model->elementSizeUm();
  }

  if (blitz::all(newPos == _positionUm)) return;

#ifdef DEBUG
  std::cerr << "ViewWidget::setPositionUm(" << positionUm
            << ") (current position is " << _positionUm << ")" << std::endl;
#endif

  _positionUm = newPos;

  bool enable = updatesEnabled();
  setUpdatesEnabled(false);
  for (std::vector<ChannelSpecs*>::iterator it =
           p_model->begin(); it != p_model->end(); ++it)
  {
    ChannelSpecsRenderer *rend =
        static_cast<ChannelSpecsRenderer*>((*it)->renderer(this));
    if ((*it)->visible() && rend != NULL) rend->updateCache();
  }
  setUpdatesEnabled(enable);
  emit positionChanged(newPos);
}
