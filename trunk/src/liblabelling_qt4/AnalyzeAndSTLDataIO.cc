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

#include "AnalyzeAndSTLDataIO.hh"

#include <QtGui/QFileDialog>
#include <QtCore/QSettings>

#include <libProgressReporter/ProgressReporterQt4.hh>

#include "LabellingMainWidget.hh"
#include "MultiChannelModel.hh"
#include "DataChannelSpecs.hh"
#include "VisualizationChannelSpecs.hh"
#include "RGBChannelSpecs.hh"
#include "SplineCurveChannelSpecs.hh"
#include "IRoCSChannelSpecs.hh"
#include "IRoCSSCTChannelSpecs.hh"
#include "AnnotationChannelSpecs.hh"
#include "ColorMap.hh"
#include "OpenGlRenderingViewWidget.hh"
#include "SplineCurveChannelSpecsOpenGlRenderingViewRenderer.hh"
#include "IRoCSChannelSpecsOpenGlRenderingViewRenderer.hh"
#include "IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer.hh"
#include "PointMarkerOpenGlRenderingViewRenderer.hh"
#include "SphereMarker.hh"
#include "SphereMarkerOpenGlRenderingViewRenderer.hh"
#include "SHSurfaceMarker.hh"
#include "SurfaceMarker.hh"

AnalyzeAndSTLDataIO::AnalyzeAndSTLDataIO(LabellingMainWidget *mainWidget)
        : p_mainWidget(mainWidget)
{}

AnalyzeAndSTLDataIO::~AnalyzeAndSTLDataIO()
{}

