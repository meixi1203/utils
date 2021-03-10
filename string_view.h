#include <cassert>
#include <functional>
#include <cstring>
#include <ostream>

class StringView {
public:
    using value_type = char;
    using pointer = char*;
    using const_pointer = const char*;
    using reference = char&;
    using const_reference = const char&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using const_iterator = const char* ;

    StringView() : StringView(nullptr, 0) {

    }

    explicit
    StringView(const char* str) : StringView(str, strlen(str)) {

    }

    explicit // be care of string's lifetime
    StringView(const std::string& str) : StringView(str.data(), str.size()) {

    }

    explicit
    StringView(std::string&& str) : StringView(str.data(), str.size()) {
        
    }

    StringView(const char* p, size_t len) : data_(p), len_(len) {

    }

    StringView(const StringView& ) = default;

    const_reference operator[](size_t index) const {
        assert (index >= 0 && index < len_);
        return data_[index];
    }

    const_pointer data() const {
        return data_;
    }

    const_reference front() const {
        assert(len_ > 0);
        return data_[0];
    }

    const_reference back() const {
        assert (len_ > 0);
        return data_[len_-1];
    }

    const_iterator begin() const {
        return data_;
    }

    const_iterator end() const {
        return data_ + len_;
    }

    size_t size() const {
        return len_;
    }

    bool empty() const {
        return len_ == 0;
    }

    void remove_prefix(size_t n) {
        data_ += n;
        len_ -= n;
    }

    void remove_suffix(size_t n) {
        len_ -= n;
    }

    size_t find(StringView v, size_t pos = 0) const {
        if (pos > size()) {
            return npos;
        }

        if ((pos + v.size()) > size()) {
            return npos;
        }

        const auto offset = pos;
        const auto increments = size() - v.size();

        for (auto i = 0u; i <= increments; ++i) {
            const auto j = i + offset;
            if (compare(substr(i, v.size()), v)) {
                return j;
            }
        }
        return npos;
    }

    size_t find(const char* s, size_t pos = 0) const {
        return find(StringView(s), pos);
    }

    size_t rfind(StringView v, size_t pos = npos) const {
        if (empty()) {
            return v.empty() ? 0u : npos;
        }

        if (v.empty()) {
            return std::min(size() - 1, pos);
        }

        if (v.size() > size()) {
            return npos;
        }

        auto i = std::min(pos, (size() - v.size()));
        while (i != npos) {
            if (compare(substr(i, v.size()), v)) {
                return i;
            }
            --i;
        }
        return npos;
    }

    size_t rfind(const char* s, size_t pos = npos) const {
        return rfind(StringView(s), pos);
    }

    void swap(StringView& other) {
        if (this != &other) {
            std::swap(this->data_, other.data_);
            std::swap(this->len_, other.len_);
        }
    }

    StringView substr(size_t pos = 0, size_t count = npos ) const {
        return StringView(data_ + pos, std::min(count, size()));
    }

    std::string ToString() const {
        return std::string(data_, len_);
    }

    bool compare(const StringView& a, const StringView& b) const {
        return a.size() == b.size() &&
               memcmp(a.data(), b.data(), a.size()) == 0;
    }

    bool compare(const StringView& b) const {
        return this->size() == b.size() &&
               memcmp(this->data(), b.data(), this->size()) == 0;
    }

    bool compare(const char* b) const {
        return compare(StringView(b));
    }

    bool compare(size_t pos, size_type count, StringView v) const {
        return substr(pos, count).compare(v);
    }

    static constexpr size_type npos = size_type(-1);
private:
    const char* data_;
    size_t len_;
};

bool operator==(const StringView& a, const StringView& b) {
    return a.compare(b);
}

bool operator==(const StringView& a, const char* b) {
    return a.compare(b);
}

bool operator==(const char*& a, const StringView& b) {
    return b == a;
}

bool operator!=(const StringView& a, const StringView& b) {
    return !(a == b);
}

bool operator<(const StringView& a, const StringView& b) {
    if (a.size() < b.size()) {
        return a.size() == 0 || memcmp(a.data(), b.data(), a.size()) <= 0;
    } 
    return memcmp(a.data(), b.data(), b.size()) < 0;
}

bool operator>(const StringView& a, const StringView& b) {
    return !(a<b || a==b);
}

bool operator<=(const StringView& a, const StringView& b) {
    return !(a > b);
}

bool operator>=(const StringView& a, const StringView& b) {
    return !(a < b);
}

inline std::ostream& operator<< (std::ostream& os, const StringView& sv) {
    return os << sv.data();
}
namespace std {
template<>
struct hash<StringView> {
    typedef StringView argument_type;
    typedef std::size_t result_type;
    result_type operator()(const argument_type& sv) const noexcept {
        std::size_t result = 0;
        for (auto ch : sv) {
            result = (result * 131) + ch;
        }
        return result;
    }
};
}