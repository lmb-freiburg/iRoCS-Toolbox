/**************************************************************************
 *
 * Copyright (C) 2010 Robert Bensch
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

/**************************************************************************
 **	Title: BlitzAnalyzeFile implementation
 **	$RCSfile$
 **	$Revision: $$Name$
 **	$Date: $
 **	Copyright: GPL $Author: $
 **	Description:
 **
 **************************************************************************/

#include "BlitzAnalyzeFile.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
BlitzAnalyzeFile::BlitzAnalyzeFile( const std::string& fName)
	: _fName( fName),
	  _saveArrayWithElemSize_called( false)
{

}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
BlitzAnalyzeFile::~BlitzAnalyzeFile()
{

}
