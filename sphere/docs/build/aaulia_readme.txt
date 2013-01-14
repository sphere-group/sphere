This is the sphere source code (from the CVS) that I modified to be more friendly and 
compileable. read the changelog.txt for more information.

What you need to have (and do) to compile this package:

1. Microsoft Visual C++ Express 2005
2. Platform SDK (latest would be better, I use MS Platform SDK For Windows 2003 R2)
3. Add <Path to your Platform SDK>\include\mfc to tools->options->Projects and Solutions->VC++ Directories->Include

how you compile it ??? open Sphere.sln, and hit F7 :D

Hope this could be usefull for everyone intersted in developing sphere.

PS: 
21-08-06 - Building complete (runable) sphere package from this source on windows

1. The package will be in './Sphere' directory of the source tree.
2. Build solution in release mode, ALL the binary will be outputed to './Sphere' and ALL the video driver will be
   outputed to './Sphere/system/video/'
3. Copy directory './Startup' to './Sphere' (overwrite if already exist).
4. Copy directory './system' to './Sphere' (overwrite if already exist, but makesure you do not overwrite the video
   sub-directory).
5. Copy the './docs' to './Sphere'

thats it you get the complete sphere package (minus the editor).