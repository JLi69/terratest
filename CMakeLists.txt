cmake_minimum_required(VERSION 3.11)

project(terratest)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -static-libgcc -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,-Bdynamic,--no-whole-archive")

	add_custom_target(
	libglad.a

	COMMAND cd lib/glad && mingw32-make 
	)

	add_custom_target(
	libglfw3.a

	COMMAND  cd lib/glfw && mingw32-make
	)

	add_custom_target(
	libSOIL2.a

	COMMAND  cd lib/SOIL2 && mingw32-make
	)
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -static-libgcc")

	add_custom_target(
	libglad.a

	COMMAND  cd lib/glad && make
	)

	add_custom_target(
	libglfw3.a

	COMMAND  cd lib/glfw && make
	)
endif()

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)
add_subdirectory(lib/glad)
include_directories(${CMAKE_BINARY_DIR}/lib/glad/include)
add_subdirectory(lib/SOIL2)
include_directories(lib/SOIL2/src)

add_custom_target(
	run
	COMMAND ./${PROJECT_NAME}	
	)

add_custom_target(
	debug
	COMMAND ./${PROJECT_NAME}_dev	
	)

find_package(OpenGL REQUIRED)

aux_source_directory(src source)

find_library(GLFW3 NAMES glfw3 glfw PATHS ${CMAKE_BINARY_DIR}/lib) 
find_library(LIBSOIL2 NAMES soil2 PATHS ${CMAKE_BINARY_DIR}/lib/SOIL2) 
find_library(LIBGLAD NAMES glad PATHS ${CMAKE_BINARY_DIR}/lib/glad)

#build release version
add_executable(
	${PROJECT_NAME}

	${source}
)
add_dependencies(${PROJECT_NAME} libglad.a libglfw3.a libSOIL2.a) 

#Link with libraries
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	target_link_libraries(${PROJECT_NAME} glfw3 gdi32 m)
else()
	target_link_libraries(${PROJECT_NAME} glfw3 m)
endif()
target_link_libraries(${PROJECT_NAME} ${LIBSOIL2} ${LIBGLAD} m OpenGL::GL)

#build development version
add_executable(
	${PROJECT_NAME}_dev

	${source}
)
add_dependencies(${PROJECT_NAME}_dev libglad.a libglfw3.a libSOIL2.a)
#Link with libraries
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	target_link_libraries(${PROJECT_NAME}_dev glfw3 gdi32 m)
else()
	target_link_libraries(${PROJECT_NAME}_dev glfw3 m)
endif()
target_link_libraries(${PROJECT_NAME}_dev ${LIBSOIL2} ${LIBGLAD} m OpenGL::GL)
target_compile_definitions(${PROJECT_NAME}_dev PUBLIC DEV_VERSION)
target_compile_options(${PROJECT_NAME}_dev PUBLIC -g)
