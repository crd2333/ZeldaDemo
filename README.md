## 环境
- 环境按照 [github/start-learning-opengl](https://github.com/yocover/start-learning-opengl/tree/main) 搭建，在 windows 下运行，使用 glfw3 + glad
- 文件目录
    ```
    root
    ├── include
    │   ├── imgui
    │   │   └── src
    │   │       ├── imgui.cpp
    │   │       └── ...
    │   ├── GLFW
    │   ├── glad
    │   ├── glm
    │   ├── stb
    │   └── assimp
    ├── lib
    ├── output
    │   ├── glfw3.dll
    │   └── main.exe
    ├── src
    │   ├── main.cpp
    │   └── ...
    ├── Makefile
    ├── ...
    └── README.md
    run 'make' at the root to compile
    run 'make run' at the root to compile and run
    run 'make clean' at the root to clean up
    ```
- lib 文件夹和 output 文件夹被 gitignore，用自己平台的库文件替换
    - 我的 lib 文件夹下有
        ```
        lib
        ├── libassimp.dll.a
        ├── libglad.a
        ├── libglfw3.a
        └── libglfw3dll.a
        ```
    - 我的 output 文件夹下有
        ```
        output
        ├── glfw3.dll
        ├── libassimp-5.dll
        └── main.exe
        ```
    - 还有记得改 `.vscode/c_cpp_properties.json` 的 `includePath` 为你的 MingW 路径

## To Be Continued
