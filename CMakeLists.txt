cmake_minimum_required(VERSION 3.10)

project(beta_reduction)
set(CMAKE_CXX_STANDARD 17)

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Release)
endif ()

add_executable(main
    lambda.cpp
    main.cpp
    )
