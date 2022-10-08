#include <cstdint>

namespace chess {

class FenHasher {
public:
    uint64_t hashFen(char const *fen);
};

} // namespace chess