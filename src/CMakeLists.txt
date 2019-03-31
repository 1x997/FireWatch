set(IWATCH_TARGET_NAME iwatch)

set(IWATCH_HEADER_ROOT ${CMAKE_SOURCE_DIR}/include/iwatch)

# 1 Add api sources
include_directories(${CMAKE_SOURCE_DIR}/include/iwatch)
set(IWATCH_API_HEADERS
    ${IWATCH_HEADER_ROOT}/entity/iwatch_abnormality.h
    ${IWATCH_HEADER_ROOT}/entity/iwatch_common.h
    ${IWATCH_HEADER_ROOT}/entity/iwatch_condition.h
    ${IWATCH_HEADER_ROOT}/entity/iwatch_skeleton.h
    ${IWATCH_HEADER_ROOT}/entity/iwatch_target.h
    ${IWATCH_HEADER_ROOT}/iwatch_engine.h
    ${IWATCH_HEADER_ROOT}/iwatch_hyper_parameter.h
    ${IWATCH_HEADER_ROOT}/iwatch_macros.h
    ${IWATCH_HEADER_ROOT}/iwatch_session.h
)

set(IWATCH_API_SRCS
    entity/iwatch_abnormality.cpp
    entity/iwatch_common.cpp
    entity/iwatch_condition.cpp
    entity/iwatch_skeleton.cpp
    iwatch_engine.cpp
    iwatch_hyper_parameter.cpp
    iwatch_session.cpp)

# 2 Add implementation sources
include_directories(${CMAKE_SOURCE_DIR}/include/iwatch/impl)

set(IWATCH_IMPL_HEADERS
    ${IWATCH_HEADER_ROOT}/impl/Incident_Recognition.h
    ${IWATCH_HEADER_ROOT}/impl/iWatch.h
    ${IWATCH_HEADER_ROOT}/impl/iwatch_manager.h
    ${IWATCH_HEADER_ROOT}/impl/Judgement.h
    ${IWATCH_HEADER_ROOT}/impl/Pose_Estimation.h
    ${IWATCH_HEADER_ROOT}/impl/Tracker.h
)

set(IWATCH_IMPL_SRCS
    impl/Incident_Recognition.cpp
    impl/iWatch.cpp
    impl/Judgement.cpp
    impl/Pose_Estimation.cpp
    impl/Tracker.cpp
    impl/iwatch_manager.cpp
    impl/AutonomousServiceTerminalAbnormalDetection.cpp
    impl/GarbageDetection.cpp
    impl/ServicePeopleAppearanceDetection.cpp
    impl/StationaryObjectDetection.cpp
    impl/VideoEquipmentAbnormalDetection.cpp
    impl/eventloop.cpp
    impl/iwatch_api_session.cpp)

add_library(iwatch ${IWATCH_IMPL_SRCS} ${IWATCH_IMPL_HEADERS} ${IWATCH_API_SRCS} ${IWATCH_API_HEADERS})	# for static lib
# add_library(iwatch SHARED ${IWATCH_IMPL_SRCS} ${IWATCH_IMPL_HEADERS} ${IWATCH_API_SRCS} ${IWATCH_API_HEADERS})	# for dynamic lib

if (WIN32)
    set_property(TARGET ${IWATCH_TARGET_NAME} PROPERTY DEBUG_POSTFIX d)
    
    source_group("Include Files" FILES ${IWATCH_API_HEADERS})
    source_group("Source Files" FILES ${IWATCH_API_SRCS})

    source_group("Include Files\\Impl" FILES ${IWATCH_IMPL_HEADERS})
    source_group("Source Files\\Impl" FILES ${IWATCH_IMPL_SRCS})
endif ()
