# funCaller
You can use this code to perform a context switch when invoking an interface. This ensures that neither the caller nor the callee is aware of the thread context change.

# Linux or WSL
`sudo apt install ninja-build clang cmake gdb python3-pip`

`pip install conan --break-system-packages`

`.devcontainer/run-conan.sh`

`cmake --preset conan-debug`

`cmake --build --preset conan-debug`

`./build/Debug/funcall`

# devcontainer

Install docker

In vs code, select `Reopen in container` and run

