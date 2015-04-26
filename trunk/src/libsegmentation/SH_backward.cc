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

Source code to test FORWARD spherical harmonic transform
using the seminaive and naive algorithms coded up during October, 1995.
It will compute the spherical coefficients of the function whose
sample values are given in the named input file, and will write
those coefficients in the named output file.

In its current state, assuming that will seminaive at ALL orders.
If you wish to change this, modify the CUTOFF variable in this file, i.e.
cutoff = at what order to switch from semi-naive to naive algorithm

WILL PRECOMPUTE IN MEMORY EVERYTHING BEFORE DOING TRANSFORM!

Sample call

 test_s2_semi_memo_for sampleFile outputFile bw [output_format]

 test_s2_semi_memo_for y31_bw8.dat y31_coef.dat 8

 output_format is an optional argument:
 = 0 -> coefficients in "code" order,
        i.e. suitable for test_s2_semi_memo_inv

 = 1 -> coefficients in prettier "human" order,
        i.e. if f_{l,m} is the coefficient of degree l, order m,
	then the coefficients will be arranged this way:
	f_{0,0},
	f_{1,-1}, f_{1,0}, f_{1,1},
	f_{2,-2}, f_{2,-1}, f_{2,0}, f_{2,1}, f_{2,2},
	...

The default output format is "code" order. To help you out,
the function

 seanindex(m, l, bw)

defined in FST_semi_memo.c, returns the array index of
the coefficient f_{l,m} (so you know where it goes). Since we're
talking in C here, the indexing begins at 0.

The format of the input sample file will be an interleaved
real/imaginary parts of the function samples arranged in
"latitude-major" format, i.e. the function will be sampled
in this order:

  (theta_0, phi_0)
  (theta_0, phi_1)
  (theta_0, phi_2)
  ...
  (theta_0, phi_{bw-1})
  (theta_1, phi_0)  
  (theta_1, phi_1)
  ...
  (theta_{bw-1}, phi_{bw-1})

  where theta_k = pi*(2*j+1)/(4*bw)
        phi_j = 2*pi*k/(2*bw)

The format of the output depends on whether or not it's human
or code-ordered. For code-ordered, it's interleaved real/imaginary.
For human-ordered, it's verbose and it's one coefficient per line,
e.g. l = 2  m = 1  2.3 + 6 I


*/

#include "SH_backward.hh"

#include <iostream>
#include <cstdlib>

SH_backward::CacheCleaner::CacheCleaner()
{}

void SH_backward::CacheCleaner::init()
{
  int result = 42;
  result++;
}

SH_backward::CacheCleaner::~CacheCleaner()
{
  std::cout << "Cleaning up Cache with " << SH_backward_cache.size()
            << " entries" << std::endl;
  while (SH_backward_cache.size() > 0)
  {
    delete SH_backward_cache.begin()->second;
    SH_backward_cache.erase(SH_backward_cache.begin());
  }
}

SH_backward::SH_backward(SH_backward const &)
{}

