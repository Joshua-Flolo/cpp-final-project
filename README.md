# Knowsall

Knowsall is a word-guessing game based on the classic "20 Questions" concept. The program attempts to guess a word you think of by asking yes/no questions or making direct guesses. It uses a dictionary file to expand its knowledge and offers multiple guessing modes for a fun and interactive experience.

## Features
- **Randomized Letter-by-Letter Guessing**: Knowsall guesses letters in a random order and updates the guessed word.
- **Randomized Word-by-Word Guessing**: Knowsall guesses entire words in a random order from the dictionary.
- **Frequency-Based Guessing**: Knowsall guesses letters based on their frequency in the English language.
- **Random Word Guessing**: Knowsall guesses random words from the dictionary.
- **AI Guessing with Enhancements**: Knowsall uses pattern matching, letter frequency analysis, and a timer to guess the word.
- **Challenging Mode**: Knowsall guesses the word with stricter rules, fewer guesses, and penalties for incorrect guesses.

## How to Play
1. Think of a word from the dictionary.
2. Enter the word when prompted (Knowsall won't peek!).
3. Choose a guessing mode:
   - Randomized Letter-by-Letter Guessing
   - Randomized Word-by-Word Guessing
   - Frequency-Based Guessing
   - Random Word Guessing
   - AI Guessing with Enhancements
   - Challenging Mode
4. Answer Knowsall's questions with "yes" or "no."
5. See if Knowsall can guess your word within the allowed number of questions!

## Requirements
- A C++ compiler (e.g., `g++`).
- A dictionary file (`dictionary.txt`) containing a list of words, one per line.

## How to Run
1. Compile the program:
   ```bash
   g++ -std=c++17 -o knowsall Knowsall.cpp
   ```

2. Run the program:
   ```bash
   ./knowsall
   ```

3. Choose a guessing mode:
   ```markdown
   Choose a guessing mode:
   0. Exit
   1. Randomized Letter-by-Letter Guessing
   2. Randomized Word-by-Word Guessing
   3. Frequency-Based Guessing
   4. Random Word Guessing
   5. AI Guessing with Enhancements
   6. Challenging Mode
   Enter your choice (0-6):
   ```

4. Follow the on-screen instructions to play the game.