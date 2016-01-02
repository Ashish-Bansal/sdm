SDM - Download Manager
======================

SDM is a multi-platform download manager written in C++ and Qt framework..

####Features

```
* Automatically takes download from web browser (requires web-browser extension)
* Uses multithreading and multipart downloading to speed up the download
* Supports downloading video from most of the websites (requires web-browser extension)
* Runs in background
```
####Installation

Currently It's in development stage and is not available as packed binary. Although if
you still want to use it, you can build it very easily.

**Dependencies**

```
1) Qt5
2) CMake
```

**Build**

```
git clone https://github.com/Ashish-Bansal/sdm
cd sdm
mkdir build
cd build
cmake ../
make -j4
```

**Usage**

Once it builds successfully, an executable will be created in `build` directory. You can just launch it to open up the Download Manager.
Temporary files are created in the build folder itself and when the download finishes, files are merged and moved to `$HOME/sdm`. Metadata about the downloads is stored in the sqlite database in the `build` directory.


**Note**

Transfering download requires installing extension in the web browser.
