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

#ifndef VISUALIZATIONCHANNELSPECS_HH
#define VISUALIZATIONCHANNELSPECS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "ChannelSpecs.hh"

#include <libArrayToolbox/Array.hh>
#include <libArrayToolbox/Neighborhood.hh>

#include <set>

class ColorMap;
class ColorMapEditorWidget;

class QComboBox;

class VisualizationChannelSpecs : public ChannelSpecs
{

  Q_OBJECT

public:

  VisualizationChannelSpecs(
      atb::Array<int,3> *data, MultiChannelModel *model = NULL,
      bool sign = false, int bitDepth = 16);
  ~VisualizationChannelSpecs();

  ChannelSpecs::ChannelType channelType() const override;

  static VisualizationChannelSpecs *load();

  atb::Array<int,3> const *data() const;
  atb::Array<int,3> *data();

/*======================================================================*/
/*!
 *   Get a float representation of the value at the given 3D world
 *   coordinates in micrometers.
 *
 *   \param positionUm world coordinates in micrometers
 *
 *   \return The value at the given world coordinates
 */
/*======================================================================*/
  float valueAt(blitz::TinyVector<double,3> const &positionUm) const override;

/*======================================================================*/
/*!
 *   Get a string representation of the value at the given 3D world
 *   coordinates in micrometers.
 *
 *   \param positionUm world coordinates in micrometers
 *
 *   \return The value at the given world coordinates as string
 */
/*======================================================================*/
  std::string stringValueAt(
      blitz::TinyVector<double,3> const &positionUm) const override;

/*======================================================================*/
/*!
 *   Get the integer label at the given coordinates relative to the upper
 *   left corner of the volume. The transformation matrix of the channel is
 *   ignored!
 *
 *   \param position_um volume coordinates in micrometers
 *
 *   \return The value at the given volume coordinates
 *
 *   \exception std::out_of_range if the given coordinates are out-of-bounds
 */
/*======================================================================*/
  int& labelAt(blitz::TinyVector<double,3> const &positionUm);

/*======================================================================*/
/*!
 *   Get the integer label at the given pixel coordinates relative to the
 *   upper left corner of the volume. The transformation matrix of the
 *   channel is ignored!
 *
 *   \param pos volume coordinates in pixels
 *
 *   \return The value at the given volume coordinates
 *
 *   \exception std::out_of_range if the given coordinates are out-of-bounds
 */
/*======================================================================*/
  int& labelAtVoxel(blitz::TinyVector<ptrdiff_t,3> const &pos);

  ColorMap const &colorMap() const;
  ColorMap &colorMap();

/*======================================================================*/
/*!
 *   Set the value for subsequent flood fill operations
 *
 *   \param value The new fill value
 */
/*======================================================================*/
  void setFillValue(int value);

/*======================================================================*/
/*!
 *   Pick the value for subsequent flood fill operations from the current
 *   volume. Sets fill value to the value at the specified world coordinates
 *   of this VisualizationChannelSpecs. Positions will be rounded to the
 *   nearest pixel. Out-of-bounds access leaves the fill value unchanged.
 *
 *   \param positionUm The world coordinates in micrometers to set the
 *     fill value from.
 */
/*======================================================================*/
  void pickFillValue(blitz::TinyVector<double,3> const &positionUm);

/*======================================================================*/
/*!
 *   Get the value for subsequent flood fill operations
 *
 *   \return The fill value
 */
/*======================================================================*/
  int fillValue() const;

/*======================================================================*/
/*!
 *   Apply a flood fill operation at the given world coordinates in
 *   micrometers. The position will be rounded to the nearest pixel which
 *   will be used as seed. If the given position is out-of-bounds this
 *   operation has no effect.
 *   If merging of neighboring segments is requested, the 8-neighborhood of
 *   adjacent pixels with different label is tested if another region with
 *   the same label exists. If so, also fill the ridge pixel with the new
 *   label.
 *   If this operation succeeds it calls emitUpdateRequest() to inform
 *   other views of changes in this VisualizationChannelSpecs.
 *
 *   \param positionUm The world coordinates in micrometers in which the
 *     flood fill operation is started
 *   \param mergeNeighboringSegments If true, boundary pixels outside the
 *     filled segment will also be filled if they touch another segment with
 *     the same label and no segments with different labels are merged
 */
/*======================================================================*/
  void floodFill(blitz::TinyVector<double,3> const &positionUm,
                 bool mergeNeighboringSegments = false);

  void setSign(bool sign);
  bool sign() const;

  void setBitDepth(int bitDepth);
  int bitDepth() const;

  void setHasNewData();

/*======================================================================*/
/*!
 *   Get the data changed flag. This flag indicates that the content of
 *   the underlying atb::Array has changed and requires saving.
 *
 *   \return The data changed flag
 */
/*======================================================================*/
  bool dataChanged() const;

/*======================================================================*/
/*!
 *   Set the data changed flag. This flag indicates that the content of
 *   the underlying atb::Array has changed and requires saving. This flag
 *   should be cleared (set to false) after saving the dataset.
 *
 *   \param dataChanged The new data changed flag value
 */
/*======================================================================*/
  void setDataChanged(bool dataChanged);

  ChannelSpecsRenderer *addRenderer(ViewWidget *view);

public slots:

  void normalizeIndexRange();
  void setTransformation(
      blitz::TinyMatrix<double,4,4> const &transformation) override;

/*======================================================================*/
/*!
 *   Reset the channel shape, element size, transformation and bounding box
 *   from the underlying data. This method must be called whenever the
 *   underlying data were programmatically changed using the direct data
 *   access methods. If updates are enabled it will also do cache updates.
 */
/*======================================================================*/
  void revalidate() override;

private slots:

  void updateIcon();

private:

  typedef std::set< blitz::TinyVector<atb::BlitzIndexT,3>,
                    atb::TinyVectorLessThan<atb::BlitzIndexT,3> > PixelSet;

  PixelSet _grassfire(blitz::TinyVector<atb::BlitzIndexT,3> const &posPx,
                      int value, bool getBoundary);

  atb::Array<int,3> *p_data;

  int _fillValue;

  bool _sign;
  int _bitDepth;

  ColorMap *p_colorMap;

  ColorMapEditorWidget *p_colorMapEditor;

  bool _dataChanged;

};

#endif
