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
