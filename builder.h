//
// Created by zhoupenghui on 2020/11/7.
//

#ifndef BUILDER_H
#define BUILDER_H

#include <iostream>
#include <vector>

#include <cstdarg>

namespace sql {
template <typename T>
inline std::string to_value(const T& data) {
    return std::to_string(data);
}

template <size_t N>
inline std::string to_value(char const(&data)[N]) {
    std::string str("'");
    str.append(data);
    str.append("'");
    return str;
}

template <>
inline std::string to_value<std::string>(const std::string& data) {
    std::string str("'");
    str.append(data);
    str.append("'");
    return str;
}

template <>
inline std::string to_value<const char*>(const char* const& data) {
    std::string str("'");
    str.append(data);
    str.append("'");
    return str;
}

template <typename T>
void join_vector(std::string& result, const std::vector<T>& vec, const char* sep) {
    size_t size = vec.size();
    for(size_t i = 0; i < size; ++i) {
        if(i < size - 1) {
            result.append(vec[i]);
            result.append(sep);
        } else {
            result.append(vec[i]);
        }
    }
}

class Column
{
public:
    Column(std::string&& column) {
        _cond = std::move(column);
    }

    ~Column() = default;

    Column& as(const std::string& s) {
        _cond.append(" as ");
        _cond.append(s);
        return *this;
    }

    Column& is_null() {
        _cond.append(" is null");
        return *this;
    }

    Column& is_not_null() {
        _cond.append(" is not null");
        return *this;
    }

    template <typename T>
    Column& in(const std::vector<T>& args) {
        size_t size = args.size();
        if(size == 1) {
            _cond.append(" = ");
            _cond.append(to_value(args[0]));
        } else {
            _cond.append(" in (");
            for(size_t i = 0; i < size; ++i) {
                if(i < size - 1) {
                    _cond.append(to_value(args[i]));
                    _cond.append(", ");
                } else {
                    _cond.append(to_value(args[i]));
                }

            }
            _cond.append(")");
        }
        return *this;
    }

    template <typename T>
    Column& not_in(const std::vector<T>& args) {
        size_t size = args.size();
        if(size == 1) {
            _cond.append(" != ");
            _cond.append(to_value(args[0]));
        } else {
            _cond.append(" not in (");
            for(size_t i = 0; i < size; ++i) {
                if(i < size - 1) {
                    _cond.append(to_value(args[i]));
                    _cond.append(", ");
                } else {
                    _cond.append(to_value(args[i]));
                }
            }
            _cond.append(")");
        }
        return *this;
    }

    Column& operator &&(Column& condition) {
        std::string str("(");
        str.append(_cond);
        str.append(") and (");
        str.append(condition._cond);
        str.append(")");
        condition._cond = str;
        return condition;
    }

    Column& operator ||(Column& condition) {
        std::string str("(");
        str.append(_cond);
        str.append(") or (");
        str.append(condition._cond);
        str.append(")");
        condition._cond = str;
        return condition;
    }

    Column& operator &&(const std::string& condition) {
        _cond.append(" and ");
        _cond.append(condition);
        return *this;
    }

    Column& operator ||(const std::string& condition) {
        _cond.append(" or ");
        _cond.append(condition);
        return *this;
    }

    Column& operator &&(const char* condition) {
        _cond.append(" and ");
        _cond.append(condition);
        return *this;
    }

    Column& operator ||(const char* condition) {
        _cond.append(" or ");
        _cond.append(condition);
        return *this;
    }

    template <typename T>
    Column& operator ==(const T& data) {
        _cond.append(" = ");
        _cond.append(to_value(data));
        return *this;
    }

    template <typename T>
    Column& operator !=(const T& data) {
        _cond.append(" != ");
        _cond.append(to_value(data));
        return *this;
    }

    template <typename T>
    Column& operator >=(const T& data) {
        _cond.append(" >= ");
        _cond.append(to_value(data));
        return *this;
    }

