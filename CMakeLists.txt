file(READ "${CMAKE_CURRENT_LIST_DIR}/pack.json" PACK_JSON)
string(JSON PACK_NAME GET "${PACK_JSON}" name)

add_library(${PACK_NAME} STATIC
	src/rigComponent.cpp
	src/rigComponent.h
	src/EntityProperty.cpp
	src/MeshFaces.cpp
	src/PrimitiveBounds.cpp
	src/PrimitiveVertices.cpp
	src/PrimitiveVertices3D.cpp
	src/rig/create.cpp
	src/rig/sketch.cpp
	src/PathEllipticArc.cpp
	src/PathEllipticArc.h
)
target_link_libraries(${PACK_NAME} PUBLIC rigkit)
target_include_directories(${PACK_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
)
message(STATUS "Configured pack: ${PACK_NAME} (data-only components)")
