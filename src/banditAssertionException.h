#ifndef BANDIT_ASSERTION_EXCEPTION_H
#define BANDIT_ASSERTION_EXCEPTION_H

// Taken from: https://github.com/joakimkarlsson/bandit
//             ( bandit/bandit/assertion_exception.h )
// commit: 1d2357d82d0cdd8b0b6c506dabc5e2e4ccabeead
// on: 2015/06/12 (June 12, 2015).
// written by: Joakim Karlsson
// Used under copyright and license below.

namespace bandit { namespace detail {

  struct assertion_exception : public std::runtime_error
  {
    assertion_exception(const std::string& message,
        const std::string& filename, const unsigned int linenumber) 
      : std::runtime_error(message), file_name_(filename), line_number_(linenumber) 
    {}

    assertion_exception(const std::string& message)
      : std::runtime_error(message), line_number_(0)
    {}

    //
    // To make gcc < 4.7 happy.
    //
    virtual ~assertion_exception() throw()
    {}

    const std::string& file_name() const
    {
      return file_name_;
    }

    unsigned int line_number() const
    {
      return line_number_;
    }

    private:
    std::string file_name_;
    unsigned int line_number_;
  };
}}

// The MIT License (MIT)
//
// Copyright (c) 2013 Joakim Karlsson
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
//    The above copyright notice and this permission notice shall be
//    included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT.
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#endif

