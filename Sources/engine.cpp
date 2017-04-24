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

      CurrentWord = SourceWord;
      CurrentWord.erase(CurrentWord.begin() + std::distance(SourceWord.begin(), Iter));
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

      CurrentWord = SourceWord;
      for (; !Apply(); Move());
      Move();
      return true;
    }

  private:
    bool Apply()
    {
      auto cur = CurrentWord.begin() + std::distance(SourceWord.begin(), Iter);
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

      CurrentWord = SourceWord;
      CurrentWord.insert(CurrentWord.begin() + std::distance(SourceWord.begin(), Iter), *Alpha);
      if (++Alpha == Alphabet.end() && Iter != SourceWord.end())
      {
        Alpha = Alphabet.begin();
        ++Iter;
      }

      return true;
    }

  private:
  };

  class MultipleChangesIterator : public ChangesIterator
  {
  public:
    MultipleChangesIterator(String const& sourceWord, String const& alphabet)
      : Remover(sourceWord)
      , Replacer(sourceWord, alphabet)
      , Inserter(sourceWord, alphabet)
    {
      Iterators = { &Remover, &Replacer, &Inserter };
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