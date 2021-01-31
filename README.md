# CSV-Parser in C++
A CSV Parser in C++ without any bells and whistles.
heady only

# demo
```
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

# StringUtils

#### Split, Trim


# Sql builder

A small C++11 library, for sql builder, support insert update delete select for sql

### Select

```cpp
    sql::Selector selector;
    auto str = selector.select({"id as user_id", "age", "name", "address"})
                       .distinct()
                       .from({"user"})
                       .join("score")
                       .on(sql::Column("user.id") == sql::Column("score.id") and sql::Column("score.id") > 60)
                       .where(sql::Column("score") > 60 and (sql::Column("age") >= 20 or sql::Column("address").is_not_null()))
                       .group_by({"age"})
                       .having(sql::Column("age") > 10)
                       .order_by("age", sql::OrderType::ASC)
                       .limit(10)
                       .offset(1)
                       .str();
    std::cout << str << std::endl;
```

### Delete

```cpp
    sql::Deleter deleter;
    str = deleter.from({"user"})
                 .where(sql::Column("id") == 1)
                 .str();
    std::cout << str << std::endl;
```
 
### Update

```cpp
    std::vector<int> a = {1, 2, 3};
    sql::Updater updater;
    str = updater.update("user")
                 .set("name", "ddc")
                 .set("age", 18)
                 .set("address", "beijing")
                 .where(sql::Column("id").in(a))
                 .str();
    std::cout << str << std::endl;
```

### Insert

```cpp
    sql::Inserter inserter;
    str = inserter.insert({"runoob_title", "runoob_author", "submission_date"})
                  .values("1234", "meixi", "2020-11-21")
                  .values("1235", "meixi", "2020-11-21")
                  .values("1236", "meixi", "2020-11-21")
                  .values("1237", "meixi", "2020-11-21")
                  .into("runoob_tbl")
                  .str();
    std::cout << str << std::endl;
```

###  Format

```cpp
    sql::Format format;
    str = format.format("%s %d %10.5f", "omg", 1, 10.5)
                .str();
    std::cout << str << std::endl;
```

# Memory pool

### demo
```cpp
        using namespace memory_pool;
        MemoryPool pool;
        pool.init();
        auto p = pool.find_node<int>();
        *p = 8;
        auto p1 = pool.find_node<double>();
        *p1 = 1.0;
        A* a = new(pool.find_node<A>())A;

        pool.free_node(p);
        pool.free_node(p1);
        pool.free_node(a);
```