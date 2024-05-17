# 4coder_byp
My personal 4coder custom layer

`cd \path\to\4ed.exe`\
`git clone https://github.com/B-Y-P/4coder_byp.git`\
`cd 4coder_byp`\
`git submodule init`\
`git submodule update`\
`build release`

</br>

### Common issue for Windows:

`'cl' is not recognized as an internal or external command, operable program or batch file.`

#### If you already have Visual Studio installed and just want to build
 - Search for `x64 Native Tools Command Prompt for VS <YYYY>` (make sure it's `x64` not `x86_64`)\
 - Open and navigate to `4coder_byp\` and run `build release`

</br>

#### If you're looking to streamline building projects on Windows in this manner
 - In `Program Files\Microsoft Visual Studio\<YYYY>\Community\VC\Auxiliary\Build`\
 - Running `vcvarsall.bat amd64` sets up your environment allowing it to run `cl.exe`

</br>

#### An example for how to cache this is given [here](https://gist.github.com/mmozeiko/37f511bac135337fc273f4bda0ebca1c) for VS 2019

</br>

#### If you don't have Visual Studio installed and don't want to download 4 GB of... stuff
 - [Download MSVC without installing full Visual Studio](https://gist.github.com/mmozeiko/7f3162ec2988e81e56d5c4e22cde9977) creates a `setup.bat` to call before running `cl.exe`
 - [PortableBuildTools](https://github.com/Data-Oriented-House/PortableBuildTools) provides a simple GUI to do the same

</br>

If all goes well, running `4ed.exe` will look something like this
---
![4coder_byp example](https://github.com/B-Y-P/4coder_byp/assets/69936251/e50bfefa-997d-401b-8b40-a77c22feb5b9)
