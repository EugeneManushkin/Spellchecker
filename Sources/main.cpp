#include "alphabet.h"
#include "vocabulary.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <fstream>
#include <iostream>
#include <vector>

namespace
{
  std::unique_ptr<Spellchecker::Alphabet> LoadAlphabet(std::ifstream& file)
  {
    std::string lowerChars;
    std::string upperChars;
    if (!std::getline(file, lowerChars) || !std::getline(file, upperChars))
      throw std::runtime_error("Invalid file format");

    return Spellchecker::CreateAlphabet(lowerChars, upperChars);
  }

  std::unique_ptr<Spellchecker::Vocabulary> LoadVocabulary(std::string const& fileName)
  {
    std::cout << "Loading " << fileName << std::endl;
    std::ifstream file;
    file.exceptions(file.exceptions() | std::ifstream::failbit | std::ifstream::badbit);
    file.open(fileName);
    std::shared_ptr<void> fileCloser(0, [&](void*) { file.close(); });
    auto voc = Spellchecker::CreateVocabulary(LoadAlphabet(file));
    while (!file.eof())
    {
      std::string word;
      std::size_t freq = 0;
      file >> freq >> word;
      if (!freq)
        throw std::runtime_error("Invalid file format");

      voc->InsertWord(word, freq);
    }

    return voc;
  }
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Voc file required";
    return 1;
  }

  try
  {
    auto voc = LoadVocabulary(argv[1]);
    std::cout << "Print word" << std::endl;
    SetConsoleCP(1251);
    std::string line;
    do
    {
      std::cin >> line;
      if (line != "exit")
        std::cout << "frequency: " << voc->Search(line) << std::endl;
    }
    while (line != "exit");
  }
  catch (std::exception const& e)
  {
    std::cout << e.what() << std::endl;
    return 1;
  }

  return 0;
}