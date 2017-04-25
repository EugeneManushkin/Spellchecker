#pragma once

#include "string.h"
#include "vocabulary.h"

#include <string>

namespace Spellchecker
{
  namespace Utils
  {
    std::string ToStdString(String const&);
    String ToString(std::string const&);
    std::string GetStdString(Word const&);
  }
}