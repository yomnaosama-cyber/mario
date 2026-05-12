# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/mario_scene_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/mario_scene_autogen.dir/ParseCache.txt"
  "mario_scene_autogen"
  )
endif()
