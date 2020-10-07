#include "CSVParser.h"

int main(int argc,char** argv)
{
    CSVParse csv("test.csv", 0);
    std::string result("");
    result = csv.GetValue("1", "age");
    result = csv[1][2];
    auto line = std::move(csv.GetLine(1));
    line = std::move(csv.GetLine("1"));
    line = std::move(csv.GetLine({{"id","1"},{"age","20"}}));
    return 0;
}
