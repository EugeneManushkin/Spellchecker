#include "alphabet.h"
#include "engine.h"
#include "vocabulary.h"

#include <algorithm>
#include <set>
#include <deque>

namespace
{
  using Spellchecker::String;
  using Spellchecker::Vocabulary;
  using Spellchecker::Word;
  using Spellchecker::Words;

  class ChangesIterator
  {
  public:
    virtual ~ChangesIterator()
    {
    }

    virtual bool Next() = 0;
    virtual String const& Current() const = 0;
  };

  class SourceWordHolder : public ChangesIterator
  {
  public:
    SourceWordHolder(String const& sourceWord)
      : SourceWord(sourceWord)
      , Iter(SourceWord.begin())
    {
    }

    virtual String const& Current() const override
    {
      return CurrentWord;
    }

  protected:
    String::iterator ResetCurrentWord()
    {
      CurrentWord = SourceWord;
      return CurrentWord.begin() + std::distance(SourceWord.begin(), Iter);
    }

    String const SourceWord;
    String::const_iterator Iter;
    String CurrentWord;
  };

  class RemoveChangeIterator : public SourceWordHolder
  {
  public:
    RemoveChangeIterator(String const& sourceWord)
      : SourceWordHolder(sourceWord)
    {
      if (SourceWord.size() < 2)
        Iter = SourceWord.end();
    }

    virtual bool Next() override
    {
      if (Iter == SourceWord.end())
        return false;

      CurrentWord.erase(ResetCurrentWord());
      ++Iter;
      return true;
    }
  };

  struct AlphabetHolder
  {
    AlphabetHolder(String const& alphabet)
      : Alphabet(alphabet)
      , Alpha(Alphabet.begin())
    {
    }

    String Alphabet;
    String::const_iterator Alpha;
  };

  class ReplaceChangeIterator : public SourceWordHolder, private AlphabetHolder
  {
  public:
    ReplaceChangeIterator(String const& sourceWord, String const& alphabet)
      : SourceWordHolder(sourceWord)
      , AlphabetHolder(alphabet)
    {
    }

    virtual bool Next() override
    {
      if (Iter == SourceWord.end())
        return false;

      while (!Apply() && Move());
      if (Iter == SourceWord.end())
        return false;

      Move();
      return true;
    }

  private:
    bool Apply()
    {
      auto cur = ResetCurrentWord();
      bool result = *cur != *Alpha;
      *cur = *Alpha;
      return result;
    }

    bool Move()
    {
      if (++Alpha != Alphabet.end())
        return true;

      Alpha = Alphabet.begin();
      return ++Iter != SourceWord.end();
    }
  };

  class InsertChangeIterator : public SourceWordHolder, private AlphabetHolder
  {
  public:
    InsertChangeIterator(String const& sourceWord, String const& alphabet)
      : SourceWordHolder(sourceWord)
      , AlphabetHolder(alphabet)
    {
    }

    virtual bool Next() override
    {
      if (Iter == SourceWord.end() && Alpha == Alphabet.end())
        return false;

      CurrentWord.insert(ResetCurrentWord(), *Alpha);
      if (++Alpha == Alphabet.end() && Iter != SourceWord.end())
      {
        Alpha = Alphabet.begin();
        ++Iter;
      }

      return true;
    }
  };

  class SwapChangeIterator : public SourceWordHolder
  {
  public:
    SwapChangeIterator(String const& sourceWord)
      : SourceWordHolder(sourceWord)
    {
    }

    virtual bool Next() override
    {
      if (Iter == SourceWord.end() || Iter == SourceWord.end() - 1)
        return false;

      auto cur = ResetCurrentWord();
      std::swap(*cur, *(cur + 1));
      ++Iter;
      return true;
    }
  };

  class MultipleChangesIterator : public ChangesIterator
  {
  public:
    MultipleChangesIterator(String const& sourceWord, String const& alphabet)
      : Remover(sourceWord)
      , Replacer(sourceWord, alphabet)
      , Inserter(sourceWord, alphabet)
      , Swapper(sourceWord)
    {
      Iterators = { &Remover, &Replacer, &Inserter, &Swapper };
    }

    virtual bool Next() override
    {
      if (Iterators.empty())
        return false;

      for (; !Iterators.empty() && !Iterators.back()->Next(); Iterators.pop_back());
      return !Iterators.empty();
    }

    virtual String const& Current() const override
    {
      return Iterators.back()->Current();
    }

  private:
    RemoveChangeIterator Remover;
    ReplaceChangeIterator Replacer;
    InsertChangeIterator Inserter;
    SwapChangeIterator Swapper;
    std::deque<ChangesIterator*> Iterators;
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

      unsigned const maxInvalidChars = 1;
      auto alignedString = vocabulary.GetAlphabet().GetAligned(str, maxInvalidChars);
      return CheckImpl(alignedString, maxMatches, vocabulary);
    }

  private:
    Words CheckImpl(String const& str, unsigned maxMatches, Vocabulary const& vocabulary) const
    {
      auto word = vocabulary.Search(str);
      if (!!Spellchecker::GetFrequency(word))
        return Words(1, word);

      std::set<Word> results;
      MultipleChangesIterator iter(str, vocabulary.GetAlphabet().GetChars());
      while (iter.Next())
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