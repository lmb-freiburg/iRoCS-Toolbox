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

#include "SVGDataIO.hh"

#include <QtGui/QVBoxLayout>
#include <QtCore/QSettings>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QtGui/QLabel>
#include <QtGui/QStatusBar>
#include <QtGui/QProgressBar>
#include <QtCore/QFile>
#include <QtSvg/QSvgGenerator>
#include <QtGui/QPainter>
#include <QtXml/QXmlStreamWriter>

#include "LabellingMainWidget.hh"
#include "MultiChannelModel.hh"
#include "FileNameSelectionControlElement.hh"
#include "DoubleControlElement.hh"
#include "StringSelectionControlElement.hh"
#include "Double3ControlElement.hh"
#include "IntControlElement.hh"
#include "BoolControlElement.hh"
#include "OrthoViewWidget.hh"
#include "OrthoViewPlane.hh"

SVGDataIOParametersDialog::SVGDataIOParametersDialog(
    LabellingMainWidget *mainWidget,
    QWidget *parent, Qt::WindowFlags f)
        : QDialog(parent, f), p_mainWidget(mainWidget)
{
  assert(mainWidget != 0);

  _acceptableScaleBarLengths.insert(0.1);
  _acceptableScaleBarLengths.insert(0.2);
  _acceptableScaleBarLengths.insert(0.5);
  _acceptableScaleBarLengths.insert(1.0);
  _acceptableScaleBarLengths.insert(2.0);
  _acceptableScaleBarLengths.insert(5.0);
  _acceptableScaleBarLengths.insert(10.0);
  _acceptableScaleBarLengths.insert(20.0);
  _acceptableScaleBarLengths.insert(50.0);
  _acceptableScaleBarLengths.insert(100.0);
  _acceptableScaleBarLengths.insert(200.0);
  _acceptableScaleBarLengths.insert(500.0);
  _acceptableScaleBarLengths.insert(1000.0);
  _acceptableScaleBarLengths.insert(2000.0);
  _acceptableScaleBarLengths.insert(5000.0);
  _acceptableScaleBarLengths.insert(10000.0);
  _acceptableScaleBarLengths.insert(20000.0);
  _acceptableScaleBarLengths.insert(50000.0);
  _acceptableScaleBarLengths.insert(100000.0);

  QVBoxLayout* mainLayout = new QVBoxLayout;

  QFrame* parameterPanel = new QFrame;
  parameterPanel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  QVBoxLayout* parameterLayout = new QVBoxLayout;
  
  std::string outFileName;
  QSettings settings;
  QString startPath = settings.value(
      "FileDialog/startpath", getenv("HOME")).toString();
  if (p_mainWidget->fileName() == "")
      outFileName = startPath.toStdString() + "/labelling.svg";
  else outFileName = p_mainWidget->fileName().substr(
      0, p_mainWidget->fileName().rfind(".")) + ".svg";
  p_fileNameControl = new FileNameSelectionControlElement(
      tr("Output file"), outFileName, false, "svg (*.svg)");
  parameterLayout->addWidget(p_fileNameControl);

  p_widthCmControl = new DoubleControlElement(
      tr("Output width (cm)"),
      settings.value("SVGExport/widthCm", 8.0).toDouble());
  p_widthCmControl->setRange(0.0, 100.0);
  p_widthCmControl->setSingleStep(0.1);
  parameterLayout->addWidget(p_widthCmControl);

  p_marginCmControl = new DoubleControlElement(
      tr("Margin width (cm)"),
      settings.value("SVGExport/marginWidthCm", 0.1).toDouble());
  p_marginCmControl->setRange(0.00, 100.0);
  p_marginCmControl->setSingleStep(0.01);
  parameterLayout->addWidget(p_marginCmControl);

  _lbUm = p_mainWidget->multiChannelModel()->lowerBoundUm();
  p_lowerBoundUmControl = new Double3ControlElement(tr("Lower bound"));
  p_lowerBoundUmControl->setRange(_lbUm, _ubUm);
  p_lowerBoundUmControl->setValue(_lbUm);
  p_lowerBoundUmControl->setSingleStep(
      p_mainWidget->multiChannelModel()->elementSizeUm());
  connect(p_lowerBoundUmControl, SIGNAL(valueChanged()),
          SLOT(updateUpperBoundControl()));
  parameterLayout->addWidget(p_lowerBoundUmControl);

  _ubUm = p_mainWidget->multiChannelModel()->upperBoundUm();
  p_upperBoundUmControl = new Double3ControlElement(tr("Upper bound"));
  p_upperBoundUmControl->setRange(_lbUm, _ubUm);
  p_upperBoundUmControl->setValue(_ubUm);
  p_upperBoundUmControl->setSingleStep(
      p_mainWidget->multiChannelModel()->elementSizeUm());
  connect(p_upperBoundUmControl, SIGNAL(valueChanged()),
          SLOT(updateLowerBoundControl()));
  parameterLayout->addWidget(p_upperBoundUmControl);

  p_scaleBarLengthUmControl = new DoubleControlElement(
      tr("Scale bar length (micron)"),
      getNextAcceptableScaleBarLengthUm(0.5 * (_ubUm(2) - _lbUm(2))));
  p_scaleBarLengthUmControl->setRange(0.1, 100000.0);
  p_scaleBarLengthUmControl->setSingleStep(0.1);
  parameterLayout->addWidget(p_scaleBarLengthUmControl);  

  QStringList fonts;
  fonts << "Helvetica (sans)" << "Times (serif)";
  p_fontControl = new StringSelectionControlElement(tr("Font"), fonts);
  parameterLayout->addWidget(p_fontControl);
  
  p_fontSizePtControl = new DoubleControlElement(tr("Font size (pt)"), 10.0);
  p_fontSizePtControl->setRange(0.1, 1000.0);
  p_fontSizePtControl->setSingleStep(0.1);
  parameterLayout->addWidget(p_fontSizePtControl);

  parameterPanel->setLayout(parameterLayout);
  mainLayout->addWidget(parameterPanel);
  
  QDialogButtonBox* buttonBox = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
  connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
          SLOT(checkAndAccept()));
  buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);
  connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
          SLOT(reject()));
  mainLayout->addWidget(buttonBox);

  setLayout(mainLayout);
}

