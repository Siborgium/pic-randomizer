import os
import platform
print("Please, specify a compiler (i.e. (without brackets) [C:/LLVM/bin/clang.exe] or [g++], if you have it in your PATH)")
compiler = input()
if platform.system() == "Windows":
	ext = ".exe"
if not os.path.exists("/build/"):
	os.system("mkdir build")
os.system(compiler + " src/pic-randomizer-source.cpp -o build/pic-rnd" + ext + " -O3 -std=c++17 -stdlib=libc++")
os.system("pause")
exit(0)