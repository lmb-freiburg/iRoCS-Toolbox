/**************************************************************************
 *
 * Copyright (C) 2015 Kun Liu, Thorsten Falk
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

#include "ltransform.hh"

std::vector< blitz::TinyVector<unsigned char,3> >
getColorMap(int n)
{
  std::vector< blitz::TinyVector<unsigned char, 3> > colorMap(n + 20);

  colorMap[0] = blitz::TinyVector<unsigned char, 3>(0, 0, 0);
  colorMap[1] = blitz::TinyVector<unsigned char, 3>(255, 255, 255);
  colorMap[2] = blitz::TinyVector<unsigned char, 3>(0, 0, 255);
  colorMap[3] = blitz::TinyVector<unsigned char, 3>(0, 255, 0);
  colorMap[4] = blitz::TinyVector<unsigned char, 3>(255, 0, 0);
  colorMap[5] = blitz::TinyVector<unsigned char, 3>(255, 255, 0);
  colorMap[6] = blitz::TinyVector<unsigned char, 3>(255, 0, 255);
  colorMap[7] = blitz::TinyVector<unsigned char, 3>(0, 255, 255);
  colorMap[8] = blitz::TinyVector<unsigned char, 3>(200, 150, 100);
  colorMap[9] = blitz::TinyVector<unsigned char, 3>(100, 150, 200);
  colorMap[10] = blitz::TinyVector<unsigned char, 3>(150, 100, 200);
  colorMap[11] = blitz::TinyVector<unsigned char, 3>(150, 200, 100);
  colorMap[12] = blitz::TinyVector<unsigned char, 3>(100, 200, 150);
  colorMap[13] = blitz::TinyVector<unsigned char, 3>(200, 100, 150);
  colorMap[14] = blitz::TinyVector<unsigned char, 3>(200, 0, 100);
  colorMap[15] = blitz::TinyVector<unsigned char, 3>(100, 0, 200);
  colorMap[16] = blitz::TinyVector<unsigned char, 3>(200, 100, 0);
  colorMap[17] = blitz::TinyVector<unsigned char, 3>(0, 100, 200);
  colorMap[18] = blitz::TinyVector<unsigned char, 3>(100, 200, 0);
  for (int i = 19; i < n + 20; i++)
  {
    unsigned char red = float(std::rand()) / RAND_MAX * 200 + 40;
    unsigned char green = 50 * i % 200 + 40;
    unsigned char blue = float(std::rand()) / RAND_MAX * 200 + 40;

    colorMap[i] = blitz::TinyVector<unsigned char, 3>(red, green, blue);
  }

  return colorMap;
}
