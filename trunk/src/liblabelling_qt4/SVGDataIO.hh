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

#ifndef SVGDATAIO_HH
#define SVGDATAIO_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QDialog>

#include <blitz/array.h>

#include <libProgressReporter/ProgressReporter.hh>

#include <set>

class LabellingMainWidget;
class FileNameSelectionControlElement;
class DoubleControlElement;
class Double3ControlElement;
class IntControlElement;
class StringSelectionControlElement;
class BoolControlElement;

class SVGDataIOParametersDialog : public QDialog
{

  Q_OBJECT
  
  public:
  
  SVGDataIOParametersDialog(
      LabellingMainWidget *mainWidget,
      QWidget *parent = NULL, Qt::WindowFlags f = 0);
  ~SVGDataIOParametersDialog();
  
  std::string fileName() const;
  double widthCm() const;
  double marginCm() const;
  blitz::TinyVector<double,3> lowerBoundUm() const;
  blitz::TinyVector<double,3> upperBoundUm() const;
  double scaleBarLengthUm() const;
  std::string font() const;
  double fontSizePt() const;

private slots:

  void updateLowerBoundControl();
  void updateUpperBoundControl();
  void checkAndAccept();

private:
  
  double getNextAcceptableScaleBarLengthUm(double lengthUm);

  LabellingMainWidget *p_mainWidget;
  FileNameSelectionControlElement *p_fileNameControl;
  DoubleControlElement *p_widthCmControl;
  DoubleControlElement *p_marginCmControl;
  Double3ControlElement *p_lowerBoundUmControl, *p_upperBoundUmControl;
  DoubleControlElement *p_scaleBarLengthUmControl;
  StringSelectionControlElement *p_fontControl;
  DoubleControlElement *p_fontSizePtControl;

  std::set<double> _acceptableScaleBarLengths;
  blitz::TinyVector<double,3> _lbUm, _ubUm;

};

class SVGDataIO
{
  
  public:

  SVGDataIO(LabellingMainWidget *mainWidget);
  ~SVGDataIO();
  
/*======================================================================*/
/*! 
 *   Save the contents of the current OrthoViewWidget to SVG using a
 *   QSvgGenerator. This function is a legacy function and is only a fall-back
 *   if saveXML() is not able to draw parts of the displayed elements.
 *   You should use it only during debugging of new channels if an explicit
 *   svg rendering routine is not yet implemented.
 *
 *   \param fileName The file name to save the SVG image to
 *   \param widthCm  The output image width in centimeters
 *   \param marginCm The margin width between the orthoview planes in
 *     centimeters
 *   \param lowerBoundUm The lower bound of the selected box to render to
 *     SVG in micrometers
 *   \param upperBoundUm The upper bound of the selected box to render to
 *     SVG in micrometers
 *   \param scaleBarLengthUm The length of the displayed scale bar in
 *     micrometers
 *   \param font The font name to use. Make sure that your system and any
 *     system you want to display the SVG with has the specified font
 *     installed
 *   \param fontSizePt The font size in points. This only affects the font
 *     size of the scale bar.
 */
/*======================================================================*/
  void save(
      std::string const &fileName, double widthCm, double marginCm,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      double scaleBarLengthUm, std::string const &font,
      double fontSizePt) const;
  
/*======================================================================*/
/*! 
 *   Save the contents of the current OrthoViewWidget to SVG using custom
 *   SVG writing routines which have to be implemented for each channel and
 *   marker type. This routine produces clean SVG code structuring the
 *   channels and markers in hierarchical groups for later editing.
 *   Explicit implementations of the renderSVG() methods of the different
 *   channels and markers should be optimized for later editing.
 *
 *   \param fileName The file name to save the SVG image to
 *   \param widthCm  The output image width in centimeters
 *   \param marginCm The margin width between the orthoview planes in
 *     centimeters
 *   \param lowerBoundUm The lower bound of the selected box to render to
 *     SVG in micrometers
 *   \param upperBoundUm The upper bound of the selected box to render to
 *     SVG in micrometers
 *   \param scaleBarLengthUm The length of the displayed scale bar in
 *     micrometers
 *   \param font The font name to use. Make sure that your system and any
 *     system you want to display the SVG with has the specified font
 *     installed
 *   \param fontSizePt The font size in points. This only affects the font
 *     size of the scale bar. 
 *   \param pr  If not NULL progress will be reported via the given
 *     ProgressReporter
 */
/*======================================================================*/
  void saveXml(
      std::string const &fileName, double widthCm, double marginCm,
      blitz::TinyVector<double,3> const &lowerBoundUm,
      blitz::TinyVector<double,3> const &upperBoundUm,
      double scaleBarLengthUm, std::string const &font,
      double fontSizePt, iRoCS::ProgressReporter *pr = NULL) const;
  
  void save();

private:
  
  LabellingMainWidget *p_mainWidget;

};

#endif
