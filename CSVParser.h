//
// Created by zhoupenghui on 2020/10/7.
//

#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <unordered_map>

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

    std::vector<std::string> split(const std::string &token, size_t len) {
        std::vector<std::string> result;
        for (size_t index = 0; index < token.size(); index = index + len) {
            std::string sub = std::move(token.substr(index, len));
            result.push_back(sub);
        }
        return result;
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
}

struct Line
{
    Line(const std::string &line) {
        array_ = std::move(utils::split(line));
    }

    Line(const Line &line) {
        this->array_ = line.array_;
    }

    Line(Line &&line) {
        this->array_ = std::move(line.array_);
        line.array_.clear();
    }

    Line() = default;

    Line& operator=(const Line &line) {
        if (this != &line) {
            this->array_ = line.array_;
            return *this;
        }
        return *this;
    }

    std::string operator[](size_t index) const {
        if (index < fields()) {
            return array_[index];
        }
        return std::string();
    }

    size_t fields() const { return array_.size(); }

    void print() const {
        std::stringstream ss;
        for (size_t i = 0; i < array_.size(); i++) {
            if (i != array_.size() - 1) {
                ss << array_[i] << ",";
            } else {
                ss << array_[i];
            }
        }
        std::cout << ss.str() << std::endl;
    }
    std::vector<std::string> array_;
};

class CSVParse
{
public:
    CSVParse(const std::string &file, std::vector<std::string> &&key) : key_(std::move(key)) {
        reserve();
        if (parse(file)) {
            isReady_ = true;
        }
    }

    bool operator!() {
      return !isReady_;
    }

    ~CSVParse() = default;

    bool parse(const std::string &file) {
        if (file.empty()) {
            return false;
        }

        std::ifstream io(file.data());
        if (!io) {
            return false;
        }

        std::string s("");
        getline(io, s);
        ParseHeader(s);

        size_t count = 0;
        while (getline(io, s)) {
            Line line(s);
            context_.push_back(line);
            GenerateIndex(line, count++);
        }
        io.close();
        return true;
    }

    Line GetLine(size_t row) {
        if (row < GetRow()) {
            return context_[row];
        }

        return Line(std::string());
    }

    Line operator[](size_t row) {
        return GetLine(row);
    }

    Line GetLine(std::unordered_map<std::string, std::string> &&keys) {
        if (keys.size() != key_.size()) {
            return query(keys);
        }

        std::vector<std::string> array(GetColumn());
        for (auto iter = keys.begin(); iter != keys.end(); iter++) {
            array[header2index_[iter->first]] = iter->second;
        }

        std::string key("");
        for (size_t i = 0; i < array.size(); i++) {
            if (!array[i].empty()) {
                key.append(array[i]);
            }
        }

        auto find = index_.find(key);
        if (find != index_.end()) {
            return context_[find->second];
        }

        return query(keys);
    }

    size_t GetColumn() const {
        return header_.fields();
    }

    size_t GetRow() const {
        return context_.size();
    }

    std::string GetValue(int row, int column) {
        auto line = GetLine(row);
        return line[column];
    }

    size_t GetIndex(std::string &&field) const {
        auto find = header2index_.find(field);
        if (find != header2index_.end()) {
            return find->second;
        }
        return 0;
    }

private:
    bool ParseHeader(const std::string &header) {
        if (header.empty()) {
            return false;
        }

        header_ = std::move(Line(header));
        for (size_t i = 0; i < header_.fields(); i++) {
            header2index_[header_[i]] = i;
        }
        return true;
    }

    bool GenerateIndex(const Line &line, size_t index) {
        std::string key("");
        for (size_t i = 0; i < key_.size(); i++) {
            key.append(line[header2index_[key_[i]]]);
        }
        index_[key] = index;
        return true;
    }

    Line query(const std::unordered_map<std::string, std::string> &keys) {
        for (size_t i = 0; i < context_.size(); i++) {
            auto line = context_[i];
            size_t count = 0;
            for (auto it = keys.begin(); it != keys.end(); it++) {
                if (line[header2index_[it->first]] != it->second) {
                    break;
                }
                count++;
            }
            if (count == keys.size()) {
                return line;
            }
        }
        return Line();
    }

    bool reserve() {
        context_.reserve(1000);
        key_.reserve(100);
        index_.reserve(1000);
        header2index_.reserve(100);
        return true;
    }

    bool isReady_ = false;
    Line header_;
    std::vector<Line> context_;
    std::vector<std::string> key_;
    std::unordered_map<std::string, int> index_;
    std::unordered_map<std::string, int> header2index_;
};

#endif //CSVPARSER_H
