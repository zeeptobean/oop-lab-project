set_project("oop-lab-project")
set_languages("c++17")

includes("imgui-sdl3/xmake.lua")

target("toy")
    add_deps("imgui-sdl3-staticlib")
    add_syslinks("SDL3")
    add_packages("sdl3")
    add_links("sdl3")
    add_files("playgui.cpp")
    add_includedirs("include")
    add_includedirs("imgui-sdl3/include")
    add_cxxflags("-Wall", "-Wextra", "-pedantic", "-g", "-march=native", {force = true})

    set_kind("binary")
    if is_plat("windows") then
        add_syslinks("SDL3", "SDL3_image", "gdi32", "winmm", "imm32", "version", "ole32", "oleaut32", "setupapi", "uuid", "shell32")
    elseif is_plat("linux") then
        add_syslinks("SDL3", "SDL3_image", "dl", "pthread", "m")
    elseif is_plat("macosx") then
        add_syslinks("SDL3", "SDL3_image", "Cocoa", "IOKit", "CoreVideo")
    end

    set_toolchains("gcc")
    set_targetdir("bin")
    set_filename("toy")

target("debug")
    add_deps("imgui-sdl3-staticlib")
    add_syslinks("SDL3")
    add_packages("sdl3")
    add_links("sdl3")
    -- add_files("src/*.cpp")
    add_files("include/main.cpp")
    add_includedirs("include")
    add_includedirs("imgui-sdl3/include")
    add_cxxflags("-Wall", "-Wextra", "-pedantic", "-g", "-march=native", {force = true})

    set_kind("binary")
    if is_plat("windows") then
        add_syslinks("SDL3", "SDL3_image", "gdi32", "winmm", "imm32", "version", "ole32", "oleaut32", "setupapi", "uuid", "shell32")
    elseif is_plat("linux") then
        add_syslinks("SDL3", "SDL3_image", "dl", "pthread", "m")
    elseif is_plat("macosx") then
        add_syslinks("SDL3", "SDL3_image", "Cocoa", "IOKit", "CoreVideo")
    end

    set_toolchains("gcc")
    set_targetdir("bin")
    set_filename("oop-lab-project-debug")

target("release")
    add_deps("imgui-sdl3-staticlib")
    add_packages("sdl3")
    add_links("sdl3")
    add_files("src/*.cpp")
    add_includedirs("include", "imgui-sdl3/include")

    if is_arch("x86_64", "i386") then
        add_cxxflags("-Wall", "-Wextra", "-pedantic", "-Os", "-march=nehalem",{force = true})
    else
        add_cxxflags("-Wall", "-Wextra", "-pedantic", "-Os", "-march=native", {force = true})
    end
    add_ldflags("-static", "-static-libgcc", "-static-libstdc++", "-s", {force = true})

    set_kind("binary")
    if is_plat("windows") then
        add_syslinks("SDL3", "SDL3_image", "gdi32", "winmm", "imm32", "version", "ole32", "oleaut32", "setupapi", "uuid", "shell32")
    elseif is_plat("linux") then
        add_syslinks("SDL3", "SDL3_image", "dl", "pthread", "m")
    elseif is_plat("macosx") then
        add_syslinks("SDL3", "SDL3_image", "Cocoa", "IOKit", "CoreVideo")
    end

    set_toolchains("gcc")
    set_targetdir("bin")
    set_filename("oop-lab-project-release")