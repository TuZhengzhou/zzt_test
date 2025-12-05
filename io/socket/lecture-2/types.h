#ifndef TYPES_H
#define TYPES_H
#include <cstdlib>
#include <memory>

using SharedFD = std::shared_ptr<int>;
using SharedFILE = std::shared_ptr<FILE>;

struct Header {
    int uuid;
    int data_size;
};

struct FileHead {
    size_t batch_num;
    size_t file_len;
    char file_name[256];
};

struct File {
    FileHead head;
    int head_size;
    int head_current;

    char* content;
    int content_size;
    int content_current;
};

enum class Type {
    HEAD,
    CONTENT
};

enum class Status {
    RECEIVEING_HEAD,
    RECIEVING_DATA,
    DONE
};

#endif