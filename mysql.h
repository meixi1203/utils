#ifndef MYSQL_H
#define MYSQL_H

#include <cstring>

#include <iostream>
#include <unordered_map>

#include <mysql/mysql.h>

namespace sql {
class Row {
public:
    Row() = default;
    Row(MYSQL_ROW row, const std::unordered_map<std::string, size_t>& field2index)
        : _row(row), _field2index(field2index) {}

    Row(Row&& x) : _row(x._row), _field2index(x._field2index) {
        x._row = nullptr;
    }

    Row& operator=(Row&& x) {
        using std::swap;
        swap(*this, x);
        return *this;
    }

    operator bool() {
        return !!_row;
    }

    std::string operator[](size_t n) {
        if (n > _field2index.size()) {
            return std::string();
        }
        return std::string(_row[n]);
    }

    std::string operator[](std::string &&field_name) {
        return GetValue(field_name);
    }

    std::string operator[](const char* field_name) {
        return GetValue(field_name);
    }

private:
    std::string GetValue(const std::string& field_name) {
        auto find = _field2index.find(field_name);
        if (find == _field2index.end()) {
            return std::string();
        }
        return std::string(_row[find->second]);
    }
private:
    MYSQL_ROW _row = nullptr;
    const std::unordered_map<std::string, size_t> _field2index;
};

class Result {
public:
    Result() : _res(nullptr){}
    Result(MYSQL_RES* res) : _res(res) {
        ParseFieldName();
    }

    Result(Result&& r) {
        if (_res) mysql_free_result(_res);
        this->_res = r._res;
        this->_fields2index = r._fields2index;

        r._res = nullptr;
    }

    ~Result() {
        if (_res) mysql_free_result(_res);
    }

    operator bool() {
        return !!_res;
    }

    Result& operator=(Result&& r) {
        if (_res) mysql_free_result(_res);
        this->_res = r._res;
        this->_fields2index = r._fields2index;

        r._res = nullptr;
        return *this;
    }

    Result& operator=(const Result&) = delete;
    Result(const Result&) = delete;

    Row fetch_row() {
        MYSQL_ROW row = mysql_fetch_row(_res);
        return Row{row, _fields2index};
    }

    inline Row next() {
        return fetch_row();
    }

    int GetRowCount() const {
        return mysql_num_rows(_res);;
    }

    bool ParseFieldName() {
        if (!_res) {
            return false;
        }

        MYSQL_FIELD *fields = mysql_fetch_fields(_res);
        if (!fields) {
            return false;
        }
        auto num_fields = mysql_num_fields(_res);
        for (size_t index = 0; index < num_fields; index++) {
            _fields2index[std::string(fields[index].name)] = index;
        }
        return true;
    }

private:
    MYSQL_RES* _res;
    std::unordered_map<std::string, size_t> _fields2index;
};

struct ConnectInfo {
    std::string host = std::string();
    std::string user = std::string();
    std::string password = std::string();
    std::string db = std::string();
    unsigned int port = 3306;

    void set(const char* host, const char* user, const char* password, const char* db, int port) {
        this->host = std::string(host);
        this->user = std::string(user);
        this->password = std::string(password);
        this->db = std::string(db);
        this->port = port;
    }
};

class Mysql {
public:
    Mysql(bool auto_commit = true) : _handle(mysql_init(0)) {
        mysql_autocommit(_handle, auto_commit);
    }

    ~Mysql() {
        if (_handle) {
            mysql_close(_handle);
        }
    }

    bool connect(const char* host, const char* user, const char* password,
                 const char* db, unsigned int port) {
        _connect_info.set(host, user, password, db, port);
        if (!_handle) return false;
        MYSQL* h = mysql_real_connect(_handle, host, user, password,
                                      db, port, 0, 0);
        if (h) return true;
        return false;
    }

    Result query(const std::string& s) {
        if (!ping()) {
            reconnect();
        }

        int x = mysql_real_query(_handle, s.data(), s.size());
        if (x != 0) {
            return Result{};
        }
        return use_result();
    }

    bool execute(const std::string& s) {
        if (!_handle) return false;
        int x = mysql_real_query(_handle, s.c_str(), s.size());
        if (x != 0) {
            return false;
        }
        return true;
    }

    std::string GetLastErrorMessage() {
        if (!_handle) return std::string();
        char error_msg[256];
        memset(error_msg, 0x00, sizeof(error_msg));

        sprintf(error_msg, "Error Message: error_no:%d, error_message:%s",
                mysql_errno(_handle),
                mysql_error(_handle));
        return error_msg;
    }

private:
    bool ping() {
        if (!_handle) {
            return false;
        }

        if (0 != mysql_ping(_handle)) {
            return false;
        }
        return true;
    }

    bool reconnect() {
        if (_handle) {
            mysql_close(_handle);
        }

        _handle = mysql_init(0);
        return connect(_connect_info.host.data(),
                       _connect_info.user.data(),
                       _connect_info.password.data(),
                       _connect_info.db.data(),
                       _connect_info.port);
    }

    inline Result use_result() {
        MYSQL_RES* result = mysql_use_result(_handle);
        if (!result) {
            Result{};
        }
        return Result{result};
    }

private:
    MYSQL* _handle;
    ConnectInfo _connect_info;
};
}

#endif // MYSQL_H