SVGDataIOParametersDialog::~SVGDataIOParametersDialog()
{}

std::string SVGDataIOParametersDialog::fileName() const
{
  return p_fileNameControl->value();
}

double SVGDataIOParametersDialog::widthCm() const
{
  return p_widthCmControl->value();
}

double SVGDataIOParametersDialog::marginCm() const
{
  return p_marginCmControl->value();
}

blitz::TinyVector<double,3> SVGDataIOParametersDialog::lowerBoundUm() const
{
  return p_lowerBoundUmControl->value();
}

blitz::TinyVector<double,3> SVGDataIOParametersDialog::upperBoundUm() const
{
  return p_upperBoundUmControl->value();
}

double SVGDataIOParametersDialog::scaleBarLengthUm() const
{
  return p_scaleBarLengthUmControl->value();
}

std::string SVGDataIOParametersDialog::font() const
{
  return p_fontControl->toStdString();
}

double SVGDataIOParametersDialog::fontSizePt() const
{
  return p_fontSizePtControl->value();
}

void SVGDataIOParametersDialog::updateLowerBoundControl()
{
  p_lowerBoundUmControl->setRange(
      _lbUm, p_upperBoundUmControl->value());
  p_scaleBarLengthUmControl->setValue(
      getNextAcceptableScaleBarLengthUm(
          0.2 * (upperBoundUm()(2) - lowerBoundUm()(2))));
}

void SVGDataIOParametersDialog::updateUpperBoundControl()
{
  p_upperBoundUmControl->setRange(p_lowerBoundUmControl->value(), _ubUm);
  p_scaleBarLengthUmControl->setValue(
      getNextAcceptableScaleBarLengthUm(
          0.2 * (upperBoundUm()(2) - lowerBoundUm()(2))));
}

void SVGDataIOParametersDialog::checkAndAccept()
{
  QFile outFile(p_fileNameControl->value().c_str());
  if (outFile.exists())
  {
    if (QMessageBox::question(
            p_mainWidget, tr("File already exists"),
            tr("The file '%1' already exists. Do you want to overwrite "
               "it?").arg(p_fileNameControl->value().c_str()),
            QMessageBox::Yes | QMessageBox::No) !=
        QMessageBox::Yes) return;
    outFile.remove();
  }
  else
  {
    if (!outFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
      p_mainWidget->showError(
          tr("Cannot create output file '%1'. Please select another file "
             "location/name.").arg(p_fileNameControl->value().c_str()));
      return;
    }
    outFile.close();
    outFile.remove();
  }

  if (blitz::any(p_mainWidget->orthoView()->positionUm() < lowerBoundUm() ||
                 p_mainWidget->orthoView()->positionUm() > upperBoundUm()))
  {
      p_mainWidget->showError(
          tr("Current cross position is not within the selected range. "
             "Please move the cross hair into the bounding box you want to "
             "render."));
      return;    
  }

  accept();
}

