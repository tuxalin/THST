#include <doctest/doctest.h>
#include <QuadTree.h>

#include <array>
#include <iterator>

template <typename T>
struct Box2 {
	T min[2];
	T max[2];

	explicit operator spatial::BoundingBox<int, 2>() const {
		return spatial::BoundingBox<int, 2>(min, max);
	}

	bool operator==(const Box2& other) const
	{
		return (min[0] == other.min[0]
		     && min[1] == other.min[1]
		     && max[0] == other.max[0]
		     && max[1] == other.max[1]);
	}
};

TEST_CASE("test count") {
	int min[2]{0, 0};
	int max[2]{1, 1};
	spatial::BoundingBox<int, 2> bbox{min, max};
	spatial::QuadTree<int, Box2<int>> qtree{bbox.min, bbox.max};
	Box2<int> box{{0, 0}, {1, 1}};

	SUBCASE("after value insert") {
		for (int inserts{0}; inserts < 3; ++inserts) {
			CAPTURE(inserts);
			CHECK(qtree.count() == inserts);
			qtree.insert(Box2<int>{box});
		}
	}

	SUBCASE("after iterator insert") {
		std::array<Box2<int>, 2> boxes{box, box};

		int expected_count{0};
		for (int inserts{0}; inserts < 2; ++inserts) {
			CAPTURE(inserts);
			qtree.insert(
				std::begin(boxes),
				std::begin(boxes) + inserts);
			expected_count += inserts;
			CHECK(qtree.count() == expected_count);
		}
	}
}
