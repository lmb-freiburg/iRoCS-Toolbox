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
 * -----------------------------
 * - Copyright Acknowledgement -
 * -----------------------------
 *
 * The Analyze header type definitions in "BlitzAnalyzeType.hh" and
 * the methode "saveArrayWithElemSize" in "BlitzAnalyzeFile.icc"
 * are mainly based on the Analyze 7.5 File Format document
 * http://www.mayo.edu/bir/PDF/ANALYZE75.pdf
 * (.../libBlitzAnalyze/docs/ANALYZE75.pdf)
 *
 * It contains the following copyright notice regarding the
 * header file format:
 *
 * ANALYZE TM Header File Format
 * (c) Copyright, 1986-1995
 * Biomedical Imaging Resource
 * Mayo Foundation
 *
 **************************************************************************/

#ifndef BLITZANALYZETYPE_HH
#define BLITZANALYZETYPE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

 /*======================================================================*/
 /*!
  *  Type definitions for Analyze file header.
  */
  /*======================================================================*/

struct header_key 				/* header key */
{ 								/* off + size */
  int sizeof_hdr; 			/* 0 + 4 */
  char data_type[10]; 		/* 4 + 10 */
  char db_name[18];			/* 14 + 18 */
  int extents; 				/* 32 + 4 */
  short int session_error; 	/* 36 + 2 */
  char regular; 				/* 38 + 1 */
  char hkey_un0; 				/* 39 + 1 */
};								/* total=40 bytes */

struct image_dimension { 								/* off + size */
  short int dim[8]; 			/* 0 + 16 */
//	short int unused8; 			/* 16 + 2 */
//	short int unused9; 			/* 18 + 2 */
//	short int unused10; 		/* 20 + 2 */
//	short int unused11; 		/* 22 + 2 */
//	short int unused12; 		/* 24 + 2 */
//	short int unused13; 		/* 26 + 2 */
//	short int unused14; 		/* 28 + 2 */
  char vox_units[4];
  char cal_units[8];
  short int unused1;
  short int datatype; 		/* 30 + 2 */
  short int bitpix; 			/* 32 + 2 */
  short int dim_un0; 			/* 34 + 2 */
  float pixdim[8]; 			/* 36 + 32 */
  /*
  pixdim[] specifies the voxel dimensitons:
  pixdim[1] - voxel width
  pixdim[2] - voxel height
  pixdim[3] - interslice distance
  ...etc
  */
  float vox_offset; 			/* 68 + 4 */
  float funused1; 			/* 72 + 4 */
  float funused2; 			/* 76 + 4 */
  float funused3; 			/* 80 + 4 */
  float cal_max; 				/* 84 + 4 */
  float cal_min; 				/* 88 + 4 */
  float compressed;			/* 92 + 4 */
  float verified; 			/* 96 + 4 */
  int glmax, glmin; 			/* 100 + 8 */
}; 								/* total=108 bytes */

struct data_history { 								/* off + size */
  char descrip[80]; 			/* 0 + 80 */
  char aux_file[24]; 			/* 80 + 24 */
  char orient; 				/* 104 + 1 */
  char originator[10]; 		/* 105 + 10 */
  char generated[10]; 		/* 115 + 10 */
  char scannum[10]; 			/* 125 + 10 */
  char patient_id[10]; 		/* 135 + 10 */
  char exp_date[10]; 			/* 145 + 10 */
  char exp_time[10]; 			/* 155 + 10 */
  char hist_un0[3]; 			/* 165 + 3 */
  int views; 					/* 168 + 4 */
  int vols_added; 			/* 172 + 4 */
  int start_field; 			/* 176 + 4 */
  int field_skip; 			/* 180 + 4 */
  int omax, omin; 			/* 184 + 8 */
  int smax, smin; 			/* 192 + 8 */
};

struct dsr {
  struct header_key hk;         /* 0 + 40 */
  struct image_dimension dime;  /* 40 + 108 */
  struct data_history hist;     /* 148 + 200 */
};                              /* total= 348 bytes */

/* Acceptable values for datatype */
#define DT_NONE 0
#define DT_UNKNOWN 0
#define DT_BINARY 1
#define DT_UNSIGNED_CHAR 2
#define DT_SIGNED_SHORT 4
#define DT_SIGNED_INT 8
#define DT_FLOAT 16
#define DT_COMPLEX 32
#define DT_DOUBLE 64
#define DT_RGB 128
#define DT_ALL 255

typedef struct {
  float real;
  float imag;
} COMPLEX;

#endif
