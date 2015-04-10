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

#ifndef ATBRANDOM_HH
#define ATBRANDOM_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>

namespace atb 
{

/*======================================================================*/
/*! 
 *   Compute a normally distributed random variable, with given mean and
 *   standard deviation
 *
 *   \param mean   Mean of the gaussian distribution to draw a sample of
 *   \param stddev Standard Deviation of the gaussian distribution to draw
 *                 a sample of
 *
 *   \return A sample point drawn from a normal distribution
 */
/*======================================================================*/
  double gaussRandomNumber(const double mean, const double stddev);

/*======================================================================*/
/*! 
 *   Draw random vector from a uniform distribution with given upper
 *   bound (lower bound is zero)
 *
 *   \param maxValue    The upper bound of the vector components
 *
 *   \return A random vector from a uniform distribution
 */
/*======================================================================*/
  template<int Dim>
  blitz::TinyVector<double,Dim>
  drawRandomVector(const blitz::TinyVector<double,Dim>& maxValue);

/*======================================================================*/
/*! 
 *   Draw a random vector from a gaussian random variable with given
 *   mean and covariance matrix
 *
 *   \param mean     Mean of the gaussian distribution
 *   \param cov      Covariance matrix of the gaussian distribution
 *
 *   \return A random vector drawn from the gaussian distribution
 */
/*======================================================================*/
  template<int Dim>
  blitz::TinyVector<double,Dim>
  drawRandomVector(const blitz::TinyVector<double,Dim>& mean,
                   const blitz::TinyMatrix<double,Dim,Dim>& cov);

}

#include "Random.icc"

#endif
