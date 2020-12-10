#include "builder.h"
#include "mysql.hpp"

void test_mysql() {
    sql::Mysql sql;
    if (!sql.connect("127.0.0.1", "zhoupenghui", "113", "zph", 3306)) {
        std::cout << sql.GetLastErrorMessage() << std::endl;
        return;
    }

    std::string str("");
    sql::Inserter inserter;
    str = inserter.insert({"runoob_title", "runoob_author", "submission_date"})
                  .values("1234", "meixi", "2020-11-21")
                  .values("1235", "meixi", "2020-11-21")
                  .values("1236", "meixi", "2020-11-21")
                  .values("1237", "meixi", "2020-11-21")
                  .into("runoob_tbl")
                  .str();
    auto res = sql.execute(str);
    std::cout << res << std::endl;

    sql::Selector selector;
    str = selector.select({"*"})
                  .from({"runoob_tbl"})
                  .str();
    std::cout << str << std::endl;
    sql::Result r = sql.query(str);
    if (!r) return;
    while (sql::Row row = r.next()) {
        std::cout << row["runoob_id"] << " "<< row["runoob_title"] << " " << row["runoob_author"] << " " << row["submission_date"] << std::endl;
    }

    sql::Updater updater;
    str = updater.update("runoob_tbl")
                 .set("runoob_title", "ddc")
                 .where(sql::Column("runoob_id") == 1)
                 .str();
    res = sql.execute(str);
    std::cout << res << std::endl;

    sql::Deleter deleter;
    str = deleter.from({"runoob_tbl"})
                 .where(sql::Column("runoob_id") == 2)
                 .str();
    std::cout << str << std::endl;
    res = sql.execute(str);
    std::cout << res << std::endl;
}

int main(void) {
    test_mysql();
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

    // format
    sql::Format format;
    str = format.format("%s %d %10.5f", "omg", 1, 10.5)
                .str();
    std::cout << str << std::endl;

    // Update
    std::vector<int> a = {1, 2, 3};
    sql::Updater updater;
    str = updater.update("user")
                 .set("name", "ddc")
                 .set("age", 18)
                 .set("address", "beijing")
                 .where(sql::Column("id").in(a))
                 .str();
    std::cout << str << std::endl;

    // Delete
    sql::Deleter deleter;
    str = deleter.from({"user"})
                 .where(sql::Column("id") == 1)
                 .str();
    std::cout << str << std::endl;

    // Insert
    sql::Inserter inserter;
    str = inserter.insert({"score", "name", "age", "address"})
                  .values(10, "six", 20, "beijing")
                  .values(10, "six", 20, "beijing")
                  .values(10, "six", 20, "beijing")
                  .values(10, "six", 20, "beijing")
                  .into("user")
                  .str();
    std::cout << str << std::endl;
    return 0;
}