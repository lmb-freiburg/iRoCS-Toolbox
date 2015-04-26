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
**       Title: KFile_BINARY
**    $RCSfile$
**   $Revision: 740 $$Name$
**       $Date: 2005-07-19 15:42:59 +0200 (Tue, 19 Jul 2005) $
**   Copyright: LGPL $Author: haasdonk $
** Description: Class for accessing complete kernel matrix from single 
**              binary float file
**              first int32 is expected to be the number of rows
**              second int32 is expected to be the number of columns
**              following array of floats is expected to be the 
**              kernel data.
**              the filename is expected to be of format
**              somefilebase-rows-00-20-cols-400-500.kmf 
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.2  2005/07/19 13:42:59  haasdonk
**  adapted header for automatic CVS-Information extraction
**
**
**************************************************************************/

#ifndef KFILE_BINARY_HH
#define KFILE_BINARY_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

// std includes
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
//#include <algorithm>
// libsvmtl includes
#include "svm_defines.hh"
//#include "SVMError.hh"
#include "ProgressReporter.hh"

// requirements of template parameters
#include "svt_check/RequireStData.hh"
#include "svt_check/RequireLoadSaveParameters.hh"
#include "svt_check/RequireNameDescriptionParamInfos.hh"
#include "svt_check/RequireFeatureVectorUniqueID.hh"

namespace svt
{
  class KFile_BINARY
  {

  public:
    KFile_BINARY()
            : is(0)
          {}

    KFile_BINARY( const KFile_BINARY& orig)
            : is(0)

          { 
            operator=( orig);
          }

        
    void operator=( const KFile_BINARY& orig)
          {
            is = orig.is;
            kmatrix_file = orig.kmatrix_file;
          }
    
  public:
    
    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            stData.getValue("kmatrix_file",kmatrix_file);
          };
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            stData.setValue( "kmatrix_file", kmatrix_file);
          };
    
    static std::string name()
          {
            return std::string( "binary");
          }

    static std::string description()
          {
            return std::string( "binary format");
          }        

    static void getParamInfos( std::vector<ParamInfo>& p)
          {
            p.push_back(
                ParamInfo( "kmatrix_file", "kmf", "string",
                           "filename of the float kernel-matrix"));
          }    

    // set kmatrix_filename to be used for next load_matrix 

    void setKmatrix_file(std::string kmf)
          {
            kmatrix_file = kmf;
          }


    // load float matrix from file into preallocated array. 
    //      Assuming sorted lists of IDs!!!
    
    void load_matrix(float** rowStarts,
                     std::vector<unsigned int>& gIDs1,
                     std::vector<unsigned int>& gIDs2)
          {
            // allocate temporary memory for row 
            unsigned int ID2min = gIDs2[0];
            unsigned int ID2max = gIDs2[gIDs2.size()-1];
            unsigned int ID1min = gIDs1[0];
            unsigned int ID1max = gIDs1[gIDs1.size()-1];
            unsigned int rmin,rmax,cmin,cmax;
            std::vector<unsigned int>::iterator git,git2;
            
            float* row_data = new float[ID2max-ID2min+1];
            
            // open file
            is = new std::ifstream(kmatrix_file.c_str());
            if (!is || !(*is))
            {
              std::cerr << "error in opening kernel file \n";
              exit(-1);
            }
            
            // get indices from filename
            //           e.g. MNIST-IDM-rows-10-999-cols-0-999.kmf
            
            std::string::size_type pr = kmatrix_file.find("rows",0);
            std::string::size_type pc = kmatrix_file.find("cols",0);
            std::string::size_type pe = kmatrix_file.find(".kmf",0);
            if (pr==std::string::npos || pc==std::string::npos || 
                pe==std::string::npos)
            {
              std::cerr << "error in getting index-ranges from filename \n";
              std::cerr << "required format of filename: \n";
              std::cerr << "*-rows-RMIN-RMAX-cols-CMIN-CMAX.kmf\n";
              exit(-1);
            }
            
            std::string rrange = kmatrix_file.substr(pr+5,pc-pr-6);
            std::string crange = kmatrix_file.substr(pc+5,pe-pc-5);
            
            pr = rrange.find("-",0);
            pc = crange.find("-",0);

            rmin = atol(rrange.substr(0,pr).c_str());
            rmax = atol(rrange.substr(pr+1).c_str());
            cmin = atol(crange.substr(0,pc).c_str());
            cmax = atol(crange.substr(pc+1).c_str());
            
// for debugging:
//            std::cout << "parsed rrange = " << rmin << " - " << rmax 
//                 << " , crange = " << cmin << " - " << cmax << std::endl;
            
            // check that index ranges of gID are within file-index-range!
            
            if (ID1min<rmin || ID1max>rmax || ID2min<cmin || ID2max>cmax) 
            {
              std::cerr << "index range in ID-lists exceed file-ID-range.\n";
              exit(-1);
            }
            
            //     for all lines 
            unsigned int rind = 0;
            for (git = gIDs1.begin();git!=gIDs1.end(); git++,rind++)
            { 
              // set correct position in file. Assuming 4-byte float files
              unsigned int pos_row_start = ((*git)-rmin)*4*(cmax-cmin+1);
              pos_row_start += 8; // skip leading 2 integers
              is->seekg(pos_row_start + 4 * (ID2min-cmin) );             
              //          read row_data 
              is->read( (char*)row_data,(ID2max-ID2min+1)*4);
              //          select required entries and assign to output-matrix
              float* rStart = rowStarts[rind];
              for (unsigned int cind = 0; cind < gIDs2.size();cind++)
                  rStart[cind] = row_data[gIDs2[cind]-ID2min];     
            }
            
            delete[] row_data;
            is->close();
            delete is;
            is = 0;
          }
     
    
  private:    
    std::ifstream* is;
    std::string kmatrix_file;
  };
  
}

#endif
