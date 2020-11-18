#include "CSVParser.h"

int main(int argc,char** argv)
{
    CSVParse csv("test.csv", {"id", "name"});
    if (!csv) {
        return 0;
    }

    auto line = std::move(csv[0]);
    auto str = line.str();
    std::cout << str << std::endl;
    std::string result = line["id"];
    result = line[1];
    result = csv[1][2];
    line = std::move(csv.GetLine({{"id","1"},{"name","xxx"}}));
    return 0;
}
