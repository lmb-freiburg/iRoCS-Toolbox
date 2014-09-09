/**************************************************************************
 **       Title: Ortho-View Data-Channel Renderer
 **    $RCSfile$
 **   $Revision: 3880 $ $Name$
 **       $Date: 2008-01-17 18:05:50 +0100 (Thu, 17 Jan 2008) $
 **   Copyright: GPL $Author: tschmidt $
 ** Description:
 **
 **    This class is the interface between the different View- and Channel
 **    types. For each Channel type and each View type one such interface
 **    class needs to be implemented. This is the specific implementation
 **    for DataChannelSpecs <=> OrthoView
 **
 **************************************************************************/

#ifndef DATACHANNELSPECSORTHOVIEWRENDERER_CC
#define DATACHANNELSPECSORTHOVIEWRENDERER_CC

#include <QtGui/QFont>

#include "DataChannelSpecsOrthoViewRenderer.hh"

#include "MultiChannelModel.hh"
#include "DataChannelSpecs.hh"
#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"

#ifdef _OPENMP
#include <omp.h>
#endif

DataChannelSpecsOrthoViewRenderer::DataChannelSpecsOrthoViewRenderer(
    DataChannelSpecs* channel, OrthoViewWidget* view)
        : ChannelSpecsOrthoViewRenderer(channel, view)
{}
  
DataChannelSpecsOrthoViewRenderer::~DataChannelSpecsOrthoViewRenderer()
{}

void DataChannelSpecsOrthoViewRenderer::render(QPainter*) const
{
  // This is rendered directly by the view using the provided cache
}

void DataChannelSpecsOrthoViewRenderer::render(
    QXmlStreamWriter &, blitz::TinyVector<double,3> const &,
    blitz::TinyVector<double,3> const &, blitz::TinyVector<double,3> const &,
    std::string const &, double, iRoCS::ProgressReporter *) const
{
  // Dummy stub... Data channel specs are rendered into an Array in the
  // updateCache() method below, and then directly written to SVG in the view
}

blitz::TinyVector<atb::BlitzIndexT,3>
DataChannelSpecsOrthoViewRenderer::cacheOffsetPx() const
{
  DataChannelSpecs *channel =
      static_cast<DataChannelSpecs*>(p_channel);
  MultiChannelModel *model = channel->model();
  return blitz::TinyVector<atb::BlitzIndexT,3>(
      blitz::floor(channel->lowerBoundUm() / model->elementSizeUm() + 0.5));
}

blitz::TinyVector<atb::BlitzIndexT,3>
DataChannelSpecsOrthoViewRenderer::cacheShapePx() const
{
  DataChannelSpecs *channel =
      static_cast<DataChannelSpecs*>(p_channel);
  MultiChannelModel *model = channel->model();
  blitz::TinyVector<atb::BlitzIndexT,3> lbPx(cacheOffsetPx());
  blitz::TinyVector<atb::BlitzIndexT,3> ubPx(
      blitz::ceil(channel->upperBoundUm() / model->elementSizeUm()));
  return blitz::TinyVector<atb::BlitzIndexT,3>(ubPx - lbPx + 1);
}

blitz::Array<blitz::TinyVector<float,3>,2> const
&DataChannelSpecsOrthoViewRenderer::cache(int direction) const
{
#ifdef DEBUG
  std::cerr << "DataChannelSpecsOrthoViewRenderer::cache(" << direction
            << ")" << std::endl;
#endif
  OrthoViewWidget *view = static_cast<OrthoViewWidget*>(p_view);
  OrthoViewPlane *plane = view->orthoViewPlane(direction);
  blitz::TinyVector<int,2> dims(plane->dimensions());

  if (_cache(direction).extent(0) != cacheShapePx()(dims(0)) ||
      _cache(direction).extent(1) != cacheShapePx()(dims(1)))
      updateCache(direction);
  return _cache(direction);
}

