CSV-Parser in C++
------------------
A CSV Parser in C++ without any bells and whistles.

heady only

# demo
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
```

StringUtils
------------------
Split, Trim