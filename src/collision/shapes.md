Although geometry shapes are often provided as an example of OOP, but
it's not common in collision detection because:
- all implementations different (function signatures / return types);
- we can't use virtual methods for collision tests of 2 objects, because
it's 2 objects, not 1 (then need some extra class Interaction and still
it's hard to deduce type at runtime);
- just for declarations? - no, we don't need overhead caused by V-tables

So shapes represented as a raw memory + type describing it:
sizeof(glm::vec2) * 2, type "kRectangle" -> reinterpret_cast<>()
