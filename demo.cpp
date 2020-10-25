#include "CSVParser.h"

static bool print(const std::unordered_map<std::string, std::string> &values) {
    for (auto iter = values.begin(); iter != values.end(); iter++) {
        std::cout << iter->first << ":" << iter->second << ",";
    }
    std::cout << std::endl;
    return true;
}

int main(int argc,char** argv)
{
    CSVParse csv("test.csv", {"id", "name"});
    if (!csv) {
        return 0;
    }

    std::unordered_map<std::string, std::string> values;
    for (size_t index = 0; index < csv.GetRow(); index++) {
        if (csv.GetLine(index, values)) {
            print(values);
        }
    }

    auto line = std::move(csv[0]);
    std::string result = line[csv.GetIndex("id")];
    line.print();
    result = csv[1][2];
    line = std::move(csv.GetLine({{"id","1"},{"name","xxx"}}));
    return 0;
}