    template <typename T>
    Column& operator <=(const T& data) {
         _cond.append(" <= ");
        _cond.append(to_value(data));
        return *this;
    }

    template <typename T>
    Column& operator >(const T& data) {
        _cond.append(" > ");
        _cond.append(to_value(data));
        return *this;
    }

    template <typename T>
    Column& operator <(const T& data) {
        _cond.append(" < ");
        _cond.append(to_value(data));
        return *this;
    }

    const std::string& str() const {
        return _cond;
    }

    operator bool() {
        return true;
    }

private:
    std::string _cond;
};

template <>
inline std::string to_value<Column>(const Column& data) {
    return data.str();
}

enum class JoinType {
    INNER, // INNER JOIN
    LEFT,  // LEFT JOIN
    RIGHT, // RIGHT JOIN
    CROSS  // CROSS JOIN (if supported)
};

enum class OrderType {
    ASC, // ORDER BY *ASC*
    DESC // ORDER BY *DESC*
};


class Model {
public:
    Model() {}

    virtual ~Model() {}

    virtual const std::string &str() = 0;

private:
    Model(const Model &m) = delete;
    Model &operator=(const Model &data) = delete;

protected:
    std::string _sql;
};

class Selector : public Model {
public:
    Selector() = default;
    ~Selector() = default;

    Selector& select(std::vector<std::string>&& fileds) {
        _select_columns = std::move(fileds);
        return *this;
    }

    Selector& distinct() {
        _distinct = true;
        return *this;
    }

    Selector& from(std::vector<std::string>&& table_name) {
        for (size_t index = 0; index < table_name.size(); index++) {
            if (_table_name.empty()) {
                _table_name = table_name[index];
            } else {
                _table_name.append(", ");
                _table_name.append(table_name[index]);
            }
        }
        return *this;
    }

    Selector& join(std::string&& table_name) {
        _join_type = "join";
         _join_table = std::move(table_name);
        return *this;
    }

    Selector& left_join(std::string&& table_name) {
        _join_type = "left join";
        _join_table = std::move(table_name);
        return *this;
    }

    Selector& left_outer_join(std::string&& table_name) {
        _join_type = "left outer join";
        _join_table = std::move(table_name);
        return *this;
    }

    Selector& right_join(std::string&& table_name) {
        _join_type = "right join";
        _join_table = std::move(table_name);
        return *this;
    }

    Selector& right_outer_join(std::string&& table_name) {
        _join_type = "right outer join";
        _join_table = std::move(table_name);
        return *this;
    }

    Selector& full_join(std::string&& table_name) {
        _join_type = "full join";
        _join_table = std::move(table_name);
        return *this;
    }

    Selector& full_outer_join(std::string&& table_name) {
        _join_type = "full outer join";
        _join_table = std::move(table_name);
        return *this;
    }

    Selector& on(std::string&& condition) {
        _join_on_condition.emplace_back(condition);
        return *this;
    }

    Selector& on(const Column& condition) {
        _join_on_condition.emplace_back(condition.str());
        return *this;
    }

    Selector& where(std::string&& condition) {
        _where_condition.emplace_back(condition);
        return *this;
    }

    Selector& where(const Column& condition) {
        _where_condition.emplace_back(condition.str());
        return *this;
    }

    Selector& group_by(std::vector<std::string>&& columns) {
        for (size_t index = 0; index < columns.size(); index++) {
            _groupby_columns.push_back(columns[index]);
        }
        return *this;
    }

    Selector& having(std::string&& condition) {
        _having_condition.emplace_back(condition);
        return *this;
    }

    Selector& having(const Column& condition) {
        _having_condition.emplace_back(condition.str());
        return *this;
    }

    Selector& order_by(std::string&& field, OrderType orderType) {
        _order_by = field;
        _order_by.append(" ");
        if (OrderType::ASC == orderType) {
            _order_by.append("asc");
        } else if (OrderType::DESC == orderType) {
            _order_by.append("desc");
        } else {
            _order_by.clear();
        }
        return *this;
    }

