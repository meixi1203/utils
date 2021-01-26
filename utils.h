//
// Created by zhoupenghui on 2021/1/23.
//

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdarg>
#include <memory>

namespace utils {

const std::string sep = " \n\t\v\f\r";

inline std::string ToUpper(const std::string &str){
    std::string s(str);
    std::transform(s.begin(), s.end(), s.begin(), toupper);
    return s;
}

inline std::string ToLower(const std::string &str){
    std::string s(str);
    std::transform(s.begin(), s.end(), s.begin(), tolower);
    return s;
}

inline std::string ltrim(const std::string& str) {
    if (str.empty() || !std::isspace(str.front())) {
        return str;
    }
    auto start = str.find_first_not_of(sep);
    return str.substr(start);
}

inline std::string rtrim(const std::string& str) {
    if (str.empty() || !std::isspace(str.back())) {
        return str;
    }
    auto end = str.find_last_not_of(sep);
    return str.substr(0, end + 1);
}

inline std::string trim(const std::string& str) {
    if (str.empty() ||
        (!std::isspace(str.front()) && !std::isspace(str.back()))) {
        return str;
    }
    auto start = str.find_first_not_of(sep);
    auto end = str.find_last_not_of(sep);
    return str.substr(start, end - start + 1);
}

inline std::string trim_(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it)) {
        it++;
    }

    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && isspace(*rit)) {
        rit++;
    }

    return std::string(it, rit.base());
}

inline std::string::size_type sep_size(const std::string& s) {
    return s.size();
}

inline std::string::size_type sep_size(const char& c) {
    return 1;
}

template <typename Sep>
std::vector<std::string> split_impl(const std::string& str, const Sep& sep) {
    std::vector<std::string> result;
    std::string::size_type last(0);
    auto index = str.find(sep, last);
    while (index != std::string::npos) {
        result.emplace_back(str.substr(last, index - last));
        last = index + sep_size(sep);
        index = str.find(sep, last);
    }
    result.emplace_back(str.substr(last));
    return result;
}

std::vector<std::string> split(const std::string& str, char c = ',') {
    std::string line("");
    std::stringstream ss(str);
    std::vector<std::string> result;
    while (std::getline(ss, line, c)) {
        result.push_back(trim(line));
    }
    return result;
}

std::vector<std::string> SplitString(const std::string& str, char seperator) {
    std::vector<std::string> results;

    std::string::size_type start = 0;
    std::string::size_type sep = str.find(seperator);
    while (sep != std::string::npos) {
        if (start < sep)
        {
            results.emplace_back(str.substr(start, sep - start));
        }

        start = sep + 1;
        sep = str.find(seperator, start);
    }

    if (start != str.size()) {
        results.emplace_back(str.substr(start));
    }

    return results;
}

std::vector<std::string> split(const std::string &token, size_t len) {
    std::vector<std::string> result;
    for (size_t index = 0; index < token.size(); index = index + len) {
        std::string sub = std::move(token.substr(index, len));
        result.push_back(sub);
    }
    return result;
}

std::vector<std::string> split(const std::string& text, const std::string& delims)
{
    std::vector<std::string> tokens;
    std::size_t start = text.find_first_not_of(delims), end = 0;

    while((end = text.find_first_of(delims, start)) != std::string::npos)
    {
        tokens.push_back(text.substr(start, end - start));
        start = text.find_first_not_of(delims, end);
    }
    if(start != std::string::npos)
        tokens.push_back(text.substr(start));

    return tokens;
}

inline std::vector<std::string> compact(const std::vector<std::string> &tokens){
    std::vector<std::string> result;
    for(size_t i=0; i<tokens.size(); ++i) {
        if (!tokens[i].empty()) {
            result.push_back(tokens[i]);
        }
    }

    return result;
}

inline std::string join(const std::vector<std::string> &tokens, const std::string &delim) {
    auto compacted = std::move(compact(tokens));
    std::stringstream ss;
    for(size_t i=0; i<tokens.size()-1; ++i) {
        ss << tokens[i] << delim;
    }
    ss << compacted[tokens.size()-1];

    return ss.str();
}

inline std::string join(const std::string &tokens, const std::string &delim, size_t len) {
    auto array = std::move(split(tokens, len));
    return join(array, delim);
}

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    std::unique_ptr<char[]> buf( new char[ size ] ); 
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

// requires at least C++11
const std::string vformat(const char * const zcFormat, ...) {

    // initialize use of the variable argument array
    va_list vaArgs;
    va_start(vaArgs, zcFormat);

    // reliably acquire the size
    // from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy(vaArgsCopy, vaArgs);
    const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
    va_end(vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    char zc[iLen + 1] = {0} ;
    std::vsnprintf(zc, iLen, zcFormat, vaArgs);
    va_end(vaArgs);
    return std::string(zc, iLen);
}

template<typename T>
inline std::string tostr(T value) {
    std::ostringstream s;
    s.precision(std::numeric_limits<T>::digits10);
    s << value;
    return s.str();
}

}

#endif // UTILS_H