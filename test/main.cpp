
#include <QuadTree.h>
#include <RTree.h>
#include <iostream>
#include <numeric>
#include <sstream>

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
};

template <typename T> struct Box2 {
  T min[2];
  T max[2];

  explicit operator spatial::BoundingBox<int, 2>() const {
    return spatial::BoundingBox<int, 2>(min, max);
  }
};

template <typename T>
std::ostream &operator<<(std::ostream &stream, const Box2<T> &bbox) {
  stream << "min: " << bbox.min[0] << " " << bbox.min[1] << " ";
  stream << "max: " << bbox.max[0] << " " << bbox.max[1];
  return stream;
}

struct Object {
  spatial::BoundingBox<int, 2> bbox;
  std::string name;
};

// example of a cache friendly allocator with contiguous memory
template <class NodeClass> struct tree_allocator {
  static const size_t kMaxNodeCount = 100;

  typedef NodeClass value_type;
  typedef NodeClass *ptr_type;

  enum { is_overflowable = 0 };

  tree_allocator() : buffer(kMaxNodeCount), count(0), index(0) {}

  ptr_type allocate(int level) {
    std::cout << "Allocate node: " << level << " current count: " << count
              << "\n";

    assert(count + 1 < kMaxNodeCount);
    ++count;
    buffer[index] = NodeClass(level);
    return &buffer[index++];
  }

  void deallocate(const ptr_type node) {
    std::cout << "Deallocate node: " << node->level << " - " << node->count
              << " current count: " << count << "\n";

    assert(count > 0);
    if (count > 1)
      --count;
    else {
      count = 0;
      index = 0;
    }
  }

  void clear() { buffer.clear(); }

  bool overflowed() const { return false; }

  std::vector<NodeClass> buffer;
  size_t count;
  size_t index;
};

const Box2<int> kBoxes[] = {
    {{16, 0}, {32, 16}},   {{0, 0}, {8, 8}},      {{4, 4}, {6, 8}},
    {{4, 2}, {8, 4}},      {{3, 3}, {12, 16}},    {{0, 0}, {64, 32}},
    {{32, 32}, {64, 128}}, {{128, 0}, {256, 64}}, {{120, 64}, {250, 128}}};

