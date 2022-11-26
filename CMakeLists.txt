cmake_minimum_required(VERSION 3.11)

project(terratest)

set(CMAKE_C_FLAGS "-O3 -static-libgcc")

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
aux_source_directory(lib deps)

#build release version
add_executable(
	${PROJECT_NAME}

	${source}
	${deps}
)
#Link with libraries
target_link_libraries(${PROJECT_NAME} glfw3 m)
target_link_libraries(${PROJECT_NAME} SOIL2 m OpenGL::GL)

#build development version
add_executable(
	${PROJECT_NAME}_dev

	${source}
	${deps}
)
#Link with libraries
target_link_libraries(${PROJECT_NAME}_dev glfw3 m)
target_link_libraries(${PROJECT_NAME}_dev SOIL2 m OpenGL::GL)
target_compile_definitions(${PROJECT_NAME}_dev PUBLIC DEV_VERSION)
target_compile_options(${PROJECT_NAME}_dev PUBLIC -g)
