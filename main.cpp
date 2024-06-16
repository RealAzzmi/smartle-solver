#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using std::cout, std::cin, std::endl;
using std::ifstream;
using std::pair;
using std::string;
using std::swap;
using std::thread;
using std::vector;

constexpr int TAKEN_COMBINATION_SOLUTION = 100'000;
constexpr int NUM_OF_THREADS = 8;

char game[5][5];
char letters[25];
int frequency[NUM_OF_THREADS][26];
vector<string> current[NUM_OF_THREADS];
bool taken[NUM_OF_THREADS][26 * 26 * 26 * 26 * 26];
vector<string> dictionary;

vector<vector<string>> word_combination_solutions[NUM_OF_THREADS];

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
            for (int i = 0; i < NUM_OF_THREADS; i++) {
                frequency[i][game[row][col] - 'a']++;
            }
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

void search(int thread_id, vector<string>::iterator current_it) {
    if (current[thread_id].size() == 5) {
        word_combination_solutions[thread_id].push_back(current[thread_id]);
        return;
    }

    for (auto it = current_it; it != dictionary.end(); it++) {
        if (word_combination_solutions[thread_id].size() == TAKEN_COMBINATION_SOLUTION)
            return;

        string word = *it;
        int word_number = word_to_int(word);
        if (taken[thread_id][word_number])
            continue;

        for (auto letter : word)
            frequency[thread_id][letter - 'a']--;

        bool valid = true;
        for (auto letter : word)
            if (frequency[thread_id][letter - 'a'] < 0)
                valid = false;

        if (!valid) {
            for (auto letter : word) {
                frequency[thread_id][letter - 'a']++;
            }
            continue;
        }

        taken[thread_id][word_number] = true;
        current[thread_id].push_back(word);
        search(thread_id, it + 1);
        taken[thread_id][word_number] = false;
        current[thread_id].pop_back();
        for (auto letter : word)
            frequency[thread_id][letter - 'a']++;
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
    int matched = -1;

    do {
        int current_weight = 0;
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 5; col++) {
                if (combination_solution[indices[row]][col] == game[row][col]) {
                    current_weight++;
                }
            }
        }
        if (current_weight > matched) {
            matched = current_weight;
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

    Solution solution;

    while (matched != 25) {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                if (game_copy[i][j] == combination[i][j])
                    continue;
                for (int x = 0; x < 5; x++) {
                    for (int y = 0; y < 5; y++) {
                        if (game_copy[x][y] == combination[x][y])
                            continue;

                        if (game_copy[i][j] == combination[x][y] && game_copy[x][y] == combination[i][j]) {
                            matched += 2;
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
                            if (game_copy[x][y] == combination[x][y])
                                continue;
                            if (game_copy[x][y] == combination[i][j]) {
                                matched++;
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

void start_searching(int thread_id) {
    for (int i = thread_id; i < dictionary.size(); i += NUM_OF_THREADS) {
        search(thread_id, dictionary.begin() + i);
    }
}

int main() {
    read_input();
    cout << "Reading input completed" << endl;
    read_dictionary();
    cout << "Reading dictionary completed" << endl;
    cout << "Searching possible 5 word combinations (number of threads = " << NUM_OF_THREADS << ", combinations limit per thread = " << TAKEN_COMBINATION_SOLUTION << ")..." << endl;

    vector<thread> threads(NUM_OF_THREADS);
    for (int i = 0; i < NUM_OF_THREADS; ++i) {
        threads[i] = thread(start_searching, i);
    }
    for (int i = 0; i < NUM_OF_THREADS; ++i) {
        threads[i].join();
    }
    cout << "Searching completed..." << endl;
    cout << "Checking each possible configuration..." << endl;

    int total_combinations = 0;
    Solution solution;
    solution.swaps = vector<pair<Coordinate, Coordinate>>(26, {Coordinate(), Coordinate()});
    vector<string> optimal_combination;
    for (int i = 0; i < NUM_OF_THREADS; ++i) {
        total_combinations += word_combination_solutions[i].size();
        for (auto combination_solution : word_combination_solutions[i]) {
            auto current_swaps = calculate_swaps(combination_solution);
            if (current_swaps.swaps.size() < solution.swaps.size()) {
                solution.swaps = current_swaps.swaps;
                optimal_combination = combination_solution;
            }
        }
    }
    cout << "Checked " << total_combinations << " combinations!" << endl;

    cout << "Minimum number of swaps: " << solution.swaps.size() << '\n';

    for (auto swap : solution.swaps) {
        cout << game[swap.first.x][swap.first.y] << "(" << swap.first.x + 1 << ", " << swap.first.y + 1 << "), " << game[swap.second.x][swap.second.y] << "(" << swap.second.x + 1 << ", " << swap.second.y + 1 << ")" << '\n';
        std::swap(game[swap.first.x][swap.first.y], game[swap.second.x][swap.second.y]);
    }
    cout << '\n';
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            cout << game[row][col];
        }
        cout << '\n';
    }
    cout << endl;
}