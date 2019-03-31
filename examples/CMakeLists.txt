file(COPY ../models DESTINATION ${CMAKE_BINARY_DIR}/examples)
file(COPY ../medias DESTINATION ${CMAKE_BINARY_DIR}/examples)

# sdk demo
add_executable(iwatch_demo iwatch_demo.cpp)
target_link_libraries(iwatch_demo iwatch ${OpenCV_LIBS} ${Dlib_LIBS} ${OpenPose_LIBS})

add_executable(iwatch_demo_phase_2_to_3 iwatch_demo_phase_2_to_3.cpp)
target_link_libraries(iwatch_demo_phase_2_to_3 iwatch ${OpenCV_LIBS} ${Dlib_LIBS} ${OpenPose_LIBS})

# functional testing demo
add_executable(iwatch_func_demo iwatch_func_demo.cpp)
target_link_libraries(iwatch_func_demo iwatch ${OpenCV_LIBS} ${Dlib_LIBS} ${OpenPose_LIBS})

# memleak demo
add_executable(iwatch_memleak_demo iwatch_memleak_demo.cpp)
target_link_libraries(iwatch_memleak_demo iwatch ${OpenCV_LIBS} ${Dlib_LIBS} ${OpenPose_LIBS})

# multitask demo
add_executable(iwatch_multitask_demo iwatch_multitask_demo.cpp)
target_link_libraries(iwatch_multitask_demo iwatch ${OpenCV_LIBS} ${Dlib_LIBS} ${OpenPose_LIBS})

# algorithm's implementation demo
include_directories(${CMAKE_SOURCE_DIR}/include/iwatch/impl)

add_executable(iwatch_impl_demo iwatch_impl_demo.cpp)
target_link_libraries(iwatch_impl_demo iwatch ${OpenCV_LIBS} ${Dlib_LIBS} ${OpenPose_LIBS})

add_executable(iwatch_impl_demo_phase_2_to_3 iwatch_impl_demo_phase_2_to_3.cpp)
target_link_libraries(iwatch_impl_demo_phase_2_to_3 iwatch ${OpenCV_LIBS} ${Dlib_LIBS} ${OpenPose_LIBS})