double SVGDataIOParametersDialog::getNextAcceptableScaleBarLengthUm(
    double lengthUm)
{
  return *(--_acceptableScaleBarLengths.lower_bound(lengthUm));
}


SVGDataIO::SVGDataIO(LabellingMainWidget *mainWidget)
        : p_mainWidget(mainWidget)
{}

SVGDataIO::~SVGDataIO()
{}

void SVGDataIO::save(
    std::string const &fileName, double widthCm, double,
    blitz::TinyVector<double,3> const &, blitz::TinyVector<double,3> const &,
    double scaleBarLengthUm, std::string const &font,
    double fontSizePt) const
{
  iRoCS::ProgressReporterQt4* pr = p_mainWidget->progressReporter();
  pr->reset();
  pr->setVisible(true);

  if (!pr->updateProgressMessage(
          qApp->tr("Saving '%1'... ").arg(fileName.c_str()).toStdString()))
      return;

  // Image width in pixels
  double widthPx = widthCm * 90.0 / 2.54;

  // Margin in pixels
  double marginPx = 0.1 * 90.0 / 2.54;

  // Original volume shape in pixels
  blitz::TinyVector<double,3> shapePx(
      p_mainWidget->multiChannelModel()->boundingBoxShapeUm() /
      p_mainWidget->multiChannelModel()->elementSizeUm()(2));

  // Scale factor to apply to the original volume to fit into the view
  double scale = ((widthPx - marginPx) / (1.0 + shapePx(2) / shapePx(0))) /
      shapePx(0);

  // The new volume shape in pixels
  blitz::TinyVector<double,3> outShapePx(shapePx * scale);

  // Height in pixels
  double heightPx = outShapePx(1) + marginPx + outShapePx(0);

  QSvgGenerator generator;
  generator.setFileName(fileName.c_str());
  generator.setResolution(300);
  generator.setSize(
      QSize(static_cast<int>(widthPx), static_cast<int>(heightPx)));
  std::cout << "SVG shape in px = (" << static_cast<int>(widthPx) << ", "
            << static_cast<int>(heightPx) << ")" << std::endl;
  QPainter painter;
  painter.begin(&generator);
  // painter.scale(scale, scale);

  // xy-view
  painter.save();
  painter.scale(p_mainWidget->orthoView()->model()->elementSizeUm()(1) /
                p_mainWidget->orthoView()->model()->elementSizeUm()(2), 1);
  painter.translate(QPointF(0.5, 0.5));  
  p_mainWidget->orthoView()->xyView()->paint(painter);
  QPen pen(painter.pen());
  pen.setWidthF(3.0f);
  pen.setColor(Qt::white);
  painter.setPen(pen);
  painter.drawLine(
      QPointF(2.0 * marginPx, shapePx(1) - 2.0 * marginPx),
      QPointF(2.0 * marginPx + scaleBarLengthUm /
              p_mainWidget->multiChannelModel()->elementSizeUm()(2),
              shapePx(1) - 2.0 * marginPx));
  pen.setWidthF(1.0f);
  painter.setPen(pen);
  painter.setFont(
      QFont(font.c_str(), static_cast<int>(fontSizePt), QFont::Normal, false));
  painter.drawText(
      QRectF(
          QPointF(2.0 * marginPx, shapePx(1) - 2.0 * marginPx - 12),
          QPointF(2.0 * marginPx + scaleBarLengthUm /
                  p_mainWidget->multiChannelModel()->elementSizeUm()(2),
                  shapePx(1) - 2.0 * marginPx)), Qt::AlignCenter,
      QString::number(scaleBarLengthUm) + " " + QChar(0x03bc) + "m");  
  painter.restore();

  // xz-view
  painter.save();
  painter.scale(p_mainWidget->orthoView()->model()->elementSizeUm()(0) /
                p_mainWidget->orthoView()->model()->elementSizeUm()(2), 1);
  painter.translate(QPointF(0.5, shapePx(1) + marginPx + 0.5));
  p_mainWidget->orthoView()->xzView()->paint(painter);
  painter.restore();
  
  // zy-view
  painter.save();
  painter.scale(p_mainWidget->orthoView()->model()->elementSizeUm()(1) /
                p_mainWidget->orthoView()->model()->elementSizeUm()(2),
                p_mainWidget->orthoView()->model()->elementSizeUm()(0) /
                p_mainWidget->orthoView()->model()->elementSizeUm()(2));
  painter.translate(QPointF(shapePx(2) + marginPx + 0.5, 0.5));
  p_mainWidget->orthoView()->zyView()->paint(painter);
  painter.restore();

  painter.end();

  pr->reset();
  pr->setVisible(false);
}

