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

#include "lBlitzRandomForest.hh"

#include "helper.hh"

#include <libBlitzHdf5/BlitzHdf5Light.hh>

void trainRFSimple(
    lRandomForest &forest, blitz::Array<float,2> &Features,
    blitz::Array<int,1> &Labels, int m_try, int labelRangeLow,
    int labelRangeHigh)
{
  // make float[][] array
  if (!Features.isStorageContiguous())
      Features.reference(Features.copy());
  int m_ = Features.extent(0);
  int n = Features.extent(1);
  if (m_try < 0)
      m_try = floor(sqrt(double(n)));
  int max_depth = 100; //not used now as we have linked tree.
  int min_samples = 1;
  int num_grid = 20;

  blitz::Array<bool, 1> flagValid(m_);
  flagValid = (Labels >= labelRangeLow) && (Labels <= labelRangeHigh);
  int m = blitz::sum(flagValid);

  float** X = new float*[m];
  int * L = new int[m];
  int count = 0;
  for (int i = 0; i < m_; i++)
  {
    if (Labels(i) >= labelRangeLow && Labels(i) <= labelRangeHigh)
    {
      X[count] = Features.data() + i * n;
      L[count++] = Labels(i);
    }
  }

  assert(count == m);
  forest.trainForest(X, L, m, n, m_try, max_depth, min_samples, num_grid);
}

void saveRFSimple(lRandomForest& forest, std::string fileName)
{
  // make float[][] array
  std::stringstream* ss;
  forest.saveForest(ss);
  try
  {
    BlitzH5File outFile(fileName, BlitzH5File::WriteOrNew);
    outFile.writeDataset(ss[0].str(), "forestInfo");
    for (int i = 0; i < forest._nTree; i++)
    {
      std::stringstream dsName;
      dsName << "tree_" << i;
      outFile.writeDataset(ss[i + 1].str(), dsName.str());
    }
  }
  catch (BlitzH5Error& e)
  {
    std::cerr << "Something went wrong while writing '" << fileName
              << "': " << e.str() << std::endl;
  }
}

void loadRFSimple(lRandomForest& forest, std::string fileName, int nTree)
{
  // make float[][] array
  std::stringstream* ss;
  ss = new std::stringstream[nTree + 1];
  try
  {
    BlitzH5File inFile(fileName);
    std::string str;
    inFile.readDataset(str, "forestInfo");
    ss[0] << str;
    for (int i = 0; i < nTree; i++)
    {
      std::stringstream dsName;
      dsName << "tree_" << i;
      std::string str;
      inFile.readDataset(str, dsName.str());
      ss[i + 1] << str;
    }
  }
  catch (BlitzH5Error& e)
  {
    std::cerr << "Something went wrong while loading '" << fileName
              << "': " << e.str() << std::endl;
  }

  forest.loadForest(ss);
}
