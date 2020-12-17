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

    class iterator
    {
    protected:
      uint8_t* base_ptr;
      size_t size;
      size_t i;
    public:
      iterator(
        uint8_t* base_ptr_,
        size_t size_,
        size_t i_ = 0)
        : base_ptr(base_ptr_),
          size(size_),
          i(i_)
      {}

      iterator(const iterator& other)
        : base_ptr(other.base_ptr),
          size(other.size),
          i(other.i)
      {}

      iterator& operator=(const iterator& other)
      {
        base_ptr = other.base_ptr;
        size = other.size;
        i = other.i;
        return *this;
      }

      iterator& operator++()
      {
        if (i < size)
        {
          i++;
        }
        return *this;
      }

      iterator operator++(int)
      {
        iterator res = *this;
        operator++();
        return res;
      }

      bool operator==(const iterator& other)
      {
        return (base_ptr == other.base_ptr
          and size == other.size
          and i == other.i);
      }

      bool operator!=(const iterator& other)
      {
        return (base_ptr != other.base_ptr
          or size != other.size
          or i != other.i);
      }

      uint8_t& operator*()
      {
        if (i < size)
        {
          return base_ptr[i];
        }
        else
        {
          throw std::out_of_range("shared_buf::iterator::operator* : invalid");
        }
      }
    };

    iterator begin()
    {
      return iterator(&ptr[0], size);
    }

    iterator end()
    {
      return iterator(&ptr[0], size, size);
    }

    //  ================
    //  Member Functions
    //  ================

    /**
      @brief  Constructor
      @param  size_
              Number of bytes in buffer
      */
    shared_buf(size_t size_)
      : size(size_)
    {
      ptr = std::shared_ptr<uint8_t[]>(new uint8_t[size]);
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
      if (i < size)
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
      return operator[](i);
    }

    /**
      @brief  Output to string
      */
    std::ostream& print(std::ostream& stream) const
    {
      std::ios_base::fmtflags orig_stream_flags(stream.flags());
      stream << std::resetiosflags(orig_stream_flags);

      stream << '[' << std::hex;

      for (size_t i = 0; i < size; i++)
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

  protected:
    //  ================
    //  Member Variables
    //  ================

    std::shared_ptr<uint8_t[]> ptr;
    size_t size;
  };
}

inline std::ostream& operator<<(std::ostream& stream, const xu::shared_buf& buf)
{
  return buf.print(stream);
}