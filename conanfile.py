from conans import ConanFile
from conans import CMake


class GameOfLife(ConanFile):
    name = "GameOfLife"
    version = "0.1.0"
    license = "MIT"
    settings = "os", "arch", "compiler", "build_type"
    generators = "cmake"
    exports = "*"
    options = {"shared": [True, False]}
    requires = "doctest/2.3.4@bincrafters/stable", "spdlog/1.5.0", "sdl2/2.0.12@bincrafters/stable", "imgui/1.80"
    default_options = "shared=False"

    def build(self):
        shared = {"BUILD_SHARED_LIBS": self.options.shared}
        cmake = CMake(self)
        cmake.configure(defs=shared)
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include")
        self.copy("*.lib", dst="lib", src="lib", keep_path=False)
        self.copy("*.dll", dst="bin", src="bin", keep_path=False)
        self.copy("*.dylib", dst="bin", src="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def imports(self):
        self.copy("imgui_impl_sdl.h", dst="../bindings", src="./res/bindings")
        self.copy("imgui_impl_sdl.cpp", dst="../bindings", src="./res/bindings")
        self.copy("imgui_impl_opengl3.h", dst="../bindings", src="./res/bindings")
        self.copy("imgui_impl_opengl3.cpp", dst="../bindings", src="./res/bindings")

    def package_info(self):
        self.cpp_info.libs = ["GameOfLife"]