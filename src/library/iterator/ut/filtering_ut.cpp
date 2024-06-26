#include <src/library/iterator/filtering.h>

#include <src/library/testing/gtest/gtest.h>



using namespace testing;

TEST(Filtering, TFilteringRangeTest) {
    const std::vector<int> x = {1, 2, 3, 4, 5};

    EXPECT_THAT(
        MakeFilteringRange(
            x,
            [](int x) { return x % 2 == 0; }
        ),
        ElementsAre(2, 4)
    );
}

TEST(Filtering, TEmptyFilteringRangeTest) {
    std::vector<int> x = {1, 2, 3, 4, 5};
    EXPECT_THAT(
        MakeFilteringRange(
            x,
            [](int x) { return x > 100; }
        ),
        ElementsAre()
    );
}

TEST(Filtering, TMutableFilteringRangeTest) {
    std::vector<int> x = {1, 2, 3, 4, 5};
    for (auto& y : MakeFilteringRange(x, [](int x) { return x % 2 == 0; })) {
        y = 7;
    }
    EXPECT_THAT(
        x,
        ElementsAre(1, 7, 3, 7, 5)
    );
}