void AnalyzeAndSTLDataIO::save(std::string const &fileNameStub) const
{
  iRoCS::ProgressReporterQt4 *pr = p_mainWidget->progressReporter();
  if (pr != NULL)
  {
    pr->reset();
    pr->setVisible(true);
  }
  MultiChannelModel *model = p_mainWidget->multiChannelModel();

  int channelIdx = 0;
  for (std::vector<ChannelSpecs*>::const_iterator it =
           model->begin(); it != model->end(); ++it, ++channelIdx)
  {
    if (pr != NULL && !pr->updateProgress(
            (100 * channelIdx) / model->nChannels())) return;
    switch ((*it)->channelType())
    {
    case ChannelSpecs::Data:
    {
      DataChannelSpecs *channel = static_cast<DataChannelSpecs*>(*it);
      if (pr != NULL && !pr->updateProgressMessage(
              "Converting float channel '" + channel->name() +
              "' to short")) return;
      blitz::Array<short,3> data(
          blitz::TinyVector<ptrdiff_t,3>(
              model->boundingBoxShapeUm() / model->elementSizeUm()));
      float valueScale = channel->displayMax() - channel->displayMin();
      // Constant array... not so good for normalization
      if (valueScale == 0.0f) valueScale = 1.0f;

#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(data.size()); ++i)
      {
        blitz::TinyVector<double,3> posUm;
        ptrdiff_t tmp = i;
        for (int d = 2; d >= 0; --d)
        {
          posUm(d) = (tmp % data.extent(d)) * model->elementSizeUm()(d);
          tmp /= data.extent(d);
        }
        float val =
            (channel->data()->valueAt(posUm) - channel->displayMin()) /
            valueScale;
        if (val < 0.0f) val = 0.0f;
        if (val > 1.0f) val = 1.0f;
        if (channel->gamma() != 1.0)
            val = channel->gammaLUT(static_cast<int>(val * 65535.0f));
        data.data()[i] = static_cast<unsigned short>(val * 32767.0f);
      }
      std::string dsName = _replaceSpecialCharacters(channel->name());
      if (pr != NULL && !pr->updateProgressMessage(
              "Saving '" + fileNameStub + "_" + dsName + ".[hdr|img]:" +
              dsName + "'")) return;
      try
      {
        AnalyzeIOWrapper::writeDataset(
            data, model->elementSizeUm(), fileNameStub + "_" + dsName,
            dsName);
      }
      catch (BlitzAnalyzeError &e)
      {
        std::cerr << "Could not save '" << fileNameStub << "_" << dsName <<
            ".[hdr|img]': " << e.what() << std::endl;
      }
      break;
    }
    case ChannelSpecs::Visualization:
    {
      VisualizationChannelSpecs *channel =
          static_cast<VisualizationChannelSpecs*>(*it);
      if (pr != NULL && !pr->updateProgressMessage(
              "Converting indexed channel '" + channel->name() +
              "' to short")) return;
      blitz::Array<short,3> data(
          blitz::TinyVector<ptrdiff_t,3>(
              model->boundingBoxShapeUm() / model->elementSizeUm()));
#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(data.size()); ++i)
      {
        blitz::TinyVector<double,3> posUm;
        ptrdiff_t tmp = i;
        for (int d = 2; d >= 0; --d)
        {
          posUm(d) = (tmp % data.extent(d)) * model->elementSizeUm()(d);
          tmp /= data.extent(d);
        }
        blitz::TinyVector<float,3> color(
            channel->colorMap().color(channel->data()->valueAt(posUm)));
        data.data()[i] = static_cast<unsigned short>(
            32767.0f * (0.2126 * color(0) + 0.7152 * color(1) +
                        0.0722 * color(2)));
      }
      std::string dsName = _replaceSpecialCharacters(channel->name());
      if (pr != NULL && !pr->updateProgressMessage(
              "Saving '" + fileNameStub + "_" + dsName + ".[hdr|img]:" +
              dsName + "'")) return;
      try
      {
        AnalyzeIOWrapper::writeDataset(
            data, channel->elementSizeUm(), fileNameStub + "_" + dsName,
            dsName);
      }
      catch (BlitzAnalyzeError &e)
      {
        std::cerr << "Could not save '" << fileNameStub << "_" << dsName <<
            ".[hdr|img]': " << e.what() << std::endl;
      }
      break;
    }
    case ChannelSpecs::RGB:
    {
      RGBChannelSpecs *channel =
          static_cast<RGBChannelSpecs*>(*it);
      if (pr != NULL && !pr->updateProgressMessage(
              "Converting RGB channel '" + channel->name() + "'")) return;
      blitz::Array<blitz::TinyVector<unsigned char,3>,3> data(
          blitz::TinyVector<ptrdiff_t,3>(
              model->boundingBoxShapeUm() / model->elementSizeUm()));
      float valueScale = channel->maxValue() - channel->minValue();
      // Constant array... not so good for normalization
      if (valueScale == 0.0f) valueScale = 1.0f;

#ifdef _OPENMP
#pragma omp parallel for
#endif
      for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(data.size()); ++i)
      {
        blitz::TinyVector<double,3> posUm;
        ptrdiff_t tmp = i;
        for (int d = 2; d >= 0; --d)
        {
          posUm(d) = (tmp % data.extent(d)) * model->elementSizeUm()(d);
          tmp /= data.extent(d);
        }
        blitz::TinyVector<float,3> val = channel->data()->valueAt(posUm);
        if (channel->normalize())
            val = (val - channel->minValue()) / valueScale;
        if (channel->gamma() != 1.0)
            for (int d = 0; d < 3; ++d)
                val(d) = channel->gammaLUT(
                    static_cast<int>(val(d) * 65535.0f));
        data.data()[i] = blitz::TinyVector<unsigned char,3>(255.0f * val);
      }
      std::string dsName = _replaceSpecialCharacters(channel->name());
      if (pr != NULL && !pr->updateProgressMessage(
              "Saving '" + fileNameStub + "_" + dsName + ".[hdr|img]:" +
              dsName + "'")) return;
      try
      {
        AnalyzeIOWrapper::writeDataset(
            data, channel->elementSizeUm(), fileNameStub + "_" + dsName,
            dsName);
      }
      catch (BlitzAnalyzeError &e)
      {
        std::cerr << "Could not save '" << fileNameStub << "_" << dsName <<
            ".[hdr|img]': " << e.what() << std::endl;
      }
      break;
    }
    case ChannelSpecs::IRoCS:
    {
      IRoCSChannelSpecs *channel =
          static_cast<IRoCSChannelSpecs*>(*it);
      OpenGlRenderingViewWidget *view =
          static_cast<OpenGlRenderingViewWidget*>(
              p_mainWidget->openGlRenderingView());
      IRoCSChannelSpecsOpenGlRenderingViewRenderer *renderer =
          static_cast<IRoCSChannelSpecsOpenGlRenderingViewRenderer*>(
              channel->renderer(view));

      std::string dsName = _replaceSpecialCharacters(channel->name());

      // Transform vertices and normals so that they fit the vgStudioMax
      // coordinate system
      blitz::TinyMatrix<float,4,4> trafo;
      trafo =
          0.0, 0.0, 1.0,
          -model->boundingBoxShapeUm()(2) / 2.0f + model->lowerBoundUm()(2),
          0.0, -1.0, 0.0,
          model->boundingBoxShapeUm()(1) / 2.0f - model->lowerBoundUm()(1),
          1.0, 0.0, 0.0,
          -model->boundingBoxShapeUm()(0) / 2.0f + model->lowerBoundUm()(0),
          0.0, 0.0, 0.0, 1.0;
      blitz::TinyMatrix<float,3,3> rotation;
      rotation =
          trafo(0, 0), trafo(0, 1), trafo(0, 2),
          trafo(1, 0), trafo(1, 1), trafo(1, 2),
          trafo(2, 0), trafo(2, 1), trafo(2, 2);

      atb::SurfaceGeometry axis(renderer->axisGeometry());
      for (size_t i = 0; i < axis.vertices().size(); ++i)
      {
        axis.vertices()[i] = atb::homogeneousToEuclidean(
            trafo * atb::euclideanToHomogeneous(axis.vertices()[i]));
        axis.normals()[i] = rotation * axis.normals()[i];
      }

      atb::SurfaceGeometry boundary(renderer->boundaryGeometry());
      for (size_t i = 0; i < boundary.vertices().size(); ++i)
      {
        boundary.vertices()[i] = atb::homogeneousToEuclidean(
            trafo * atb::euclideanToHomogeneous(boundary.vertices()[i]));
        boundary.normals()[i] = rotation * boundary.normals()[i];
      }

      try
      {
        std::ofstream outFile(
            (fileNameStub + "_" + dsName + "_axis.stl").c_str(),
            std::ios::trunc | std::ios::binary);
        std::string header(
            "-- iRoCS channel written by the iRoCS Toolbox (c) LMB "
            "University of Freiburg --");
        outFile.write(header.c_str(), 80);
        uint32_t nTriangles = axis.indices().size() / 3;
        outFile.write(reinterpret_cast<char*>(&nTriangles), sizeof(uint32_t));
        axis.renderSTL(outFile, false);
      }
      catch (std::exception &e)
      {
        std::cerr << "Could not save axis geometry: " << e.what()
                  << std::endl;
      }
      
      try
      {
        std::ofstream outFile(
            (fileNameStub + "_" + dsName + "_surface.stl").c_str(),
            std::ios::trunc | std::ios::binary);
        std::string header(
            "-- iRoCS channel written by the iRoCS Toolbox (c) LMB "
            "University of Freiburg --");
        outFile.write(header.c_str(), 80);
        uint32_t nTriangles = boundary.indices().size() / 3;
        outFile.write(reinterpret_cast<char*>(&nTriangles), sizeof(uint32_t));
        boundary.renderSTL(outFile, false);
      }
      catch (std::exception &e)
      {
        std::cerr << "Could not save surface geometry: " << e.what()
                  << std::endl;
      }
      break;
    }
    case ChannelSpecs::IRoCSSCT:
    {
      IRoCSSCTChannelSpecs *channel =
          static_cast<IRoCSSCTChannelSpecs*>(*it);
      OpenGlRenderingViewWidget *view =
          static_cast<OpenGlRenderingViewWidget*>(
              p_mainWidget->openGlRenderingView());
      IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer *renderer =
          static_cast<IRoCSSCTChannelSpecsOpenGlRenderingViewRenderer*>(
              channel->renderer(view));

      std::string dsName = _replaceSpecialCharacters(channel->name());

      // Transform vertices and normals so that they fit the vgStudioMax
      // coordinate system
      blitz::TinyMatrix<float,4,4> trafo;
      trafo =
          0.0, 0.0, 1.0,
          -model->boundingBoxShapeUm()(2) / 2.0f + model->lowerBoundUm()(2),
          0.0, -1.0, 0.0,
          model->boundingBoxShapeUm()(1) / 2.0f - model->lowerBoundUm()(1),
          1.0, 0.0, 0.0,
          -model->boundingBoxShapeUm()(0) / 2.0f + model->lowerBoundUm()(0),
          0.0, 0.0, 0.0, 1.0;
      blitz::TinyMatrix<float,3,3> rotation;
      rotation =
          trafo(0, 0), trafo(0, 1), trafo(0, 2),
          trafo(1, 0), trafo(1, 1), trafo(1, 2),
          trafo(2, 0), trafo(2, 1), trafo(2, 2);

      atb::SurfaceGeometry axis(renderer->axisGeometry());
      for (size_t i = 0; i < axis.vertices().size(); ++i)
      {
        axis.vertices()[i] = atb::homogeneousToEuclidean(
            trafo * atb::euclideanToHomogeneous(axis.vertices()[i]));
        axis.normals()[i] = rotation * axis.normals()[i];
      }

      atb::SurfaceGeometry boundary(channel->sct()->surface());
      for (size_t i = 0; i < boundary.vertices().size(); ++i)
      {
        boundary.vertices()[i] = atb::homogeneousToEuclidean(
            trafo * atb::euclideanToHomogeneous(boundary.vertices()[i]));
        boundary.normals()[i] = rotation * boundary.normals()[i];
      }

      try
      {
        std::ofstream outFile(
            (fileNameStub + "_" + dsName + "_axis.stl").c_str(),
            std::ios::trunc | std::ios::binary);
        std::string header(
            "-- iRoCS channel written by the iRoCS Toolbox (c) LMB "
            "University of Freiburg --");
        outFile.write(header.c_str(), 80);
        uint32_t nTriangles = axis.indices().size() / 3;
        outFile.write(reinterpret_cast<char*>(&nTriangles), sizeof(uint32_t));
        axis.renderSTL(outFile, false);
      }
      catch (std::exception &e)
      {
        std::cerr << "Could not save axis geometry: " << e.what()
                  << std::endl;
      }
      
      try
      {
        std::ofstream outFile(
            (fileNameStub + "_" + dsName + "_surface.stl").c_str(),
            std::ios::trunc | std::ios::binary);
        std::string header(
            "-- iRoCS channel written by the iRoCS Toolbox (c) LMB "
            "University of Freiburg --");
        outFile.write(header.c_str(), 80);
        uint32_t nTriangles = boundary.indices().size() / 3;
        outFile.write(reinterpret_cast<char*>(&nTriangles), sizeof(uint32_t));
        boundary.renderSTL(outFile, false);
      }
      catch (std::exception &e)
      {
        std::cerr << "Could not save surface geometry: " << e.what()
                  << std::endl;
      }
      break;
    }
    case ChannelSpecs::SplineCurve:
    {
      SplineCurveChannelSpecs *channel =
          static_cast<SplineCurveChannelSpecs*>(*it);
      OpenGlRenderingViewWidget *view =
          static_cast<OpenGlRenderingViewWidget*>(
              p_mainWidget->openGlRenderingView());
      SplineCurveChannelSpecsOpenGlRenderingViewRenderer *renderer =
          static_cast<SplineCurveChannelSpecsOpenGlRenderingViewRenderer*>(
              channel->renderer(view));

      std::string dsName = _replaceSpecialCharacters(channel->name());

      // Transform vertices and normals so that they fit the vgStudioMax
      // coordinate system
      blitz::TinyMatrix<float,4,4> trafo;
      trafo =
          0.0, 0.0, 1.0,
          -model->boundingBoxShapeUm()(2) / 2.0f + model->lowerBoundUm()(2),
          0.0, -1.0, 0.0,
          model->boundingBoxShapeUm()(1) / 2.0f - model->lowerBoundUm()(1),
          1.0, 0.0, 0.0,
          -model->boundingBoxShapeUm()(0) / 2.0f + model->lowerBoundUm()(0),
          0.0, 0.0, 0.0, 1.0;
      blitz::TinyMatrix<float,3,3> rotation;
      rotation =
          trafo(0, 0), trafo(0, 1), trafo(0, 2),
          trafo(1, 0), trafo(1, 1), trafo(1, 2),
          trafo(2, 0), trafo(2, 1), trafo(2, 2);

      atb::SurfaceGeometry curve(renderer->curveGeometry());
      for (size_t i = 0; i < curve.vertices().size(); ++i)
      {
        curve.vertices()[i] = atb::homogeneousToEuclidean(
            trafo * atb::euclideanToHomogeneous(curve.vertices()[i]));
        curve.normals()[i] = rotation * curve.normals()[i];
      }

      try
      {
        std::ofstream outFile(
            (fileNameStub + "_" + dsName + ".stl").c_str(),
            std::ios::trunc | std::ios::binary);
        std::string header(
            "-- SplineCurve   written by the iRoCS Toolbox (c) LMB "
            "University of Freiburg --");
        outFile.write(header.c_str(), 80);
        uint32_t nTriangles = curve.indices().size() / 3;
        outFile.write(reinterpret_cast<char*>(&nTriangles), sizeof(uint32_t));
        curve.renderSTL(outFile, false);
      }
      catch (std::exception &e)
      {
        std::cerr << "Could not save spline curve geometry: " << e.what()
                  << std::endl;
      }
      break;
    }
    case ChannelSpecs::Annotation:
    {
      AnnotationChannelSpecs *channel =
          static_cast<AnnotationChannelSpecs*>(*it);
      if (channel->markers().size() == 0) return;
      OpenGlRenderingViewWidget *view =
          static_cast<OpenGlRenderingViewWidget*>(
              p_mainWidget->openGlRenderingView());
      std::string dsName = _replaceSpecialCharacters(channel->name());

      switch (channel->markerType())
      {
      case Marker::Point:
      {
        // Count the number of triangles for each label
        std::map<int,uint32_t> nTriangles;
        for (std::vector<Marker*>::const_iterator it = channel->begin();
             it != channel->end(); ++it)
        {
          if (!view->positionVisible((*it)->positionUm())) continue;
          PointMarkerOpenGlRenderingViewRenderer *renderer =
              static_cast<PointMarkerOpenGlRenderingViewRenderer*>(
                  (*it)->renderer(view));
          if (nTriangles.find((*it)->label()) == nTriangles.end())
              nTriangles[(*it)->label()] = 0;
          nTriangles[(*it)->label()] += renderer->geometry().indices().size();
        }
        for (std::map<int,uint32_t>::iterator it = nTriangles.begin();
             it != nTriangles.end(); ++it) it->second /= 3;

        // for each label save one STL file
        for (std::map<int,uint32_t>::iterator it = nTriangles.begin();
             it != nTriangles.end(); ++it)
        {
          try
          {
            std::stringstream fileName;
            fileName << fileNameStub << "_" << dsName << "_label_"
                     << it->first << ".stl";
            std::ofstream outFile(
                fileName.str().c_str(), std::ios::trunc | std::ios::binary);
            std::string header(
                "-- PointChannel  written by the iRoCS Toolbox (c) LMB "
                "University of Freiburg --");
            outFile.write(header.c_str(), 80);
            outFile.write(
                reinterpret_cast<char*>(&it->second), sizeof(uint32_t));
            for (std::vector<Marker*>::const_iterator itM = channel->begin();
                 itM != channel->end(); ++itM)
            {
              if (!view->positionVisible((*itM)->positionUm())) continue;
              if ((*itM)->label() != it->first) continue;
              PointMarkerOpenGlRenderingViewRenderer *renderer =
                  static_cast<PointMarkerOpenGlRenderingViewRenderer*>(
                      (*itM)->renderer(view));
              atb::SurfaceGeometry geometry(renderer->geometry());
              for (size_t i = 0; i < geometry.vertices().size(); ++i)
                  geometry.vertices()[i] += (*itM)->positionUm();
              // Transform vertices and normals so that they fit the vgStudioMax
              // coordinate system
              blitz::TinyMatrix<float,4,4> trafo;
              trafo =
                  0.0, 0.0, 1.0,
                  -model->boundingBoxShapeUm()(2) / 2.0f +
                  model->lowerBoundUm()(2),
                  0.0, -1.0, 0.0,
                  model->boundingBoxShapeUm()(1) / 2.0f -
                  model->lowerBoundUm()(1),
                  1.0, 0.0, 0.0,
                  -model->boundingBoxShapeUm()(0) / 2.0f +
                  model->lowerBoundUm()(0),
                  0.0, 0.0, 0.0, 1.0;
              blitz::TinyMatrix<float,3,3> rotation;
              rotation =
                  trafo(0, 0), trafo(0, 1), trafo(0, 2),
                  trafo(1, 0), trafo(1, 1), trafo(1, 2),
                  trafo(2, 0), trafo(2, 1), trafo(2, 2);
              for (size_t i = 0; i < geometry.vertices().size(); ++i)
              {
                geometry.vertices()[i] = atb::homogeneousToEuclidean(
                    trafo * atb::euclideanToHomogeneous(
                        geometry.vertices()[i]));
                geometry.normals()[i] = rotation * geometry.normals()[i];
              }
              geometry.renderSTL(outFile, false);
            }
          }
          catch (std::exception &e)
          {
            std::cerr << "Could not save marker geometry: " << e.what()
                      << std::endl;
          }
        }
        break;
      }
      case Marker::Sphere:
      {
        // Count the number of triangles for each label
        std::map<int,uint32_t> nTriangles;
        for (std::vector<Marker*>::const_iterator it = channel->begin();
             it != channel->end(); ++it)
        {
          if (!view->positionVisible((*it)->positionUm())) continue;
          SphereMarkerOpenGlRenderingViewRenderer *renderer =
              static_cast<SphereMarkerOpenGlRenderingViewRenderer*>(
                  (*it)->renderer(view));
          if (nTriangles.find((*it)->label()) == nTriangles.end())
              nTriangles[(*it)->label()] = 0;
          nTriangles[(*it)->label()] += renderer->geometry().indices().size();
        }
        for (std::map<int,uint32_t>::iterator it = nTriangles.begin();
             it != nTriangles.end(); ++it) it->second /= 3;

        // for each label save one STL file
        for (std::map<int,uint32_t>::iterator it = nTriangles.begin();
             it != nTriangles.end(); ++it)
        {
          try
          {
            std::stringstream fileName;
            fileName << fileNameStub << "_" << dsName << "_label_"
                     << it->first << ".stl";
            std::ofstream outFile(
                fileName.str().c_str(), std::ios::trunc | std::ios::binary);
            std::string header(
                "-- SphereChannel written by the iRoCS Toolbox (c) LMB "
                "University of Freiburg --");
            outFile.write(header.c_str(), 80);
            outFile.write(
                reinterpret_cast<char*>(&it->second), sizeof(uint32_t));
            for (std::vector<Marker*>::const_iterator itM = channel->begin();
                 itM != channel->end(); ++itM)
            {
              if (!view->positionVisible((*itM)->positionUm())) continue;
              if ((*itM)->label() != it->first) continue;
              SphereMarkerOpenGlRenderingViewRenderer *renderer =
                  static_cast<SphereMarkerOpenGlRenderingViewRenderer*>(
                      (*itM)->renderer(view));
              atb::SurfaceGeometry geometry(renderer->geometry());
              for (size_t i = 0; i < geometry.vertices().size(); ++i)
                  geometry.vertices()[i] = geometry.vertices()[i] *
                      static_cast<SphereMarker*>(*itM)->radiusUm() +
                      (*itM)->positionUm();
              // Transform vertices and normals so that they fit the vgStudioMax
              // coordinate system
              blitz::TinyMatrix<float,4,4> trafo;
              trafo =
                  0.0, 0.0, 1.0,
                  -model->boundingBoxShapeUm()(2) / 2.0f +
                  model->lowerBoundUm()(2),
                  0.0, -1.0, 0.0,
                  model->boundingBoxShapeUm()(1) / 2.0f -
                  model->lowerBoundUm()(1),
                  1.0, 0.0, 0.0,
                  -model->boundingBoxShapeUm()(0) / 2.0f +
                  model->lowerBoundUm()(0),
                  0.0, 0.0, 0.0, 1.0;
              blitz::TinyMatrix<float,3,3> rotation;
              rotation =
                  trafo(0, 0), trafo(0, 1), trafo(0, 2),
                  trafo(1, 0), trafo(1, 1), trafo(1, 2),
                  trafo(2, 0), trafo(2, 1), trafo(2, 2);
              for (size_t i = 0; i < geometry.vertices().size(); ++i)
              {
                geometry.vertices()[i] = atb::homogeneousToEuclidean(
                    trafo * atb::euclideanToHomogeneous(
                        geometry.vertices()[i]));
                geometry.normals()[i] = rotation * geometry.normals()[i];
              }
              geometry.renderSTL(outFile, false);
            }
          }
          catch (std::exception &e)
          {
            std::cerr << "Could not save marker geometry: " << e.what()
                      << std::endl;
          }
        }
        break;
      }
      case Marker::SHSurface:
      case Marker::Nucleus:
      {
        // Count the number of triangles for each label
        std::map<int,uint32_t> nTriangles;
        for (std::vector<Marker*>::const_iterator it = channel->begin();
             it != channel->end(); ++it)
        {
          if (!view->positionVisible((*it)->positionUm())) continue;
          SHSurfaceMarker *marker = static_cast<SHSurfaceMarker*>(*it);
          if (nTriangles.find((*it)->label()) == nTriangles.end())
              nTriangles[(*it)->label()] = 0;
          if (marker->coefficients().size() == 0)
              nTriangles[(*it)->label()] +=
                  static_cast<SphereMarkerOpenGlRenderingViewRenderer*>(
                      (*it)->renderer(view))->geometry().indices().size();
          else nTriangles[(*it)->label()] += marker->indices().size();
        }
        for (std::map<int,uint32_t>::iterator it = nTriangles.begin();
             it != nTriangles.end(); ++it) it->second /= 3;

        // for each label save one STL file
        for (std::map<int,uint32_t>::iterator it = nTriangles.begin();
             it != nTriangles.end(); ++it)
        {
          try
          {
            std::stringstream fileName;
            fileName << fileNameStub << "_" << dsName << "_label_"
                     << it->first << ".stl";
            std::ofstream outFile(
                fileName.str().c_str(), std::ios::trunc | std::ios::binary);
            std::string header(
                "-- SHSurfaces    written by the iRoCS Toolbox (c) LMB "
                "University of Freiburg --");
            outFile.write(header.c_str(), 80);
            outFile.write(
                reinterpret_cast<char*>(&it->second), sizeof(uint32_t));
            for (std::vector<Marker*>::const_iterator itM = channel->begin();
                 itM != channel->end(); ++itM)
            {
              if (!view->positionVisible((*itM)->positionUm())) continue;
              if ((*itM)->label() != it->first) continue;
              SHSurfaceMarker *marker = static_cast<SHSurfaceMarker*>(*itM);

              atb::SurfaceGeometry geometry;
              if (marker->coefficients().size() == 0)
              {
                SphereMarkerOpenGlRenderingViewRenderer *renderer =
                    static_cast<SphereMarkerOpenGlRenderingViewRenderer*>(
                        (*itM)->renderer(view));
                geometry = renderer->geometry();
                for (size_t i = 0; i < geometry.vertices().size(); ++i)
                    geometry.vertices()[i] = geometry.vertices()[i] *
                        static_cast<SphereMarker*>(*itM)->radiusUm() +
                        (*itM)->positionUm();
              }
              else
              {
                geometry = marker->geometry();
                for (size_t i = 0; i < geometry.vertices().size(); ++i)
                    geometry.vertices()[i] = geometry.vertices()[i] +
                        (*itM)->positionUm();
              }
              
              // Transform vertices and normals so that they fit the vgStudioMax
              // coordinate system
              blitz::TinyMatrix<float,4,4> trafo;
              trafo =
                  0.0, 0.0, 1.0,
                  -model->boundingBoxShapeUm()(2) / 2.0f +
                  model->lowerBoundUm()(2),
                  0.0, -1.0, 0.0,
                  model->boundingBoxShapeUm()(1) / 2.0f -
                  model->lowerBoundUm()(1),
                  1.0, 0.0, 0.0,
                  -model->boundingBoxShapeUm()(0) / 2.0f +
                  model->lowerBoundUm()(0),
                  0.0, 0.0, 0.0, 1.0;
              blitz::TinyMatrix<float,3,3> rotation;
              rotation =
                  trafo(0, 0), trafo(0, 1), trafo(0, 2),
                  trafo(1, 0), trafo(1, 1), trafo(1, 2),
                  trafo(2, 0), trafo(2, 1), trafo(2, 2);
              for (size_t i = 0; i < geometry.vertices().size(); ++i)
              {
                geometry.vertices()[i] = atb::homogeneousToEuclidean(
                    trafo * atb::euclideanToHomogeneous(
                        geometry.vertices()[i]));
                geometry.normals()[i] = rotation * geometry.normals()[i];
              }
              geometry.renderSTL(outFile, false);
            }
          }
          catch (std::exception &e)
          {
            std::cerr << "Could not save marker geometry: " << e.what()
                      << std::endl;
          }
        }
        break;
      }
      case Marker::Surface:
      case Marker::Cell:
      {
        // Count the number of triangles for each label
        std::map<int,uint32_t> nTriangles;
        for (std::vector<Marker*>::const_iterator it = channel->begin();
             it != channel->end(); ++it)
        {
          if (!view->positionVisible((*it)->positionUm())) continue;
          SurfaceMarker *marker = static_cast<SurfaceMarker*>(*it);
          if (nTriangles.find((*it)->label()) == nTriangles.end())
              nTriangles[(*it)->label()] = 0;
          nTriangles[(*it)->label()] += marker->indices().size();
        }
        for (std::map<int,uint32_t>::iterator it = nTriangles.begin();
             it != nTriangles.end(); ++it) it->second /= 3;

        // for each label save one STL file
        for (std::map<int,uint32_t>::iterator it = nTriangles.begin();
             it != nTriangles.end(); ++it)
        {
          try
          {
            std::stringstream fileName;
            fileName << fileNameStub << "_" << dsName << "_label_"
                     << it->first << ".stl";
            std::ofstream outFile(
                fileName.str().c_str(), std::ios::trunc | std::ios::binary);
            std::string header(
                "-- Surfaces      written by the iRoCS Toolbox (c) LMB "
                "University of Freiburg --");
            outFile.write(header.c_str(), 80);
            outFile.write(
                reinterpret_cast<char*>(&it->second), sizeof(uint32_t));
            for (std::vector<Marker*>::const_iterator itM = channel->begin();
                 itM != channel->end(); ++itM)
            {
              if (!view->positionVisible((*itM)->positionUm())) continue;
              if ((*itM)->label() != it->first) continue;
              SurfaceMarker *marker = static_cast<SurfaceMarker*>(*itM);

              atb::SurfaceGeometry geometry(marker->surface());
              for (size_t i = 0; i < geometry.vertices().size(); ++i)
                  geometry.vertices()[i] = geometry.vertices()[i] +
                      (*itM)->positionUm();

              // Transform vertices and normals so that they fit the vgStudioMax
              // coordinate system
              blitz::TinyMatrix<float,4,4> trafo;
              trafo =
                  0.0, 0.0, 1.0,
                  -model->boundingBoxShapeUm()(2) / 2.0f +
                  model->lowerBoundUm()(2),
                  0.0, -1.0, 0.0,
                  model->boundingBoxShapeUm()(1) / 2.0f -
                  model->lowerBoundUm()(1),
                  1.0, 0.0, 0.0,
                  -model->boundingBoxShapeUm()(0) / 2.0f +
                  model->lowerBoundUm()(0),
                  0.0, 0.0, 0.0, 1.0;
              blitz::TinyMatrix<float,3,3> rotation;
              rotation =
                  trafo(0, 0), trafo(0, 1), trafo(0, 2),
                  trafo(1, 0), trafo(1, 1), trafo(1, 2),
                  trafo(2, 0), trafo(2, 1), trafo(2, 2);
              for (size_t i = 0; i < geometry.vertices().size(); ++i)
              {
                geometry.vertices()[i] = atb::homogeneousToEuclidean(
                    trafo * atb::euclideanToHomogeneous(
                        geometry.vertices()[i]));
                geometry.normals()[i] = rotation * geometry.normals()[i];
              }
              geometry.renderSTL(outFile, false);
            }
          }
          catch (std::exception &e)
          {
            std::cerr << "Could not save marker geometry: " << e.what()
                      << std::endl;
          }
        }
        break;
      }
      case Marker::Cylinder:
      {
        std::cerr << __FILE__ << ":" << __LINE__ << ": ToDo: Implement "
                  << "Cylinder marker export to STL" << std::endl;
        break;
      }
      default:
      {
        std::cout << "Omitting annotation channel '" << (*it)->name()
                  << "'. The marker type is not supported." << std::endl;
      }
      }
      break;
    }
    default:
    {
      std::cout << "Omitting channel '" << (*it)->name()
                << "'. Channel cannot be exported to Analyze or STL"
                << std::endl;
    }
    }
  }
  if (pr != NULL)
  {
    pr->reset();
    pr->setVisible(false);
  }
}

void AnalyzeAndSTLDataIO::save()
{
  if (p_mainWidget == NULL || p_mainWidget->multiChannelModel() == NULL ||
      p_mainWidget->multiChannelModel()->nChannels() == 0) return;

  QSettings settings;
  QString startPath = settings.value(
      "FileDialog/vglstartpath",
      settings.value(
          "FileDialog/startpath", getenv("HOME")).toString()).toString();
  QString filePrefix = QFileDialog::getSaveFileName(
      p_mainWidget, "Select file prefix for Analyze and STL files",
      startPath, QString(), NULL, QFileDialog::DontConfirmOverwrite);
  if (filePrefix == QString::Null()) return;
  settings.setValue("FileDialog/vglstartpath", startPath);
  save(filePrefix.toStdString());
}

std::string AnalyzeAndSTLDataIO::_replaceSpecialCharacters(std::string const &s)
{
  std::string res = s;
  size_t i = 0;
  while (
      (i = res.find_first_of(std::string("!\"§$%&/()=?*#~^°`''\\<>|;:,."), i))
      != std::string::npos) res[i] = '_';
  return res;
}
