# Raspberry Pi Pico (ArduCAM Pico4ML) Realtime MNIST classifier.

![Demo](https://raw.githubusercontent.com/britannio/pico-mnist/main/docs/demo1.png)


## Setup

The easiest way to test this is via the Docker devcontainer.

1. Install [Docker](https://www.docker.com/get-started/).
I highly recommend [Orbstack](https://orbstack.dev/) for MacOS users as it 
currently uses fewer resources.
2. Make sure Docker/Orbstack is running
3. Install [VSCode](https://code.visualstudio.com/Download).
4. Install [Dev Containers Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)
5. `git clone https://github.com/britannio/pico-mnist && code pico-mnist/`

After following the above instructions open your project in Visual Studio Code and run the `>Dev Containers: Reopen in Container` command. If the instructions are followed correctly Visual Studio Code should also automatically suggest opening the repository in container mode when the project is loaded.


## Notes to self

`PICO_SDK_PATH` should be set as an environment variable. It should point to the root of the repo https://github.com/raspberrypi/pico-sdk.git once cloned.

`chmod u+x compile.sh` to make `compile.sh` executable.

`./compile.sh` to produce `pico_vision.uf2`

`CLANG=1 python3 tinygrad_compile_mnist.py` to train the model and convert it to C.
