
#include <QuadTree.h>
#include <RTree.h>
#include <iostream>
#include <numeric>
#include <sstream>

struct Object {
  spatial::BoundingBox<int, 2> bbox;
  std::string name;
};

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
