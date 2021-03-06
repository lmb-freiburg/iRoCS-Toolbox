/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
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

#include "BlitzH5Traits.hh"

std::vector<hsize_t> BlitzH5Traits<unsigned char>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<unsigned char>::h5Type()
{
  return H5T_NATIVE_UCHAR;
}
    
void *BlitzH5Traits<unsigned char>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<unsigned char>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<signed char>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<signed char>::h5Type()
{
  return H5T_NATIVE_UCHAR;
}
    
void *BlitzH5Traits<signed char>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<signed char>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<char>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<char>::h5Type()
{
  return H5T_NATIVE_CHAR;
}
    
void *BlitzH5Traits<char>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<char>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<unsigned short>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<unsigned short>::h5Type()
{
  return H5T_NATIVE_USHORT;
}

void *BlitzH5Traits<unsigned short>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<unsigned short>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<short>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<short>::h5Type()
{
  return H5T_NATIVE_SHORT;
}

void *BlitzH5Traits<short>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<short>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<unsigned int>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<unsigned int>::h5Type()
{
  return H5T_NATIVE_UINT;
}

void *BlitzH5Traits<unsigned int>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<unsigned int>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<int>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<int>::h5Type()
{
  return H5T_NATIVE_INT;
}

void *BlitzH5Traits<int>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<int>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<unsigned long>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<unsigned long>::h5Type()
{
  return H5T_NATIVE_ULONG;
}

void *BlitzH5Traits<unsigned long>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<unsigned long>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<long>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<long>::h5Type()
{
  return H5T_NATIVE_LONG;
}

void *BlitzH5Traits<long>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<long>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<unsigned long long>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<unsigned long long>::h5Type()
{
  return H5T_NATIVE_ULLONG;
}

void *BlitzH5Traits<unsigned long long>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<unsigned long long>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<long long>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<long long>::h5Type()
{
  return H5T_NATIVE_LLONG;
}

void *BlitzH5Traits<long long>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<long long>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<float>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<float>::h5Type()
{
  return H5T_NATIVE_FLOAT;
}

void *BlitzH5Traits<float>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<float>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<double>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<double>::h5Type()
{
  return H5T_NATIVE_DOUBLE;
}

void *BlitzH5Traits<double>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<double>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<long double>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<long double>::h5Type()
{
  return H5T_NATIVE_LDOUBLE;
}

void *BlitzH5Traits<long double>::data(DataT &array)
{
  return reinterpret_cast<void*>(&array);
}

void const *BlitzH5Traits<long double>::data(DataT const &array)
{
  return reinterpret_cast<void const*>(&array);
}

std::vector<hsize_t> BlitzH5Traits<std::string>::h5Dims(DataT const &)
{
  std::vector<hsize_t> dims(1);
  dims[0] = 1;
  return dims;
}

hid_t BlitzH5Traits<std::string>::h5Type()
{
  return H5T_C_S1;
}

void *BlitzH5Traits<std::string>::data(DataT &)
{
  // std::string has no way of accessing the char array directly (it is not
  // even guaranteed that there is something like a countinuous char array)
  return NULL;
}

void const *BlitzH5Traits<std::string>::data(DataT const &)
{
  // std::string has no way of accessing the char array directly (it is not
  // even guaranteed that there is something like a countinuous char array)
  return NULL;
}

