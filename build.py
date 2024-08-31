import os
import shutil
import sys

def build():
    toolchain = ""
    if not check_weapons():
        exit(1)
        pass
    if os.system("vcpkg") > 0:
        os.system("git clone https://github.com/microsoft/vcpkg")
        sys.path.append("./vcpkg")
        toolchain = os.path.abspath(os.curdir) + "/vcpkg/scripts/buildsystems/vcpkg.cmake"
        pass
    else:
        toolchain = shutil.which("vcpkg") + "/scripts/buildsystems/vcpkg.cmake"
        pass
    if not os.path.exists("./build"):
        os.mkdir("./build")
        pass
    os.chdir("./build")
    cxx_compiler = detect_cxx_compiler()
    c_compiler = detect_c_compiler()
    os.system(f"cmake -DCMAKE_TOOLCHAIN_FILE={toolchain} -DCMAKE_CXX_COMPILER={cxx_compiler} -DCMAKE_C_COMPILER={c_compiler} -DCMAKE_MAKE_PROGRAM='ninja' -DCMAKE_BUILD_TYPE=Release -G Ninja  ..")
    os.system("cmake --build .")
    
    pass

def check_weapon(cmd) -> bool:
    return os.system(cmd) == 0
    pass


def check_weapons() -> bool:
    if not check_weapon("cmake"):
        print("Cmake is required to build this project")
        return False
    if not check_weapon("gcc -v") and not check_weapon("clang -v"):
        print("GCC or clang is required to build this project")
        return False
        pass
    return True

def detect_cxx_compiler() -> str | None:
    if check_weapon("g++ -v"):
        return shutil.which("g++")
        pass
    elif check_weapon("clang -v"):
        return shutil.which("clang")
    return None

def detect_c_compiler() -> str| None:
    if check_weapon("gcc -v"):
        return shutil.which("gcc")
        pass
    elif check_weapon("clang -v"):
        return shutil.which("clang")
    return None

def detect_cmake_generator() -> str| None:

    return None


if __name__ == "__main__":
    build()
    pass