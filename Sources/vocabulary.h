#pragma once

#include "string.h"

#include <memory>

namespace Spellchecker
{
  using Frequency = std::size_t;

  class Alphabet;

  class Vocabulary
  {
  public:
    Vocabulary(std::shared_ptr<Spellchecker::Alphabet> const& alphabet)
      : Alphabet(alphabet)
    {
    }

    virtual ~Vocabulary() 
    {
    }

    virtual Frequency Search(String const& word) = 0;
    virtual void InsertWord(String const& word, Frequency) = 0;

    Alphabet const& GetAlphabet()
    {
      return *Alphabet;
    }

  private:
    std::shared_ptr<Alphabet> Alphabet;
  };

  std::unique_ptr<Vocabulary> CreateVocabulary(std::shared_ptr<Alphabet> const& alphabet);
}