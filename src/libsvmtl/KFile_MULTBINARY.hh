/**************************************************************************
**       Title: KFile_MULTBINARY.hh 
**    $RCSfile$
**   $Revision: 741 $$Name$
**       $Date: 2005-07-21 08:34:49 +0200 (Thu, 21 Jul 2005) $
**   Copyright: LGPL $Author: haasdonk $
** Description: Class for accessing complete kernel matrices from several 
**              binary float files. In each file:
**              first int32 is expected to be the number of rows
**              second int32 is expected to be the number of columns
**              following array of floats is expected to be the 
**              kernel data.
**              the filename is expected to be of type
**              somefilebase-rows-00-20-cols-400-500.kmf
**              all files in the directory passed by the option 
**              -kmd are parsed like this and should be an "axis"-parallel
**              decomposition of the whole kernel matrix 
**
**-------------------------------------------------------------------------
**
**  $Log$
**  Revision 1.3  2005/07/21 06:34:49  haasdonk
**  modified directory access in KFile_MULTBINARY.hh, now following
**  POSIX standard
**
**  Revision 1.2  2005/07/19 13:42:59  haasdonk
**  adapted header for automatic CVS-Information extraction
**
**
**************************************************************************/

#ifndef KFILE_MULTBINARY_HH
#define KFILE_MULTBINARY_HH

