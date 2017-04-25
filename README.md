# Spellchecker
This is test program for IT interview.

# Install
Download latest release zip archive and unpack it

# Run
For test run use run.bat script. Provided script changes current codepage to CP1251 to use both english and russian test vocabularies.
To run with custom .voc files:
1. Start cmd application and navigate to folder with unpacked archive.
2. CHCP <vocabulary_codepage>
3. Spellchecker.exe vocabulary1.voc vocabulary2.voc vocabularyn.voc

# Usage
```
'-exit' '-x' '-q'  to quit program
'-help' '-h'       print this message
'-<number>'        show <number> spelling suggestions
'<word>'           any word to check spelling. Output:
                   <word1> : <word1_frequency>
                   <word2> : <word2_frequency>
                     ...
                   <wordn> : <wordn_frequency>
```
