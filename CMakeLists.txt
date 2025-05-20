cmake_minimum_required(VERSION 3.16)
project(quantum_ns3_playground)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include paths
include_directories(
    ${CMAKE_SOURCE_DIR}/simulations
    ${CMAKE_SOURCE_DIR}/simulations/quantum_v0
    ${CMAKE_SOURCE_DIR}/simulations/quantum_v1
    ${CMAKE_SOURCE_DIR}/simulations/quantum_v2

    $ENV{HOME}/ns3-install/include
    ${CMAKE_SOURCE_DIR}/external/qpp/include
    ${CMAKE_SOURCE_DIR}/external/eigen
    ${CMAKE_SOURCE_DIR}/external/qasmtools/include
)

# Link directories
link_directories($ENV{HOME}/ns3-install/lib)

# Grab quantum sources
file(GLOB QUANTUM_SRCS ${CMAKE_SOURCE_DIR}/simulations/quantum_v0/*.cc)
file(GLOB QUANTUM_SRCS ${CMAKE_SOURCE_DIR}/simulations/quantum_v1/*.cc)
file(GLOB QUANTUM_SRCS ${CMAKE_SOURCE_DIR}/simulations/quantum_v2/*.cc)

# Grab all simulation demos
file(GLOB SIMS ${CMAKE_SOURCE_DIR}/simulations/*.cc)

foreach(f ${SIMS})
    get_filename_component(name ${f} NAME_WE)
    add_executable(${name} ${f} ${QUANTUM_SRCS})
    target_link_libraries(${name}
        ns3.44-core
        ns3.44-network
        ns3.44-internet
        ns3.44-point-to-point
    )
endforeach()