// std includes
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include "KFile_BINARY.hh"
#include <dirent.h>

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
  class KFile_MULTBINARY
  {
    
  public:
    KFile_MULTBINARY()
          {}

    KFile_MULTBINARY( const KFile_MULTBINARY& orig)
          { 
            operator=( orig);
          }

        
    void operator=( const KFile_MULTBINARY& orig)
          {
            kmatrix_dir = orig.kmatrix_dir;
            kfile_bin = orig.kfile_bin;
          }
    
  public:
    
//    ~KFile_MULTBINARY()
//          {
//          };

    template<typename STDATA>
    void loadParameters( STDATA& stData)
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            stData.getValue("kmatrix_dir",kmatrix_dir);
          };
    
    template<typename STDATA>
    void saveParameters( STDATA& stData) const
          {
            CHECK_MEMBER_TEMPLATE( svt_check::RequireStData<STDATA>);
            stData.setValue( "kmatrix_dir", kmatrix_dir);
          };
    
    static std::string name()
          {
            return std::string( "multbinary");
          }

    static std::string description()
          {
            return std::string( "multiple binary format");
          }        

    static void getParamInfos( std::vector<ParamInfo>& p)
          {
            p.push_back(
                ParamInfo( "kmatrix_dir", "kmd", "string",
                           "name of directory with float kernel-matrices"));
          }    

    // load float matrix from files into preallocated array. 
    //            Assuming sorted lists of IDs!!!
    
    void load_matrix(float** rowStarts,
                     std::vector<unsigned int>& gIDs1,
                     std::vector<unsigned int>& gIDs2)
          {
            
            // load all filenames from path
            
//            struct dirent **namelist;
            DIR *dir;
            struct dirent *current_name;
            
//            int n;
            
            // cut eventual "/" or "\" at end of path
            while (( !kmatrix_dir.empty()) 
                && ((kmatrix_dir[kmatrix_dir.size()-1] == '/' ) ||
                    (kmatrix_dir[kmatrix_dir.size()-1] == '\\' ) ))
                    kmatrix_dir.erase(kmatrix_dir.end()-1);

            dir = opendir(kmatrix_dir.c_str());
            
//            n = scandir(kmatrix_dir.c_str(), &namelist, 0, alphasort);
            if (dir == NULL)
            {
              std::cerr << "error in reading kmf-directory\n";
              exit(-1);
            }
            // extract filebase and initialize collection of index-ranges
            
            // search first file without leading "."
            int find_min = -1;
            std::string file1;
            do
            {
              current_name = readdir(dir);              
              find_min +=1;
//              file1 = std::string(namelist[find_min]->d_name);
              file1 = std::string(current_name->d_name);
//              std::cout << " processing file " << 
//              current_name->d_name << " \n ";
//              std::string fn(current_name->d_name);
              
            } while ((current_name!=NULL) && 
                     (file1.find("rows",0)==std::string::npos));            
            closedir(dir);

            //           if (current_name != NULL)
            //    std::cout << " after closedir, found first kmf-file " <<
            //        current_name->d_name << "\n ";
            // else
            //    std::cout << " after closedir, found no kmf-file !\n ";
            
            std::string::size_type pr = file1.find("rows",0);
            std::string filebase = file1.substr(0,pr);
            std::vector<unsigned int> min_rrange, max_rrange, min_crange,max_crange;
            std::vector<unsigned int>::iterator IDi1,IDi2;
            std::vector<std::vector<unsigned int> > gIDs1_parts, gIDs2_parts;
            float** rowStarts_shifted;                        
	    rowStarts_shifted = new float*[gIDs1.size()];
            
	    // for all files read in chunk of the matrix
            
            dir = opendir(kmatrix_dir.c_str());           
            while (current_name!=NULL) // is initially not NULL
//            for (int fnumber=find_min;fnumber<n;fnumber++)
            {
              bool is_kmf_file;
              do  // search next filename with "rows" in it
              {
                is_kmf_file = false;
                current_name = readdir(dir); 
                if (current_name!=NULL)
                {                  
//                  std::cout << " processing dir entry " << 
//                  current_name->d_name << " \n ";
                  file1 = std::string(current_name->d_name);
                  if (file1.find("rows",0)!=std::string::npos)
                      is_kmf_file = true;                  
                }
                
              } while ((current_name!=NULL) && !is_kmf_file);

              if (is_kmf_file) // found a following file
              {
                std::string fn(current_name->d_name);
                
                std::string::size_type pr = fn.find("rows",0);
                std::string::size_type pc = fn.find("cols",0);
                std::string::size_type pe = fn.find(".kmf",0);
                if (pr==std::string::npos || pc==std::string::npos || 
                    pe==std::string::npos)
                {
                  std::cerr << "KFile_MULTBINARY.hh: \n";
                  std::cerr << "error in getting index-ranges from filename " 
                            << fn << "\n";     
                  std::cerr << "required format of filename: \n";
                  std::cerr << filebase << "-rows-RMIN-RMAX-cols-CMIN-CMAX.kmf\n";
                  exit(-1);
                }
                
                std::string rrange = fn.substr(pr+5,pc-pr-6);
                std::string crange = fn.substr(pc+5,pe-pc-5);
                
                pr = rrange.find("-",0);
                pc = crange.find("-",0);
                
                unsigned int rmin = atol(rrange.substr(0,pr).c_str());
                unsigned int rmax = atol(rrange.substr(pr+1).c_str());
                unsigned int cmin = atol(crange.substr(0,pc).c_str());
                unsigned int cmax = atol(crange.substr(pc+1).c_str());              
                
                // extract all 'ranges' in first direction
                IDi1 = find(min_rrange.begin(),min_rrange.end(),rmin);              
                if (IDi1!=min_rrange.end()) // if already found
                {
                  // check whether corresponding entries coincide
                  int po = IDi1-min_rrange.begin();
                  if ((max_rrange[po]!=rmax))
                  { 
                    std::cerr << "error in loading files: " <<
                        "rows-ranges not coinciding: " <<
                        "old_rr = (" << 
                        min_rrange[po] << ","<< max_rrange[po] << "),"<<
                        "new_rr = (" << 
                        rmin << ","<< rmax << ")\n";
                    exit(-1);
                  }
                }
                else // insert into list
                {
                  min_rrange.push_back(rmin);
                  max_rrange.push_back(rmax);
                }
                
                
                // extract all 'ranges' in second direction
                IDi2 = find(min_crange.begin(),min_crange.end(),cmin);              
                if (IDi2!=min_crange.end()) // if already found
                {
                  // check whether corresponding entries coincide
                  int po = IDi2-min_crange.begin();
                  if ((max_crange[po]!=cmax))
                  { 
                    std::cerr << "error in loading files: " <<
                        "cols-ranges not coinciding: " <<
                        "old_cr = (" << 
                        min_crange[po] << ","<< max_crange[po] << "),"<<
                        "new_cr = (" << 
                        cmin << ","<< cmax << ")\n";
                    exit(-1);
                  }
                }
                else // insert into list
                {
                  min_crange.push_back(cmin);
                  max_crange.push_back(cmax);
                }  
              } // end current_name!=0
              
            } // end loop over files
            
              // determine all subvectors of gIDs1 in gIDs1_parts           
            
            gIDs1_parts.resize(min_rrange.size());
            for (unsigned int i=0;i<min_rrange.size();i++)
            {
              IDi1 = gIDs1.begin();
              IDi2 = gIDs1.end();
              
              while (((*IDi1)<min_rrange[i]) && (IDi1!= gIDs1.end()))
		  IDi1++;
              while (((*(IDi2-1))>max_rrange[i]) && ((IDi2-1)!= gIDs1.begin()))
		  IDi2--;
              
              if ((IDi2>IDi1) && (*IDi1)<=max_rrange[i])
              {
                gIDs1_parts[i].resize(IDi2-IDi1);
                copy(IDi1,IDi2,gIDs1_parts[i].begin());
// for debugging:
//                    std::cout << "min = " << *(gIDs1_parts[i].begin()) <<
//                       ", max = " << *(gIDs1_parts[i].end()-1)  << " in \n";
                
              }
// for debugging:
//                std::cout << "   next part of rranges ("<< 
//                    min_rrange[i]<< "," << max_rrange[i] << ") = " <<
//                    gIDs1_parts[i].size() << " gIDs \n";
              
            }
	    
	    // determine all subvectors of gIDs2             
	    gIDs2_parts.resize(min_crange.size());
	    for (unsigned int i=0;i<min_crange.size();i++)
            {
              IDi1 = gIDs2.begin();
              IDi2 = gIDs2.end()-1;
              
              while (((*IDi1)<min_crange[i]) && (IDi1!= gIDs2.end()))
		  IDi1++;
              while (((*IDi2)>max_crange[i]) && (IDi2!= gIDs2.begin()))
		  IDi2--;
              IDi2++; // point behind the range to be copied
              
              if ((IDi2>IDi1) && (*IDi1)<=max_crange[i])
              {
                gIDs2_parts[i].resize(IDi2-IDi1);
                copy(IDi1,IDi2,gIDs2_parts[i].begin());
// for debugging:
//                    std::cout << "min = " << *(gIDs2_parts[i].begin()) << 
//                        ", max = " << *(gIDs2_parts[i].end()-1)  << " in\n";
              }
// for debugging:
//                std::cout << "   next part of cranges ("<< 
//                    min_crange[i]<< "," << max_crange[i] << ") = " <<
//                    gIDs2_parts[i].size() << " gIDs \n";
            }
	    
            // check that sum of all partial sequences equals input-sequences!!
            unsigned int sum1 = 0;            
	    for (unsigned int i=0;i<min_rrange.size();i++)
                sum1 +=gIDs1_parts[i].size();
            
            unsigned int sum2 = 0;            
	    for (unsigned int i=0;i<min_crange.size();i++)
                sum2 +=gIDs2_parts[i].size();
            
            if ((sum1!=gIDs1.size()) || (sum2!=gIDs2.size()))
            {
              std::cerr << "error in determining gID subsequences!\n";
              exit(-1);
            }

	    // for all ind2-ranges read in blocks from files
	    for (unsigned int i=0;i< gIDs2_parts.size();i++)
	      if (!gIDs2_parts[i].empty())
		{
		  //   generate temporary vector of pointers into array
		  unsigned int shift2;
		  IDi2 = find(gIDs2.begin(),gIDs2.end(),gIDs2_parts[i][0]);  
		  shift2 = IDi2-gIDs2.begin(); 
		  for (unsigned int j=0;j<gIDs1.size();j++)
		    rowStarts_shifted[j]=rowStarts[j]+shift2;
		  
		  //   for all ind1-ranges read corresponding submatrix
		  for (unsigned int j=0;j<gIDs1_parts.size();j++)
		    if (!gIDs1_parts[j].empty()) 
		      {
			unsigned int shift1;
			IDi1 = find(gIDs1.begin(),gIDs1.end(),
				    gIDs1_parts[j][0]);  
			shift1 = IDi1-gIDs1.begin(); 
			
			//     set corresponding filename and read array
			//     load matrix
			std::ostringstream o;
			o << kmatrix_dir << "/" << filebase << 
			  "rows-" << min_rrange[j] << "-" << max_rrange[j] << 
			  "-cols-" << min_crange[i] << "-" << max_crange[i] << 
			  ".kmf";
			std::string filename = o.str();
// for debugging
//			std::cout << "reading matrix from file "<< 
//			  filename << "\n";
			kfile_bin.setKmatrix_file(filename);
			kfile_bin.load_matrix(rowStarts_shifted+shift1,
					      gIDs1_parts[j],gIDs2_parts[i]);
		      }
		} // end loop over gIDs2
	    
            // cleanup temporary variables
	    delete[] rowStarts_shifted;            
//            for (int fnumber=0;fnumber<n;fnumber++)
//	      free(namelist[fnumber]);
//            free(namelist);
          }
    
  private:    
    std::string kmatrix_dir;
    svt::KFile_BINARY kfile_bin;    
  };
  
}

#endif
