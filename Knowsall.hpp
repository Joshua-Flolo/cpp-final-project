#ifndef KNOWSALL_HPP
#define KNOWSALL_HPP

#include <string>
#include <vector>

// Function declarations
bool loadDictionary(const std::string& filename, std::vector<std::string>& dictionary);
void letterByLetterGuessing(const std::string& secretWord, const std::vector<std::string>& dictionary);
void wordByWordGuessing(const std::string& secretWord, const std::vector<std::string>& dictionary);
void frequencyBasedGuessing(const std::string& secretWord, const std::vector<std::string>& dictionary);
void randomWordGuessing(const std::string& secretWord, const std::vector<std::string>& dictionary);
void playGame();

#endif // KNOWSALL_HPP