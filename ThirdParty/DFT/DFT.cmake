add_subdirectory(${CMAKE_CURRENT_LIST_DIR})
set(LEARNING_TRACKERS ${LEARNING_TRACKERS} DFT/src/DFT)
set(THIRD_PARTY_INCLUDE_DIRS ${THIRD_PARTY_INCLUDE_DIRS} DFT/include)
# target_link_libraries(mtf opentld cvblobs)
set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} dft)
set(THIRD_PARTY_RUNTIME_FLAGS ${THIRD_PARTY_RUNTIME_FLAGS} -fopenmp)
