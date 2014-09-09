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
