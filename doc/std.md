# Coding Standard

Practical rules to follow.

## Formatter

Use **clang-format** as the default formatter, along with **Google C++ Style**.

```bash
clang-format -style=llvm -dump-config > .clang-format
```

With **Clangd** extension in VSCode, this can be configured with the editor.

## Environment

It's been tested that both Linux, MacOS, MSVC are able to build the project. There are some issues with Clangd when using MSVC though.

For MacOS, the compiler is clang.

```bash
zhou ~/projects/anorency $ g++ --version
Apple clang version 17.0.0 (clang-1700.3.19.1)
Target: arm64-apple-darwin25.0.0
Thread model: posix
InstalledDir: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin
```

For MSVC, the compiler is cl from Visual Studio 2022 Community.

## C++ Standard

- C++ 23 **BUT** most of the features are from C++ 20.
- **Enable** exception, unlike Google.
- **Disable** template meta programming.
- **No** seperation of implementation for template class/method.
- Upper-case comel for everything **except** type alias.
- Snake for type alias.