cmake_minimum_required(VERSION 3.11)

project(terratest)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mwindows -O3 -static-libgcc -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,-Bdynamic,--no-whole-archive -Wl,--stack,4194304")

	add_custom_target(
	lib_glad

	COMMAND cd lib/glad && mingw32-make 
	)

	add_custom_target(
	lib_glfw

	COMMAND  cd lib/glfw && mingw32-make
	)

	add_custom_target(
	lib_soil2

	COMMAND  cd lib/SOIL2 && mingw32-make
	)
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -static-libgcc")

	add_custom_target(
	lib_glad

	COMMAND  cd lib/glad && make
	)

	add_custom_target(
	lib_glfw

	COMMAND  cd lib/glfw && make
	)

	add_custom_target(
	lib_soil2

	COMMAND  cd lib/SOIL2 && make
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

link_directories(${CMAKE_BINARY_DIR}/lib/glfw/src)
link_directories(${CMAKE_BINARY_DIR}/lib/glad)
link_directories(${CMAKE_BINARY_DIR}/lib/SOIL2)

#build release version
add_executable(
	${PROJECT_NAME}

	${source}
)
add_dependencies(${PROJECT_NAME} lib_glad lib_glfw lib_soil2) 

#Link with libraries
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	target_link_libraries(${PROJECT_NAME} libglfw3.a gdi32 m)	
else()
	target_link_libraries(${PROJECT_NAME} libglfw3.a m)
endif()
target_link_libraries(${PROJECT_NAME} libsoil2.a libglad.a m OpenGL::GL)

#build development version
add_executable(
	${PROJECT_NAME}_dev

	${source}
)
add_dependencies(${PROJECT_NAME}_dev lib_glad lib_glfw lib_soil2)
#Link with libraries
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	target_link_libraries(${PROJECT_NAME}_dev libglfw3.a gdi32 m)
else()
	target_link_libraries(${PROJECT_NAME}_dev libglfw3.a m)
endif()
target_link_libraries(${PROJECT_NAME}_dev libsoil2.a libglad.a m OpenGL::GL)
target_compile_definitions(${PROJECT_NAME}_dev PUBLIC DEV_VERSION)
target_compile_options(${PROJECT_NAME}_dev PUBLIC "-Wl,--stack,4194304 -g")
