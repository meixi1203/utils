#include "CSVParser.h"

int main(int argc,char** argv)
{
    CSVParse csv("test.csv", "id");
    std::string result("");
    result = csv.GetValue("1", "age");
    result = csv[1][2];
    auto line = std::move(csv.GetLine(1));
    line = std::move(csv.GetLine("1"));
    line = std::move(csv.GetLine({{"id","1"},{"age","20"}}));
    auto keyValue = line.GetKeyValue(csv.GetHeader());
    line.print(keyValue);
    return 0;
}
