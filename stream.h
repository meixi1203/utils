//
// Created by zhoupenghui on 2021/2/1.
//

#ifndef STREAM_H
#define STREAM_H

#include <iostream>
#include <cstring>

template <typename T>
T SwapEndian(T u)
{
    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

struct Buffer {
    Buffer(size_t size, char *src) {
        data = new char[size + 1];
        memcpy(data, src, size);
        data[size] = '\0';
        limit = data + size,
        pos = data;
    }

    ~Buffer() {
        delete []data;
    }

    inline size_t GetRemaining() {
        return limit - pos;
    }

    inline bool GetByte(uint8_t &b) {
        if (GetRemaining() == 0)
            return false;
        b = static_cast<uint8_t>(*(pos++));
        return true;
    }

    inline size_t GetBytes(uint8_t *buf, size_t size) {
        size_t len = GetRemaining() >= size ? size : GetRemaining();
        memcpy(buf, pos, len);
        pos += len;
        return len;
    }

    char *data;
    char *pos;
    char *limit;
    Buffer *next = nullptr;
};

struct Stream {
    size_t GetRemaining() {
        Buffer *p = head;
        size_t remaining = 0;
        while (p) {
            remaining += p->GetRemaining();
            p = p->next;
        }
        return remaining;
    }

    void Add(size_t size, char *data) {
        Buffer *p = new Buffer(size, data);
        if (!head)
            head = p;
        if (tail)
            tail->next = p;
        tail = p;
    }

    bool GetByte(uint8_t &b) {
        bool res = head->GetByte(b);
        CleanBuffers();
        return res;
    }

    size_t GetBytes(uint8_t *buf, size_t size) {
        Buffer *p = head;
        size_t read = 0;
        while (p && read < size) {
            read += p->GetBytes(buf + read , size - read);
            p = p->next;
        }
        CleanBuffers();
        return read;
    }

    void CleanBuffers() {
        if (head && head->GetRemaining() == 0) {
            Buffer *p = head;
            head = head->next;
            if (tail == p)
                tail = nullptr;
            delete p;
        }
    }

    Buffer *head = nullptr;
    Buffer *tail = nullptr;
};

class Handler {
public:
    Handler() {}
    void ParseBuffers() {
        while (true) {
            if (nextCommandSize == 0 && stream.GetRemaining() > sizeof(nextCommandSize)) {
                stream.GetBytes(reinterpret_cast<uint8_t *>(&nextCommandSize), sizeof(nextCommandSize));
            }
            if (nextCommandSize && stream.GetRemaining() >= nextCommandSize) {
                uint8_t command[nextCommandSize + 1];
                stream.GetBytes(command, nextCommandSize);
                command[nextCommandSize] = '\0';
                printf("command = %s\n", std::string((char *)command).data());
                nextCommandSize = 0;
            } else {
                break;
            }
        }
    }

public:
    Stream stream;
    uint8_t nextCommandSize = 0;
};

#endif // STREAM_H