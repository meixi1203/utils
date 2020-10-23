CSV-Parser in C++
------------------
A CSV Parser in C++ without any bells and whistles.

heady only

# demo
    CSVParse csv("test.csv", {"id", "name"});
    if (!csv) {
        return 0;
    }

    std::string result("");
    auto line = csv[0];
    line.print();
    result = csv[1][2];
    line = std::move(csv.GetLine({{"id","1"},{"name","xxx"}}));
```

StringUtils
------------------
Split, Trim