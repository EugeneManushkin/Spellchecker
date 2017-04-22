#include "alphabet.h"
#include "vocabulary.h"

#include <unordered_map>

namespace
{
  using Spellchecker::String;
  using Spellchecker::Frequency;

  class VocabularyImpl : public Spellchecker::Vocabulary
  {
  public:
    VocabularyImpl(std::shared_ptr<Spellchecker::Alphabet> const& alphabet)
      : Vocabulary(alphabet)
    {
    }

    virtual Frequency Search(String const& word) override
    {
      auto result = Words.find(word);
      return result == Words.end() ? 0 : result->second;
    }

    virtual void InsertWord(String const& word, Frequency freq) override
    {
      Words[GetAlphabet().GetAligned(word, 0)] = freq;
    }

  private:
    std::unordered_map<String, Frequency> Words;
  };
}

namespace Spellchecker
{
  std::unique_ptr<Vocabulary> CreateVocabulary(std::shared_ptr<Alphabet> const& alphabet)
  {
    return std::unique_ptr<Vocabulary>(new VocabularyImpl(alphabet));
  }
}