/**************************************************************************
**       Title: Channel Specs for Dense Annotations
**    $RCSfile$
**   $Revision: 3896 $ $Name$
**       $Date: 2008-07-16 15:17:47 +0200 (Wed, 16 Jul 2008) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#include "VisualizationChannelSpecs.hh"

#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QIcon>
#include <QtGui/QColorDialog>
#include <QtGui/QComboBox>
#include <QtGui/QVBoxLayout>

#include "MultiChannelModel.hh"
#include "OrthoViewWidget.hh"
#include "VisualizationChannelSpecsOrthoViewRenderer.hh"
#include "VisualizationChannelSpecsOpenGlRenderingViewRenderer.hh"
#include "ColorMap.hh"
#include "ColorMapEditorWidget.hh"

#include <iostream>
#include <stdexcept>

VisualizationChannelSpecs::VisualizationChannelSpecs(
  atb::Array<int,3> *data, MultiChannelModel *model, bool sign, int bitDepth)
        : ChannelSpecs(model), p_data(data), _sign(sign), _bitDepth(bitDepth)
{
  this->_lowerBoundUm = data->lowerBoundUm();
  this->_upperBoundUm = data->upperBoundUm();
  this->_elementSizeUm = data->elementSizeUm();
  this->_originalTransformation = data->transformation();
  setTransformation(this->_originalTransformation);
  p_colorMap = new ColorMap(0, 1 << (_bitDepth - 1));

  QPushButton *normalizeButton = new QPushButton(tr("normalize"));
  connect(normalizeButton, SIGNAL(clicked()), SLOT(normalizeIndexRange()));
  p_channelControlLayout->addWidget(normalizeButton);

  p_colorMapEditor = new ColorMapEditorWidget(p_colorMap);
  p_colorMap->setColorMapEditor(p_colorMapEditor);
  p_channelControlLayout->addWidget(p_colorMapEditor);
  connect(p_colorMapEditor, SIGNAL(colorMapChanged()),
          SLOT(emitUpdateRequest()));
  connect(p_colorMapEditor, SIGNAL(colorMapChanged()), SLOT(updateIcon()));

  p_channelControlLayout->addStretch(1);

  updateIcon();
}

VisualizationChannelSpecs::~VisualizationChannelSpecs()
{
  delete p_data;
  p_colorMapEditor->setColorMap(NULL);
  delete p_colorMap;
}

ChannelSpecs::ChannelType VisualizationChannelSpecs::channelType() const
{
  return ChannelSpecs::Visualization;
}

void VisualizationChannelSpecs::hasNewData()
{
  _hasNewData = true;
}

atb::Array<int,3> const *VisualizationChannelSpecs::data() const
{
  return p_data;
}

atb::Array<int,3> *VisualizationChannelSpecs::data()
{
  return p_data;
}

float VisualizationChannelSpecs::valueAt(
    blitz::TinyVector<double,3> const &positionUm) const
{
  return static_cast<float>(p_data->valueAt(positionUm));
}

std::string VisualizationChannelSpecs::stringValueAt(
    blitz::TinyVector<double,3> const &positionUm) const
{
  std::stringstream os;
  os << p_data->valueAt(positionUm);
  return os.str();
}

int &VisualizationChannelSpecs::labelAt(
    blitz::TinyVector<double,3> const &positionUm)
{
  blitz::TinyVector<ptrdiff_t, 3> p =
      blitz::TinyVector<ptrdiff_t, 3>(
          positionUm / p_data->elementSizeUm() + 0.5);
  return labelAtVoxel(p);
}

int &VisualizationChannelSpecs::labelAtVoxel(
  blitz::TinyVector<ptrdiff_t,3> const &pos)
{
  if (blitz::all(pos >= 0) && blitz::all(pos < p_data->shape()))
      return (*p_data)(pos);
  else
      throw std::runtime_error(
          "VisualizationChannelSpecs: Called labelAtVoxel with position out of "
          "bounds");
}

void VisualizationChannelSpecs::setTransformation(
    blitz::TinyMatrix<double,4,4> const &transformation)
{
  if (blitz::all(_transformation == transformation)) return;
  p_data->setTransformation(transformation);
  _lowerBoundUm = p_data->lowerBoundUm();
  _upperBoundUm = p_data->upperBoundUm();
  ChannelSpecs::setTransformation(transformation);
}

void VisualizationChannelSpecs::revalidate()
{
  this->_lowerBoundUm = p_data->lowerBoundUm();
  this->_upperBoundUm = p_data->upperBoundUm();
  this->_elementSizeUm = p_data->elementSizeUm();
  this->_originalTransformation = p_data->transformation();
  setTransformation(this->_originalTransformation);
  ChannelSpecs::revalidate();
}

ChannelSpecsRenderer *VisualizationChannelSpecs::addRenderer(ViewWidget *view)
{
  if (_renderers.find(view) != _renderers.end())
  {
    std::cout << "VisualizationChannelSpecs::addRenderer(): WARNING: "
              << "The channel already has an associated renderer" << std::endl;
    return _renderers[view];
  }
  switch (view->viewType())
  {
  case ViewWidget::OrthoView:
    _renderers[view] = new VisualizationChannelSpecsOrthoViewRenderer(
        this, static_cast<OrthoViewWidget*>(view));
    break;
  case ViewWidget::OpenGlRenderingView:
    _renderers[view] = new VisualizationChannelSpecsOpenGlRenderingViewRenderer(
        this, reinterpret_cast<OpenGlRenderingViewWidget*>(view));
    break;
  default:
    std::cerr << __FILE__ << ":" << __LINE__ << ": Implementation missing"
              << std::endl;
    break;
  }
  return _renderers[view];
}

ColorMap const &VisualizationChannelSpecs::colorMap() const
{
  return *p_colorMap;
}

ColorMap &VisualizationChannelSpecs::colorMap()
{
  return *p_colorMap;
}

void VisualizationChannelSpecs::setSign(bool sign)
{
  _sign = sign;
  if (_sign)
  {
    switch (_bitDepth)
    {
    case 8:
      p_colorMap->setSigned8BitRange();
      break;
    case 16:
      p_colorMap->setSigned16BitRange();
      break;
    case 32:
      p_colorMap->setSigned32BitRange();
      break;
    default:
      p_colorMap->setSigned64BitRange();
    }
  }
  else
  {
    switch (_bitDepth)
    {
    case 8:
      p_colorMap->setUnsigned8BitRange();
      break;
    case 12:
      p_colorMap->setUnsigned12BitRange();
      break;
    case 16:
      p_colorMap->setUnsigned16BitRange();
      break;
    default:
      p_colorMap->setUnsigned32BitRange();
    }
  }
}

bool VisualizationChannelSpecs::sign() const
{
  return _sign;
}

void VisualizationChannelSpecs::setBitDepth(int bitDepth)
{
  _bitDepth = bitDepth;
  if (_sign)
  {
    switch (_bitDepth)
    {
    case 8:
      p_colorMap->setSigned8BitRange();
      break;
    case 16:
      p_colorMap->setSigned16BitRange();
      break;
    case 32:
      p_colorMap->setSigned32BitRange();
      break;
    default:
      p_colorMap->setSigned64BitRange();
    }
  }
  else
  {
    switch (_bitDepth)
    {
    case 8:
      p_colorMap->setUnsigned8BitRange();
      break;
    case 12:
      p_colorMap->setUnsigned12BitRange();
      break;
    case 16:
      p_colorMap->setUnsigned16BitRange();
      break;
    default:
      p_colorMap->setUnsigned32BitRange();
    }
  }
}

int VisualizationChannelSpecs::bitDepth() const
{
  return _bitDepth;
}

void VisualizationChannelSpecs::updateIcon()
{
  QImage image(16, 16, QImage::Format_RGB32);
  for (int y = 0; y < 16; ++y)
  {
    for (int x = 0; x < 16; ++x)
    {
      blitz::TinyVector<float,3> color = p_colorMap->color(
          static_cast<long long>(
              static_cast<double>(x) / 15.0 *
              static_cast<double>(
                  p_colorMap->endIndex() - p_colorMap->startIndex()) + 0.5) +
          p_colorMap->startIndex());
      QRgb col =
          (static_cast<int>(255.0f * color(0)) << 16) +
          (static_cast<int>(255.0f * color(1)) << 8) +
          static_cast<int>(255.0f * color(2));
      image.setPixel(x, y, col);
    }
  }
  setIcon(QIcon(QPixmap::fromImage(image)));
}

void VisualizationChannelSpecs::normalizeIndexRange()
{
  p_colorMap->setStartIndex(blitz::min(*p_data));
  p_colorMap->setEndIndex(blitz::max(*p_data));
  emitUpdateRequest();
}
