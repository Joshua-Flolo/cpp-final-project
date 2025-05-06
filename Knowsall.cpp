#include "Knowsall.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unordered_set>
#include <algorithm>
#include <map>
#include <chrono>
#include <thread>
#include <random> // For std::shuffle

// Function prototype for filterWords
std::vector<std::string> filterWords(const std::vector<std::string>& dictionary, const std::string& guessedWord, const std::unordered_set<char>& guessedLetters);

/**
 * Function to load the dictionary from a file.
 * @param filename The name of the dictionary file.
 * @param dictionary A vector to store the loaded words.
 * @return True if the file was loaded successfully, false otherwise.
 */
bool loadDictionary(const std::string& filename, std::vector<std::string>& dictionary) {
    std::ifstream file(filename); // Fixed: Corrected the file stream declaration
    if (!file.is_open()) {
        std::cerr << "Error: Could not open dictionary file: " << filename << "\n";
        return false;
    }

    std::string word;
    while (std::getline(file, word)) {
        dictionary.push_back(word);
    }

    file.close();
    return true;
}

/**
 * Function to filter the dictionary based on the current guessed word pattern.
 * @param dictionary The list of possible words.
 * @param guessedWord The current state of the guessed word (e.g., "_p_l_").
 * @param guessedLetters The set of letters that have already been guessed.
 * @return A filtered list of words that match the pattern.
 */
std::vector<std::string> filterWords(const std::vector<std::string>& dictionary, const std::string& guessedWord, const std::unordered_set<char>& guessedLetters) {
    std::vector<std::string> filteredWords;

    for (const auto& word : dictionary) {
        if (word.length() != guessedWord.length()) continue;

        bool matches = true;
        for (size_t i = 0; i < word.length(); ++i) {
            if (guessedWord[i] != '_' && guessedWord[i] != word[i]) {
                matches = false;
                break;
            }
            if (guessedWord[i] == '_' && guessedLetters.find(word[i]) != guessedLetters.end()) {
                matches = false;
                break;
            }
        }

        if (matches) {
            filteredWords.push_back(word);
        }
    }

    return filteredWords;
}

/**
 * Function to calculate the frequency of each letter in the remaining possible words.
 * @param possibleWords The list of possible words.
 * @param guessedLetters The set of letters that have already been guessed.
 * @return A map of letter frequencies.
 */
std::map<char, int> calculateLetterFrequencies(const std::vector<std::string>& possibleWords, const std::unordered_set<char>& guessedLetters) {
    std::map<char, int> letterFrequencies;

    for (const auto& word : possibleWords) {
        for (char letter : word) {
            if (guessedLetters.find(letter) == guessedLetters.end()) {
                letterFrequencies[letter]++;
            }
        }
    }

    return letterFrequencies;
}

/**
 * Function to log guesses and responses
 * @param guessedLetterOrWord The guessed letter or word.
 * @param response The user's response.
 */
void logGuess(const std::string& guessedLetterOrWord, const std::string& response) {
    std::ofstream logFile("knowsall_log.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << "Guess: " << guessedLetterOrWord << ", Response: " << response << "\n";
        logFile.close();
    }
}

/**
 * Function to display the guessed word with better formatting
 * @param guessedWord The current state of the guessed word.
 */
void displayGuessedWord(const std::string& guessedWord) {
    std::cout << "\nCurrent Word: ";
    for (char c : guessedWord) {
        std::cout << (c == '_' ? "_ " : std::string(1, c) + " ");
    }
    std::cout << "\n";
}

/**
 * Function to add a timer for user responses
 * @param response The user's response.
 * @param timeLimitSeconds The time limit in seconds.
 * @return True if the user responded within the time limit, false otherwise.
 */
