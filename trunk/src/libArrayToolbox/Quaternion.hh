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

#ifndef QUATERNION_HH
#define QUATERNION_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

#include<stdexcept>

namespace atb
{

/*======================================================================*/
/*!
 *  \class Quaternion Quaternion.hh
 *  \brief The Quaternion class implements templated quaternions with
 *         few important operations
 *
 *  Quaternions are an extension to complex numbers. They consist
 *  of 3 imaginary units \f$i\f$, \f$j\f$ and \f$k\f$ and a real part,
 *  and thus span a 4-D space. A typical quaternion is given by
 *  \f$q = q_0 + i q_1 + j q_2 + k q_3\f$
 *
 *  Operations:
 *
 *  Addition:
 *
 *    \f$p + q = p_0 + q_0  + i \cdot (p_1 + q_1) + j \cdot (p_2 + q_2) +
                 k \cdot (p_3 + q_3)\f$
 *
 *  Multiplication:
 *
 *    \f$p \cdot q = p_0 q_0 - p_1 q_1 - p_2 q_2 - p_3 q_3  +
                     i \cdot (p_0 q_1 + p_1 q_0 + p_2 q_3 - p_3 q_2) +
                     j \cdot (p_0 q_2 - p_1 q_3 + p_2 q_0 + p_3 q_1) +
                     k \cdot (p_0 q_3 + p_1 q_2 - p_2 q_1 + p_3 q_0)\f$
 *
 *  Conjugate:
 *
 *    \f$\bar{q} = q_0 - i q_1 - j q_2 - k q_3\f$
 *
 *
 *  The multiplication rule can be obtained from:
 *
 *    \f$i^2 = j^2 = k^2 = -1\f$, \f$i j = -j i = k\f$, \f$j k = -k j = i\f$
 *    and \f$k i = -i k = j\f$
 *
 *  \tparam Type The basic numeric type (typically \c float or \c double)
 *    of the quaternion coefficients
 */
/*======================================================================*/
  template<typename Type>
  class Quaternion 
  {
  
