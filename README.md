#Rock Text 2 - make awesome Rock Text using this Gimp Plugin (and image file overlay textures)


![image preview](rock_text_2.png  )
![image preview](rock_text_22.png  )


This filter will only work proper on  Gimp 2.10.32 and up and if you are not compiling from source (by using binaries) you can ignore the task below. If you are compiling you must also compile **Custom Bevel** and **Long Shadow PD**. Make sure to grab all three binaries and overwrite the ones you already have.  Custom bevel and Long Shadow pd are quality plugins of their own. It is impossible to use Rock Text 2 without them.


**REQUIREMENTS FOR COMPILERS**

[https://github.com/LinuxBeaver/GEGL-Custom-Bevel](https://github.com/LinuxBeaver/GEGL-Custom-Bevel/releases)

and

[https://github.com/LinuxBeaver/GEGL-Extrusion-2----Fork-of-GEGL-Long-Shadow/ ](https://github.com/LinuxBeaver/GEGL-Extrusion-2----Fork-of-GEGL-Long-Shadow/releases) to work. 



## OS specific location to put GEGL Filter binaries 

Windows
 C:\\Users\<YOUR NAME>\AppData\Local\gegl-0.4\plug-ins
 
 Linux 
 /home/(USERNAME)/.local/share/gegl-0.4/plug-ins
 
 Linux (Flatpak)
 /home/(USERNAME)/.var/app/org.gimp.GIMP/data/gegl-0.4/plug-ins


## Compiling and Installing

### Linux

To compile and install you will need the GEGL header files (`libgegl-dev` on
Debian based distributions or `gegl` on Arch Linux) and meson (`meson` on
most distributions).

```bash
meson setup --buildtype=release build
ninja -C build

```
This is the first filter of mine that will NOT work on GEGL 0.3 - You need to have a recent GEGL 0.4 or figure out how to get DCT-Noise on GEGL 0.3




### Windows

The easiest way to compile this project on Windows is by using msys2.  Download
and install it from here: https://www.msys2.org/

Open a msys2 terminal with `C:\msys64\mingw64.exe`.  Run the following to
install required build dependencies:

```bash
pacman --noconfirm -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-meson mingw-w64-x86_64-gegl
```

Then build the same way you would on Linux:

```bash
meson setup --buildtype=release build
ninja -C build
```

# More previews of this plugin just to show off how based it is.

![image](https://github.com/LinuxBeaver/GEGL-STONE-TEXT-2/assets/78667207/12821b5c-bf77-408c-80bd-1e38a1b89634)

![image](https://github.com/LinuxBeaver/GEGL-STONE-TEXT-2/assets/78667207/4939ed5c-25e4-4515-bcde-5f85f0225133)

![image](https://github.com/LinuxBeaver/GEGL-STONE-TEXT-2/assets/78667207/037cbd95-e0ff-403e-b7b6-287aa7defc23)




