import os 
import platform
import shutil
import argparse



if __name__ == "__main__":
    parse = argparse.ArgumentParser()
    parse.add_argument("--profile",required=False)
    var = parse.parse_args()
    profile = "" if var.profile == None else "--profile {}".format(var.profile)
    os.system("conan build . --output-folder=build/Debug -s build_type=Debug --build=missing {} {}".
              format(profile, "-s os={}".format("Windows" if platform.system() == "Windows" else "linux")))
    os.system("conan build . --output-folder=build/Release -s build_type=Release --build=missing {} {}".
              format(profile, "-s os={}".format("Windows" if platform.system() == "Windows" else "linux")))
    shutil.copy("./build/Debug/compile_commands.json","./build/compile_commands.json")
    pass