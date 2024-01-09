REMARK #1: (NOT relevant)
File:   tiny_gltf.h
Lines:  1708 - 1714
"""
#ifndef TINYGLTF_NO_INCLUDE_RAPIDJSON
#include "document.h"
#include "prettywriter.h"
#include "rapidjson.h"
#include "stringbuffer.h"
#include "writer.h"
#endif
"""
replaced by:
"""
#ifndef TINYGLTF_NO_INCLUDE_RAPIDJSON
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#endif
"""
Because....todo...
_____________________________
REMARK #2:
astc-enc directory has its own stb_image.h and stb_image_write.h in astc-enc/Source/,
so in order to use only those from external/stb we include full external path:
${CMAKE_SOURCE_DIR}/external/
instead of
${CMAKE_SOURCE_DIR}/external/astc-encoder/Source
We could also simply delete them from astc-enc/Source/,
but this directory may still contain other headers, causing ambiguity.

_____________________________
audio backend: alsa