bool getUserResponseWithTimer(std::string& response, int timeLimitSeconds) {
    auto start = std::chrono::steady_clock::now();
    while (true) {
        if (std::cin.peek() != EOF) {
            std::cin >> response;
            return true;
        }
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() >= timeLimitSeconds) {
            std::cout << "\nTime's up! Proceeding automatically...\n";
            response = "no"; // Default response if time runs out
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

/**
 * Letter-by-letter guessing mode.
 * Knowsall guesses letters sequentially and updates the guessed word.
 */
void letterByLetterGuessing(const std::string& secretWord, const std::vector<std::string>& dictionary) {
    std::unordered_set<char> guessedLetters;
    std::string guessedWord(secretWord.length(), '_');
    int maxQuestions = 20, questionsAsked = 0;

    std::cout << "\nKnowsall will guess your word letter by letter.\n";

    while (questionsAsked < maxQuestions && guessedWord != secretWord) {
        char guessedLetter = 'a' + (questionsAsked % 26); // Sequential guessing
        std::cout << "Is the letter '" << guessedLetter << "' in your word? (yes/no): ";
        std::string response;
        std::cin >> response;

        // Handle invalid responses
        if (response != "yes" && response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue; // Skip to the next iteration
        }

        if (response == "yes") {
            for (size_t i = 0; i < secretWord.length(); ++i) {
                if (secretWord[i] == guessedLetter) {
                    guessedWord[i] = guessedLetter;
                }
            }
            std::cout << "Updated word: " << guessedWord << "\n";
        } else if (response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue;
        }

        ++questionsAsked;
        std::cout << "Questions remaining: " << (maxQuestions - questionsAsked) << "\n";
    }

    if (guessedWord == secretWord) {
        std::cout << "Knowsall guessed your word: " << secretWord << "\n";
    } else {
        std::cout << "Knowsall couldn't guess your word within " << maxQuestions << " questions.\n";
    }
}

/**
 * Randomized Letter-by-Letter Guessing Mode.
 * Knowsall guesses letters in a random order and updates the guessed word.
 */
void randomizedLetterByLetterGuessing(const std::string& secretWord, const std::vector<std::string>& dictionary) {
    std::unordered_set<char> guessedLetters;
    std::string guessedWord(secretWord.length(), '_');
    int maxQuestions = 20, questionsAsked = 0;

    // Create a shuffled alphabet
    std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(alphabet.begin(), alphabet.end(), g);

    std::cout << "\nKnowsall will guess your word letter by letter in random order.\n";

    while (questionsAsked < maxQuestions && guessedWord != secretWord) {
        char guessedLetter = '\0';

        // Find the next unguessed letter
        for (char letter : alphabet) {
            if (guessedLetters.find(letter) == guessedLetters.end()) {
                guessedLetter = letter;
                guessedLetters.insert(letter);
                break;
            }
        }

        if (guessedLetter == '\0') {
            std::cout << "Knowsall has run out of letters to guess.\n";
            break;
        }

        std::cout << "Is the letter '" << guessedLetter << "' in your word? (yes/no): ";
        std::string response;
        std::cin >> response;

        // Handle invalid responses
        if (response != "yes" && response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue; // Skip to the next iteration
        }

        if (response == "yes") {
            for (size_t i = 0; i < secretWord.length(); ++i) {
                if (secretWord[i] == guessedLetter) {
                    guessedWord[i] = guessedLetter;
                }
            }
            std::cout << "Updated word: " << guessedWord << "\n";
        } else if (response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue;
        }

        ++questionsAsked;
        std::cout << "Questions remaining: " << (maxQuestions - questionsAsked) << "\n";
    }

    if (guessedWord == secretWord) {
        std::cout << "Knowsall guessed your word: " << secretWord << "\n";
    } else {
        std::cout << "Knowsall couldn't guess your word within " << maxQuestions << " questions.\n";
    }
}

/**
 * Word-by-word guessing mode.
 * Knowsall guesses entire words from the dictionary.
 */
void wordByWordGuessing(const std::string& secretWord, const std::vector<std::string>& dictionary) {
    int maxQuestions = 20, questionsAsked = 0;

    std::cout << "\nKnowsall will guess your word word by word.\n";

    for (const auto& word : dictionary) {
        if (questionsAsked >= maxQuestions) break;

        std::cout << "Is your word '" << word << "'? (yes/no): ";
        std::string response;
        std::cin >> response;

        // Handle invalid responses
        if (response != "yes" && response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue; // Skip to the next iteration
        }

        if (response == "yes") {
            std::cout << "Knowsall guessed your word: " << word << "\n";
            return;
        } else if (response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue;
        }

        ++questionsAsked;
    }

    std::cout << "Knowsall couldn't guess your word within " << maxQuestions << " questions.\n";
}

/**
 * Randomized Word-by-Word Guessing Mode.
 * Knowsall guesses entire words in random order from the dictionary.
 */
void randomizedWordByWordGuessing(const std::string& secretWord, std::vector<std::string> dictionary) {
    int maxQuestions = 20, questionsAsked = 0;

    // Shuffle the dictionary
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(dictionary.begin(), dictionary.end(), g);

    std::cout << "\nKnowsall will guess your word word by word in random order.\n";

    for (const auto& word : dictionary) {
        if (questionsAsked >= maxQuestions) break;

        std::cout << "Is your word '" << word << "'? (yes/no): ";
        std::string response;
        std::cin >> response;

        // Handle invalid responses
        if (response != "yes" && response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue; // Skip to the next iteration
        }

        if (response == "yes") {
            std::cout << "Knowsall guessed your word: " << word << "\n";
            return;
        } else if (response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue;
        }

        ++questionsAsked;
    }

    std::cout << "Knowsall couldn't guess your word within " << maxQuestions << " questions.\n";
}

/**
 * Frequency-based guessing mode.
 * Knowsall guesses letters based on their frequency in the English language.
 */
void frequencyBasedGuessing(const std::string& secretWord, const std::vector<std::string>& dictionary) {
    std::unordered_set<char> guessedLetters;
    std::string guessedWord(secretWord.length(), '_');
    int maxQuestions = 20, questionsAsked = 0;
    std::string letterFrequency = "etaoinshrdlcumwfgypbvkjxqz";

    std::cout << "\nKnowsall will guess your word based on letter frequency.\n";

    while (questionsAsked < maxQuestions && guessedWord != secretWord) {
        char guessedLetter = '\0';
        for (char letter : letterFrequency) {
            if (guessedLetters.find(letter) == guessedLetters.end()) {
                guessedLetter = letter;
                guessedLetters.insert(letter);
                break;
            }
        }

        if (guessedLetter == '\0') break;

        std::cout << "Is the letter '" << guessedLetter << "' in your word? (yes/no): ";
        std::string response;
        std::cin >> response;

        // Handle invalid responses
        if (response != "yes" && response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue; // Skip to the next iteration
        }

        if (response == "yes") {
            for (size_t i = 0; i < secretWord.length(); ++i) {
                if (secretWord[i] == guessedLetter) {
                    guessedWord[i] = guessedLetter;
                }
            }
            std::cout << "Updated word: " << guessedWord << "\n";
        } else if (response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue;
        }

        ++questionsAsked;
        std::cout << "Questions remaining: " << (maxQuestions - questionsAsked) << "\n";
    }

    if (guessedWord == secretWord) {
        std::cout << "Knowsall guessed your word: " << secretWord << "\n";
    } else {
        std::cout << "Knowsall couldn't guess your word within " << maxQuestions << " questions.\n";
    }
}

/**
 * Random word guessing mode.
 * Knowsall guesses random words from the dictionary.
 */
void randomWordGuessing(const std::string& secretWord, const std::vector<std::string>& dictionary) {
    int maxQuestions = 20, questionsAsked = 0;
    std::srand(std::time(nullptr));

    std::cout << "\nKnowsall will guess random words from the dictionary.\n";

    while (questionsAsked < maxQuestions) {
        std::string guessedWord = dictionary[std::rand() % dictionary.size()];
        std::cout << "Is your word '" << guessedWord << "'? (yes/no): ";
        std::string response;
        std::cin >> response;

        // Handle invalid responses
        if (response != "yes" && response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue; // Skip to the next iteration
        }

        if (response == "yes") {
            std::cout << "Knowsall guessed your word: " << guessedWord << "\n";
            return;
        } else if (response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue;
        }

        ++questionsAsked;
    }

    std::cout << "Knowsall couldn't guess your word within " << maxQuestions << " questions.\n";
}

/**
 * AI-based guessing mode.
 * Knowsall uses pattern matching and letter frequency analysis to guess the word.
 */
void aiGuessing(const std::string& secretWord, const std::vector<std::string>& dictionary) {
    std::unordered_set<char> guessedLetters;
    std::string guessedWord(secretWord.length(), '_');
    int maxQuestions = 20, questionsAsked = 0;

    std::cout << "\nKnowsall will guess your word using AI-like features.\n";

    std::vector<std::string> possibleWords = dictionary;

    while (questionsAsked < maxQuestions && guessedWord != secretWord) {
        // Filter the dictionary based on the current guessed word pattern
        possibleWords = filterWords(possibleWords, guessedWord, guessedLetters);

        if (possibleWords.empty()) {
            std::cout << "Knowsall has no more possible words to guess.\n";
            break;
        }

        // Calculate letter frequencies in the remaining possible words
        auto letterFrequencies = calculateLetterFrequencies(possibleWords, guessedLetters);

        // Find the most frequent letter
        char guessedLetter = '\0';
        int maxFrequency = 0;
        for (const auto& pair : letterFrequencies) {
            char letter = pair.first;
            int frequency = pair.second;
            if (frequency > maxFrequency) {
                guessedLetter = letter;
                maxFrequency = frequency;
            }
        }

        if (guessedLetter == '\0') {
            std::cout << "Knowsall has run out of letters to guess.\n";
            break;
        }

        guessedLetters.insert(guessedLetter);

        std::cout << "Is the letter '" << guessedLetter << "' in your word? (yes/no): ";
        std::string response;
        std::cin >> response;

        // Handle invalid responses
        if (response != "yes" && response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue; // Skip to the next iteration
        }

        if (response == "yes") {
            for (size_t i = 0; i < secretWord.length(); ++i) {
                if (secretWord[i] == guessedLetter) {
                    guessedWord[i] = guessedLetter;
                }
            }
            std::cout << "Updated word: " << guessedWord << "\n";
        } else if (response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue;
        }

        ++questionsAsked;
        std::cout << "Questions remaining: " << (maxQuestions - questionsAsked) << "\n";
    }

    if (guessedWord == secretWord) {
        std::cout << "Knowsall guessed your word: " << secretWord << "\n";
    } else {
        std::cout << "Knowsall couldn't guess your word within " << maxQuestions << " questions.\n";
    }
}

/**
 * AI-based guessing mode with enhancements.
 * Knowsall uses pattern matching, letter frequency analysis, logging, and a timer to guess the word.
 */
void aiGuessingWithEnhancements(const std::string& secretWord, const std::vector<std::string>& dictionary) {
    std::unordered_set<char> guessedLetters;
    std::string guessedWord(secretWord.length(), '_');
    int maxQuestions = 20, questionsAsked = 0;

    std::cout << "\nKnowsall will guess your word using AI-like features.\n";

    std::vector<std::string> possibleWords = dictionary;

    while (questionsAsked < maxQuestions && guessedWord != secretWord) {
        // Filter the dictionary based on the current guessed word pattern
        possibleWords = filterWords(possibleWords, guessedWord, guessedLetters);

        if (possibleWords.empty()) {
            std::cout << "Knowsall has no more possible words to guess.\n";
            break;
        }

        // Calculate letter frequencies in the remaining possible words
        auto letterFrequencies = calculateLetterFrequencies(possibleWords, guessedLetters);

        // Find the most frequent letter
        char guessedLetter = '\0';
        int maxFrequency = 0;
        for (const auto& pair : letterFrequencies) {
            char letter = pair.first;
            int frequency = pair.second;
            if (frequency > maxFrequency) {
                guessedLetter = letter;
                maxFrequency = frequency;
            }
        }

        if (guessedLetter == '\0') {
            std::cout << "Knowsall has run out of letters to guess.\n";
            break;
        }

        guessedLetters.insert(guessedLetter);

        std::cout << "Is the letter '" << guessedLetter << "' in your word? (yes/no): ";
        std::string response;
        getUserResponseWithTimer(response, 10); // 10-second timer for user response

        logGuess(std::string(1, guessedLetter), response); // Log the guess and response

        // Handle invalid responses
        if (response != "yes" && response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue; // Skip to the next iteration
        }

        if (response == "yes") {
            for (size_t i = 0; i < secretWord.length(); ++i) {
                if (secretWord[i] == guessedLetter) {
                    guessedWord[i] = guessedLetter;
                }
            }
            displayGuessedWord(guessedWord); // Display the updated guessed word
        } else if (response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue;
        }

        ++questionsAsked;
        std::cout << "Questions remaining: " << (maxQuestions - questionsAsked) << "\n";
    }

    if (guessedWord == secretWord) {
        std::cout << "Knowsall guessed your word: " << secretWord << "\n";
        return; // Fixed: Added return after successful guess
    } else {
        std::cout << "Knowsall couldn't guess your word within " << maxQuestions << " questions.\n";
    }
}

/**
 * Challenging mode.
 * Knowsall guesses the word with stricter rules and fewer guesses.
 */
void challengingMode(const std::string& secretWord, const std::vector<std::string>& dictionary) {
    std::unordered_set<char> guessedLetters;
    std::string guessedWord(secretWord.length(), '_');
    int maxQuestions = 10; // Reduced number of guesses
    int questionsAsked = 0;

    std::cout << "\nKnowsall will guess your word in Challenging Mode!\n";
    std::cout << "Rules: Knowsall has only " << maxQuestions << " guesses, and incorrect guesses will cost extra.\n";

    std::vector<std::string> possibleWords = dictionary;

    while (questionsAsked < maxQuestions && guessedWord != secretWord) {
        // Filter the dictionary based on the current guessed word pattern
        possibleWords = filterWords(possibleWords, guessedWord, guessedLetters);

        if (possibleWords.empty()) {
            std::cout << "Knowsall has no more possible words to guess.\n";
            break;
        }

        // Calculate letter frequencies in the remaining possible words
        auto letterFrequencies = calculateLetterFrequencies(possibleWords, guessedLetters);

        // Find the most frequent letter
        char guessedLetter = '\0';
        int maxFrequency = 0;
        for (const auto& pair : letterFrequencies) {
            char letter = pair.first;
            int frequency = pair.second;
            if (frequency > maxFrequency) {
                guessedLetter = letter;
                maxFrequency = frequency;
            }
        }

        if (guessedLetter == '\0') {
            std::cout << "Knowsall has run out of letters to guess.\n";
            break;
        }

        guessedLetters.insert(guessedLetter);

        std::cout << "Is the letter '" << guessedLetter << "' in your word? (yes/no): ";
        std::string response;
        getUserResponseWithTimer(response, 5); // 5-second timer for user response

        // Handle invalid responses
        if (response != "yes" && response != "no") {
            std::cout << "Invalid response. Please answer 'yes' or 'no'.\n";
            continue; // Skip to the next iteration
        }

        if (response == "yes") {
            for (size_t i = 0; i < secretWord.length(); ++i) {
                if (secretWord[i] == guessedLetter) {
                    guessedWord[i] = guessedLetter;
                }
            }
            displayGuessedWord(guessedWord); // Display the updated guessed word
        } else if (response == "no") {
            std::cout << "Incorrect guess! Knowsall loses an extra chance.\n";
            questionsAsked += 2; // Penalty for incorrect guesses
        }

        ++questionsAsked;
        std::cout << "Questions remaining: " << (maxQuestions - questionsAsked) << "\n";
    }

    if (guessedWord == secretWord) {
        std::cout << "Knowsall guessed your word: " << secretWord << "\n";
    } else {
        std::cout << "Knowsall couldn't guess your word within " << maxQuestions << " questions.\n";
    }
}

/**
 * Main game loop.
 * Allows the user to play multiple rounds of the game.
 */
void playGame() {
    std::vector<std::string> dictionary;
    if (!loadDictionary("dictionary.txt", dictionary)) {
        std::cerr << "Failed to load the dictionary. Exiting...\n";
        return;
    }

    if (dictionary.empty()) {
        std::cerr << "The dictionary is empty. Please provide a valid dictionary file.\n";
        return;
    }

    while (true) {
        std::string secretWord;
        std::cout << "Think of a word from the dictionary and Knowsall will try to guess it.\n";
        std::cout << "Enter your secret word (Knowsall won't peek!): ";
        std::cin >> secretWord;

        // Validate the input for secretWord
        if (secretWord.empty()) {
            std::cout << "Invalid input. Please enter a non-empty word.\n";
            continue;
        }

        if (!std::all_of(secretWord.begin(), secretWord.end(), ::isalpha)) {
            std::cout << "Invalid input. Please enter a word containing only letters.\n";
            continue;
        }

        if (std::find(dictionary.begin(), dictionary.end(), secretWord) == dictionary.end()) {
            std::cout << "The word is not in the dictionary. Please choose a valid word.\n";
            continue;
        }

        int choice;
        while (true) {
            std::cout << "\nChoose a guessing mode:\n";
            std::cout << "0. Exit\n"; // Exit option
            std::cout << "1. Randomized Letter-by-Letter Guessing\n"; // Replaced Game Mode 1
            std::cout << "2. Randomized Word-by-Word Guessing\n";     // Replaced Game Mode 2
            std::cout << "3. Frequency-Based Guessing\n";
            std::cout << "4. Random Word Guessing\n";
            std::cout << "5. AI Guessing with Enhancements\n";
            std::cout << "6. Challenging Mode\n";
            std::cout << "Enter your choice (0-6): ";
            std::cin >> choice;

            if (choice >= 0 && choice <= 6) break;
            std::cout << "Invalid choice. Please enter a number between 0 and 6.\n";
        }

        if (choice == 0) {
            std::cout << "Exiting the game. Thank you for playing Knowsall!\n";
            break; // Exit the game loop
        }

        switch (choice) {
            case 1:
                randomizedLetterByLetterGuessing(secretWord, dictionary); // Mode 1 replaced with Mode 7
                break;
            case 2:
                randomizedWordByWordGuessing(secretWord, dictionary);     // Mode 2 replaced with Mode 8
                break;
            case 3:
                frequencyBasedGuessing(secretWord, dictionary);
                break;
            case 4:
                randomWordGuessing(secretWord, dictionary);
                break;
            case 5:
                aiGuessingWithEnhancements(secretWord, dictionary);
                break;
            case 6:
                challengingMode(secretWord, dictionary);
                break;
            default:
                std::cout << "Invalid choice. Exiting...\n";
                break;
        }

        std::string playAgain;
        while (true) {
            std::cout << "\nDo you want to play again? (yes/no): ";
            std::cin >> playAgain;

            if (playAgain == "yes" || playAgain == "no") break;
            std::cout << "Invalid response. Please enter 'yes' or 'no'.\n";
        }

        if (playAgain == "no") {
            break;
        }
    }
    std::cout << "Thank you for playing Knowsall! Goodbye.\n";
}

// Main function to start the game
int main() {
    playGame();
}