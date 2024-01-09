#fmt/10.2.1
#gtest/1.14.0

from conan import ConanFile
from conan.tools.cmake import CMake,CMakeToolchain,CMakeDeps,cmake_layout

class SimFile(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("fmt/10.2.1")
        self.tool_requires("gtest/1.14.0")
        pass

    def build(self):
        cmake = CMake(self)
        cmake.configure({'CMAKE_EXPORT_COMPILE_COMMANDS':'ON'})
        cmake.build()
        pass




