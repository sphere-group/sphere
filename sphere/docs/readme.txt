Sphere v1.4
--------

Chad Austin
Brian Robb
Spherical (http://www.spheredev.org/)
2008.01.08


Sphere is a 2D RPG creation system designed to facilitate creation of games
in the style of Final Fantasy and Phantasy Star.

Read the index.txt files in each directory in docs/ for a description of the
files contained within them.


== Minimum system requirements ==

* Pentium 200 MHz
* 64 MB of RAM
* Video card with 2MB VRAM
* Sound card for audio
* Windows 95, 98, NT4, 2000, or later
  OR
  Linux
* DirectX 8 for fullscreen video drivers
* OpenGL-accelerated video card for hardware acceleration (sphere_gl)


== Features ==

* unlimited map layers
* parallax and automatic layer scrolling
* flexible video driver subsystem
* supports any screen resolution
* 32-bit color and 256 levels of translucency
* 8-directional movement
* animated and reflective tiles
* plays Ogg Vorbis, MP3, FLAC, MOD, S3M, XM, IT, and uncompressed WAV
* graphical editing environment
* keyboard, mouse, and joystick support
* portable, cross-platform game development


== Included video drivers ==

* standard32.dll     - 32-bit color (default)
* standard16.dll     - 16-bit color (faster on 16-bit desktops)
* standard8.dll      - 8-bit color (fast, but ugly)
* greyscale.dll      - 8-bit greyscale (as fast as standard8, but completely in
                       greyscale)
* interpolate32.dll  - same as standard32, but resolution is doubled for some
                       smoothing effects (2xSaI)
* interpolate16.dll  - same as standard16, but resolution is doubled for some
                       smoothing effects (including 2xSaI!  Check it out!)
* sphere_gl.dll      - hardware acceleration using OpenGL (can be VERY fast)
* sphere_dx8.dll     - experimental DirectX 8.0 graphics driver


== Known Issues ==

* sphere_dx8.dll has some major issues. Avoid using it if possible.
* sphere_gl.dll has some problems with drawing and capturing primitives. This
  can cause some graphical glitches. It performs very poorly when games create
  and use many surfaces.
* Visit the Spherical forums for bug reports and feature requests.
  http://www.spheredev.org/smforums/


== Executable listing for Windows ==

* engine.exe    - Sphere engine
* editor.exe    - Sphere editor (a.k.a. the IDE)
* config.exe    - Sphere configuration utility


== Binaries listing for Linux ==

* engine   - Sphere engine
* wxeditor - wxWidgets-based editor (poor quality)

You'll have to edit engine.ini manually under Linux to change settings.
Use your favourite text editor.


== Credits ==

=== Code ===

* Chad Austin (AegisKnight)
* Theo Reed (rizen)
* Brandon Mechtley (malis)
* Jacky Chong (Darklich)
* Brian Robb (Flik)
* rjt
* aaulia/L_O_J
* Tung Nguyen (tunginobi)

=== Startup Game ===

* Tung Nguyen (tunginobi)

Please send all suggestions and bug reports to:
http://www.spheredev.org/smforums/
OR
the Sourceforge bug reports page.
