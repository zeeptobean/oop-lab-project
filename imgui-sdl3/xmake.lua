set_project("imgui-sdl3")
set_languages("c++17")

target("imgui-sdl3-staticlib")
    add_files("src/*.cpp")
    add_includedirs("include")
    add_cxxflags("-Wall", "-Os", {force = true})

    set_toolchains("gcc")
    set_kind("static")
    set_filename("libimgui-sdl3.a")
    set_targetdir("lib")

    

