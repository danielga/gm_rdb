# gm_lrdb

[![Build Status](https://metamann.visualstudio.com/GitHub%20danielga/_apis/build/status/danielga.gm_lrdb?branchName=master)](https://metamann.visualstudio.com/GitHub%20danielga/_build/latest?definitionId=16&branchName=master)

A Garry's Mod module that creates a Lua Remote DeBugger server.

## Compiling

The only supported compilation platform for this project on Windows is **Visual Studio 2017**. However, it's possible it'll work with *Visual Studio 2015* and *Visual Studio 2019* because of the unified runtime.

On Linux, everything should work fine as is.

For macOS, any **Xcode (using the GCC compiler)** version *MIGHT* work as long as the **Mac OSX 10.7 SDK** is used.

These restrictions are not random; they exist because of ABI compatibility reasons.

If stuff starts erroring or fails to work, be sure to check the correct line endings (`\n` and such) are present in the files for each OS.

## Requirements

This project requires [garrysmod\_common][1], a framework to facilitate the creation of compilations files (Visual Studio, make, XCode, etc). Simply set the environment variable `GARRYSMOD_COMMON` or the premake option `--gmcommon=path` to the path of your local copy of [garrysmod\_common][1].

  [1]: https://github.com/danielga/garrysmod_common
