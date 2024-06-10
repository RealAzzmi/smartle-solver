#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cassert>

using std::cout, std::cin, std::endl;
using std::ifstream;
using std::pair;
using std::string;
using std::swap;
using std::vector;

constexpr int TAKEN_COMBINATION_SOLUTION = 1000;

char game[5][5];
char letters[25];
int frequency[26];
vector<string> current;
bool taken[26 * 26 * 26 * 26 * 26];
vector<string> dictionary;

vector<vector<string>> word_combination_solutions;

int count = 0;

int char_to_int(char c) {
    return c - 'a';
}

int word_to_int(string word) {
    int result = 0;
    result += char_to_int(word[0]);
    result += char_to_int(word[1]) * 26;
    result += char_to_int(word[2]) * 26 * 26;
    result += char_to_int(word[3]) * 26 * 26 * 26;
    result += char_to_int(word[4]) * 26 * 26 * 26 * 26;
    return result;
}

void read_input() {
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            cin >> game[row][col];
            letters[5 * row + col] = game[row][col];
            frequency[game[row][col] - 'a']++;
        }
    }
}

void read_dictionary() {
    string filename = "priority_words.txt";

    std::ifstream input_file(filename);

    if (input_file.is_open()) {
        string word;

        while (std::getline(input_file, word)) {
            dictionary.push_back(word);
        }

        input_file.close();
    } else {
        std::cerr << "Error: Could not open dictionary '" << filename << "'" << endl;
    }
}

void search() {
    if (current.size() == 5) {
        word_combination_solutions.push_back(current);
        return;
    }

    for (auto word : dictionary) {
        if (word_combination_solutions.size() == TAKEN_COMBINATION_SOLUTION)
            return;

        int word_number = word_to_int(word);
        if (taken[word_number])
            continue;

        for (auto letter : word)
            frequency[letter - 'a']--;

        bool valid = true;
        for (auto letter : word)
            if (frequency[letter - 'a'] < 0)
                valid = false;

        if (!valid) {
            for (auto letter : word) {
                frequency[letter - 'a']++;
            }
            continue;
        }

        taken[word_number] = true;
        current.push_back(word);
        search();
        taken[word_number] = false;
        current.pop_back();
        for (auto letter : word)
            frequency[letter - 'a']++;
    }
}

class Coordinate {
public:
    int x, y;
    Coordinate(int x, int y) : x(x), y(y) {}
    Coordinate() : x(0), y(0) {}
};

class Solution {
public:
    vector<pair<Coordinate, Coordinate>> swaps;
    Solution(vector<pair<Coordinate, Coordinate>> swaps) : swaps(swaps) {}
    Solution() {}
};

Solution calculate_swaps(vector<string> combination_solution) {
    vector<int> indices{0, 1, 2, 3, 4};
    vector<int> optimal_indices;
    int optimal_indices_weight = -1;

    do {
        int current_weight = 0;
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 5; col++) {
                if (combination_solution[indices[row]][col] == game[row][col]) {
                    current_weight++;
                }
            }
        }
        if (current_weight > optimal_indices_weight) {
            optimal_indices_weight = current_weight;
            optimal_indices = indices;
        }
    } while (std::next_permutation(indices.begin(), indices.end()));

    char combination[5][5];
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            combination[i][j] = combination_solution[optimal_indices[i]][j];
        }
    }
    char game_copy[5][5];
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            game_copy[i][j] = game[i][j];
        }
    }

    int number_of_swaps = 0;
    Solution solution;

    while (optimal_indices_weight != 25) {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                if (game_copy[i][j] == combination[i][j])
                    continue;
                for (int x = 0; x < 5; x++) {
                    for (int y = 0; y < 5; y++) {
                        if (i == x && j == y)
                            continue;
                        if (game_copy[x][y] == combination[x][y])
                            continue;

                        if (game_copy[i][j] != game_copy[x][y] && game_copy[i][j] == combination[x][y] && game_copy[x][y] == combination[i][j]) {
                            optimal_indices_weight += 2;
                            number_of_swaps++;
                            solution.swaps.push_back({Coordinate(x, y), Coordinate(i, j)});
                            swap(game_copy[x][y], game_copy[i][j]);
                        }
                    }
                }
            }
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                if (game_copy[i][j] != combination[i][j]) {
                    for (int x = 0; x < 5; x++) {
                        for (int y = 0; y < 5; y++) {
                            if (i == x && j == y)
                                continue;
                            if (game_copy[x][y] == combination[x][y])
                                continue;
                            if (game_copy[x][y] == combination[i][j]) {
                                optimal_indices_weight++;
                                number_of_swaps++;
                                swap(game_copy[x][y], game_copy[i][j]);
                                solution.swaps.push_back({Coordinate(x, y), Coordinate(i, j)});
                                goto done;
                            }
                        }
                    }
                }
            }
        }
    done:;
    }

    return solution;
}

int main() {
    read_input();
    read_dictionary();
    cout << "Searching..." << endl;
    search();
    cout << "Done searching..." << endl;

    Solution solution;
    solution.swaps = vector<pair<Coordinate, Coordinate>>(26, {Coordinate(), Coordinate()});
    vector<string> optimal_combination;
    for (auto combination_solution : word_combination_solutions) {
        auto current_swaps = calculate_swaps(combination_solution);
        if (current_swaps.swaps.size() < solution.swaps.size()) {
            solution.swaps = current_swaps.swaps;
            optimal_combination = combination_solution;
        }
    }

    cout << "Minimum number of swaps: " << solution.swaps.size() << '\n';

    for (auto swap : solution.swaps) {
        cout << "(" << swap.first.x << ", " << swap.first.y << "), (" << swap.second.x << ", " << swap.second.y << ")" << '\n';
    }
    cout << '\n';
    for (auto word : optimal_combination) {
        cout << word << ' ';
    }
    cout << endl;
}