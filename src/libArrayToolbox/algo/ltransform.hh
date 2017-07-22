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

/*
 * ltransform.h
 *
 *  Created on: Oct 27, 2010
 *      Author: liu
 */

#ifndef LTRANSFORM_H_
#define LTRANSFORM_H_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>
#include <vector>

template<typename LabelT, int Dim>
void randomColorMapping(
    blitz::Array<LabelT,Dim> &L,
    blitz::Array<unsigned char,Dim+1> &randomColor);

std::vector< blitz::TinyVector<unsigned char,3> > getColorMap(int n);

#include "ltransform.icc"

#endif /* LTRANSFORM_H_ */