SH_backward::SH_backward(int bw)
  : _bw(bw)
{
  static CacheCleaner cleaner;
  cleaner.init();
  cutoff = bw;
  size = 2*bw;
  
  /* allocate memory */
  rdata = (double *) malloc(sizeof(double) * (size * size));
  idata = (double *) malloc(sizeof(double) * (size * size));
  rcoeffs = (double *) malloc(sizeof(double) * (bw * bw));
  icoeffs = (double *) malloc(sizeof(double) * (bw * bw));
  weights = (double *) malloc(sizeof(double) * 4 * bw);
  
  seminaive_naive_tablespace =
      (double *) malloc(sizeof(double) *
			(Reduced_Naive_TableSize(bw,cutoff) +
			 Reduced_SpharmonicTableSize(bw,cutoff)));
  workspace = (double *) malloc(sizeof(double) * 
                                ((8 * (bw*bw)) + 
                                 (10 * bw)));
  
  trans_seminaive_naive_tablespace =
      (double *) malloc(sizeof(double) *
			(Reduced_Naive_TableSize(bw,cutoff) +
			 Reduced_SpharmonicTableSize(bw,cutoff)));
  
  
  
  /****
       At this point, check to see if all the memory has been
       allocated. If it has not, there's no point in going further.
  ****/
  
  if ( (rdata == NULL) || (idata == NULL) ||
       (rcoeffs == NULL) || (icoeffs == NULL) ||
       (weights == NULL) ||
       (seminaive_naive_tablespace == NULL) ||
       (trans_seminaive_naive_tablespace == NULL) ||
       (workspace == NULL) )
  {
    perror("Error in allocating memory");
    exit( 1 ) ;
  }
  
  /* now precompute the Legendres */
  fprintf(stdout,"Generating seminaive_naive tables...\n");
  seminaive_naive_table = SemiNaive_Naive_Pml_Table(
      bw, cutoff, seminaive_naive_tablespace, workspace);
  fprintf(stdout,"Generating trans_seminaive_naive tables...\n");
  trans_seminaive_naive_table =
      Transpose_SemiNaive_Naive_Pml_Table(
          seminaive_naive_table, bw, cutoff,
          trans_seminaive_naive_tablespace, workspace);
  /* construct fftw plans */
  
  /* make DCT plan -> note that I will be using the GURU
     interface to execute these plans within the routines*/
  
  /* forward DCT */
  idctPlan = fftw_plan_r2r_1d( 2*bw, weights, rdata,
                               FFTW_REDFT01, FFTW_ESTIMATE ) ;
  
  /*
    fftw "preamble" ;
    note that this plan places the output in a transposed array
  */
  rank = 1 ;
  dims[0].n = 2*bw ;
  dims[0].is = 2*bw ;
  dims[0].os = 1 ;
  howmany_rank = 1 ;
  howmany_dims[0].n = 2*bw ;
  howmany_dims[0].is = 1;
  howmany_dims[0].os = 2*bw ;
  
  /* inverse fft */
  ifftPlan = fftw_plan_guru_split_dft( rank, dims,
                                       howmany_rank, howmany_dims,
                                       rdata, idata,
                                       workspace, workspace+(4*bw*bw),
                                       FFTW_ESTIMATE );
  
  
  /* now make the weights */
  makeweights( bw, weights );
}

SH_backward &SH_backward::instance(int bw)
{
  if(SH_backward_cache.find(bw)==SH_backward_cache.end())
      SH_backward_cache[bw]= new SH_backward(bw);
  return *SH_backward_cache[bw];
}
      
SH_backward::~SH_backward()
{
  /* clean up */
  fftw_destroy_plan( ifftPlan );
  fftw_destroy_plan( idctPlan );
  
  free(workspace);  
  free(trans_seminaive_naive_table);
  free(trans_seminaive_naive_tablespace);
  free(seminaive_naive_table);
  free(seminaive_naive_tablespace);
  free(weights);
  free(icoeffs);
  free(rcoeffs);
  free(idata);
  free(rdata);
}

int SH_backward::sh_semi_memo_back(double const *indata, double *outdata)
{
  int j=0;
  /* now read in coefficients */
  for(i = 0 ; i < _bw*_bw ; i++ )
  {  
    /* first the real part of the sample */
    *(rcoeffs + i)=indata[j];
    j++; 
    /* now the imaginary part */
    *(icoeffs + i)=indata[j];
    j++;
  }
  
  /* now do the forward spherical transform */
  //tstart = csecond();
  InvFST_semi_memo(
      rcoeffs, icoeffs, rdata, idata, _bw, trans_seminaive_naive_table,
      workspace, 0, cutoff, &idctPlan, &ifftPlan);
  // tstop = csecond();
    
  // fprintf(stdout,"forward time \t = %.4e\n", tstop - tstart);
  
  // fprintf(stdout,"about to write out coefficients\n");
  
  /* now write out coefficients, but in what format ? */
  
  j=0;
  for( i = 0 ; i < 2*_bw*2*_bw ; i ++ )
  {
    outdata[j]=rdata[i];
    j++;
    outdata[j]=idata[i];
    j++;
  }

  return 0 ;
}

std::map<int,SH_backward*> SH_backward::SH_backward_cache = 
    std::map<int,SH_backward*>();
