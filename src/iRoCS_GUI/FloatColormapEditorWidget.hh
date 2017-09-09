/**************************************************************************
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

#ifndef IROCS_FLOATCOLORMAPEDITORWIDGET_HH
#define IROCS_FLOATCOLORMAPEDITORWIDGET_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <QtGui/QWidget>
#include <libMultiChannelModel/FloatColormapObserver.hh>

class QDoubleSpinBox;
class QComboBox;
class QToolButton;
class QPushButton;

namespace iRoCS {

  class FloatColormap;
  class ColorControlElement;
  class FloatColormapBarWidget;
  class BoolControlElement;
  class DoubleControlElement;

  class FloatColormapEditorWidget
    : public QWidget, virtual public FloatColormapObserver {

    Q_OBJECT

  public:

    FloatColormapEditorWidget(
      FloatColormap &colormap, QWidget *parent = NULL);
    ~FloatColormapEditorWidget();

    /*======================================================================*/
    /*!
     *   This method is called when the color index range of the color map
     *   changes. Only colors between the smallest and largest color index
     *   are properly handled by this color map. Values exceeding the range
     *   are clipped.
     */
     /*======================================================================*/
    void updateIndexRange();

    /*======================================================================*/
    /*!
     *   This method is called when the color for the smallest index changed.
     */
     /*======================================================================*/
    void updateStartColor();

    /*======================================================================*/
    /*!
     *   This method is called when the color for the greatest index changed.
     */
     /*======================================================================*/
    void updateEndColor();

    /*======================================================================*/
    /*!
     *   This method is called when an intermediate interpolation color at
     *   the given position is changed in this color map. If there is no
     *   intermediate color at the given position it will be created.
     *
     *   \param position The position of the intermediate interpolation color
     *     in this color map in (0, 1) (0 = start, 1 = end)
     */
     /*======================================================================*/
    void updateIntermediateColor(double position);

    /*======================================================================*/
    /*!
     *   This method is called when an intermediate interpolation color is moved
     *   from the given old position to the given new position.
     *
     *   \param oldPosition The old position of the intermediate interpolation
     *     color in this color map in (0, 1) (0 = start, 1 = end)
     *   \param newPosition The new position of the interpolation color
     */
     /*======================================================================*/
    void updateMoveIntermediateColor(double oldPosition, double newPosition);

    /*======================================================================*/
    /*!
     *   This method is called when the intermediate interpolation color at
     *   the given position is removed from this color map.
     *
     *   \param position The position of the intermedaite interpolation color
     *     in (0, 1) (0 = start, 1 = end)
     */
     /*======================================================================*/
    void updateRemoveIntermediateColor(double position);

    /*======================================================================*/
    /*!
     *   This method is called when all intermediate interpolation colors
     *   are have been removed from this color map.
     */
     /*======================================================================*/
    void updateClearIntermediateColors();

    /*======================================================================*/
    /*!
     *   This method is called when the monochrome color of the FloatColormap
     *   changes. The monochrome color is only used if the color map is
     *   monochrome, otherwise the colors are defined by the start, end and
     *   intermediate color mapping.
     */
     /*======================================================================*/
    void updateMonochromeColor();

    /*======================================================================*/
    /*!
     *   This method is called whenever the range indicator is enabled or
     *   disabled.
     */
     /*======================================================================*/
    void updateRangeIndicator();

    /*======================================================================*/
    /*!
     *   This method is called whenever the gamma correction value for the
     *   colormap changes.
     */
     /*======================================================================*/
    void updateGamma();

    /*======================================================================*/
    /*!
     *   This method forces an update of the full Observer. It is mainly
     *   intended for initialization. If the observer is a QWidget it should
     *   just call the update() method of the QWidget.
     */
     /*======================================================================*/
    virtual void updateColormap();

  signals:

    void normalizeRequest();

    public slots:

    void setStartIndexInColormap();
    void setEndIndexInColormap();
    void setPresetInColormap();
    void setIndexRangeInColormap();
    void setMonochromeColorInColormap();
    void setGammaInColormap();
    void setRangeIndicatorFlagInColormap();
    void selectStartColor();
    void selectEndColor();

  private:

    void _updateIndexRangeSelector();
    void _updatePresetSelector();

    QComboBox *p_presetSelector;
    QComboBox *p_indexRangeSelector;
    QPushButton *p_normalizeButton;
    QDoubleSpinBox *p_startIndexControl;
    QToolButton *p_startColorButton;
    QDoubleSpinBox *p_endIndexControl;
    QToolButton *p_endColorButton;
    FloatColormapBarWidget *p_colorbar;
    ColorControlElement *p_monochromeColorControl;
    DoubleControlElement *p_gammaControl;
    BoolControlElement *p_rangeIndicatorControl;

  };

}

#endif
