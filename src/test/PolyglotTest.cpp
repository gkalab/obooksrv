#include <memory>

#include <gtest/gtest.h>

#include "Polyglot.h"

using chess::Polyglot;

class PolyglotTest : public ::testing::Test {
public:
    void givenAPolyglotFile(std::string const &fileName) {
        polyglot.reset(new Polyglot(fileName));
    }

    void whenMovesAreRequestedForPosition(std::string const &fen) {
        result = polyglot->getMoves(fen);
    }

    void thenReturnedPositionsShouldBe(std::string const &expected) {
        EXPECT_EQ(expected, result);
    }

private:
    std::unique_ptr<Polyglot> polyglot;
    std::string result;
};

TEST_F(PolyglotTest, getMovesFromInitialPosition) {
    givenAPolyglotFile("../testdata/opening.data");
    whenMovesAreRequestedForPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    thenReturnedPositionsShouldBe("g1f3 30 49 53980\n"
                                  "c2c4 32 47 40793\n"
                                  "e2e4 32 44 218543\n"
                                  "d2d4 32 47 205407\n"
                                  "g2g3 31 45 4122\n"
                                  "b2b3 33 39 1248\n"
                                  "f2f4 28 34 849\n"
                                  "b1c3 29 36 274\n"
                                  "b2b4 38 34 171\n"
                                  "e2e3 30 30 91\n"
                                  "a2a3 39 29 51\n"
                                  "d2d3 30 32 53\n"
                                  "c2c3 31 40 22\n"
                                  "g2g4 31 18 16\n"
                                 );
}


TEST_F(PolyglotTest, rochadeInResult) {
    givenAPolyglotFile("../testdata/opening.data");
    whenMovesAreRequestedForPosition("r1bqk2r/pppp1ppp/2n2n2/1B2p3/1b2P3/2N2N2/PPPP1PPP/R1BQK2R w KQkq - 6 5");
    thenReturnedPositionsShouldBe("e1g1 29 48 823\n"
                                  "d2d3 25 42 28\n"
                                  "c3d5 38 27 18\n"
                                 );
}