    template <typename T>
    Selector& limit(const T& limit) {
        _limit = std::to_string(limit);
        return *this;
    }

    template <typename T>
    Selector& limit(const T& offset, const T& limit) {
        _offset = std::to_string(offset);
        _limit = std::to_string(limit);
        return *this;
    }

    template <typename T>
    Selector& offset(const T& offset) {
        _offset = std::to_string(offset);
        return *this;
    }

    const std::string& str() override {
        _sql.clear();
        _sql.append("select ");
        if(_distinct) {
            _sql.append("distinct ");
        }
        join_vector(_sql, _select_columns, ", ");
        _sql.append(" from ");
        _sql.append(_table_name);
        if(!_join_type.empty()) {
            _sql.append(" ");
            _sql.append(_join_type);
            _sql.append(" ");
            _sql.append(_join_table);
        }
        if(!_join_on_condition.empty()) {
            _sql.append(" on ");
            join_vector(_sql, _join_on_condition, " and ");
        }
        if(!_where_condition.empty()) {
            _sql.append(" where ");
            join_vector(_sql, _where_condition, " and ");
        }
        if(!_groupby_columns.empty()) {
            _sql.append(" group by ");
            join_vector(_sql, _groupby_columns, ", ");
        }
        if(!_having_condition.empty()) {
            _sql.append(" having ");
            join_vector(_sql, _having_condition, " and ");
        }
        if(!_order_by.empty()) {
            _sql.append(" order by ");
            _sql.append(_order_by);
        }
        if(!_limit.empty()) {
            _sql.append(" limit ");
             _sql.append(_limit);
        }
        if(!_offset.empty()) {
            _sql.append(" offset ");
            _sql.append(_offset);
        }
        return _sql;
    }

    Selector& reset() {
        _select_columns.clear();
        _distinct = false;
        _groupby_columns.clear();
        _table_name.clear();
        _join_type.clear();
        _join_table.clear();
        _join_on_condition.clear();
        _where_condition.clear();
        _having_condition.clear();
        _order_by.clear();
        _limit.clear();
        _offset.clear();
        return *this;
    }

    friend inline std::ostream& operator<< (std::ostream& out, Selector& mod) {
        out<<mod.str();
        return out;
    }

protected:
    std::vector<std::string> _select_columns;
    bool _distinct = false;
    std::vector<std::string> _groupby_columns;
    std::string _table_name;
    std::string _join_type;
    std::string _join_table;
    std::vector<std::string> _join_on_condition;
    std::vector<std::string> _where_condition;
    std::vector<std::string> _having_condition;
    std::string _order_by;
    std::string _limit;
    std::string _offset;
};

class Format : public Model {
public:
    Format() = default;
    ~Format() = default;

    template<typename ... Args>
    Format& format(std::string&& fmt, Args ... args) {
        size_t size = snprintf( nullptr, 0, fmt.data(), args ... ) + 1;
        _sql.clear();
        _sql.resize(size);
        snprintf((char*)_sql.data(), size, fmt.data(), args ... );
        return *this;
    }

    virtual const std::string& str() override {
        return _sql;
    }
};

class Updater : public Model {
public:
    Updater() = default;
    ~Updater() = default;

    Updater& update(std::string&& table_name) {
        _table_name = std::move(table_name);
        return *this;
    }

    template <typename T>
    Updater& set(std::string&& c, const T& data) {
        std::string str = std::move(c);
        str.append(" = ");
        str.append(to_value(data));
        _set_columns.push_back(str);
        return *this;
    }

    Updater& where(std::string&& condition) {
        _where_condition.emplace_back(condition);
        return *this;
    }

    Updater& where(const Column& condition) {
        _where_condition.emplace_back(condition.str());
        return *this;
    }

