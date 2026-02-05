add_rules("mode.debug", "mode.release")

set_project("enet-ipv6")
set_version("6.1.3")

if not is_plat("windows", "mingw") then
    -- detect features on Unix platforms
    option("fcntl", { cincludes = {"fcntl.h", "unistd.h"}, cfuncs = "fcntl", defines = "HAS_FCNTL=1"})
    option("poll", { cincludes = {"poll.h"}, cfuncs = "poll", defines = "HAS_POLL=1"})
    option("getaddrinfo", { cincludes = {"sys/types.h", "sys/socket.h", "netdb.h"}, cfuncs = "getaddrinfo", defines = "HAS_GETADDRINFO=1"})
    option("getnameinfo", { cincludes = {"sys/types.h", "sys/socket.h", "netdb.h"}, cfuncs = "getnameinfo", defines = "HAS_GETNAMEINFO=1"})
    option("gethostbyaddr_r", { cincludes = {"netdb.h"}, cfuncs = "gethostbyaddr_r", defines = "HAS_GETHOSTBYADDR_R=1"})
    option("gethostbyname_r", { cincludes = {"netdb.h"}, cfuncs = "gethostbyname_r", defines = "HAS_GETHOSTBYNAME_R=1"})
    option("inet_pton", { cincludes = {"arpa/inet.h"}, cfuncs = "inet_pton", defines = "HAS_INET_PTON=1"})
    option("inet_ntop", { cincludes = {"arpa/inet.h"}, cfuncs = "inet_ntop", defines = "HAS_INET_NTOP=1"})
    option("socklen_t", { cincludes = {"sys/types.h", "sys/socket.h"}, ctypes = "socklen_t", defines = "HAS_SOCKLEN_T=1"})

    option("msghdr_flags", function ()
        add_csnippets("msghdr_flags", [[
            #include <sys/socket.h>
            int get_flags(struct msghdr* m)
            { 
                return m->msg_flags;
            }]])
    end)
end

option("examples", { default = true })

add_includedirs("include")

target("enet6", function ()
    set_kind("$(kind)")
    set_group("lib")

    add_headerfiles("include/(enet6/*.h)")
    add_files("src/*.c")

    if is_kind("shared") then
        add_defines("ENET_DLL", { public = true })
    end

    if is_plat("windows", "mingw") then
        add_syslinks("winmm", "ws2_32", { public = true })
    else
        add_options(
            "fcntl",
            "poll", 
            "getaddrinfo",
            "getnameinfo",
            "gethostbyaddr_r",
            "gethostbyname_r",
            "inet_pton",
            "inet_ntop",
            "msghdr_flags",
            "socklen_t")
    end
end)

if has_config("examples") then
    includes("examples/xmake.lua")
end
