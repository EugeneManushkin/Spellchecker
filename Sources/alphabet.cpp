#include "alphabet.h"

#include <algorithm>
#include <unordered_map>

namespace
{
  using Spellchecker::String;
  using Spellchecker::Char;

  class AlphabetImpl : public Spellchecker::Alphabet
  {
  public:
    AlphabetImpl(String const& lowerChars, String const& upperChars)
    {
      if (lowerChars.length() != upperChars.length())
        throw std::invalid_argument("Length mismatch");

      auto lower = lowerChars.begin();
      auto upper = upperChars.begin();
      while (lower != lowerChars.end() && upper != upperChars.end())
      {
        if (!UpperToLower.insert(std::make_pair(*upper, *lower)).second || !LowerToUpper.insert(std::make_pair(*lower, *upper)).second)
          throw std::invalid_argument("Duplicated char");

        ++lower;
        ++upper;
      }
    }

    virtual String GetAligned(String const& str, Case charCase, unsigned maxMismatch) const override
    {
      String result;
      auto const& container = charCase == Case::Upper ? UpperToLower : LowerToUpper; 
      auto const& oppositeContainer = charCase == Case::Upper ? LowerToUpper : UpperToLower;
      for (auto i : str)
      {
        result.push_back(i);
        if (container.find(i) == container.end())
        {
          auto x = oppositeContainer.find(i);
          if (x != oppositeContainer.end())
          {
            result.back() = x->second;
          }
          else
          {
            if (!maxMismatch)
              throw ErrorStringNotSuitable("String contains symbols from different alphabet");

            --maxMismatch;
          }
        }
      }

      return result;
    }

    virtual String GetChars(Case charCase) const override
    {
      String result;
      auto const& container = charCase == Case::Upper ? UpperToLower : LowerToUpper;
      std::transform(container.begin(), container.end(), std::back_inserter(result), [](CaseToCase::value_type const& v) { return v.first; });
      return result;
    }

  private:
    using CaseToCase = std::unordered_map<Char, Char>;
    CaseToCase UpperToLower;
    CaseToCase LowerToUpper;
  };
}

namespace Spellchecker
{
  std::unique_ptr<Alphabet> CreateAlphabet(String const& lowerChars, String const& upperChars)
  {
    return std::unique_ptr<Alphabet>(new AlphabetImpl(lowerChars, upperChars));
  }
}