    const std::string& str() override {
        _sql.clear();
        _sql.append("update ");
        _sql.append(_table_name);
        _sql.append(" set ");
        join_vector(_sql, _set_columns, ", ");
        size_t size = _where_condition.size();
        if(size > 0) {
            _sql.append(" where ");
             join_vector(_sql, _where_condition, " and ");
        }
        return _sql;
    }

    friend inline std::ostream& operator<< (std::ostream& out, Updater& mod) {
        out<<mod.str();
        return out;
    }

protected:
    std::vector<std::string> _set_columns;
    std::string _table_name;
    std::vector<std::string> _where_condition;
};

class Deleter : public Model {
public:
    Deleter() = default;
    ~Deleter() = default;

    Deleter& from(std::vector<std::string>&& table_names) {
        for (size_t index = 0; index < table_names.size(); index++) {
            if(_table_name.empty()) {
                _table_name = table_names[index];
            } else {
                _table_name.append(", ");
                _table_name.append(table_names[index]);
            }
        }
        return *this;
    }

    Deleter& where(std::string&& condition) {
        _where_condition.emplace_back(condition);
        return *this;
    }

    Deleter& where(const Column& condition) {
        _where_condition.emplace_back(condition.str());
        return *this;
    }

    const std::string& str() override {
        _sql.clear();
        _sql.append("delete from ");
        _sql.append(_table_name);
        size_t size = _where_condition.size();
        if(size > 0) {
            _sql.append(" where ");
            join_vector(_sql, _where_condition, " and ");
        }
        return _sql;
    }

    friend inline std::ostream& operator<< (std::ostream& out, Deleter& mod) {
        out<<mod.str();
        return out;
    }

protected:
    std::string _table_name;
    std::vector<std::string> _where_condition;
};

class Inserter : public Model {
public:
    Inserter() = default;
    virtual ~Inserter() = default;

    Inserter& insert(std::vector<std::string> &&fields) {
        for (size_t index = 0; index < fields.size(); index++) {
            _columns.emplace_back(fields[index]);
        }
        return *this;
    }

    template<typename TT>
    void values(TT end)
    {
        _values.push_back(to_value(end));
    }

    template <typename T, typename... Args>
    Inserter& values(const T &t, const Args& ... rest) {
        _values.push_back(to_value(t));
        values(rest...);
        return *this;
    }

    Inserter& replace(bool var) {
        _replace = var;
        return *this;
    }

    Inserter& into(std::string&& table_name) {
        _table_name = std::move(table_name);
        return *this;
    }

    const std::string& str() override {
        _sql.clear();
        if (_replace) {
            _sql.append("insert or replace into ");
        } else {
            _sql.append("insert into ");
        }

        _sql.append(_table_name);
        _sql.append(" (");
        std::string v_ss(" values (");
        size_t size = _columns.size();
        for(size_t i = 0; i < size; ++i) {
            if(i < size - 1) {
                _sql.append(_columns[i]);
                _sql.append(", ");
            } else {
                _sql.append(_columns[i]);
                _sql.append(")");
            }
        }

        auto count = _columns.size();
        for (size_t index = 0; index < _values.size(); index += count) {
            for (size_t i = 0; i < count; i++) {
                auto tmp = _values[index + i];
                if(i < count - 1) {
                    v_ss.append(tmp);
                    v_ss.append(", ");
                } else {
                    v_ss.append(tmp);
                    if (index != _values.size() - count) {
                        v_ss.append("), (");
                    } else {
                        v_ss.append(")");
                    }
                }
            }
        }
        _sql.append(v_ss);
        return _sql;
    }

    Inserter& reset() {
        _table_name.clear();
        _columns.clear();
        _values.clear();
        return *this;
    }

    friend inline std::ostream& operator<< (std::ostream& out, Inserter& mod) {
        out<<mod.str();
        return out;
    }

protected:
    bool _replace = false;
    std::string _table_name;
    std::vector<std::string> _columns;
    std::vector<std::string> _values;
};
}
#endif // BUILDER_H