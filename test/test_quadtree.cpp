#include <doctest/doctest.h>
#include <QuadTree.h>

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

TEST_CASE("quad tree count - value insert") {
	int min[2]{0, 0};
	int max[2]{1, 1};
	spatial::BoundingBox<int, 2> bbox{min, max};
	spatial::QuadTree<int, Box2<int>> qtree{bbox.min, bbox.max};

	for (int i{0}; i < 3; ++i) {
		CAPTURE(i);
		CHECK(qtree.count() == i);
		qtree.insert(Box2<int>{{0, 0}, {1, 1}});
	}
}
