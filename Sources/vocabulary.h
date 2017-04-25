#pragma once

#include "string.h"

#include <memory>

namespace Spellchecker
{
  using Frequency = std::size_t;
  using Word = std::pair<Frequency, String>;

  class Alphabet;

  class Vocabulary
  {
  public:
    Vocabulary(std::shared_ptr<Alphabet> const& alphabet)
      : Alpha(alphabet)
    {
    }

    virtual ~Vocabulary() 
    {
    }

    virtual Word Search(String const& str) const = 0;
    virtual void InsertWord(String const& word, Frequency) = 0;

    Alphabet const& GetAlphabet() const
    {
      return *Alpha;
    }

  private:
    std::shared_ptr<Alphabet> Alpha;
  };

  std::unique_ptr<Vocabulary> CreateVocabulary(std::shared_ptr<Alphabet> const& alphabet);
  Frequency GetFrequency(Word const& w);
  String GetString(Word const& w);
}