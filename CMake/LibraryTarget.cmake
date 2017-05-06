
macro(__rayne_create_target _NAME _TYPE _SOURCES _HEADERS _RAYNE_LIBRARIES _VERSION _ABI)
    # Input check
    if(NOT (("${_TYPE}" STREQUAL "STATIC") OR ("${_TYPE}" STREQUAL "SHARED")))
        message(FATAL_ERROR "Type must be either \"STATIC\" or \"SHARED\", is \"${_TYPE}\"")
    endif()

    # Create the target name
    set(TARGET_NAME "${_NAME}")

    if("${_TYPE}" STREQUAL "STATIC")
        set(TARGET_NAME "${TARGET_NAME}-static")
    endif()

    # Create the target
    if(APPLE)

        set(CMAKE_OSX_ARCHITECTURES x86_64)

        if(POLICY CMP0042)
            cmake_policy(SET CMP0042 NEW) # Set MACOSX_RPATH=YES by default
        endif()

    elseif(UNIX)
    elseif(WIN32)
    endif()

    set(ALL_SOURCES "${_SOURCES}" "${_HEADERS}")

    add_library("${TARGET_NAME}" ${_TYPE} ${ALL_SOURCES})
    #set_target_properties("${TARGET_NAME}" PROPERTIES VERSION ${_VERSION} SOVERSION ${_ABI})
    #set_target_properties("${TARGET_NAME}" PROPERTIES PUBLIC_HEADER "${_HEADERS}")

    if(UNIX)
        target_compile_options(${TARGET_NAME} PUBLIC -m64)
    endif()

    if(MINGW)
        target_compile_options(${TARGET_NAME} PUBLIC -m64)
    endif()

    if(NOT ("${_HEADERS}" STREQUAL ""))
        foreach(HEADER ${_HEADERS})
            add_custom_command(TARGET "${TARGET_NAME}" PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}" "${CMAKE_CURRENT_BINARY_DIR}/include/${HEADER}")
        endforeach()
    endif()

    if(NOT ("${_RAYNE_LIBRARIES}" STREQUAL ""))
        foreach(LIBRARY ${_RAYNE_LIBRARIES})

            set(LIBRARY_NAME ${LIBRARY})

            if("${_TYPE}" STREQUAL "STATIC")
                set(LIBRARY_NAME "${LIBRARY_NAME}-static")
            endif()

            target_link_libraries("${TARGET_NAME}" ${LIBRARY_NAME})
        endforeach()
    endif()

endmacro()

macro(rayne_add_library _NAME _SOURCES _HEADERS _RAYNE_LIBRARIES _VERSION _ABI)

    __rayne_create_target(${_NAME} SHARED "${_SOURCES}" "${_HEADERS}" "${_RAYNE_LIBRARIES}" "${_VERSION}" "${_ABI}")

endmacro()

macro(rayne_copy_resources _TARGET _RESOURCES)

    foreach(_RESOURCE ${_RESOURCES})

        if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_RESOURCE}")
            add_custom_command(TARGET ${_TARGET} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/${_RESOURCE}" "$<TARGET_FILE_DIR:${_TARGET}>/${_RESOURCE}")
        else()
            add_custom_command(TARGET ${_TARGET} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${_RESOURCE}" "$<TARGET_FILE_DIR:${_TARGET}>/${_RESOURCE}")
        endif()

    endforeach()

endmacro()


macro(rayne_install)
    install(${ARGV})
endmacro(rayne_install)


macro(rayne_set_module_output_directory _TARGET)

    if(WIN32)
        set_target_properties(${_TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/${_TARGET}"
            LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/${_TARGET}"
            ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug/${_TARGET}")

        set_target_properties(${_TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/${_TARGET}"
            RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/RelWithDebInfo/${_TARGET}"
            RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/RelWithDebInfo/${_TARGET}")

        set_target_properties(${_TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/${_TARGET}"
            LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${_TARGET}"
            ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Release/${_TARGET}")
    else()
        set_target_properties(${_TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_TARGET}"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${_TARGET}"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${_TARGET}")
    endif()

endmacro()


macro(rayne_set_test_output_directory _TARGET)

    if(WIN32)
        set_target_properties(${_TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/Tests/${_TARGET}"
            LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/Tests/${_TARGET}"
            ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug/Tests/${_TARGET}")

        set_target_properties(${_TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/Tests/${_TARGET}"
            RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/RelWithDebInfo/Tests/${_TARGET}"
            RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/RelWithDebInfo/Tests/${_TARGET}")

        set_target_properties(${_TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/Tests/${_TARGET}"
            LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/Tests/${_TARGET}"
            ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Release/Tests/${_TARGET}")
    else()
        set_target_properties(${_TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Tests/${_TARGET}"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Tests/${_TARGET}"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Tests/${_TARGET}")
    endif()

endmacro()