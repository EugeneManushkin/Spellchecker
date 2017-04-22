#include "vocabulary.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <fstream>
#include <iostream>
#include <vector>

namespace
{
  void LoadVocabulary(std::string const& fileName, Spellchecker::Vocabulary& voc)
  {
    std::cout << "Loading " << fileName << std::endl;
    std::ifstream file;
    file.exceptions(file.exceptions() | std::ifstream::failbit | std::ifstream::badbit);
    file.open(fileName);
    std::shared_ptr<void> fileCloser(0, [&](void*) { file.close(); });
    while (!file.eof())
    {
      std::string word;
      std::size_t freq = 0;
      file >> freq >> word;
      if (!freq)
        throw std::runtime_error("Invalid file format");

      voc.InsertWord(word, freq);
    }
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
    std::unique_ptr<Spellchecker::Vocabulary> voc = Spellchecker::CreateVocabulary();
    LoadVocabulary(argv[1], *voc);
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