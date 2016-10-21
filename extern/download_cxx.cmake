if(NOT TARGET cxx)
    find_package(cxx QUIET)
    if(cxx_FOUND)
        message(STATUS "Found Package cxx")
    else()
        message(STATUS "Cannot Find Pacakage cxx, will download from github.")
        include(FetchContent)
        FetchContent_Declare(
            cxx_library
            GIT_REPOSITORY  git@github.com:goteet/cppxx.git
            GIT_TAG         master
           
            SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/extern/cxx
        )
        
        FetchContent_GetProperties(cxx_library)
        if(NOT cxx_library_POPULATED)
            message(STATUS "Populate cxx library")
            FetchContent_Populate(cxx_library)
            FetchContent_GetProperties(cxx_library)
            add_subdirectory(${cxx_library_SOURCE_DIR} ${cxx_library_BINARY_DIR})
        endif()
    endif()
else()
    message(STATUS "Detected Target cxx")
endif()

