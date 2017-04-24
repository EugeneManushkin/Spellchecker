#pragma once

#include "string.h"

#include <memory>
#include <stdexcept>

namespace Spellchecker
{
  class Alphabet
  {
  public:
    class ErrorStringNotSuitable : public std::invalid_argument
    {
    public:
      ErrorStringNotSuitable(char const* str)
        : std::invalid_argument(str)
      {
      }
    };

    enum class Case
    {
      Lower,
      Upper,
    };

    virtual ~Alphabet() 
    {
    }
    
    virtual String GetAligned(String const& str, Case charCase, unsigned maxMismatch) const = 0;
    virtual String GetChars(Case charCase) const = 0;
    
    String GetAligned(String const& str, unsigned maxMismatch) const
    {
      return GetAligned(str, Case::Lower, maxMismatch);
    }

    virtual String GetChars() const
    {
      return GetChars(Case::Lower);
    }
  };

  std::unique_ptr<Alphabet> CreateAlphabet(String const& lowerChars, String const& upperChars);
}