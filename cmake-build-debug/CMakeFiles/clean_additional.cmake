# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "src/CMakeFiles/Cats_autogen.dir/AutogenUsed.txt"
  "src/CMakeFiles/Cats_autogen.dir/ParseCache.txt"
  "src/Cats_autogen"
  "test/CMakeFiles/Test_autogen.dir/AutogenUsed.txt"
  "test/CMakeFiles/Test_autogen.dir/ParseCache.txt"
  "test/Test_autogen"
  )
endif()