void DataChannelSpecsOrthoViewRenderer::updateCache(int direction) const
{
  if (!cacheUpdatesEnabled()) return;
#ifdef DEBUG
  std::cerr << "DataChannelSpecsOrthoViewRenderer::updateCache(" << direction
            << ")" << std::endl;
#endif
  
  OrthoViewWidget *view = static_cast<OrthoViewWidget*>(p_view);
  blitz::TinyVector<double,3> const &viewPosUm = view->positionUm();

  OrthoViewPlane *plane = view->orthoViewPlane(direction);
  blitz::TinyVector<int,2> dims(plane->dimensions());

  blitz::TinyVector<atb::BlitzIndexT,3> lbPx(cacheOffsetPx());
  blitz::TinyVector<atb::BlitzIndexT,3> shapePx(cacheShapePx());
  _cache(direction).resize(shapePx(dims(0)), shapePx(dims(1)));
  
  DataChannelSpecs *channel = static_cast<DataChannelSpecs*>(p_channel);
  blitz::TinyVector<double,3> const &channelLbUm = channel->lowerBoundUm();
  blitz::TinyVector<double,3> const &channelUbUm = channel->upperBoundUm();
  float alpha = channel->alpha();

  MultiChannelModel *model = channel->model();
  blitz::TinyVector<double,3> const &modelElSizeUm = model->elementSizeUm();

  blitz::TinyVector<float,3> channelColor(alpha * channel->color());
  blitz::TinyVector<float,3> underFlowColor(0.0f, 0.0f, alpha);
  blitz::TinyVector<float,3> overFlowColor(alpha, 0.0f, 0.0f);
  float valueScale = channel->displayMax() - channel->displayMin();

  // Constant array... not so good for normalization
  if (valueScale == 0.0f) valueScale = 1.0f;

  if (viewPosUm(direction) >= channelLbUm(direction) -
      modelElSizeUm(direction) / 2.0 &&
      viewPosUm(direction) <= channelUbUm(direction) +
      modelElSizeUm(direction) / 2.0)
  {
    atb::Array<float,3>* data = channel->data();
    blitz::TinyVector<double,3> const &dataElSizeUm = data->elementSizeUm();

    blitz::TinyMatrix<double,4,4> const &trafo = data->transformation();
    bool simpleScaleTranslateTransform =
        trafo(0, 1) == 0.0 && trafo(0, 2) == 0.0 &&
        trafo(1, 0) == 0.0 && trafo(1, 2) == 0.0 &&
        trafo(2, 0) == 0.0 && trafo(2, 1) == 0.0 &&
        trafo(3, 0) == 0.0 && trafo(3, 1) == 0.0 &&
        trafo(3, 2) == 0.0 && trafo(3, 3) == 1.0;
    blitz::TinyVector<double,3> scaling(1.0);
    blitz::TinyVector<double,3> translation(0.0);
    if (simpleScaleTranslateTransform)
    {
      scaling = trafo(0, 0), trafo(1, 1), trafo(2, 2);
      translation = trafo(0, 3), trafo(1, 3), trafo(2, 3);
    }

    if (simpleScaleTranslateTransform)
    {
      if (blitz::all(scaling == 1.0) &&
          blitz::all(translation + lbPx * modelElSizeUm == 0.0) &&
          blitz::all(dataElSizeUm == modelElSizeUm))
      {
        // Direct copy
        atb::BlitzIndexT z = static_cast<atb::BlitzIndexT>(
            std::floor((viewPosUm(direction) + translation(direction)) /
                       dataElSizeUm(direction) + 0.5));
#ifdef DEBUG
        std::cerr << "DataChannelSpecsOrthoViewRenderer::updateCache("
                  << direction << "): Identity transform detected: Direct copy "
                  << "of slice " << z << std::endl;
#endif
        if (z >= 0 && z < data->extent(direction))
        {
#ifdef _OPENMP
#pragma omp parallel for
#endif
          for (atb::BlitzIndexT y = 0; y < _cache(direction).extent(0); ++y)
          {
            blitz::TinyVector<atb::BlitzIndexT,3> srcPos;
            srcPos(direction) = z;
            srcPos(dims(0)) = y;
            if (srcPos(dims(0)) >= 0 && srcPos(dims(0)) < data->extent(dims(0)))
            {
              for (atb::BlitzIndexT x = 0; x < _cache(direction).extent(1); ++x)
              {
                srcPos(dims(1)) = x;
                if (srcPos(dims(1)) >= 0 &&
                    srcPos(dims(1)) < data->extent(dims(1)))
                {
                  float val = (*data)(srcPos);
                  float rawVal = val;
                  val = (val - channel->displayMin()) / valueScale;
                  if (val < 0.0f) val = 0.0f;
                  if (val > 1.0f) val = 1.0f;
                  if (channel->gamma() != 1.0)
                      val = channel->gammaLUT(static_cast<int>(val * 65535.0f));
                  _cache(direction)(y, x) = channelColor * val;
                  if (channel->showExposureProblems())
                  {
                    if (rawVal <= channel->displayMin())
                        _cache(direction)(y, x) = underFlowColor;
                    if (rawVal >= channel->displayMax())
                        _cache(direction)(y, x) = overFlowColor;          
                  }
                }
                else _cache(direction)(y, x) = blitz::TinyVector<float,3>(0.0f);
              }
            }
            else std::memset(&_cache(direction)(y, 0), 0,
                             _cache(direction).extent(1) * 3 * sizeof(float));
          }
        }
        else std::memset(_cache(direction).data(), 0,
                         _cache(direction).size() * 3 * sizeof(float));
      }
      else if (blitz::all(translation == 0.0))
      {
        // Scaled copy
#ifdef DEBUG
        std::cerr << "DataChannelSpecsOrthoViewRenderer::updateCache("
                  << direction << "): Pure scaling transform detected"
                  << std::endl;
#endif
        double zUm =
            scaling(direction) * viewPosUm(direction) /
            dataElSizeUm(direction);
        double s0 = scaling(dims(0)) * modelElSizeUm(dims(0)) /
            dataElSizeUm(dims(0));
        double s1 = scaling(dims(1)) * modelElSizeUm(dims(1)) /
            dataElSizeUm(dims(1));
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT y = 0; y < _cache(direction).extent(0); ++y)
        {
          blitz::TinyVector<double,3> srcPosPx;
          srcPosPx(direction) = zUm;
          srcPosPx(dims(0)) = s0 * y;
          for (atb::BlitzIndexT x = 0; x < _cache(direction).extent(1); ++x)
          {
            srcPosPx(dims(1)) = s1 * x;
            float val = data->interpolator().get(*data, srcPosPx);
            float rawVal = val;
            val = (val - channel->displayMin()) / valueScale;
            if (val < 0.0f) val = 0.0f;
            if (val > 1.0f) val = 1.0f;
            if (channel->gamma() != 1.0)
                val = channel->gammaLUT(static_cast<int>(val * 65535.0f));
            _cache(direction)(y, x) = channelColor * val;
            if (channel->showExposureProblems())
            {
              if (rawVal <= channel->displayMin())
                  _cache(direction)(y, x) = underFlowColor;
              if (rawVal >= channel->displayMax())
                  _cache(direction)(y, x) = overFlowColor;          
            }
          }
        }
      }
      else
      {
        // Scaled copy with subpixel translation
#ifdef DEBUG
        std::cerr << "DataChannelSpecsOrthoViewRenderer::updateCache("
                  << direction << "): Scaling and translation detected: "
                  << "scaling = " << scaling << ", translation = "
                  << translation << std::endl;
#endif        
        double zUm =
            (scaling(direction) * viewPosUm(direction) +
             translation(direction)) / dataElSizeUm(direction);
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (atb::BlitzIndexT y = 0; y < _cache(direction).extent(0); ++y)
        {
          blitz::TinyVector<double,3> srcPosPx;
          srcPosPx(direction) = zUm;
          srcPosPx(dims(0)) =
              (scaling(dims(0)) *
               (y * modelElSizeUm(dims(0)) + channelLbUm(dims(0))) +
               translation(dims(0))) / dataElSizeUm(dims(0));
          for (atb::BlitzIndexT x = 0; x < _cache(direction).extent(1); ++x)
          {
            srcPosPx(dims(1)) =
                (scaling(dims(1)) *
                 (x * modelElSizeUm(dims(1)) + channelLbUm(dims(1))) +
                 translation(dims(1))) / dataElSizeUm(dims(1));
            float val = data->interpolator().get(*data, srcPosPx);
            float rawVal = val;
            val = (val - channel->displayMin()) / valueScale;
            if (val < 0.0f) val = 0.0f;
            if (val > 1.0f) val = 1.0f;
            if (channel->gamma() != 1.0)
                val = channel->gammaLUT(static_cast<int>(val * 65535.0f));
            _cache(direction)(y, x) = channelColor * val;
            if (channel->showExposureProblems())
            {
              if (rawVal <= channel->displayMin())
                  _cache(direction)(y, x) = underFlowColor;
              if (rawVal >= channel->displayMax())
                  _cache(direction)(y, x) = overFlowColor;          
            }
          }
        }        
      }
    }
    else
    {
      // Slanted plane copy
#ifdef DEBUG
      std::cerr << "DataChannelSpecsOrthoViewRenderer::updateCache("
                << direction << "): General transform detected"
                << std::endl;
#endif
      double zUm = viewPosUm(direction);
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t i = 0;
           i < static_cast<ptrdiff_t>(_cache(direction).size()); ++i)
      {
        blitz::TinyVector<atb::BlitzIndexT,2> pos(
            i / _cache(direction).extent(1), i % _cache(direction).extent(1));
        blitz::TinyVector<double,3> posUm;
        posUm(dims(0)) = pos(0) * modelElSizeUm(dims(0)) + channelLbUm(dims(0));
        posUm(dims(1)) = pos(1) * modelElSizeUm(dims(1)) + channelLbUm(dims(1));
        posUm(direction) = zUm;
        float val = data->valueAt(posUm);
        float outVal = val;
        outVal = (outVal - channel->displayMin()) / valueScale;
        if (outVal < 0.0f) outVal = 0.0f;
        if (outVal > 1.0f) outVal = 1.0f;
        if (channel->gamma() != 1.0)
            outVal = channel->gammaLUT(static_cast<int>(outVal * 65535.0f));
        _cache(direction).data()[i] = channelColor * outVal;
        if (channel->showExposureProblems())
        {
          if (val <= channel->displayMin())
              _cache(direction).data()[i] = underFlowColor;
          if (val >= channel->displayMax())
              _cache(direction).data()[i] = overFlowColor;          
        }
      }
    }
  }
  else std::memset(_cache(direction).data(), 0,
                   _cache(direction).size() * 3 * sizeof(float));

  emit cacheUpdated(this, direction);
  p_view->update();
}

#endif