  public:
  
/*======================================================================*/
/*! 
 *   Create a new quaternion \f$q := q_0 + i q_1 + j q_2 + k q_3\f$.
 *
 *   \param q0,q1,q2,q3 Quaternion coefficients
 */
/*======================================================================*/
    Quaternion(const Type& q0 = Type(),
               const Type& q1 = Type(),
               const Type& q2 = Type(),
               const Type& q3 = Type());

/*======================================================================*/
/*! 
 *   Create a new quaternion form the given quaternion.
 *
 *   \param q The quaternion to copy. This function allows to promote the
 *     basic type to a different data type
 *
 *   \tparam Type2 The basic data type of the source quaternion
 */
/*======================================================================*/
    template<typename Type2>
    Quaternion(Quaternion<Type2> const &q);
    
/*======================================================================*/
/*! 
 *   Get a reference to the coefficient for the given quaternion dimension.
 *
 *   \param index The dimension to get the quaternion coefficient for
 *
 *   \return A random-access reference to the requested coefficient
 *
 *   \exception out_of_range If the given index is not within the [0, 3]
 *     range
 */
/*======================================================================*/
    Type& operator()(const int index);

/*======================================================================*/
/*! 
 *   Get a read-only reference to the coefficient for the given
 *   quaternion dimension.
 *
 *   \param index The dimension to get the quaternion coefficient for
 *
 *   \return A read-only reference to the requested coefficient
 *
 *   \exception out_of_range If the given index is not within the [0, 3]
 *     range
 */
/*======================================================================*/
    Type const &operator()(const int index) const;
  
/*======================================================================*/
/*! 
 *   Set this quaternion to the given real number.
 *
 *   \param scalar The scalar value to set this quaternion to.
 *
 *   \return A reference to this quaternion for operator chaining
 */
/*======================================================================*/
    Quaternion<Type>& operator=(Type const &scalar);

/*======================================================================*/
/*! 
 *   Add the given real number to this quaternion.
 *
 *   \param scalar The scalar value to add to this quaternion.
 *
 *   \return A reference to this quaternion for operator chaining
 */
/*======================================================================*/
    Quaternion<Type>& operator+=(Type const &scalar);

/*======================================================================*/
/*! 
 *   Subtract the given real number from this quaternion.
 *
 *   \param scalar The scalar value to subtract from this quaternion.
 *
 *   \return A reference to this quaternion for operator chaining
 */
/*======================================================================*/
    Quaternion<Type>& operator-=(Type const &scalar);

/*======================================================================*/
/*! 
 *   Scale this quaternion with the given scalar value.
 *
 *   \param scalar The scaling factor to apply
 *
 *   \return A reference to this quaternion for operator chaining
 */
/*======================================================================*/
    Quaternion<Type>& operator*=(Type const &scalar);

/*======================================================================*/
/*! 
 *   Divide this quaternion by the given scalar value.
 *
 *   \param scalar The value to divide this quaternion with
 *
 *   \return A reference to this quaternion for operator chaining
 */
/*======================================================================*/
    Quaternion<Type>& operator/=(Type const &scalar);
  
/*======================================================================*/
/*! 
 *   Set this quaternion to the given quaternion.
 *
 *   \param q The quaternion to copy into this quaternion.
 *
 *   \return A reference to this quaternion for operator chaining
 *
 *   \tparam Type2 The source quaternion basic scalar type
 */
/*======================================================================*/
    template<typename Type2>
    Quaternion<Type>& operator=(Quaternion<Type2> const &q);
  
/*======================================================================*/
/*! 
 *   Add the given quaternion to this quaternion.
 *
 *   \param q The quaternion to add to this quaternion.
 *
 *   \return A reference to this quaternion for operator chaining
 *
 *   \tparam Type2 The source quaternion basic scalar type
 */
/*======================================================================*/
    template<typename Type2>
    Quaternion<Type>& operator+=(Quaternion<Type2> const &q);
  
/*======================================================================*/
/*! 
 *   Subtract the given quaternion from this quaternion.
 *
 *   \param q The quaternion to subtract from this quaternion.
 *
 *   \return A reference to this quaternion for operator chaining
 *
 *   \tparam Type2 The source quaternion basic scalar type
 */
/*======================================================================*/
    template<typename Type2>
    Quaternion<Type>& operator-=(Quaternion<Type2> const &q);
  
/*======================================================================*/
/*! 
 *   Multiply this quaternion with the given quaternion.
 *
 *   \param q The quaternion to multiply to this quaternion.
 *
 *   \return A reference to this quaternion for operator chaining
 *
 *   \tparam Type2 The source quaternion basic scalar type
 */
/*======================================================================*/
    template<typename Type2>
    Quaternion<Type>& operator*=(Quaternion<Type2> const &q);
  
/*======================================================================*/
/*! 
 *   Divide this quaternion by the given quaternion.
 *
 *   \param q The quaternion to divide this quaternion by.
 *
 *   \return A reference to this quaternion for operator chaining
 *
 *   \tparam Type2 The source quaternion basic scalar type
 */
/*======================================================================*/
    template<typename Type2>
    Quaternion<Type>& operator/=(Quaternion<Type2> const &q);
 
  
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
  inline Type &Quaternion<Type>::operator()(const int index)
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
  inline Type const &Quaternion<Type>::operator()(const int index) const 
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

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Add two quaternions
 *
 *   \param t1 The lhs quaternion
 *   \param t2 The rhs quaternion
 *
 *   \return t1 + t2
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator+(const Quaternion<Type>& t1,
            const Quaternion<Type>& t2) 
  {
    return Quaternion<Type>(t1) += t2;
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Add quaternion and scalar
 *
 *   \param t1 The lhs quaternion
 *   \param t2 The rhs scalar
 *
 *   \return t1 + t2
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator+(const Quaternion<Type>& t1,
            const Type& t2) 
  {
    return Quaternion<Type>(t1) += t2;
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Add scalar and quaternion
 *
 *   \param t1 The lhs scalar
 *   \param t2 The rhs quaternion
 *
 *   \return t1 + t2
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator+(const Type& t1,
            const Quaternion<Type>& t2) 
  {
    return Quaternion<Type>(t1) += t2;
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Compute difference between two quaternions
 *
 *   \param t1 The lhs quaternion
 *   \param t2 The rhs quaternion
 *
 *   \return t1 - t2
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator-(const Quaternion<Type>& t1,
            const Quaternion<Type>& t2) 
  {
    return Quaternion<Type>(t1) -= t2;
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Subtract scalar from quaternion.
 *
 *   \param t1 The lhs quaternion
 *   \param t2 The rhs scalar
 *
 *   \return t1 - t2
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator-(const Quaternion<Type>& t1,
            const Type& t2) 
  {
    return Quaternion<Type>(t1) -= t2;
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Subtract quaternion from scalar.
 *
 *   \param t1 The lhs scalar
 *   \param t2 The rhs quaternion
 *
 *   \return t1 - t2
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator-(const Type& t1,
            const Quaternion<Type>& t2) 
  {
    return Quaternion<Type>(t1) -= t2;
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Compute product of two quaternions
 *
 *   \param t1 The lhs quaternion
 *   \param t2 The rhs quaternion
 *
 *   \return t1 * t2
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator*(const Quaternion<Type>& t1,
            const Quaternion<Type>& t2) 
  {
    return Quaternion<Type>(t1) *= t2;
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Compute product of quaternion and scalar
 *
 *   \param t1 The lhs quaternion
 *   \param t2 The rhs scalar
 *
 *   \return t1 * t2
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator*(const Quaternion<Type>& t1,
            const Type& t2) 
  {
    return Quaternion<Type>(t1) *= t2;
  }
  
/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Compute product of scalar and quaternion
 *
 *   \param t1 The lhs scalar
 *   \param t2 The rhs quaternion
 *
 *   \return t1 * t2
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator*(const Type& t1,
            const Quaternion<Type>& t2)
  {
    return Quaternion<Type>(t1) *= t2;
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Compute quotient of two quaternions
 *
 *   \param t1 The lhs quaternion
 *   \param t2 The rhs quaternion
 *
 *   \return t1 / t2
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator/(const Quaternion<Type>& t1,
            const Quaternion<Type>& t2) 
  {
    return Quaternion<Type>(t1) /= t2;
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Compute quotient of quaternion and scalar
 *
 *   \param t1 The lhs quaternion
 *   \param t2 The rhs scalar
 *
 *   \return t1 / t2
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator/(const Quaternion<Type>& t1,
            const Type& t2) 
  {
    return Quaternion<Type>(t1) /= t2;
  }
  
/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Compute quotient of scalar and quaternion
 *
 *   \param t1 The lhs scalar
 *   \param t2 The rhs quaternion
 *
 *   \return t1 / t2
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator/(const Type& t1,
            const Quaternion<Type>& t2)
  {
    return Quaternion<Type>(t1) /= t2;
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Unary PLUS operator
 *
 *   \param t The quaternion to apply the operator to
 *
 *   \return t
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator+(const Quaternion<Type>& t) 
  {
    return t;
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Unary MINUS operator
 *
 *   \param t The quaternion to apply the operator to
 *
 *   \return -t
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  operator-(const Quaternion<Type>& t) 
  {
    return Quaternion<Type>(-t(0), -t(1), -t(2), -t(3));
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Equals comparison operator.
 *
 *   \param t1 The lhs quaternion
 *   \param t2 The rhs quaternion
 *
 *   \return \c true if both quaternions are equal, \c false otherwise
 */
/*======================================================================*/
  template<typename Type>
  inline bool
  operator==(const Quaternion<Type>& t1,
             const Quaternion<Type>& t2) 
  {
    return t1(0) == t2(0) && t1(1) == t2(1) &&
        t1(2) == t2(2) && t1(3) == t2(3);
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Equals comparison operator.
 *
 *   \param t1 The lhs quaternion
 *   \param t2 The rhs scalar
 *
 *   \return \c true if quaternion and scalar are equal, \c false otherwise
 */
/*======================================================================*/
  template<typename Type>
  inline bool
  operator==(const Quaternion<Type>& t1,
             const Type& t2) 
  {
    return t1(0) == t2     && t1(1) == Type() &&
        t1(2) == Type() && t1(3) == Type();
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Equals comparison operator.
 *
 *   \param t1 The lhs scalar
 *   \param t2 The rhs quaternion
 *
 *   \return \c true if quaternion and scalar are equal, \c false otherwise
 */
/*======================================================================*/
  template<typename Type>
  inline bool
  operator==(const Type& t1,
             const Quaternion<Type>& t2) 
  {
    return t1 == t2(0)     && t2(1) == Type() &&
        t2(2) == Type() && t2(3) == Type();
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Unequal comparison operator.
 *
 *   \param t1 The lhs quaternion
 *   \param t2 The rhs quaternion
 *
 *   \return \c false if both quaternions are equal, \c true otherwise
 */
/*======================================================================*/
  template<typename Type>
  inline bool
  operator!=(const Quaternion<Type>& t1,
             const Quaternion<Type>& t2) 
  {
    return t1(0) != t2(0) || t1(1) != t2(1) ||
        t1(2) != t2(2) || t1(3) != t2(3);
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Unequal comparison operator.
 *
 *   \param t1 The lhs quaternion
 *   \param t2 The rhs scalar
 *
 *   \return \c false if scalar and quaternion are equal, \c true otherwise
 */
/*======================================================================*/
  template<typename Type>
  inline bool
  operator!=(const Quaternion<Type>& t1,
             const Type& t2) 
  {
    return t1(0) != t2 || t1(1) != Type() ||
        t1(2) != Type() || t1(3) != Type();
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Unequal comparison operator.
 *
 *   \param t1 The lhs scalar
 *   \param t2 The rhs quaternion
 *
 *   \return \c false if scalar and quaternion are equal, \c true otherwise
 */
/*======================================================================*/
  template<typename Type>
  inline bool
  operator!=(const Type& t1,
             const Quaternion<Type>& t2) 
  {
    return t1 != t2(0)     || t2(1) != Type() ||
        t2(2) != Type() || t2(3) != Type();
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Read a quaternion from its string (stream) representation.
 *
 *   \param is The input stream to read a quaternion from
 *   \param t  The quaternion to store the parsed string to
 *
 *   \return A reference to the input stream for operator chaining
 */
/*======================================================================*/
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

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Write a quaternion to a character stream.
 *
 *   \param os The output stream to write the quaternion to
 *   \param t  The quaternion to write
 *
 *   \return A reference to the output stream for operator chaining
 */
/*======================================================================*/
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

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Compute the absolute value of the given quaternion.
 *
 *   \param t The quaternion to compute the absolute value of
 *
 *   \return The absolute scalar value of the quaternion (its "length")
 */
/*======================================================================*/
  template<typename Type>
  inline Type
  abs(const Quaternion<Type>& t)
  {
    return std::sqrt(t(0) * t(0) + t(1) * t(1) +
                     t(2) * t(2) + t(3) * t(3));
  }

/*======================================================================*/
/*!
 *   \relates atb::Quaternion
 *
 *   Compute the conjugate of the given quaternion.
 *
 *   \f$\bar{q} = q_0 - i q_1 - j q_2 - k q_3\f$
 *
 *   \param t The quaternion to compute the conjugate for
 *
 *   \return The conjugate of the given quaternion
 */
/*======================================================================*/
  template<typename Type>
  inline Quaternion<Type>
  conj(const Quaternion<Type>& t) 
  {
    return Quaternion<Type>(t(0), -t(1), -t(2), -t(3));
  }

}

#endif
