#ifndef CHESS_POLYGLOT_H
#define CHESS_POLYGLOT_H

#include <string>
#include <vector>

#include "FenHasher.h"

typedef struct {
    uint64_t key;
    uint16_t move;
    uint8_t wins; // wins for white
    uint8_t draws;
    uint32_t n;
} entry_t;

namespace chess {

class Polyglot {
public:
    explicit Polyglot(std::string const &fileName);
    ~Polyglot() = default;

    std::string getMoves(std::string const &fen, bool asJson=false, int maxRows=0);

private:
    std::string toString(std::vector<entry_t> entries);
    std::string toJson(std::vector<entry_t> entries, int maxRows);

    std::string fileName;
    FenHasher hasher;
};

} // namespace chess

#endif