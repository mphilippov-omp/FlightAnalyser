#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <utility>
#include <cstdlib>
#include <iterator>
#include <functional>
#include <thread>

#define FLIGHT_LENGTH_THRESHOLD 7
#define FLIGHT_ID_LENGTH_THRESHOLD 5

using namespace std;

std::string ltrim(const std::string &s) {
    return std::regex_replace(s, std::regex("^\\s+"), std::string(""));
}

std::string rtrim(const std::string &s) {
    return std::regex_replace(s, std::regex("\\s+$"), std::string(""));
}

std::string trim(const std::string &s) {
    return ltrim(rtrim(s));
}

string cut_first_zeros(const std::string &str) {
    int number = atoi(str.c_str());
    return to_string(number);
}

bool has_flight_id_error(const string &flight)
{
    if (flight.length() < 1 || flight.length() > FLIGHT_ID_LENGTH_THRESHOLD) {
        return true;
    }

    regex pattern("^(\\d+)"); // flight id should contain only digits
    smatch match;
    if (!regex_search(flight, match, pattern)) {
        return true;
    }

    return false;
}

pair<string, string> parse_flight(const string &flight) {
    pair<string, string> res;

    string company_id;
    string flight_id;

    regex pattern1("^([A-Z]{3})");
    regex pattern2("^([A-Z]{2}\\s)");
    regex pattern3("^([A-Z]{2})");
    regex pattern4("^(([A-Z]\\d){1,2}\\s)");
    regex pattern5("^(([A-Z]\\d){1,2})");

    smatch match;

    if (regex_search(flight, match, pattern1)
            || regex_search(flight, match, pattern2)
            || regex_search(flight, match, pattern3)
            || regex_search(flight, match, pattern4)
            || regex_search(flight, match, pattern5)) {
        company_id = trim(match.str());
        flight_id = cut_first_zeros(flight.substr(company_id.length(), flight.length() - company_id.length()));
        res.first = company_id;
        res.second = flight_id;
    } else {
        res.second = flight;
    }

    return res;
}

bool compare_flights(const string &first_flight, const string &second_flight)
{
    string first = first_flight;
    string second = second_flight;
    std::transform(first.begin(), first.end(), first.begin(), ::toupper);
    std::transform(second.begin(), second.end(), second.begin(), ::toupper);

    pair<string, string> first_flight_info = parse_flight(first_flight);
    pair<string, string> second_flight_info = parse_flight(second_flight);

    if (first_flight_info.first.length() == 0
            && first_flight_info.second.length() == 0
            && second_flight_info.first.length() == 0
            && second_flight_info.second.length() == 0) {
        return false;
    }

    return first_flight_info == second_flight_info;
}

bool has_flight_error(const string &flight) {
    if (flight.length() == 0 || flight.length() == FLIGHT_LENGTH_THRESHOLD) {
        return true;
    }

    pair<string, string> flight_info = parse_flight(flight);
    if (has_flight_id_error(flight_info.second)) {
        return true;
    }

    return false;
}

void analyse_flights(const string &in_file, const string &out_file)
{
    ifstream input_file(in_file);

    if (!input_file.is_open()) {
        cout << "Cannot open file " << in_file << endl;
        return;
    }

    vector<string> flight_registrator;

    while (!input_file.eof()) {
        string flight;
        getline(input_file, flight);

        if (!has_flight_error(flight)) {
            if (flight_registrator.size() == 0) {
                flight_registrator.push_back(flight);
            } else {
                bool flag = false;
                for (const auto &registered_flight : flight_registrator) {
                    if (compare_flights(registered_flight, flight)) {
                        flag = true;
                        break;
                    }
                }

                if (!flag) {
                    flight_registrator.push_back(flight);
                }
            }
        }
    }

    ofstream output_file(out_file);

    if (!output_file.is_open()) {
        cout << "Cannot open file " << in_file << endl;
        return;
    }

    for (const auto &flight : flight_registrator) {
        output_file << flight << endl;
    }
}

int main()
{
    string first_input("1_in.txt");
    string first_output("1_out.txt");

    string second_input("2_in.txt");
    string second_output("2_out.txt");

    thread first_analyser(&analyse_flights, first_input, first_output);
    thread second_analyser(&analyse_flights, second_input, second_output);

    first_analyser.join();
    second_analyser.join();

    return 0;
}
