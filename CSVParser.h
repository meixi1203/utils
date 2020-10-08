//
// Created by zhoupenghui on 2020/10/7.
//

#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <unordered_map>

namespace utils {
    const std::string sep = " \n\t\v\f\r";
    std::string ltrim(const std::string& str) {
        if (str.empty() || !std::isspace(str.front())) {
            return str;
        }
        auto start = str.find_first_not_of(sep);
        return str.substr(start);
    }

    std::string rtrim(const std::string& str) {
        if (str.empty() || !std::isspace(str.back())) {
            return str;
        }
        auto end = str.find_last_not_of(sep);
        return str.substr(0, end + 1);
    }

    std::string trim(const std::string& str) {
        if (str.empty() ||
            (!std::isspace(str.front()) && !std::isspace(str.back()))) {
            return str;
        }
        auto start = str.find_first_not_of(sep);
        auto end = str.find_last_not_of(sep);
        return str.substr(start, end - start + 1);
    }

    std::string::size_type sep_size(const std::string& s) {
        return s.size();
    }

    std::string::size_type sep_size(const char& c) {
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

    std::string operator[](int index) const {
        if (index < fields()) {
            return array_[index];
        }
        return std::string();
    }

    int fields() const { return array_.size(); }

    std::vector<std::string> GetArray() const { return array_; }

    std::unordered_map<std::string, std::string> GetKeyValue(std::unordered_map<int, std::string> &&header) {
        std::unordered_map<std::string, std::string> result;
        if (header.size() != array_.size()) {
            return result;
        }

        for (size_t index = 0; index < array_.size(); index++) {
            result[header[index]] = array_[index];
        }
        return result;
    }

    void print(const std::unordered_map<std::string, std::string> &data) {
        std::stringstream ss;
        for (auto iter = data.begin(); iter != data.end(); iter++) {
            ss << iter->first << ":" << iter->second << " ";
        }
        std::cout << ss.str() << std::endl;
    }

    std::vector<std::string> array_;
};

class CSVParse
{
public:
    CSVParse(const std::string &file, const std::string &key) {
        if (!parse(file, key)) {
            std::cout << "parse error!" << std::endl;
        }
    }

    ~CSVParse() = default;

    bool parse(const std::string &file, const std::string &key) {
        if (file.empty() || key.empty()) {
            std::cout << "parse args is error!" << std::endl;
            return false;
        }

        std::ifstream io(file.data());
        if (!io) {
            std::cout << "open cvs is error!, file = " << file << std::endl;
            return false;
        }

        std::string s("");
        getline(io, s);
        ParseHeader(s);

        size_t count = 0;
        while (getline(io, s)) {
            Line line(s);
            auto array = std::move(line.GetArray());
            auto index = header2index_[key];
            context_[array[index]] = line;
            index_[count++] = array[index];
        }
        io.close();
        return true;
    }

    Line GetLine(int row) {
        if (row < GetRow()) {
            auto find = index_.find(row);
            if (find != index_.end()) {
                return context_[find->second];
            }
        }
        return Line(std::string());
    }

    Line operator[](int row) {
        return GetLine(row);
    }

    Line GetLine(const std::string &key) {
        auto find = context_.find(key);
        if (find != context_.end()) {
            return find->second;
        }

        return Line();
    }

    Line GetLine(const std::unordered_map<std::string, std::string> &keys) {
        for (auto iter = context_.begin(); iter != context_.end(); iter++) {
            auto line = std::move(iter->second);
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

    int GetColumn() const {
        return header2index_.size();
    }

    int GetRow() const {
        return index_.size();
    }

    std::string GetValue(int row, int column) {
        auto line = GetLine(row);
        return line[column];
    }

    std::string GetValue(const std::string &key, const std::string &field) {
        auto find = context_.find(key);
        if (find != context_.end()) {
            auto line = find->second;
            return find->second[header2index_[field]];
        }
        return std::string();
    }

    std::unordered_map<int, std::string> GetHeader() const {
        return index2header_;
    }

private:
    bool ParseHeader(const std::string &header) {
        if (header.empty()) {
            return false;
        }

        auto array = std::move(utils::split(header));
        for (size_t index = 0; index < array.size(); index++) {
            header2index_[array[index]] = index;
            index2header_[index] = array[index];
        }
        return true;
    }

    std::unordered_map<std::string, int> header2index_;
    std::unordered_map<int, std::string> index2header_;

    std::unordered_map<std::string, Line> context_;
    std::unordered_map<int, std::string> index_;
};

#endif //CSVPARSER_H
