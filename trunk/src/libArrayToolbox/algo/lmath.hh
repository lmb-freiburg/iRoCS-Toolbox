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
 * lMath.hh
 *
 *  Created on: Nov 1, 2010
 *      Author: liu
 */

#ifndef LMATH_HH_
#define LMATH_HH_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>

#define SORT_eigenValueBy_ascendingAlgebra 0
#define SORT_eigenValueBy_descendingAlgebra 1
#define SORT_eigenValueBy_descendingABS 2

/* Calculation of eigenvalues (D) and eigenvectors (V) of 3x3 matrix A
 */
void eigen_decomposition(
    double A[][3], double V[][3], double d[], int sortMode);

template<typename DataT, int Dim>
void eig3DSym(
    blitz::Array<blitz::TinyVector<DataT,6>,3> &hessianMatrix,
    blitz::Array<blitz::TinyVector<DataT,Dim>,Dim> &hessianEigValues,
    blitz::Array<blitz::TinyVector<DataT,Dim>,Dim> &v1,
    blitz::Array<blitz::TinyVector<DataT,Dim>,Dim> &v2,
    blitz::Array<blitz::TinyVector<DataT,Dim>,Dim> &v3, int sortMode);

#include "lmath.icc"

#endif /* LMATH_HH_ */
