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
 * helper.h
 *
 *  Created on: Oct 10, 2010
 *      Author: kun
 */

#ifndef HELPER_H_
#define HELPER_H_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <cstdlib>

#include <vector>
#include <blitz/array.h>

#include <libArrayToolbox/ATBTiming.hh>

#define LOG std::cerr << atb::MyDateTime::prettyDate() << " " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"

#define lmax(a,b)  (a > b ? a: b)
#define lmin(a,b)  (a < b ? a: b)

template<typename Type>
void randomSample(
    std::vector<Type>& dataset, std::vector<Type>& samples,
    int numberOfSamples);

template<typename Type>
void downSample(
    std::vector<Type>& dataset, std::vector<Type>& samples, float downSample);

template<typename Type, typename Type2, typename Type3>
void keepInRange3D(Type& input, Type2 lb, Type3 ub);

template<typename Type, typename Type2>
void keepHigherThan3D(Type& input, Type2 lb);

template<typename Type, typename Type2>
void keepLowerThan3D(Type& input, Type2 ub);

template<typename Type, typename Type2, typename Type3>
bool testInRange3D(Type& input, Type2 lb, Type3 ub);

#include "helper.icc"

#endif /* HELPER_H_ */
