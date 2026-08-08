file(READ "${CMAKE_CURRENT_LIST_DIR}/pack.json" PACK_JSON)
string(JSON PACK_NAME GET "${PACK_JSON}" name)

add_library(${PACK_NAME} STATIC
	src/rigComponent.cpp
	src/rigComponent.h
	src/PrimitiveBounds.cpp
	src/PrimitiveVertices.cpp
	src/PrimitiveVertices3D.cpp
	src/rig/create.cpp
)
target_link_libraries(${PACK_NAME} PUBLIC rigkit)
target_include_directories(${PACK_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
)
# One TU registers every component; MinGW/MSVC Debug hits the PE COFF section limit
# without big-obj (relocation truncated to fit: IMAGE_REL_AMD64_ADDR32NB).
if(MSVC)
	target_compile_options(${PACK_NAME} PRIVATE /bigobj)
elseif(MINGW OR (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	target_compile_options(${PACK_NAME} PRIVATE -Wa,-mbig-obj)
elseif(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(${PACK_NAME} PRIVATE -bigobj)
endif()
message(STATUS "Configured pack: ${PACK_NAME} (data-only components)")
