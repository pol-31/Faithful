TODO: deprecated..............
now there is no Collision, Loading, Update thread pools, but 3 managers and
1 thread pool GameLogicThreadPool and 1 dynamic SupportThreadPool that don't
have its own queue. Its usage: between main menu and actual game we need to load
a lot of data from memory (all assets: shaders, audio, textures, models, etc.)

## Thread Pools
"auto" describes work that handled by thread pool automatically often
using internal Pool / Manager which contains all models/events related to this;

"extra" means events which were added from the outside (e.g. other thread pool)
and which were added to some internal queue of the thread pool.

___
### AudioThreadPool
##### 1 thread always
- auto: streaming background music wrt current biome
- extra: sounds emitted by hits/mobs/...

NOTE: dynamic can also set ambient music of liquid/gas (for such things thread
pool can contain few queues/slots)

*Don't affect other thread pools (consumer only), can only handle task from others.*
___
### CollisionThreadPool
##### 0-N dynamic number of threads (see explanation below)
- auto: internal BVH (probably LHBVH) structure managing (updating). We always
have only static amount of models, so we can preallocate enough memory from
the very beginning. In our game makes sense represent it in a 2D space, because
y-coord (height) is not often used. TODO: ________________
- extra: runtime collision checks like hit, shoot, gas, fire, water - each
possible emitted by the player_character or enemy with other mob of dynamic
ambient. Also represent user mouse input using kd-trees.

NOTE: we have single LHBVH for the entire game, but kd-trees have a lot of
instances (e.g. each menu/config/inventory page is another one kd-tree with
its own button collisions). So which one kd-tree currently are in use defined
by current state. Example: e.g. user push "hit" from input thread pool

*Don't affect other thread pools (consumer only), can only handle task from others.*
___
### DisplayInteractionThreadPoll
##### 1 thread always (full blocking Main thread)
- auto: rendering / input processing according to current state (represented
as a while(state) loop). Depends on input sends "signals" to Audio, Collision,
UpdateThreadPool
- extra: missing, what means it's not consumer but only producer; full auto work

NOTE: is in charge or FaithfulState (all scene changing);
**Why both rendering / input:** because of how GLFW handler OpenGL context
(provided by Glad loader). We can't separate OpenGL and GLFW (simple example:
render loop requires both drawing (e.g. glDrawArrays()) and buffer swapping
(e.g. glfwSwapBuffers())). Input handled by GLFW and all drawing / input processing 
required to work with in main (where glfwInit() called) thread.

*Affect other thread pools because of user's input: all except the LoadingThreadPool*
___
### LoadingThreadPool
##### 0-N dynamic number of threads (see explanation below)
- auto: missing, what means if there's no extra tasks, it suspended.
But there is no waste or cpu time/resources because in this case all used threads
were "released" and utilized by other thread pools with dynamic number of threads
- extra: tasks from UpdateThreadPool, which track current biome and states; and
which also defines how many threads LoadingThreadPool needs (in may be some
_lazy_ loading or _intensive_ which consumes different amount or memory/threads)

NOTE: loads most of the assets (shaders, textures, audio, etc.), but not all (!):
some frequently used assets are embedded (linked) into the source code for
the optimization purposes (see Faithful/assets/embedded/)

*Don't affect other thread pools (consumer only), can only handle task from others.*
___
### UpdateThreadPool
##### 1-N dynamic number of threads (see explanation below)
- auto: updates all Updatable objects (which has method Update()), holds internal
timer (provided by system or glfw) and updates time of a day; other events based
on time; weather (randomly or not); current biome tracking.
- extra: dynamic events (based on time) like some phenomenon area / effect

NOTE: the number of Updatable object always static which allows to allocate
memory only once; always need 1 thread by default;

Current biome deduced based on PlayerCharacter location (radar & predictions)

*In charge or thread number for CollisionThreadPool, LoadingThreadPool and itself*
*Always affect other thread pools (this is its job): all except the DisplayInteractionThreadPoll*
---
---
## Static Vs Dynamic number of threads
General thread number: min = 4 to max = std::thread::hardware_concurrency().
We're trying to use as much parallelization as possible. By default,
(nowadays almost everyone has 4-8 logical cores) we have such distribution:
- DisplayInteractionThreadPoll: 1 thread;
- AudioThreadPool: 1 thread
- CollisionThreadPool + LoadingThreadPool + UpdateThreadPool: std::thread::hardware_concurrency() - 2
Why 4 is minimum: 1 for GLFW+OpenGL, 1 for OpenAL, 1 for CollisionThreadPool,
1 for UpdateThreadPool. (This is in case of normal situations - those two last
can work for LoadingThreadPool)
___
#### Static number of threads
Some thread pools require initialized context (GLFW & Glad (OpenGL), OpenAL)
and for each of them 1 specialized thread was used, so we have
AudioThreadPool and DisplayInteractionThreadPoll with 1 thread each.
Each aforementioned context unique (not shared), which don't allow us to use
more threads for each (yes, we could create more OpenAL and GLFW context, but
they have no shared state and it brings additional complexity what we don't want).
___
#### Dynamic number of threads
TODO: not thread pools but rather queues with threads with states

StaticThreadPool<1> audio;
StaticThreadPool<0> render_input;
StaticThreadPool<std::thread::hardware_concurrency - 2> update_collision_dynamic_loading;
DynamicThreadPool<4> static_loading;
