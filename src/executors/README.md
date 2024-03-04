All of this probably should be refactored, because current approach with
thread specialization is invalid (wow, finally i've thought about preemption...).
So current "bad" situation: GLFW, OpenGL and OpenAL can be used only in 1 thread -
they contexts are not shared between threads and in most of the cases (if not always)
thread unsafe. Current CPUs obviously has more than 1 logical core, so
need to somehow utilize such power.
The problem with "thread specialization" where one thread is in charge of OpenGL,
other - OpenAL, other - building BVH tree for collision is obviously preemption,
which doesn't allow us to feel safe about correctness or working.
So......... idk... need to rethink approach