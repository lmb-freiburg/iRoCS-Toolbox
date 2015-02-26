#ifndef ATBSPARSEMATRIX_LEGACY_HH
#define ATBSPARSEMATRIX_LEGACY_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include <map>
#include <iostream>
#include <limits>
#include <cassert>

#include <libBlitzHdf5/BlitzHdf5Light.hh>

#include "SparseVector.hh"

namespace atb
{

  class SparseMatrixError : public std::exception
  {
  public:
    
    SparseMatrixError(char const *message)
            : _message(message)
          {}

    ~SparseMatrixError() throw()
          {}
    
    char const *what() const throw()
          {
            return _message;
          }
    
  private:
    
    char const *_message;    
  };

  template <typename T>
  class SparseMatrix 
  {

  public:

    SparseMatrix();
    SparseMatrix(size_t r, size_t c);
    SparseMatrix(SparseMatrix const &m);
    SparseMatrix(blitz::Array<T,2> const &m);
    template<int NRows, int NCols>
    SparseMatrix(blitz::TinyMatrix<T,NRows,NCols> const &m);
    ~SparseMatrix();

    void resize(size_t r, size_t c);
    size_t nRows() const;
    size_t nColumns() const;
    blitz::TinyVector<size_t,2> shape() const;
    size_t nNonZeroEntries() const;
    void clear();
    void clearRow(size_t r);
    void clearColumn(size_t c);

    SparseVector<T> const &row(size_t r) const;
    SparseVector<T> const &column(size_t c) const;

    bool nonZero(size_t r, size_t c) const;

    T const &operator()(size_t r, size_t c) const;
    T &operator()(size_t r, size_t c);

    typename SparseVector<T>::const_iterator erase(size_t r, size_t c);

    blitz::Array<T,2> toBlitz() const;

    void operator+=(SparseMatrix<T> const &m);
    void operator-=(SparseMatrix<T> const &m);
    void operator*=(T const &alpha);
    void operator/=(T const &alpha);

    void save(std::string const &fileName, std::string const &groupName,
              bool throwErrors = false);
    void load(std::string const &fileName, std::string const &groupName,
              bool throwErrors = false);

  private:
  
    void updateColumn(size_t c) const;

    size_t _nRows, _nColumns;
    std::vector< SparseVector<T> > _rows;

    // Internal structures that need to be modified by const methods
    mutable std::vector<bool> _columnsValid;
    mutable std::vector< SparseVector<T> > _columns;
  
  };

  template<typename T>
  SparseVector<T> operator*(SparseMatrix<T> const &A, SparseVector<T> const &x);
  template<typename T>
  blitz::Array<T,1> operator*(
      SparseMatrix<T> const &A, blitz::Array<T,1> const &x);
  template<typename T, int Dim>
  blitz::TinyVector<T,Dim> operator*(
      SparseMatrix<T> const &A, blitz::TinyVector<T,Dim> const &x);

  template<typename T>
  SparseMatrix<T> operator*(SparseMatrix<T> const &A, SparseMatrix<T> const &B);
  template<typename T>
  SparseMatrix<T> operator*(
      blitz::Array<T,2> const &A, SparseMatrix<T> const &B);
  template<typename T>
  SparseMatrix<T> operator*(
      SparseMatrix<T> const &A, blitz::Array<T,2> const &B);

}

#include "SparseMatrix.icc"

#endif
