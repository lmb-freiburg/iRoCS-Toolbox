/**************************************************************************
 *
 * Copyright (C) 2004-2015 Olaf Ronneberger, Florian Pigorsch, Jörg Mechnich,
 *                         Thorsten Falk
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
**       Title: Helper functions for all Require-Classes
**    $RCSfile$
**   $Revision: 2825 $$Name$
**       $Date: 2009-09-15 17:04:15 +0200 (Tue, 15 Sep 2009) $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.1  2004/08/26 08:36:59  ronneber
**  initital import
**
**
**
**************************************************************************/

#ifndef REQUIREHELPERS_HH
#define REQUIREHELPERS_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#define CHECK_CLASS_TEMPLATE1( c) c ___requirement1;
#define CHECK_CLASS_TEMPLATE2( c) c ___requirement2;
#define CHECK_CLASS_TEMPLATE3( c) c ___requirement3;
#define CHECK_MEMBER_TEMPLATE( c) if(false){ c ___req; svt_check::avoidUnunsedVariableWarning(___req);}

// awfull workaround for Classes with 2 template parameters
#define CHECK_MEMBER_TEMPLATE_2PARAM( c1,c2) if(false){ c1,c2 ___req; svt_check::avoidUnunsedVariableWarning(___req);}

#define CHECK_CLASS_TEMPLATE_2PARAM1( c1,c2) c1,c2 ___requirement1b;}
#define CHECK_CLASS_TEMPLATE_2PARAM2( c1,c2) c1,c2 ___requirement2b;}
#define CHECK_CLASS_TEMPLATE_2PARAM3( c1,c2) c1,c2 ___requirement3b;}


namespace svt_check
{
  template<typename T> void avoidUnunsedVariableWarning( const T&){}    
}

#endif
