#include <algorithm>
#include <array>
#include <execution>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <numeric>
#include <string>
#include <thread>
#include <vector>


enum class color { none, yellow, green };

using input_t = std::array<std::pair<char, color>, 5>;

constexpr int wordlimit = 20;
constexpr int suggestlimit = 20;

std::vector<std::string> allwords, validwords, allvalidwords;

static long long numelim(const std::string& word);

static inline void new_game()
{
	validwords = allvalidwords;
	std::cout << std::endl << std::endl << "Starting new game..." << std::endl << std::endl;
}

// ask user to input result of word
static inline input_t getuserinput()
{
	std::string word, result;
	input_t final_result;
	bool invalid = true;
	while (word.size() != 5 || result.size() != 5 || invalid)
	{
		std::cout << "Word entered    : ";
		std::getline(std::cin, word);
		if (word == "new game")
		{
			new_game();
			continue;
		}

		if (!std::binary_search(allwords.begin(), allwords.end(), word))
		{
			invalid = true;
			continue;
		}

		std::cout << "Result (colors) : ";
		std::getline(std::cin, result);
		if (result == "eval")
		{
			auto score = numelim(word);
			std::cout << score << " " << word << std::endl << std::endl;
			continue;
		}
		else if (result == "compare")
		{
			std::string word2;
			std::cout << "Second word     : ";
			std::getline(std::cin, word2);
			auto score1 = numelim(word);
			auto score2 = numelim(word2);
			if (score1 == score2)
			{
				std::cout << "Words have same score" << std::endl << std::endl;
			}
			else if (score1 < score2)
			{
				std::cout << word2 << " is better by ~" << (static_cast<double>(score2) / score1) * 100 - 100 << "%" << std::endl << std::endl;
			}
			else
			{
				std::cout << word << " is better by ~" << (static_cast<double>(score1) / score2) * 100 - 100 << "%" << std::endl << std::endl;
			}
		}

		invalid = false;
		for (int i = 0; i < 5; i++)
		{
			if (result[i] != ' ' && result[i] != 'y' && result[i] != 'g')
			{
				invalid = true;
				break;
			}
		}
	}
	for (int i = 0; i < 5; i++)
	{
		color c;
		if (result[i] == ' ')
		{
			c = color::none;
		}
		if (std::tolower(result[i]) == 'y')
		{
			c = color::yellow;
		}
		if (std::tolower(result[i]) == 'g')
		{
			c = color::green;
		}
		final_result[i] = { std::tolower(word[i]), c };
	}
	return final_result;
}

// check if all green (won)
static inline bool win(const input_t& input)
{
	for (int i = 0; i < 5; i++)
	{
		if (input[i].second != color::green)
		{
			return false;
		}
	}
	return true;
}

// check if word matches input
static inline bool valid(const input_t& input, const std::string& word)
{
	std::array<char, 5> yellows = {};
	std::array<bool, 5> used = {};
	int yellows_size = 0;

	// green, char in same position must match
	for (int i = 0; i < 5; i++)
	{
		if (input[i].second == color::green)
		{
			if (word[i] != input[i].first)
			{
				return false;
			}
			used[i] = true;
		}
	}

	// yellow, char must be somewhere in the word, but at a different position
	for (int i = 0; i < 5; i++)
	{
		if (input[i].second == color::yellow)
		{
			if (word[i] == input[i].first)
			{
				return false;
			}
			yellows[yellows_size] = input[i].first;
			yellows_size++;
		}
	}
	for (int i = 0; i < yellows_size; i++)
	{
		bool valid = false;
		for (int j = 0; j < 5; j++)
		{
			if (!used[j] && yellows[i] == word[j])
			{
				valid = true;
				used[j] = true;
				break;
			}
		}
		if (!valid)
		{
			return false;
		}
	}

	// gray, char must not be present
	for (int i = 0; i < 5; i++)
	{
		if (input[i].second == color::none)
		{
			for (int j = 0; j < 5; j++)
			{
				if (!used[j] && input[i].first == word[j])
				{
					return false;
				}
			}
		}
	}


	return true;
}

// remove invalid words from validwords
static inline void eliminatewords(const input_t& input)
{
	validwords.erase(std::remove_if(std::execution::unseq, validwords.begin(), validwords.end(), [&](const std::string& s) -> bool
	{
		// return true (remove) if not valid
		return !valid(input, s);
	}), validwords.end());
}

