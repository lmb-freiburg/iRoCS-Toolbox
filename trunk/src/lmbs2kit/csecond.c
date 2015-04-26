/***************************************************************************
  **************************************************************************
  
                           S2kit 1.0

          A lite version of Spherical Harmonic Transform Kit

   Peter Kostelec, Dan Rockmore
   {geelong,rockmore}@cs.dartmouth.edu
  
   Contact: Peter Kostelec
            geelong@cs.dartmouth.edu
  
   Copyright 2004 Peter Kostelec, Dan Rockmore

   This file is part of S2kit.

   S2kit is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   S2kit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with S2kit; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   See the accompanying LICENSE file for details.
  
  ************************************************************************
  ************************************************************************/



/*

  a function used for timing; provided by Mark Taylor

      real*8 second
      before = second()
      code_to_time
      after = second()
      cpu_time = after - before

 *----------------------------------------------------------------------
 * csecond.c
 *----------------------------------------------------------------------
 * Last written:
 * Time-stamp: "1995/11/10 16:52:56 thibaud@kether.cgd.ucar.edu"
 *----------------------------------------------------------------------
 */

/* This has been changed to a dummy function, because it's incompatible
   with every system we use
*/

double csecond()
{
  return 0.0;
}








