cmake_minimum_required(VERSION 3.16)

project(enet6 VERSION 6.1.2 LANGUAGES C)

include(CheckFunctionExists)
include(CheckStructHasMember)
include(CheckTypeSize)

# Feature checks
check_function_exists("fcntl" HAS_FCNTL)
check_function_exists("poll" HAS_POLL)
check_function_exists("getaddrinfo" HAS_GETADDRINFO)
check_function_exists("getnameinfo" HAS_GETNAMEINFO)
check_function_exists("gethostbyname_r" HAS_GETHOSTBYNAME_R)
check_function_exists("gethostbyaddr_r" HAS_GETHOSTBYADDR_R)
check_function_exists("inet_pton" HAS_INET_PTON)
check_function_exists("inet_ntop" HAS_INET_NTOP)
check_struct_has_member("struct msghdr" "msg_flags" "sys/types.h;sys/socket.h" HAS_MSGHDR_FLAGS)

set(CMAKE_EXTRA_INCLUDE_FILES "sys/types.h" "sys/socket.h")
check_type_size("socklen_t" HAS_SOCKLEN_T BUILTIN_TYPES_ONLY)
unset(CMAKE_EXTRA_INCLUDE_FILES)

# Source and header files
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/enet6)
set(INCLUDE_FILES
    ${INCLUDE_DIR}/callbacks.h
    ${INCLUDE_DIR}/enet.h
    ${INCLUDE_DIR}/list.h
    ${INCLUDE_DIR}/protocol.h
    ${INCLUDE_DIR}/time.h
    ${INCLUDE_DIR}/types.h
    ${INCLUDE_DIR}/unix.h
    ${INCLUDE_DIR}/utility.h
    ${INCLUDE_DIR}/win32.h
)

set(SOURCE_FILES
    src/address.c
    src/callbacks.c
    src/compress.c
    src/host.c
    src/list.c
    src/packet.c
    src/peer.c
    src/protocol.c
    src/unix.c
    src/win32.c
)

add_library(enet6 ${INCLUDE_FILES} ${SOURCE_FILES})

target_include_directories(enet6 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Add compile definitions based on feature detection
target_compile_definitions(enet6 PRIVATE
    $<$<BOOL:${HAS_FCNTL}>:HAS_FCNTL=1>
    $<$<BOOL:${HAS_POLL}>:HAS_POLL=1>
    $<$<BOOL:${HAS_GETADDRINFO}>:HAS_GETADDRINFO=1>
    $<$<BOOL:${HAS_GETNAMEINFO}>:HAS_GETNAMEINFO=1>
    $<$<BOOL:${HAS_GETHOSTBYNAME_R}>:HAS_GETHOSTBYNAME_R=1>
    $<$<BOOL:${HAS_GETHOSTBYADDR_R}>:HAS_GETHOSTBYADDR_R=1>
    $<$<BOOL:${HAS_INET_PTON}>:HAS_INET_PTON=1>
    $<$<BOOL:${HAS_INET_NTOP}>:HAS_INET_NTOP=1>
    $<$<BOOL:${HAS_MSGHDR_FLAGS}>:HAS_MSGHDR_FLAGS=1>
    $<$<BOOL:${HAS_SOCKLEN_T}>:HAS_SOCKLEN_T=1>
)

# Platform/compiler specific settings
if(MSVC)
    target_compile_options(enet6 PRIVATE /W3)
else()
    target_compile_options(enet6 PRIVATE -Wall -Wextra)
endif()

if(WIN32 AND BUILD_SHARED_LIBS AND (MSVC OR CMAKE_C_COMPILER_ID MATCHES "Clang"))
    target_compile_definitions(enet6 PRIVATE
        ENET_DLL=1
        ENET_BUILDING_LIB
    )
endif()

if(WIN32)
    target_link_libraries(enet6 PRIVATE winmm ws2_32)
endif()

# Installation
include(GNUInstallDirs)
install(TARGETS enet6
    EXPORT enet6Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/enet6
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT enet6Targets
    FILE enet6Targets.cmake
    NAMESPACE enet6::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/enet6
)
