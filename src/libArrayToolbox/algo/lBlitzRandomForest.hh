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

#ifndef LBLITZRANDOMFOREST_H_
#define LBLITZRANDOMFOREST_H_

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <blitz/array.h>
#include "lRandomForest.hh"

void trainRFSimple(
    lRandomForest &forest, blitz::Array<float,2> &Features,
    blitz::Array<int,1> &Labels, int m_try = -1, int labelRangeLow = 0,
    int labelRangeHigh = 9999);

void saveRFSimple(lRandomForest& forest, std::string fileName);

void loadRFSimple(lRandomForest& forest, std::string fileName, int nTree);

#endif

