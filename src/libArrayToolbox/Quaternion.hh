/**************************************************************************
 **       Title: Quaternions
 **    $RCSfile: Quaternion.hh,v $
 **   $Revision: 3841 $ $Name:  $
 **       $Date: 2010-07-02 16:25:10 +0200 (Fri, 02 Jul 2010) $
 **   Copyright: GPL $Author: tschmidt $
 ** Description:
 **
 **
 **-------------------------------------------------------------------------
 **
 **  $Log: Quaternion.hh,v $
 **  Revision 1.1.1.1  2007/11/06 13:35:27  tschmidt
 **  Initial revision
 **
 **  Revision 1.1  2006/10/18 12:52:51  tschmidt
 **  - Library split up into different parts
 **  - Moved to own location
 **
 **
 **************************************************************************/


#ifndef QUATERNION_HH
#define QUATERNION_HH

#include<stdexcept>

namespace atb
{

/*======================================================================*/
/*!
 *  \class Quaternion Quaternion.hh
 *  \brief The Quaternion class implements templated quaternions with
 *         few important operations
 *
 *  Quaternions are sort of an extension to complex numbers. They consist
 *  of 3 imaginary units i, j and k and a real part, and thus span a 4-D
 *  space. A typical quaternion looks like:
 *  q = a + ib + jc + kd
 *
 *  Operations:
 *
 *  Addition:
 *    q_1 + q_2 =      a_1 + a_2  +
 *                i * (b_1 + b_2) +
 *                j * (c_1 + c_2) +
 *                k * (d_1 + d_2)
 *
 *  Multiplication:
 *    q_1 * q_2 =      a_1 * a_2 - b_1 * b_2 - c_1 * c_2 - d_1 * d_2  +
 *                i * (a_1 * b_2 + b_1 * a_2 + c_1 * d_2 - d_1 * c_2) +
 *                j * (a_1 * c_2 - b_1 * d_2 + c_1 * a_2 + d_1 * b_2) +
 *                k * (a_1 * d_2 + b_1 * c_2 - c_1 * b_2 + d_1 * a_2)
 *
 *  Conjugate:
 *    conj(q) = a - ib - jc - kd
 *
 *
 *  The multiplication rule can be obtained from:
 *
 *    i^2 = j^2 = k^2 = -1
 *
 *    i * j = -j * i = k
 *
 *    j * k = -k * j = i
 *
 *    k * i = -i * k = j
 */
/*======================================================================*/
  template<typename Type>
  class Quaternion 
  {
  
  public:
  
    Quaternion(const Type& = Type(),
               const Type& = Type(),
               const Type& = Type(),
               const Type& = Type());

    template<typename Type2>
    Quaternion(const Quaternion<Type2>&);
    
    Type& operator()(const int index);
    Type operator()(const int index) const;
  
    Quaternion<Type>& operator=(const Type&);
    Quaternion<Type>& operator+=(const Type&);
    Quaternion<Type>& operator-=(const Type&);
    Quaternion<Type>& operator*=(const Type&);
    Quaternion<Type>& operator/=(const Type&);
  
    template<typename Type2>
    Quaternion<Type>& operator=(const Quaternion<Type2>&);
    template<typename Type2>
    Quaternion<Type>& operator+=(const Quaternion<Type2>&);
    template<typename Type2>
    Quaternion<Type>& operator-=(const Quaternion<Type2>&);
    template<typename Type2>
    Quaternion<Type>& operator*=(const Quaternion<Type2>&);
    template<typename Type2>
    Quaternion<Type>& operator/=(const Quaternion<Type2>&);
 
  
  private:
  
    Type _quat[4];
  
  };  


  template<typename Type>
  inline
  Quaternion<Type>::Quaternion(const Type& a,
                               const Type& b,
                               const Type& c,
                               const Type& d) 
  {
    _quat[0] = a;
    _quat[1] = b;
    _quat[2] = c;
    _quat[3] = d;
  }

  template<typename Type>
  template<typename Type2>
  inline
  Quaternion<Type>::Quaternion(const Quaternion<Type2>& quat)
  {
    _quat[0] = quat(0);
    _quat[1] = quat(1);
    _quat[2] = quat(2);
    _quat[3] = quat(3);  
  }

  template<typename Type>
  inline Type&
  Quaternion<Type>::operator()(const int index)
  {
    if (index < 0 || index > 3) 
    {
      std::stringstream errmsg;
      errmsg << "Index exceeds quaternion size\n"
             << "  Index must be between 0 and 3 - got " << index;
      std::out_of_range err(errmsg.str());
      throw err;
    }
    return _quat[index];
  }

  template<typename Type>
  inline Type
  Quaternion<Type>::operator()(const int index) const 
  {
    if (index < 0 || index > 3) 
    {
      std::stringstream errmsg;
      errmsg << "Index exceeds quaternion size\n"
             << "  Index must be between 0 and 3 - got " << index;
      std::out_of_range err(errmsg.str());
      throw err;
    }
    return _quat[index];
  }