void SVGDataIO::saveXml(
    std::string const &fileName, double widthCm, double marginCm,
    blitz::TinyVector<double,3> const &lowerBoundUm,
    blitz::TinyVector<double,3> const &upperBoundUm,
    double scaleBarLengthUm, std::string const &font,
    double fontSizePt, iRoCS::ProgressReporter *pr) const
{
  blitz::TinyVector<double,3> shapeUm(upperBoundUm - lowerBoundUm);
  double heightCm = (widthCm - marginCm) * (1.0 + shapeUm(1) / shapeUm(0)) /
      (1.0 + shapeUm(2) / shapeUm(0)) + marginCm;
  blitz::TinyVector<double,3> shapeCm;
  shapeCm(0) = (widthCm - marginCm) / (1.0 + shapeUm(2) / shapeUm(0));
  shapeCm(1) = heightCm - marginCm - shapeCm(0);
  shapeCm(2) = widthCm - marginCm - shapeCm(0);

  blitz::TinyVector<double,3> shapePx(shapeCm * 90.0 / 2.54);
  double marginPx = marginCm * 90.0 / 2.54;

  QFile svgFile(fileName.c_str());
  if (svgFile.open(QIODevice::WriteOnly | QIODevice::Truncate |
                   QIODevice::Text)) 
  {
    QXmlStreamWriter svgStream(&svgFile);
    svgStream.setAutoFormatting(true);
    svgStream.setAutoFormattingIndent(2);
    svgStream.writeStartDocument();
    svgStream.writeDTD(
        "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
        "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">");
    svgStream.writeStartElement("svg");
    svgStream.writeAttribute("version", "1.1");
    svgStream.writeAttribute("xmlns", "http://www.w3.org/2000/svg");
    svgStream.writeAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    svgStream.writeAttribute("width", QString::number(widthCm) + "cm");
    svgStream.writeAttribute("height", QString::number(heightCm) + "cm");
    svgStream.writeStartElement("defs");
    svgStream.writeStartElement("marker");
    svgStream.writeAttribute("id", "EndArrowWhite");
    svgStream.writeAttribute("viewBox", "0 0 6 8");
    svgStream.writeAttribute("refX", "6");
    svgStream.writeAttribute("refY", "3");
    svgStream.writeAttribute("markerUnits", "strokeWidth");
    svgStream.writeAttribute("markerWidth", "8");
    svgStream.writeAttribute("markerHeight", "6");
    svgStream.writeAttribute("orient", "auto");
    svgStream.writeEmptyElement("path");
    svgStream.writeAttribute("d", "M 0,0 l 2,3 l -2,3 l 8,-3 z");
    svgStream.writeAttribute("style", "fill:#ffffff;stroke:none");
    svgStream.writeEndElement(); // marker
    svgStream.writeEndElement(); // defs
    p_mainWidget->orthoView()->paint(
        svgStream, shapePx, marginPx, lowerBoundUm,
        upperBoundUm, scaleBarLengthUm, font, fontSizePt, pr);
    svgStream.writeEndElement(); // svg
    svgStream.writeEndDocument();
  }
  else p_mainWidget->showError(
      qApp->tr("Could not save '%1'").arg(fileName.c_str()));
}

void SVGDataIO::save()
{
  if (p_mainWidget->multiChannelModel()->nChannels() == 0) return;

  iRoCS::ProgressReporterQt4* pr = p_mainWidget->progressReporter();

  SVGDataIOParametersDialog dlg(p_mainWidget, p_mainWidget);
  if (dlg.exec() == QDialog::Rejected) return;

  if (pr != NULL)
  {
    if (!pr->updateProgressMessage(
            qApp->tr("Saving '%1'... ").arg(
                dlg.fileName().c_str()).toStdString())) return;
    pr->reset();
    pr->setVisible(true);
  }

  saveXml(dlg.fileName(), dlg.widthCm(), dlg.marginCm(),
          dlg.lowerBoundUm(), dlg.upperBoundUm(),
          dlg.scaleBarLengthUm(), dlg.font(), dlg.fontSizePt(), pr);

  if (pr != NULL)
  {
    pr->reset();
    pr->setVisible(false);
  }
}

