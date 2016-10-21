if(NOT TARGET res)
    find_package(res QUIET)
    if(res_FOUND)
        message(STATUS "Found Package res")
    else()
        message(STATUS "Cannot Find Pacakage res, will download from github.")
        include(FetchContent)
        FetchContent_Declare(
            res_library
            GIT_REPOSITORY  git@github.com:goteet/res.git
            GIT_TAG         master
           
            SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/extern/res
        )
        
        FetchContent_GetProperties(res_library)
        if(NOT res_library_POPULATED)
            message(STATUS "Populate res library")
            FetchContent_Populate(res_library)
            FetchContent_GetProperties(res_library)
            add_subdirectory(${res_library_SOURCE_DIR} ${res_library_BINARY_DIR})
        endif()
    endif()
else()
    message(STATUS "Detected Target res")
endif()