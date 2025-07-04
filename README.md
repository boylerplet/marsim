# MARSIM - A stock market simulator

This is a tool to render / visualize stock market chart using the OLHC of a symbol



https://github.com/user-attachments/assets/25dc2632-51fb-46e6-9d6e-9df130ad0c2f



# Build and installation

The installation instructions for this project is solely restricted to `Linux` environments.

## Prerequisites

- Download the latest release / v5.5 of [RayLib](https://github.com/raysan5/raylib) at `./raylib/`.
- A C compiler

## Installation

Build `nob.c` using a C compiler

```console
gcc -o nob nob.c
```

Following that, build the `main.c` using the [nob](https://github.com/tsoding/nob.h) executable

```console
./nob
```

Finally, run the `main` executable

```console
./main
```

---

## Note

This project is heavily dpendent on the [nob](https://github.com/tsoding/nob.h) build tool. If you encounter problems in the above instructions, you have to update the `nob.c` and change the include path and library path (`I./<<PATH TO RAYLIB.H FILE>>` and `-L./<<PATH TO LIBRAYLIB.SO>>`) respectively. 
Also if you are using a C complier apart from [GCC](https://gcc.gnu.org), you have to update it in the same file in the `cc` function.

---

# Configurations

There are some options that you can customize to change the view of the tool.

## Color

You can define colors / schemes in the `marsim.h` header file.

## Chart config

[TODO]

# Build Phase

This project is not complete at the moment. The following are the planned features for future releases

- Scale on X-Axis
- Highlight last candle price on the Y-Axis
- Project cursor position on the axes
- Realtime graph update for the latest candlestick.
- Auto panning
- Improve the Data generation logic and build system
- Add more color schemes
- Have a Sidebar with Symbol Details
- More time frames
- More Candle types

---
