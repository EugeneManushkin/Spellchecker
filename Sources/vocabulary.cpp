#include "alphabet.h"
#include "vocabulary.h"

#include <unordered_map>

namespace
{
  using Spellchecker::Word;
  using Spellchecker::String;
  using Spellchecker::Frequency;

  Word MakeWord(String str, Frequency freq)
  {
    return std::make_pair(freq, str);
  }

  class VocabularyImpl : public Spellchecker::Vocabulary
  {
  public:
    VocabularyImpl(std::shared_ptr<Spellchecker::Alphabet> const& alphabet)
      : Vocabulary(alphabet)
    {
    }

    virtual Word Search(String const& str) const override
    {
      auto result = Words.find(str);
      return result == Words.end() ? MakeWord(str, 0) : MakeWord(result->first, result->second);
    }

    virtual void InsertWord(String const& word, Frequency freq) override
    {
      Words[word] = freq;
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

  Frequency GetFrequency(Word const& w)
  {
    return w.first;
  }

  String GetString(Word const& w)
  {
    return w.second;
  }
}