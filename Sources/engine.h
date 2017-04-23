#pragma once

#include "vocabulary.h"

#include <memory>
#include <vector>

namespace Spellchecker
{
  using Words = std::vector<Word>;

  class Vocabulary;

  class Engine
  {
  public:
    virtual ~Engine() 
    {
    }

    virtual Words Check(String const& str, unsigned maxMatches, Vocabulary const& vocabulary) const = 0;
  };

  std::unique_ptr<Engine> CreateEngine();
}