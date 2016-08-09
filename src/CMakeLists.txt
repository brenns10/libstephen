list(APPEND libstephen_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/args.c
  ${CMAKE_CURRENT_LIST_DIR}/arraylist.c
  ${CMAKE_CURRENT_LIST_DIR}/bitfield.c
  ${CMAKE_CURRENT_LIST_DIR}/charbuf.c
  ${CMAKE_CURRENT_LIST_DIR}/hashtable.c
  ${CMAKE_CURRENT_LIST_DIR}/iter.c
  ${CMAKE_CURRENT_LIST_DIR}/linkedlist.c
  ${CMAKE_CURRENT_LIST_DIR}/log.c
  ${CMAKE_CURRENT_LIST_DIR}/smbunit.c
  ${CMAKE_CURRENT_LIST_DIR}/string.c
  ${CMAKE_CURRENT_LIST_DIR}/util.c
  ${CMAKE_CURRENT_LIST_DIR}/ringbuf.c
  )

include("${CMAKE_CURRENT_LIST_DIR}/regex/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/lisp/CMakeLists.txt")
