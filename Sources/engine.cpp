#include "alphabet.h"
#include "engine.h"
#include "vocabulary.h"

#include <algorithm>
#include <set>

namespace
{
  using Spellchecker::String;
  using Spellchecker::Vocabulary;
  using Spellchecker::Word;
  using Spellchecker::Words;

  class ChangesIterator
  {
  public:
    ChangesIterator(String const& word, String const& chars)
      : CurrentWord(word)
      , SourceWord(word)
      , Chars(chars)
      , CurrentSymbol(SourceWord.begin())
      , CurrentAlpha(Chars.begin())
    {
      Changes.push_back(ChangeType::Insert);
      Changes.push_back(ChangeType::Replace);
      if (SourceWord.length() > 1)
        Changes.push_back(ChangeType::Remove);

      CurrentChange = Changes.begin();
      ApplyChange();
    }

    String const& Current() const
    {
      return CurrentWord;
    }

    bool Next()
    {
      if (CurrentChange == Changes.end())
        return false;

      if (*CurrentChange == ChangeType::Remove)
        CurrentAlpha = Chars.end();
      else
        ++CurrentAlpha;

      if (CurrentAlpha == Chars.end())
      {
        CurrentAlpha = Chars.begin();
        ++CurrentSymbol;
      }

      if (CurrentSymbol == SourceWord.end())
      {
        CurrentSymbol = SourceWord.begin();
        ++CurrentChange;
      }

      if (CurrentChange == Changes.end())
        return false;

      ApplyChange();
      return true;
    }

  private:
    enum class ChangeType
    {
      Remove,
      Replace,
      Insert,
    };

    void ApplyChange()
    {
      CurrentWord = SourceWord;
      auto iter = CurrentWord.begin() + (CurrentSymbol - SourceWord.begin());
      if (*CurrentChange == ChangeType::Insert)
        CurrentWord.insert(iter, *CurrentAlpha);
      if (*CurrentChange == ChangeType::Remove)
        CurrentWord.erase(iter);
      else
        *iter = *CurrentAlpha;
    }

    String CurrentWord;
    String SourceWord;
    String Chars;
    std::vector<ChangeType> Changes;
    String::iterator CurrentSymbol;
    String::const_iterator CurrentAlpha;
    std::vector<ChangeType>::const_iterator CurrentChange;
  };

  class EngineImpl : public Spellchecker::Engine
  {
  public:
    virtual Words Check(String const& str, unsigned maxMatches, Vocabulary const& vocabulary) const override
    {
      if (!maxMatches)
        throw std::invalid_argument("maxMatches must be greater zero");

      if (str.empty())
        throw std::invalid_argument("Cannot check empty string");

      auto word = vocabulary.Search(str);
      if (!!Spellchecker::GetFrequency(word))
        return Words(1, word);

      std::set<Word> results;
      ChangesIterator iter(str, vocabulary.GetAlphabet().GetChars());
      do
      {
        auto word = vocabulary.Search(iter.Current());
        if (!Spellchecker::GetFrequency(word))
          continue;

        if (!maxMatches && !results.empty())
          results.erase(results.begin());
        else
          --maxMatches;

        results.insert(word);
      }
      while (iter.Next());

      return Words(results.rbegin(), results.rend());
    }
  };
}

namespace Spellchecker
{
  std::unique_ptr<Engine> CreateEngine()
  {
    return std::unique_ptr<Engine>(new EngineImpl());
  }
}