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

#include "lmorph.hh"

#if defined HAVE_CONFIG_H && !defined PACKAGE
#include <config.hh>
#endif

#ifdef HAVE_BLITZ_V9
#include <blitz/tinyvec-et.h>
#endif

Walker3D::Walker3D()
{}

Walker3D::Walker3D(blitz::TinyVector<int, 3> shape, int conn)
        : m_conn(conn), m_shape(shape)
{}

void Walker3D::setLocation(const blitz::TinyVector<int, 3> &p)
{
  m_p = p;
  walks = 0;
}

bool Walker3D::getNextNeighbor(blitz::TinyVector<int, 3> &q)
{
  if (walks >= m_conn)
      return false;
  else
  {
    do
    {
      q = m_p + blitz::TinyVector<int,3>(
          moveIn3DNeighbor[walks][0], moveIn3DNeighbor[walks][1],
          moveIn3DNeighbor[walks][2]);
      walks++;
      if (blitz::all(q >= 0) && blitz::all(q < m_shape))
          return true;
    }
    while (walks < m_conn);
    return false;
  }
}


Scanner3D::Scanner3D()
{}

Scanner3D::Scanner3D(blitz::TinyVector<int, 3> shape, int conn)
          : m_conn(conn / 2), m_shape(shape)
        {}

void Scanner3D::setLocation(const blitz::TinyVector<int, 3> &p)
{
  m_p = p;
  walks = 0;
}

bool Scanner3D::getNextNeighbor(blitz::TinyVector<int, 3> &q)
{
  if (walks >= m_conn)
      return false;
  else
  {
    do
    {
      q = m_p + blitz::TinyVector<int,3>(
          moveIn3DHalfNeighbor[walks][0],
          moveIn3DHalfNeighbor[walks][1],
          moveIn3DHalfNeighbor[walks][2]);
      walks ++;
      //check if the neighbor inside the image border
      if (blitz::all(q >= 0) && blitz::all(q < m_shape))
          return true;
    }
    while (walks < m_conn);
    return false;
  }
}
