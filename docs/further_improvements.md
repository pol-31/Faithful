- 3d-collision detection;
- menu - exact the same place from last safe (including mobs pos)
- there's no "map", there's only current view distance, so to "open" map
you should Ctrl+ few times

(std::pmr::monotonic_buffer?)
- global log string allocator
- global collision shape data allocator

- appealing and pleasurable for eyes documentation:
/// Computes the bounding rectangle of a set of vertices.
/// Iterates through all points to find the minimum and maximum coordinates.
/// \param vertex_num The number of vertices in the input array.
/// \param vertex_data Pointer to the array of vertex coordinates.
/// \return A Rectangle structure representing the bounding rectangle.

- folly::future / folly::executors (interesting ideas)
- make audio playback spatial
- delete more useless tinygltf data after loading to gpu
- FAITHFUL_OPENGL_SUPPORT_ASTC:
GL_COMPRESSED_RGBA_ASTC_6x6_KHR + normal_map z-coord reconstruction in shader
- src/loader/ all make thread-safe + separate "registration" and actual loading


- LoadingManager (in case if it contains assets that haven't been loaded
yet but be already don't need them due to loading of other tiles AND
in case of priority):
// TODO: we need priority queue with possibility to change order
// TODO: there's two types of Load Manager checking: enqueueing; loading
// TODO: not actual loading, but pushing id to task_queue_


- GameLogicThreadPool:
// TODO: lazy predict first loading entry? - if we're in Menu we can
"preload" first save
// TODO: PriorityState::kNoLoading -
//   boss fight / just standing (LoadManager queue empty)


- Weather: wind, falling snow/rain (particles in Draw()), but....
How to create such __events__? we don't have mechanism for this by now

- GameLogicThreadPool: UpdateGameTimeOfADay ? do we need it? idk, maybe not,
maybe we want location has only one state (not two: day/night)