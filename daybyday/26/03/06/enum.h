enum Color: unsigned char {
    RED = 1,
    BLACK = 255
};

// enum AnotherColr: unsigned char {
//     BLUE = 1,
//     BLACK = 2        // 这里的定义会覆盖上述的 BLACK 定义。后续使用 BLOCK 都会定向到 AnotherColr::BLACK
// };

enum class Feeling: unsigned char {
    RED = 1,
    BLACK = 255
};