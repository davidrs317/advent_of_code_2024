#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <expected>
#include <numeric>

enum class input_error
{
    empty_input,
    different_sizes
};

auto get_difference_score(std::string filename) -> std::expected<int, input_error>;
auto get_similarity_score(std::string filename) -> std::expected<int, input_error>;
auto get_number_lists(std::string filename) -> std::expected<std::pair<std::vector<int>, std::vector<int>>, input_error>;
auto main(int, char**) -> int
{
    std::string filename = "aoc_puzzle_one_input.txt";
    auto difference_score_result = get_difference_score(filename);
    if (difference_score_result.has_value())
        std::cout << "total distance: " << difference_score_result.value() << "\n";
    else if (difference_score_result.error() == input_error::different_sizes)
        std::cout << "total distance could not be calculated because input lists were of different sizes, please verify input\n";
    else if (difference_score_result.error() == input_error::empty_input)
        std::cout << "one or both of the input lists are empty, please verify input\n";
    else
        std::cout << "unknown error occurred, debugging necessary\n";
    auto similarity_score_result = get_similarity_score(filename);
    if(similarity_score_result.has_value())
        std::cout << "total similarity score is: " << similarity_score_result.value() << "\n";
    else if(similarity_score_result.error() == input_error::different_sizes)
        std::cout << "total similarity could not be calculated because input lists were of different sizes, please verify input\n";
    else if (similarity_score_result.error() == input_error::empty_input)
        std::cout << "one or both of the input lists are empty, please verify input\n";
    else
        std::cout << "unknown error occurred, debugging necessary\n";
}

auto get_number_lists(std::string filename) -> std::expected<std::pair<std::vector<int>, std::vector<int>>, input_error>
{
    std::ifstream infile(filename);
    std::string cur_str;
    std::vector<int> list_one_nums;
    std::vector<int> list_two_nums;

    while (std::getline(infile, cur_str))
    {
        std::stringstream ss(cur_str);
        std::string first_word;
        std::string second_word;

        ss >> first_word;
        ss >> second_word;
        list_one_nums.push_back(std::stoi(first_word));
        list_two_nums.push_back(std::stoi(second_word));
    }

    if (list_one_nums.size() != list_two_nums.size())
        return std::unexpected(input_error::different_sizes);
    else if (list_one_nums.empty() || list_two_nums.empty())
        return std::unexpected(input_error::empty_input);
    else
        return std::make_pair(list_one_nums, list_two_nums);
}

auto get_difference_score(std::string filename) -> std::expected<int, input_error>
{
    std::vector<int> list_one_nums;
    std::vector<int> list_two_nums;
    std::vector<int> differences;

    auto list_pair = get_number_lists(filename);
    if (list_pair.has_value())
        std::tie(list_one_nums, list_two_nums) = list_pair.value();
    else
        return std::unexpected(list_pair.error());

    std::sort(list_one_nums.begin(), list_one_nums.end());
    std::sort(list_two_nums.begin(), list_two_nums.end());

    for (int i = 0; i < list_one_nums.size(); ++i)
        differences.push_back(abs(list_one_nums[i] - list_two_nums[i]));

    return std::accumulate(differences.begin(), differences.end(), 0);
}

auto get_similarity_score(std::string filename) -> std::expected<int, input_error> 
{
    std::unordered_map<int, int> seen_nums;
    std::unordered_map<int, int> similarity_scores;
    std::vector<int> list_one_nums;
    std::vector<int> list_two_nums;
    int total_similarity_score = 0;

    auto expected_number_lists = get_number_lists(filename);
    if (expected_number_lists.has_value())
        std::tie(list_one_nums, list_two_nums) = expected_number_lists.value();
    else
        return std::unexpected(expected_number_lists.error());

    for (const auto & num : list_two_nums)
    {
        auto it = seen_nums.find(num);
        if (it != seen_nums.end())
            it->second += 1;
        else
            seen_nums.insert(std::make_pair(num, 1));
    }

    for (const auto & num : list_one_nums)
    {
        auto it = similarity_scores.find(num);
        if (it != similarity_scores.end())
        {
            auto it2 = seen_nums.find(num);
            if (it2 != seen_nums.end())
                it->second += (num * it2->second);
            else
                it->second = 0;
        }
        else
        {
            auto it2 = seen_nums.find(num);
            if (it2 != seen_nums.end())
                similarity_scores.insert(std::make_pair(num, (it2->second * num)));
            else
                similarity_scores.insert(std::make_pair(num, 0));
        }
    }

    for (const auto & keyVal : similarity_scores)
        total_similarity_score += keyVal.second;
    return total_similarity_score;   
}