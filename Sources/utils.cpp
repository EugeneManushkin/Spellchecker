#include "utils.h"

namespace Spellchecker
{
  namespace Utils
  {
    std::string ToStdString(String const& str)
    {
      return std::string(str.begin(), str.end());
    }

    String ToString(std::string const& str)
    {
      return String(str.begin(), str.end());
    }

    std::string GetStdString(Word const& w)
    {
      return ToStdString(GetString(w));
    }

  }
}