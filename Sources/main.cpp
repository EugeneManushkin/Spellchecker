#include "alphabet.h"
#include "engine.h"
#include "vocabulary.h"
#include "utils.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

namespace
{
  using VocabularyPtr = std::unique_ptr<Spellchecker::Vocabulary>;
  using Vocabularies = std::vector<VocabularyPtr>;
  using namespace Spellchecker::Utils;

  std::unique_ptr<Spellchecker::Alphabet> LoadAlphabet(std::ifstream& file)
  {
    std::string lowerChars;
    std::string upperChars;
    if (!std::getline(file, lowerChars) || !std::getline(file, upperChars))
      throw std::runtime_error("Invalid file format");

    return Spellchecker::CreateAlphabet(ToString(lowerChars), ToString(upperChars));
  }

  std::unique_ptr<Spellchecker::Vocabulary> LoadVocabulary(std::string const& fileName)
  {
    std::cout << "Loading " << fileName << std::endl;
    std::ifstream file;
    file.exceptions(file.exceptions() | std::ifstream::failbit | std::ifstream::badbit);
    file.open(fileName);
    std::shared_ptr<void> fileCloser(0, [&](void*) { file.close(); });
    auto voc = Spellchecker::CreateVocabulary(LoadAlphabet(file));
    auto const& alphabet = voc->GetAlphabet();
    while (!file.eof())
    {
      std::string word;
      std::size_t freq = 0;
      file >> freq >> word;
      if (!freq)
        throw std::runtime_error("Invalid file format");

      voc->InsertWord(alphabet.GetAligned(ToString(word), 0), freq);
    }

    return voc;
  }

  Vocabularies LoadVocabularies(int argc, char* argv[])
  {
    Vocabularies result;
    std::for_each(argv + 1, argv + argc, [&](char* value) 
    {
      try
      {
        result.push_back(LoadVocabulary(value));
      }
      catch (std::exception const& err)
      {
        std::cout << "Failed to load vocabulary " << value << ": " << err.what() << std::endl;
      }
    });

    return result;
  }

  void CheckWord(Spellchecker::String const& word, Spellchecker::Engine const& engine, Spellchecker::Vocabulary const& voc, unsigned maxResults)
  {
    try
    {
      auto result = engine.Check(word, maxResults, voc);
      for (auto const& w : result)
        std::cout << GetStdString(w) << " : " << Spellchecker::GetFrequency(w) << std::endl;
    }
    catch (Spellchecker::Alphabet::ErrorStringNotSuitable const&)
    {
    }
  
    std::cout << std::endl;
  }

  unsigned GetMaxSuggestions(std::string const& inputStr)
  {
    unsigned const maxNumber = 50;
    try
    {
      auto result = std::stoul(inputStr.substr(1));
      return result < maxNumber ? static_cast<unsigned>(result) : maxNumber;
    }
    catch (std::logic_error const& err)
    {
      std::cout << err.what() << std::endl;
    }

    return 0;
  }

  void Usage()
  {
    std::cout << "Usage: '-exit' '-x' '-q'  to quit program" << std::endl;
    std::cout << "       '-help' '-h'       print this message" << std::endl;
    std::cout << "       '-<number>'        show <number> spelling suggestions" << std::endl;
    std::cout << "       '<word>'           any word to check spelling. Output:" << std::endl;
    std::cout << "                          <word1> : <word1_frequency>" << std::endl;
    std::cout << "                          <word2> : <word2_frequency>" << std::endl;
    std::cout << "                            ..." << std::endl;
    std::cout << "                          <wordn> : <wordn_frequency>" << std::endl;
    std::cout << "To use vocabulary with specific codepage set proper codepage with CHCP utility before running this program" << std::endl;
  }

  void MainLoop(Vocabularies const& vocabularies)
  {
    Usage();
    auto engine = Spellchecker::CreateEngine();
    std::string line;
    unsigned maxResults = 3;
    do
    {
      std::cout << ">";
      std::cin >> line;
      if (line == "-exit" || line == "-x" || line == "-q")
        break;

      if (line == "-help" || line == "-h")
      {
        Usage();
        continue;
      }

      if (!line.empty() && line[0] == '-')
      {
        auto n = GetMaxSuggestions(line);
        maxResults = !n ? maxResults : n;
        std::cout << "Show " << maxResults << " suggestions" << std::endl;
        continue;
      }

      for (auto const& voc : vocabularies)
        CheckWord(ToString(line), *engine, *voc, maxResults);
    } 
    while (line != "-exit");
  }

  std::string ExtractExeName(char const* str)
  {
    std::string exeFile(str);
    auto begin = exeFile.find_last_of("/\\");
    return begin == std::string::npos ? exeFile : exeFile.substr(begin + 1);
  }
}

int main(int argc, char* argv[])
{
  auto exeFile = ExtractExeName(argv[0]);
  std::cout << exeFile << std::endl;
  auto vocabularies = LoadVocabularies(argc, argv);
  if (vocabularies.empty())
  {
    std::cout << "No vocabularies loaded" << std::endl <<
                 "Usage: " << exeFile << "vocabulary1.voc vocabulary2.voc ... vocabularyn.voc" << std::endl;
    return 1;
  }

  try
  {
    MainLoop(vocabularies);
  }
  catch (std::exception const& e)
  {
    std::cout << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}