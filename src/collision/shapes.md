Although geometry shapes are often provided as an example of OOP, but
it's not common in collision detection because:
- all implementations different;
- we can't use virtual methods for collision tests of 2 objects, because
it's 2 objects, not 1 (then need some extra class Interaction and still
it's hard to deduce type at runtime);
- just for declarations? - no, we don't need overhead caused by V-tables

But for documentation, we can provide interface, each shape should provide:


class Shape {
  public:
    static std::vector<Shape> Create(
        const std::string &name, const Transform *renderFromObject,
        const Transform *objectFromRender, bool reverseOrientation,
        const ParameterDictionary &parameters,
        const std::map<std::string, floatTexture> &floatTextures, const FileLoc *loc,
        Allocator alloc);

    std::string ToString() const;

    inline Bounds3f Bounds() const;

    inline std::optional<ShapeIntersection> Intersect(
        const Ray &ray, float tMax = Infinity) const;

    inline bool IntersectP(const Ray &ray, float tMax = Infinity) const;
};

