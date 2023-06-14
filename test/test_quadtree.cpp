#include <doctest/doctest.h>
#include <QuadTree.h>

#include <array>
#include <iterator>
#include <ostream>
#include <sstream>

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

template<typename T>
std::ostream& operator<<(std::ostream& os, const Box2<T> box) {
	os << "{{" << box.min[0] << ", " << box.min[1] << "}, {"
	   << box.max[0] << ", " << box.max[1] << "}}";
	return os;
}


TEST_CASE("test count") {
	int min[]{0, 0};
	int max[]{1, 1};
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

TEST_CASE("test query") {
	int min[]{-2, -1};
	int max[]{9, 10};
	spatial::BoundingBox<int, 2> bbox{min, max};
	spatial::QuadTree<int, Box2<int>> qtree{bbox.min, bbox.max};

	SUBCASE("with empty tree") {
		CHECK(qtree.query(spatial::intersects(bbox)) == false);
	}

	Box2<int> box{{0, 1}, {5, 6}};
	qtree.insert(box);

	SUBCASE("with lower left corner touching") {
		int corner_min[]{-1, 0};
		int corner_max[]{0, 1};
		spatial::BoundingBox<int, 2> qbox{corner_min, corner_max};
		CHECK(qtree.query(spatial::intersects(qbox)) == true);
	}

	SUBCASE("with upper right corner touching") {
		int corner_min[]{5, 6};
		int corner_max[]{6, 7};
		spatial::BoundingBox<int, 2> qbox{corner_min, corner_max};
		CHECK(qtree.query(spatial::intersects(qbox)) == true);
	}

	SUBCASE("with non-intersecting box") {
		std::array<Box2<int>, 4> queries{
			Box2<int>{{ 0, -1}, { 1, 0}},
			Box2<int>{{-2,  1}, {-1, 2}},
			Box2<int>{{ 5,  7}, { 6, 8}},
			Box2<int>{{ 6,  5}, { 7, 8}}};

		for (const auto query : queries) {
			CAPTURE(query);
			spatial::BoundingBox<int, 2> qbox{query};
			CHECK(qtree.query(spatial::intersects(qbox)) == false);
		}	
	}
}
