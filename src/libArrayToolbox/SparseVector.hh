#ifndef ATBSPARSEVECTOR_LEGACY_HH
#define ATBSPARSEVECTOR_LEGACY_HH

#include <map>
#include <blitz/array.h>

namespace atb
{
  
  class SparseVectorError : public std::exception
  {
  public:
    
    SparseVectorError(char const *message)
            : _message(message)
          {}

    ~SparseVectorError() throw()
          {}
    
    char const *what() const throw()
          {
            return _message;
          }
    
  private:
    
    char const *_message;    
  };

  template <typename T>
  class SparseVector
  {
    
  public:
    
    static T zero;
    typedef typename std::map<size_t,T>::const_iterator const_iterator;
    typedef typename std::map<size_t,T>::const_iterator iterator;    

    SparseVector();
    SparseVector(size_t size);
    SparseVector(SparseVector const &v);
    SparseVector(blitz::Array<T,1> const &v);
    template<int Dim>
    SparseVector(blitz::TinyVector<T,Dim> const &v);
    ~SparseVector();

    void resize(size_t size);
    size_t size() const;

    bool nonZero(size_t c) const;

    T const &operator()(size_t c) const;
    T &operator()(size_t c);

    size_t nNonZeroEntries() const;
    void clear();
    const_iterator begin() const;
    const_iterator end() const;
    iterator begin();
    iterator end();
    const_iterator erase(size_t c);

    blitz::Array<T,1> toBlitz() const;

    void operator+=(SparseVector<T> const &v);
    void operator-=(SparseVector<T> const &v);
    void operator*=(T const &alpha);
    void operator/=(T const &alpha);

  private:
    
    size_t _size;
    std::map<size_t,T> _data;
    
  };
  
  template<typename T>
  T dot(SparseVector<T> const &v1, SparseVector<T> const &v2);
  template<typename T>
  T dot(blitz::Array<T,1> const &v1, SparseVector<T> const &v2);
  template<typename T>
  T dot(SparseVector<T> const &v1, blitz::Array<T,1> const &v2);
  template<typename T, int Dim>
  T dot(blitz::TinyVector<T,Dim> const &v1, SparseVector<T> const &v2);
  template<typename T, int Dim>
  T dot(SparseVector<T> const &v1, blitz::TinyVector<T,Dim> const &v2);
  
  template<typename T>
  SparseVector<T> operator*(SparseVector<T> const &v, T const &alpha);

  template<typename T>
  SparseVector<T> operator*(T const &alpha, SparseVector<T> const &v);

}

#include "SparseVector.icc"

#endif
