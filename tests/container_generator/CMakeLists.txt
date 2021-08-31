cmake_minimum_required( VERSION 3.13 )

set( target "container_generator" )

# Config library
target_include_directories(nrf5_config INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable( ${target}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/app_container_generator.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/container_stack.c"

    ${${PLATFORM}_SOURCE_FILES}
)

set_target_link_options(${target}
    "${CMAKE_CURRENT_SOURCE_DIR}/${target}")

target_compile_options(${target} PUBLIC
    ${${ARCH}_DEFINES})

target_compile_definitions(${target} PUBLIC
    ${USER_DEFINITIONS}
    ${${PLATFORM}_DEFINES}
    ${${SOFTDEVICE}_DEFINES}
    ${${BOARD}_DEFINES})

target_link_libraries( ${target} PRIVATE
    # Luos support
    luos

    # RTB builder container
    rtb_builder

    # NRF
    # Drivers
    nrf5_nrfx_gpiote
    nrf5_nrfx_prs
    # Application
    nrf5_app_error
    nrf5_app_util_platform
    nrf5_app_timer
    nrf5_app_button

    # Mesh
    uECC_${PLATFORM}
)

target_include_directories( ${target} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

create_hex(${target})
add_flash_target(${target})

get_property(target_include_dirs TARGET ${target} PROPERTY INCLUDE_DIRECTORIES)
add_pc_lint(${target}
    "${CMAKE_CURRENT_SOURCE_DIR}/main.c;"
    "${target_include_dirs}"
    "${${PLATFORM}_DEFINES};${${SOFTDEVICE}_DEFINES};${${BOARD}_DEFINES}")