int main() {
  std::vector<Object> objects;
  std::vector<Box2<int>> boxes;

  size_t i = 0;
  std::cout << "Creating objects:\n";
  for (const auto &bbox : kBoxes) {
    boxes.push_back(bbox);
    objects.emplace_back();
    Object &obj = objects.back();

    std::stringstream ss;
    ss << "object" << i++;
    obj.name = ss.str();
    obj.bbox = (spatial::BoundingBox<int, 2>)bbox;

    std::cout << obj.name << " " << obj.bbox << "\n";
  }
  std::cout << std::endl;

  // create a quad tree with the given box
  spatial::BoundingBox<int, 2> bbox;
  Point<int> point;
  point.set(0, 0);
  bbox.extend(point.data);
  point.set(256, 128);
  bbox.extend(point.data);

  spatial::QuadTree<int, Box2<int>, 2> qtree(bbox.min, bbox.max);
  spatial::RTree<int, Box2<int>, 2> rtree;

  // insert elements into the trees
  {
    qtree.insert(kBoxes, kBoxes + sizeof(kBoxes) / sizeof(kBoxes[0]));
    rtree.insert(kBoxes, kBoxes + sizeof(kBoxes) / sizeof(kBoxes[0]));
    Box2<int> box = {{7, 3}, {14, 6}};
    qtree.insert(box);
    rtree.insert(box);

    std::cout << "Created trees, element count: " << qtree.count() << "\n";
    std::cout << std::endl;
  }

  // query for results within the search box
  {
    Box2<int> searchBox = {{0, 0}, {8, 31}};
    std::vector<Box2<int>> results;
    rtree.overlaps(searchBox.min, searchBox.max, results);
    std::cout << "Overlaps results: " << std::endl;
    for (const auto &res : results)
      std::cout << res << "\n";

    results.clear();
    rtree.contains(searchBox.min, searchBox.max, results);
    std::cout << "Contains results: " << std::endl;
    for (const auto &res : results)
      std::cout << res << "\n";
    std::cout << std::endl;
  }

  // use a custom indexable for custom objects
  struct Indexable {
    const int *min(const Object &value) const { return value.bbox.min; }
    const int *max(const Object &value) const { return value.bbox.max; }
  };
  {

    spatial::QuadTree<int, Object, 2, Indexable> qtree(bbox.min, bbox.max);
    qtree.insert(objects.begin(), objects.end());

    spatial::RTree<int, Object, 2, 4, 2, Indexable> rtree;
    rtree.insert(objects.begin(), objects.end());
  }

  // leaf traversal of the tree
  {
    spatial::RTree<int, Object, 2, 4, 2, Indexable> rtree;
    rtree.insert(objects.begin(), objects.end());

    std::cout << "Initial order:\n";
    for (const auto &obj : objects) {
      std::cout << obj.name << "\n";
    }
    std::cout << "Leaf tree traversal order:\n";
    // gives the spatial partioning order within the tree
    for (auto it = rtree.lbegin(); it.valid(); it.next()) {
      std::cout << (*it).name << "\n";
    }
    std::cout << std::endl;
  }

  // depth traversal of the tree
  {
    spatial::RTree<int, Object, 2, 4, 2, Indexable> rtree;
    rtree.insert(objects.begin(), objects.end());

    std::cout << "Depth tree traversal: "
              << "\n";
    std::cout << std::endl;

    assert(rtree.levels() > 0);
    for (auto it = rtree.dbegin(); it.valid(); it.next()) {
      std::string parentName;
      spatial::BoundingBox<int, 2> parentBBox;

      // traverse current children of the parent node(i.e. upper level)
      for (auto nodeIt = it.child(); nodeIt.valid(); nodeIt.next()) {
        std::cout << "level: " << nodeIt.level() << " " << (*nodeIt).name
                  << "\n";
        parentName += (*nodeIt).name + " + ";
        parentBBox.extend(nodeIt.bbox());
      }
      (*it).name = parentName;
      (*it).bbox = parentBBox;
      std::cout << "level: " << it.level() << " " << parentName << "\n";
    }
    std::cout << std::endl;

    // special hierachical query
    {
      std::cout << "Hierarchical query: "
                << "\n";
      std::vector<Object> results;
      Box2<int> searchBox = {{0, 0}, {8, 31}};
      rtree.hierachical_query(searchBox.min, searchBox.max, results);
      for (const auto &res : results)
        std::cout << res.name << "\n";
    }
    std::cout << std::endl;
  }

  // use a custom indexable for array and indices as values
  {
    struct ArrayIndexable {

      ArrayIndexable(const std::vector<Box2<int>> &array) : array(array) {}

      const int *min(const uint32_t index) const { return array[index].min; }
      const int *max(const uint32_t index) const { return array[index].max; }

      const std::vector<Box2<int>> &array;
    };

    ArrayIndexable indexable(boxes);

    typedef uint32_t IndexType;
    spatial::RTree<int, IndexType, 2, 4, 2, ArrayIndexable> rtree(indexable);

    std::vector<uint32_t> indices(boxes.size());
    std::iota(indices.begin(), indices.end(), 0);
    rtree.insert(indices.begin(), indices.end());

    indices.clear();
    Box2<int> searchBox = {{0, 0}, {8, 31}};
    rtree.overlaps(searchBox.min, searchBox.max, indices);
    std::cout << "Object query results: " << std::endl;
    for (auto index : indices)
      std::cout << "index: " << index << " " << objects[index].name << " "
                << objects[index].bbox << "\n";
    std::cout << std::endl;

    // custom allocator example
    {
      std::cout << "Custom allocator example:\n";

      const int kMaxKeysPerNode = 4;
      const int kVolumeMode = spatial::box::eSphericalVolume;

      typedef spatial::BoundingBox<int, 2, kVolumeMode, double> tree_bbox_type;
      typedef spatial::detail::Node<uint32_t, tree_bbox_type, kMaxKeysPerNode>
          tree_node_type;
      typedef tree_allocator<tree_node_type> tree_allocator_type;

      typedef spatial::RTree<int, uint32_t, 2, kMaxKeysPerNode,
                             kMaxKeysPerNode / 2, ArrayIndexable, kVolumeMode,
                             double, tree_allocator_type>
          CustomTree;

      CustomTree rtree(indexable, tree_allocator_type());

      indices.resize(boxes.size());
      std::iota(indices.begin(), indices.end(), 0);
      rtree.insert(indices.begin(), indices.end());
      std::cout << std::endl;
    }
  }

  return 0;
}
