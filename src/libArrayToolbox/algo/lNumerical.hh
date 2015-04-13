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
 * lNumerical.hh
 *
 *  Created on: Nov 19, 2010
 *      Author: liu
 */

#ifndef LNUMERICAL_HH_
#define LNUMERICAL_HH_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include "helper.hh"

/*
 * Central First Derivative
 * Parameter:   flag    -       0 - first order approximation
 *
 * on boundary, derivative is set to left or right derivative
 */
template<typename Type, typename dType, int Dim>
void nCentralFirst(
    blitz::Array<Type,Dim> const &data,
    blitz::Array<dType,Dim> &derivative, int dim, int flag = 0);

/*
 * Central First Derivative for all dimensions
 * Parameter:   flag    -       0 - first order approximation
 *
 * on boundary, derivative is set to left or right derivative
 */
template<typename Type, typename dType, int Dim>
void nCentralFirst(
    blitz::Array<Type,Dim> const &data,
    blitz::Array<blitz::TinyVector<dType,Dim>,Dim> &derivative, int flag = 0);

/*
 * Central Second Derivative
 * but with small change to Dxy(), referenced to Prof. Brox PhD thesis
 * pg109 (5.21)
 *
 * Parameter:   flag    -       0 - first order approximation
 *
 * on boundary, derivative is set to be continuous
 */
template<typename Type, typename dType, int Dim>
void nCentralSecond(
    blitz::Array<Type,Dim> const &data,
    blitz::Array<dType,Dim> &dd, int dim, int flag = 0);

/*
 * Central Second Derivative for all dimensions
 * but with small change to Dxy(), referenced to Prof. Brox PhD thesis
 * pg109 (5.21)
 *
 * Parameter:   flag    -       0 - first order approximation
 *
 * on boundary, derivative is set to be continuous
 */
template<typename Type, typename dType, int Dim>
void nCentralSecond(
    blitz::Array<Type,Dim> const &data,
    blitz::Array<blitz::TinyVector<dType,Dim>,Dim> const &derivative,
    blitz::Array<blitz::TinyVector<dType,(Dim+1)*Dim/2>,Dim>& dd,
    int flag = 0);

#include "lNumerical.icc"

#endif /* LNUMERICAL_HH_ */
