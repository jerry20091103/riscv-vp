file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE AMS-HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/ams-model/*.h)
file(GLOB_RECURSE AMS-CPP ${CMAKE_CURRENT_SOURCE_DIR}/ams-model/*.cpp)
list(REMOVE_ITEM AMS-CPP ${CMAKE_CURRENT_SOURCE_DIR}/ams-model/testbench.cpp)

add_executable(ams-vp
        ams_main.cpp
        ${HEADERS}
	${AMS-HEADERS}
	${AMS-CPP})

target_link_libraries(ams-vp rv32 platform-common gdb-mc
	${Boost_LIBRARIES} ${SystemCAMS_LIBRARIES} ${SystemC_LIBRARIES} pthread)

INSTALL(TARGETS ams-vp RUNTIME DESTINATION bin)
