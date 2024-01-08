#fmt/10.2.1
#gtest/1.14.0

from conan import ConanFile
from conan.tools.cmake import CMake,CMakeToolchain,CMakeDeps

class SimFile(ConanFile):
    requires= "fmt/10.2.1","gtest/1.14.0"
    settings = "build_type"
    def imports(self):
        
        pass
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        pass

    def install(self):

        pass

    def generate(self):
        tc= CMakeToolchain(self, generator="Ninja")
        dep = CMakeDeps(self)
        dep.generate()
        tc.generate()

