# FATX Filesystem Library
#
# Copyright (C) 2015  Matt Borgerson
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.1)

project(fatxfs)

set(CMAKE_BUILD_TYPE Debug)
set(QEMU true)

find_package(PkgConfig)

set(FATX_SOURCES "")
add_subdirectory(${CMAKE_SOURCE_DIR}/libfatx)
add_library(fatx ${FATX_SOURCES})
target_compile_options(fatx PUBLIC -D_FILE_OFFSET_BITS=64)

target_compile_options(fatx PUBLIC -O2)
target_link_options(fatx PUBLIC -O2)

if (QEMU)
    set(QEMU_DIR ${CMAKE_SOURCE_DIR}/../xemu)
    set(QEMU_BUILD_DIR ${QEMU_DIR}/build)
    pkg_search_module(GLIB glib-2.0)
    find_package(ZLIB REQUIRED)
    target_link_libraries(fatx ${GLIB_LIBRARIES} ${ZLIB_LIBRARIES})
    target_compile_options(fatx PUBLIC ${GLIB_CFLAGS_OTHER} ${ZLIB_CFLAGS_OTHER})
    add_definitions(-DFATX_QEMU)
    include_directories(${GLIB_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
    include_directories(${QEMU_DIR}/include)
    include_directories(${QEMU_BUILD_DIR})

    set(QEMU_LIBS 
        ${QEMU_BUILD_DIR}/libblock.fa
        ${QEMU_BUILD_DIR}/libauthz.fa
        ${QEMU_BUILD_DIR}/libqom.fa
        ${QEMU_BUILD_DIR}/libio.fa
        ${QEMU_BUILD_DIR}/libcrypto.fa
        ${QEMU_BUILD_DIR}/libqemuutil.a
        )
    target_link_libraries(fatx ${QEMU_LIBS})
endif()

if (MSVC)
    target_compile_options(fatx PUBLIC /std:c11)
endif()
set_target_properties(fatx PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (EXISTS ${CMAKE_SOURCE_DIR}/fatxfs)
add_subdirectory(${CMAKE_SOURCE_DIR}/fatxfs)
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/gfatx)
add_subdirectory(${CMAKE_SOURCE_DIR}/gfatx)
endif()
