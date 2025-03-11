#include <iostream>
#include <endian.h>
#include <arpa/inet.h>

using namespace std;

typedef union endianness_test {
    short a;
    char b;
} ENDIAN;

int main () {
    ENDIAN e;
    e.a = 0x0102;

    // By Union
    cout << "TEST ENDIANNESS BY UNION" << endl;
    cout << "e.b = " << (int) e.b << endl;
    if (e.b == 1) {
        cout << "Big Endian" << endl;
    } else {
        cout << "Small Endian" << endl;
    }

    // By pointer
    cout << "TEST ENDIANNESS BY POINTER" << endl;
    char* c_p = (char*) &e.a;
    cout << "*c_p = " << (int) *c_p << endl;
    if ((int) *c_p == 1) {
        cout << "Big Endian" << endl;
    } else {
        cout << "Small Endian" << endl;
    }

    // By library function
    cout << "TEST ENDIANNESS BY LIBRARY FUNCTION" << endl;
    if (__BYTE_ORDER == __BIG_ENDIAN) {
        cout << "Big Endian" << endl;
    } else {
        cout << "Small Endian" << endl;
    }

    // By compare with net
    cout << "TEST ENDIANNESS BY NET" << endl;
    uint32_t host = 0x01020304;
    uint32_t net  = htonl(host); // host to net long(unsigned long)
    if (host == net) {
        cout << "Big Endian" << endl;
    } else {
        cout << "Small Endian" << endl;
    }

    // include <arpa/inet.h>
    // htonl(uint32_t) host to net long(unsigned long)
    // htons(uint16_t) host to net short(unsigned short)
    // ntohl(uint32_t) net to host long(unsigned long)
    // ntohs(uint16_t) net to host short(unsigned short)

    return 0;
}