  template<typename Type>
  inline Quaternion<Type>&
  Quaternion<Type>::operator=(const Type& t) 
  {
    _quat[0] = t;
    _quat[1] = Type();
    _quat[2] = Type();
    _quat[3] = Type();
    return *this;
  }

  template<typename Type>
  inline Quaternion<Type>&
  Quaternion<Type>::operator+=(const Type& t) 
  {
    _quat[0] += t;
    return *this;
  }

  template<typename Type>
  inline Quaternion<Type>&
  Quaternion<Type>::operator-=(const Type& t) 
  {
    _quat[0] -= t;
    return *this;
  }

  template<typename Type>
  inline Quaternion<Type>&
  Quaternion<Type>::operator*=(const Type& t) 
  {
    _quat[0] *= t;
    _quat[1] *= t;
    _quat[2] *= t;
    _quat[3] *= t;
    return *this;
  }

  template<typename Type>
  inline Quaternion<Type>&
  Quaternion<Type>::operator/=(const Type& t) 
  {
    _quat[0] /= t;
    _quat[1] /= t;
    _quat[2] /= t;
    _quat[3] /= t;
    return *this;
  }

  template<typename Type>
  template<typename Type2>
  inline Quaternion<Type>&
  Quaternion<Type>::operator=(const Quaternion<Type2>& t) 
  {
    _quat[0] = t(0);
    _quat[1] = t(1);
    _quat[2] = t(2);
    _quat[3] = t(3);
    return *this;
  }

  template<typename Type>
  template<typename Type2>
  inline Quaternion<Type>&
  Quaternion<Type>::operator+=(const Quaternion<Type2>& t) 
  {
    _quat[0] += t(0);
    _quat[1] += t(1);
    _quat[2] += t(2);
    _quat[3] += t(3);
    return *this;
  }

  template<typename Type>
  template<typename Type2>
  inline Quaternion<Type>&
  Quaternion<Type>::operator-=(const Quaternion<Type2>& t) 
  {
    _quat[0] -= t(0);
    _quat[1] -= t(1);
    _quat[2] -= t(2);
    _quat[3] -= t(3);
    return *this;
  }

  template<typename Type>
  template<typename Type2>
  inline Quaternion<Type>&
  Quaternion<Type>::operator*=(const Quaternion<Type2>& t) 
  {
    const Type a = _quat[0] * t(0) - _quat[1] * t(1) -
        _quat[2] * t(2) - _quat[3] * t(3);
    const Type b = _quat[0] * t(1) + _quat[1] * t(0) +
        _quat[2] * t(3) - _quat[3] * t(2);
    const Type c = _quat[0] * t(2) - _quat[1] * t(3) +
        _quat[2] * t(0) + _quat[3] * t(1);
    _quat[3] =     _quat[0] * t(3) + _quat[1] * t(2) -
        _quat[2] * t(1) + _quat[3] * t(0);
    _quat[2] = c;
    _quat[1] = b;
    _quat[0] = a;
    return *this;
  }

  template<typename Type>
  template<typename Type2>
  inline Quaternion<Type>&
  Quaternion<Type>::operator/=(const Quaternion<Type2>& t) 
  {
    *this *= conj(t);
    *this /= t(0) * t(0) + t(1) * t(1) + t(2) * t(2) + t(3) * t(3);
    return *this;
  }


// Class independent operators

  template<typename Type>
  inline Quaternion<Type>
  operator+(const Quaternion<Type>& t1,
            const Quaternion<Type>& t2) 
  {
    return Quaternion<Type>(t1) += t2;
  }

  template<typename Type>
  inline Quaternion<Type>
  operator+(const Quaternion<Type>& t1,
            const Type& t2) 
  {
    return Quaternion<Type>(t1) += t2;
  }

  template<typename Type>
  inline Quaternion<Type>
  operator+(const Type& t1,
            const Quaternion<Type>& t2) 
  {
    return Quaternion<Type>(t1) += t2;
  }

  template<typename Type>
  inline Quaternion<Type>
  operator-(const Quaternion<Type>& t1,
            const Quaternion<Type>& t2) 
  {
    return Quaternion<Type>(t1) -= t2;
  }

  template<typename Type>
  inline Quaternion<Type>
  operator-(const Quaternion<Type>& t1,
            const Type& t2) 
  {
    return Quaternion<Type>(t1) -= t2;
  }

  template<typename Type>
  inline Quaternion<Type>
  operator-(const Type& t1,
            const Quaternion<Type>& t2) 
  {
    return Quaternion<Type>(t1) -= t2;
  }

  template<typename Type>
  inline Quaternion<Type>
  operator*(const Quaternion<Type>& t1,
            const Quaternion<Type>& t2) 
  {
    return Quaternion<Type>(t1) *= t2;
  }

