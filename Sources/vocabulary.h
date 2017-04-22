#pragma once

#include "string.h"

#include <memory>

namespace Spellchecker
{
  using Frequency = std::size_t;

  class Vocabulary
  {
  public:
    virtual ~Vocabulary() 
    {
    }

    virtual Frequency Search(String const&) = 0;
    virtual void InsertWord(String const&, Frequency) = 0;
  };

  std::unique_ptr<Vocabulary> CreateVocabulary();
}