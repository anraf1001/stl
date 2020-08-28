#include "advancedCalculator.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>
#include <map>
#include <regex>

std::map<std::string, std::function<double(double, double)>> commands{{"+", std::plus<double>()},
                                                                      {"-", std::minus<double>()},
                                                                      {"*", std::multiplies<double>()},
                                                                      {"/", std::divides<double>()},
                                                                      {"%", std::modulus<int>()},
                                                                      {"^", [](double base, double exponent) { return pow(base, exponent); }},
                                                                      {"$", [](double num, double root) { return pow(num, 1.0 / root); }},
                                                                      {"!", [](double num, double whatever = 0.0) { return tgamma(num + 1.0); }}};

void exportKeys(std::string& operators) {
    for (const auto& el : commands) {
        operators.append(el.first);
    }
}

bool checkCharacters(const std::string& input) {
    std::string allowedSigns{" ,."};
    exportKeys(allowedSigns);

    return std::any_of(input.cbegin(), input.cend(), [&allowedSigns](char letter) {
        return std::find(allowedSigns.cbegin(), allowedSigns.cend(), letter) == allowedSigns.cend() && !isdigit(letter);
    });
}

bool checkIfNumberIsFloat(const std::string& num) {
    return stoi(num) != stod(num);
}

ErrorCode process(std::string input, double* out) {
    if (checkCharacters(input)) {
        return ErrorCode::BadCharacter;
    }

    const std::regex commandRegex(R"((([-]?\d+[.]*\d*)[ ]*([+\-*\/$^%])[ ]*([-]?\d+[.]*\d*))|(([-]?\d+[.]*\d*)[ ]*!))");
    std::smatch match;

    if (std::regex_search(input, match, commandRegex)) {
        if (match[0] != input) {
            return ErrorCode::BadFormat;
        } else if (match[1] == input) {
            if (match[3] == "/" && stod(match[4]) == 0.0) {
                return ErrorCode::DivideBy0;
            } else if (match[3] == "%" && (checkIfNumberIsFloat(match[2]) || checkIfNumberIsFloat(match[4]))) {
                return ErrorCode::ModuleOfNonIntegerValue;
            } else if (match[3] == "$" && stod(match[2]) < 0) {
                return ErrorCode::SqrtOfNagativeNumber;
            }

            *out = commands[match[3]](stod(match[2]), stod(match[4]));
            return ErrorCode::OK;
        }

        *out = commands["!"](stod(match[6]), 0.0);
        return ErrorCode::OK;
    }
    return ErrorCode::BadFormat;
}
