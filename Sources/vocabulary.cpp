#include "vocabulary.h"

#include <unordered_map>

namespace
{
  using Spellchecker::String;
  using Spellchecker::Frequency;

  class VocabularyImpl : public Spellchecker::Vocabulary
  {
  public:
    VocabularyImpl()
    {
    }

    virtual Frequency Search(String const& word)
    {
      auto result = Words.find(word);
      return result == Words.end() ? 0 : result->second;
    }

    virtual void InsertWord(String const& word, Frequency freq)
    {
      Words[word] = freq;
    }

  private:
    std::unordered_map<String, Frequency> Words;
  };
}

namespace Spellchecker
{
  std::unique_ptr<Vocabulary> CreateVocabulary()
  {
    return std::unique_ptr<Vocabulary>(new VocabularyImpl);
  }
}