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
    std::vector<std::string> split(const std::string& str, char c = ',') {
        std::string line("");
        std::stringstream ss(str);
        std::vector<std::string> result;
        while (std::getline(ss, line, c)) {
            result.push_back(line);
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

    std::vector<std::string> getArray() const { return array_; }

    std::vector<std::string> array_;
};

class CSVParse
{
public:
    CSVParse(const std::string &file, int key = 0) {
        if (!parse(file, key)) {
            std::cout << "parse error!" << std::endl;
        }
    }

    ~CSVParse() {}

    bool parse(const std::string &file, int key = 0) {
        if (file.empty() || key < 0) {
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
        auto array = std::move(utils::split(s));
        for (size_t index = 0; index < array.size(); index++) {
            header_[array[index]] = index;
        }

        int index = 0;
        while (getline(io, s)) {
            Line line(s);
            auto array = std::move(line.getArray());
            context_[array[key]] = line;
            index_[index++] = array[key];
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
                if (line[header_[it->first]] != it->second) {
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
        return header_.size();
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
            return find->second[header_[field]];
        }
        return std::string();
    }

private:
    std::unordered_map<std::string, int> header_;
    std::unordered_map<std::string, Line> context_;
    std::unordered_map<int, std::string> index_;
};

#endif //CSVPARSER_H
