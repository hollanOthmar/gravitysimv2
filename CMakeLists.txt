cmake_minimum_required(VERSION 3.5)
project(GravitySim)
set (CMAKE_CXX_STANDARD 14)

#LINKING SHARED LIBRARIES
set( PROJECT_LINK_LIBS libGL.so.1)

#glfw library
set( GLFW_LINK_LIBS libglfw.so )

#set( GDAL_LINK_LIBS libgdal.so )

#CGAL LINK
set (CGAL_LINK_LIBS libCGAL.so.11)

#GMP LINK
set (GMP_LINK_LIBS libgmp.so.3)

#MPFR LINK
set (MPFR_LINK_LIBS libmpfr.so.1)

link_directories(/snap/gnome-3-26-1604/74/usr/lib/x86_64-linux-gnu/mesa/ /usr/local/lib/)

#ADDING INCLUDE DIRECTORIES
include_directories(/usr/local/include/ include )

file(GLOB SOURCES "src/*")

add_executable(GravitySim ${SOURCES})

target_link_libraries(GravitySim ${PROJECT_LINK_LIBS} ${GLFW_LINK_LIBS} ${CMAKE_DL_LIBS} ${CGAL_LINK_LIBS} ${GMP_LINK_LIBS} ${MPFR_LINK_LIBS})