  template<typename Type>
  inline Quaternion<Type>
  operator*(const Quaternion<Type>& t1,
            const Type& t2) 
  {
    return Quaternion<Type>(t1) *= t2;
  }
  
  template<typename Type>
  inline Quaternion<Type>
  operator*(const Type& t1,
            const Quaternion<Type>& t2)
  {
    return Quaternion<Type>(t1) *= t2;
  }

  template<typename Type>
  inline Quaternion<Type>
  operator/(const Quaternion<Type>& t1,
            const Quaternion<Type>& t2) 
  {
    return Quaternion<Type>(t1) /= t2;
  }

  template<typename Type>
  inline Quaternion<Type>
  operator/(const Quaternion<Type>& t1,
            const Type& t2) 
  {
    return Quaternion<Type>(t1) /= t2;
  }
  
  template<typename Type>
  inline Quaternion<Type>
  operator/(const Type& t1,
            const Quaternion<Type>& t2)
  {
    return Quaternion<Type>(t1) /= t2;
  }

  template<typename Type>
  inline Quaternion<Type>
  operator+(const Quaternion<Type>& t) 
  {
    return t;
  }

  template<typename Type>
  inline Quaternion<Type>
  operator-(const Quaternion<Type>& t) 
  {
    return Quaternion<Type>(-t(0), -t(1), -t(2), -t(3));
  }

  template<typename Type>
  inline bool
  operator==(const Quaternion<Type>& t1,
             const Quaternion<Type>& t2) 
  {
    return t1(0) == t2(0) && t1(1) == t2(1) &&
        t1(2) == t2(2) && t1(3) == t2(3);
  }

  template<typename Type>
  inline bool
  operator==(const Quaternion<Type>& t1,
             const Type& t2) 
  {
    return t1(0) == t2     && t1(1) == Type() &&
        t1(2) == Type() && t1(3) == Type();
  }

  template<typename Type>
  inline bool
  operator==(const Type& t1,
             const Quaternion<Type>& t2) 
  {
    return t1 == t2(0)     && t2(1) == Type() &&
        t2(2) == Type() && t2(3) == Type();
  }

  template<typename Type>
  inline bool
  operator!=(const Quaternion<Type>& t1,
             const Quaternion<Type>& t2) 
  {
    return t1(0) != t2(0) || t1(1) != t2(1) ||
        t1(2) != t2(2) || t1(3) != t2(3);
  }

  template<typename Type>
  inline bool
  operator!=(const Quaternion<Type>& t1,
             const Type& t2) 
  {
    return t1(0) != t2     || t1(1) != Type() ||
        t1(2) != Type() || t1(3) != Type();
  }

  template<typename Type>
  inline bool
  operator!=(const Type& t1,
             const Quaternion<Type>& t2) 
  {
    return t1 != t2(0)     || t2(1) != Type() ||
        t2(2) != Type() || t2(3) != Type();
  }

  template<typename Type, typename CharT, class Traits>
  std::basic_istream<CharT, Traits>&
  operator>>(std::basic_istream<CharT, Traits>& is, Quaternion<Type>& t)
  {
    Type a, b, c, d;
    CharT ch;
    is >> ch;
    if (ch == '(') 
    {
      is >> a >> ch;
      if (ch == ',') 
      {
        is >> b >> ch;
        if (ch == ',') 
        {
          is >> c >> ch;
          if (ch == ',') 
          {
            is >> d >> ch;
            if (ch == ')') t = Quaternion<Type>(a, b, c, d);
            else is.setstate(std::ios_base::failbit);
          }
          else is.setstate(std::ios_base::failbit);
        }
        else is.setstate(std::ios_base::failbit);
      }  
      else is.setstate(std::ios_base::failbit);
    }
    else 
    {
      is.putback(ch);
      is >> a;
      t = Quaternion<Type>(a, Type(0), Type(0), Type(0));
    }
    return is;
  }

  template<typename Type, typename CharT, class Traits>
  std::basic_ostream<CharT, Traits>&
  operator<<(std::basic_ostream<CharT, Traits>& os, const Quaternion<Type>& t)
  {
    std::basic_ostringstream<CharT, Traits> s;
    s.flags(os.flags());
    s.imbue(os.getloc());
    s.precision(os.precision());
    s << '(' << t(0) << ',' << t(1) << ',' << t(2) << ',' << t(3) << ')';
    return os << s.str();
  }

  template<typename Type>
  inline Type
  abs(const Quaternion<Type>& t)
  {
    return std::sqrt(t(0) * t(0) + t(1) * t(1) +
                     t(2) * t(2) + t(3) * t(3));
  }

  template<typename Type>
  inline Quaternion<Type>
  conj(const Quaternion<Type>& t) 
  {
    return Quaternion<Type>(t(0), -t(1), -t(2), -t(3));
  }

}

#endif