static inline input_t guess(const std::string& guessed_word, const std::string& real_word)
{
	input_t res = {};
	// whether letters in real_word have been accounted for
	std::array<bool, 5> used = {};
	for (int i = 0; i < 5; i++)
	{
		res[i].first = guessed_word[i];
		if (guessed_word[i] == real_word[i])
		{
			res[i].second = color::green;
			used[i] = true;
			continue;
		}
	}
	for (int i = 0; i < 5; i++)
	{
		if (res[i].second != color::none)
		{
			continue;
		}

		bool found = false;
		for (int j = 0; j < 5; j++)
		{
			if (!used[j] && guessed_word[i] == real_word[j])
			{
				res[i].second = color::yellow;
				used[j] = true;
				found = true;
				break;
			}
		}
		if (!found)
		{
			res[i].second = color::none;
		}
	}
	return res;
}

// number of eliminations for input
static inline long long numelim(const input_t& input)
{
	unsigned long long num = std::count_if(std::execution::unseq, validwords.begin(), validwords.end(), [&input](const std::string& s)
	{
		// eliminate if not valid
		return !valid(input, s);
	});

	//num *= validwords.size() - num;
	return num;
}

// total number of eliminations for word
static inline long long numelim(const std::string& word)
{
	unsigned long long sum = std::transform_reduce(std::execution::unseq, validwords.begin(), validwords.end(), 0, std::plus<>(), [&word](const auto& test_real_word)
	{
		input_t temp = guess(word, test_real_word);
		return numelim(temp);
	});

	return sum;
}

// best next guesses based on number of eliminations
static inline std::vector<std::pair<std::string, long long>> bestchoices()
{
	std::vector<std::pair<std::string, long long>> v(allwords.size());
	std::transform(std::execution::par_unseq, allwords.begin(), allwords.end(), v.begin(), [](const std::string& word)
	{
		auto num = numelim(word);
		return std::make_pair(word, num);
	});

	// remove if 0
	v.erase(std::remove_if(v.begin(), v.end(), [](const std::pair<std::string, long long>& p) -> bool
	{
		return p.second == 0;
	}), v.end());

	// sort by second, descending, then if equal prioritize valid words
	std::sort(v.begin(), v.end(), [](const std::pair<std::string, long long>& a, const std::pair<std::string, long long>& b) -> bool
	{
		if (a.second > b.second)
		{
			return true;
		}
		if (a.second < b.second)
		{
			return false;
		}
		// prioritize valid words if value is equal
		bool a_res = std::binary_search(validwords.begin(), validwords.end(), a.first);
		bool b_res = std::binary_search(validwords.begin(), validwords.end(), b.first);
		if (a_res && !b_res)
		{
			return true;
		}
		if (b_res && !a_res)
		{
			return false;
		}
		// prioritize possible answers if value is equal and neither are valid
		a_res = std::binary_search(allvalidwords.begin(), allvalidwords.end(), a.first);
		b_res = std::binary_search(allvalidwords.begin(), allvalidwords.end(), b.first);
		if (a_res && !b_res)
		{
			return true;
		}
		return false;
	});

	// limit number of suggestions
	if (v.size() > suggestlimit)
	{
		v.erase(v.begin() + suggestlimit, v.end());
	}

	return v;
}

int main()
{
	// read list of all words
	{
		std::string s;
		std::ifstream fin("wordle_words.txt"); // assuming lowercase, length 5
		while (std::getline(fin, s))
		{
			if (!s.empty())
			{
				allwords.push_back(s);
			}
		}
		std::sort(allwords.begin(), allwords.end());
	}

	{
		std::string s;
		std::ifstream fin("wordle_valid_words.txt"); // assuming lowercase, length 5
		while (std::getline(fin, s))
		{
			if (!s.empty())
			{
				allvalidwords.push_back(s);
			}
		}
		std::sort(allvalidwords.begin(), allvalidwords.end());
	}

	validwords = allvalidwords;

	/*auto v = bestchoices();
	for (const auto& p : v)
	{
	std::cout << p.second << " " << p.first << std::endl;
	}*/

	while (true)
	{
		input_t curinput = getuserinput();
		if (win(curinput))
		{
			new_game();
			continue;
		}

		eliminatewords(curinput);

		std::cout << std::endl << validwords.size() << " possible words remaining" << std::endl;

		if (validwords.size() <= wordlimit)
		{
			std::vector<std::pair<std::string, long long>> v;
			for (const auto& s : validwords)
			{
				v.emplace_back(s, numelim(s));
			}
			std::sort(v.begin(), v.end(), [](const std::pair<std::string, long long>& a, const std::pair<std::string, long long>& b) -> bool
			{
				return a.second > b.second;
			});
			for (const auto& p : v)
			{
				std::cout << p.second << " " << p.first << std::endl;
			}
			std::cout << std::endl;
		}

		auto v = bestchoices();

		std::cout << "Top choices: " << std::endl;
		for (const auto& p : v)
		{
			std::cout << p.second << " " << p.first << std::endl;
		}
		std::cout << std::endl;
	}
}
