#include <doctest/doctest.h>
#include <THST/QuadTree.h>

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

template <typename T> struct Point {
	union {
		T data[2];
		struct {
			T x, y;
		};
	};

	void set(T x, T y) {
		this->x = x;
		this->y = y;
	}

	inline float distance(const Point point) const {

		float d = data[0] - point.data[0];
		d *= d;
		for (int i = 1; i < 2; i++)
		{
			float temp = data[i] - point.data[i];
			d += temp * temp;
		}
		return std::sqrt(d);
	}
};

const Box2<int> kBoxes[] = {
	{{5, 2}, {16, 7}},  {{1, 1}, {2, 2}},  {{26, 24}, {44, 28}}, {{22, 21}, {23, 24}},
	{{16, 0}, {32, 16}},   {{0, 0}, {8, 8}},      {{4, 4}, {6, 8}}, {{2, 1}, {2, 3}},
	{{4, 2}, {8, 4}},      {{3, 3}, {12, 16}},    {{0, 0}, {64, 32}}, {{3, 2}, {32, 35}},
	{{32, 32}, {64, 128}}, {{128, 0}, {256, 64}}, {{120, 64}, {250, 128}}, {{123, 84}, {230, 122}} };

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
			CHECK(qtree.count() <= 1);
			qtree.insert(box);
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
	int min[]{ -2, -1 };
	int max[]{ 9, 10 };
	spatial::BoundingBox<int, 2> bbox{ min, max };
	spatial::QuadTree<int, Box2<int>> qtree{ bbox.min, bbox.max };

	SUBCASE("with empty tree") {
		CHECK(qtree.query(spatial::intersects(bbox)) == false);
	}

	Box2<int> box{ {0, 1}, {5, 6} };
	qtree.insert(box);

	SUBCASE("with lower left corner touching") {
		int corner_min[]{ -1, 0 };
		int corner_max[]{ 0, 1 };
		spatial::BoundingBox<int, 2> qbox{ corner_min, corner_max };
		CHECK(qtree.query(spatial::intersects(qbox)) == true);
	}

	SUBCASE("with upper right corner touching") {
		int corner_min[]{ 5, 6 };
		int corner_max[]{ 6, 7 };
		spatial::BoundingBox<int, 2> qbox{ corner_min, corner_max };
		CHECK(qtree.query(spatial::intersects(qbox)) == true);
	}

	SUBCASE("with non-intersecting box") {
		std::array<Box2<int>, 4> queries{
			Box2<int>{ { 0, -1}, { 1, 0}},
			Box2<int>{ {-2,  1}, {-1, 2}},
			Box2<int>{ { 5,  7}, { 6, 8}},
			Box2<int>{ { 6,  5}, { 7, 8}} };

		for (const auto query : queries) {
			CAPTURE(query);
			spatial::BoundingBox<int, 2> qbox{ query };
			CHECK(qtree.query(spatial::intersects(qbox)) == false);
		}
	}
}

TEST_CASE("test insert") {
	// create a quad tree with the given box
	spatial::BoundingBox<int, 2> bbox((spatial::box::empty_init()));
	Point<int> point;
	point.set(0, 0);
	bbox.extend(point.data);
	point.set(256, 128);
	bbox.extend(point.data);

	typedef spatial::QuadTree<int, Box2<int>, 2> qtree_box_t;
	qtree_box_t qtree(bbox.min, bbox.max);

	CHECK(qtree.count() == 0u);
	qtree = qtree_box_t(bbox.min, bbox.max, std::begin(kBoxes), std::end(kBoxes));
	CHECK(qtree.count() == 16u);
	qtree.clear();
	CHECK(qtree.count() == 0u);

	// or construction via insert
	qtree.insert(std::begin(kBoxes), std::end(kBoxes));
	CHECK(qtree.count() == 16u);
	Box2<int> box = { {7, 3}, {14, 6} };
	qtree.insert(box);
	CHECK(qtree.count() == 17u);

	SUBCASE("count and remove")
	{
		// remove an element
		bool removed = qtree.remove(box);
		CHECK(removed);

		removed = qtree.remove(box);
		CHECK(!removed);
		std::vector<Box2<int>> results;
		qtree.query(spatial::contains<2>(box.min, box.max), std::back_inserter(results));
		CHECK(results.empty());

		box = { {0, 0}, {20, 50} };
		qtree.query(spatial::contains<2>(box.min, box.max), std::back_inserter(results));
		CHECK(!results.empty());
		CHECK(results.size() == 7);
	}
}
