/*
 *  MIT License
 *
 *  Copyright (c) 2020 Kevin Xu
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#pragma once

#include <cstdint>
#include <iomanip>
#include <memory>
#include <ostream>
#include <iostream>

namespace xu
{
  /**
    @brief  Implements a shared buffer containing a known number of bytes in memory
            Wraps a smart pointer with a size and implements helpful operators and an iterator
    */
  class shared_buf
  {
  public:
    //  =========
    //  Iterators
    //  =========

    template<typename Val_T>
    class iterator_
    {
    protected:
      uint8_t* base_ptr;
      size_t sz;
      size_t i;
    public:
      iterator_(
        uint8_t* base_ptr_,
        size_t sz_,
        size_t i_ = 0)
        : base_ptr(base_ptr_),
          sz(sz_),
          i(i_)
      {
        if (i > sz)
        {
          i = sz;
        }
      }

      iterator_(const iterator_& other)
        : base_ptr(other.base_ptr),
          sz(other.sz),
          i(other.i)
      {}

      iterator_& operator=(const iterator_& other)
      {
        base_ptr = other.base_ptr;
        sz = other.sz;
        i = other.i;
        return *this;
      }

      iterator_& operator++()
      {
        if (i < sz)
        {
          i++;
        }
        return *this;
      }

      iterator_ operator++(int)
      {
        iterator_ res = *this;
        operator++();
        return res;
      }

      iterator_& operator+=(size_t n)
      {
        size_t next = i + n;

        /* check for overflow (as implemented we only allow forward traversal) */
        if (next < i or next > sz)
        {
          i = sz;
        }
        else
        {
          i = next;
        }

        return *this;
      }

      bool operator==(const iterator_& other) const
      {
        return (base_ptr == other.base_ptr
          and sz == other.sz
          and i == other.i);
      }

      bool operator!=(const iterator_& other) const
      {
        return (base_ptr != other.base_ptr
          or sz != other.sz
          or i != other.i);
      }

      Val_T& operator*() const
      {
        if (i < sz)
        {
          return base_ptr[i];
        }
        else
        {
          throw std::out_of_range("shared_buf::iterator_::operator* : invalid");
        }
      }

      Val_T* ptr() const
      {
        if (i < sz)
        {
          return base_ptr + i;
        }
        else
        {
          throw std::out_of_range("shared_buf::iterator_::ptr() : invalid");
        }
      }

      /**
        @brief  Returns distance between two iterators, measured in bytes
        @note   Result is a scalar
        @note   Result is non-negative, so if lhs < rhs, 0 is returned
        */
      size_t operator-(const iterator_& other) const
      {
        if (other.i > i)
        {
          return 0;
        }
        else
        {
          return i - other.i;
        }
      }

      operator iterator_<const Val_T>() const
      {
        return iterator_<const Val_T>(base_ptr, sz, i);
      }
    };

    using iterator = iterator_<uint8_t>;
    using const_iterator = iterator_<const uint8_t>;

    iterator begin()
    {
      return iterator(&ptr[0], sz);
    }

    iterator end()
    {
      return iterator(&ptr[0], sz, sz);
    }

    const_iterator begin() const
    {
      return const_iterator(&ptr[0], sz);
    }

    const_iterator end() const
    {
      return const_iterator(&ptr[0], sz, sz);
    }

    //  ================
    //  Member Functions
    //  ================

    /**
      @brief  Constructor
      @param  sz_
              Number of bytes in buffer
      */
    shared_buf(size_t sz_)
      : sz(sz_)
    {
      ptr = std::shared_ptr<uint8_t[]>(new uint8_t[sz]);
    }

    /**
      @brief  Copy constructor
      */
    shared_buf(const shared_buf& other)
      : ptr(other.ptr),
        sz(other.sz)
    {
      ptr = other.ptr;
      sz = other.sz;
    }

    /**
      @brief  Copy assignment
      */
    shared_buf& operator=(const shared_buf& other)
    {
      ptr = other.ptr;
      sz = other.sz;

      return *this;
    }

    /**
      @brief  Move constructor
      */
    shared_buf(shared_buf&& other)
      : ptr(std::move(other.ptr)),
        sz(other.sz)
    {
      other.sz = 0;
    }

    /**
      @brief  Move assignment
      */
    shared_buf& operator=(shared_buf&& other)
    {
      ptr = std::move(other.ptr);
      sz = other.sz;

      other.sz = 0;

      return *this;
    }

    /**
      @brief  Byte access
      @param  i
              Index
      @throw  std::out_of_range
              If index is not within size
      */
    uint8_t& operator[](size_t i)
    {
      if (i < sz)
      {
        return ptr[i];
      }
      else
      {
        throw std::out_of_range("shared_buf::operator[] : index out of range");
      }
    }

    /**
      @brief  Byte access, const-qualified
      @param  i
              Index
      @throw  std::out_of_range
              If index is not within size
      */
    uint8_t operator[](size_t i) const
    {
      if (i < sz)
      {
        return ptr[i];
      }
      else
      {
        throw std::out_of_range("shared_buf::operator[] : index out of range");
      }
    }

    /**
      @brief  Output to string
      */
    std::ostream& print(std::ostream& stream) const
    {
      std::ios_base::fmtflags orig_stream_flags(stream.flags());
      stream << std::resetiosflags(orig_stream_flags);

      stream << '[' << std::hex;

      for (size_t i = 0; i < sz; i++)
      {
        if (i != 0)
        {
          stream << ',';
        }

        stream << (int)ptr[i] << 'h';
      }

      stream << ']';

      stream.flags(orig_stream_flags);

      return stream;
    }

    /**
      @brief  Returns size
      */
    size_t size() const
    {
      return sz;
    }

  protected:
    //  ================
    //  Member Variables
    //  ================

    std::shared_ptr<uint8_t[]> ptr;
    size_t sz;
  };
}

inline std::ostream& operator<<(std::ostream& stream, const xu::shared_buf& buf)
{
  return buf.print(stream);
}