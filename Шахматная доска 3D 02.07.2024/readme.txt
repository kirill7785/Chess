clang++ -Ofast -c chess.cpp -o library.o
clang++ -Ofast -shared -o chess.dll library.o
clang++ -Ofast  -c chess_gui.exe library.o
clang++ -m32  chess_gui.cpp -o chess_gui.exe
clang++ -m32  chess_3D.cpp -o chess_gui.exe
clang++ -m32 -fopenmp chess_3D.cpp -o chess_gui.exe

C:/Android/ndk/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++ --target=armv7a-linux-androideabi21 -Ofast --sysroot=C:/Android/ndk/toolchains/llvm/prebuilt/windows-x86_64/sysroot -c chess.cpp -o library.o

// Команды компиляции под Андройд 32 bit
C:/Android/ndk/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++ --target=armv7a-linux-androideabi21 -Ofast --sysroot=C:/Android/ndk/toolchains/llvm/prebuilt/windows-x86_64/sysroot -fPIC -c chess.cpp -o library.o
C:/Android/ndk/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++ --target=armv7a-linux-androideabi21 -Ofast --sysroot=C:/Android/ndk/toolchains/llvm/prebuilt/windows-x86_64/sysroot -shared -o chess.dll library.o

// Команды компиляции под Андройд 64 bit
C:/Android/ndk/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++ --target=aarch64-linux-android21 -Ofast --sysroot=C:/Android/ndk/toolchains/llvm/prebuilt/windows-x86_64/sysroot -fPIC -c chess.cpp -o library.o
C:/Android/ndk/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++ --target=aarch64-linux-android21 -Ofast --sysroot=C:/Android/ndk/toolchains/llvm/prebuilt/windows-x86_64/sysroot -fPIC -shared -o chess.dll library.o

C:\Android\ndk
$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ --target=armv7a-linux-androideabi21 -Ofast --sysroot=$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/sysroot -c chess.cpp -o library.o