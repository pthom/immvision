* [ ] Post reddit r/place ?


* [ ] Deploy
  * [ ] Add more warnings
  * [ ] All in one version (minimalist header / warning make opengl possibly external)
    * [ ] AutoDetect Opencv (or Eigen) presence (via OPENCV_VERSION ?)
  * [ ] build script 
    * [ ] Separate Emscripten in build script
    * [ ] Review args input (from command line also ?)
  * [ ] Bug in hello_imgui: first cmake (empty dir) fails during configure
  * [ ] hello_imgui : Check deploy assets Windows release & Debug build
  * [ ] Work on immvision_gl_loader
  * [ ] Compat Eigen ?
  * [ ] doc


* [ ] Inspector:
  * [ ] Find nice name (close to ImmDebug ? flag global sur ImDebug qui appelle inspector ?)
  * [?] Separate File
  * [N] add Inspector_Run() ? (requires runner in main lib...) 
  * [X] draw thumbnail in list
  * [ ] load images
  * [ ] Remove images
  * [ ] List / Different views : list, thumbs, thumbs size, etc
  * [ ] Save inspector setup (options and window size ?)
  * [ ] Show dates
  * [ ] Test themes


* [ ] Viewer
  * [ ] Remove Window Title


* [ ] Image Debugger
  * [ ] Tester resistance a fichier corrompu (doit le supprimer !) 
  * [ ] LaunchViewer: use https://github.com/sheredom/subprocess.h
  * [ ] Drag Watched Pixels & reorder ? + clear ?
  * [ ] Voir https://github.com/OpenImageDebugger/OpenImageDebugger
  * [ ] Much later : make usable on iPhone & Android (idea / server: https://crowcpp.org/) 

* [ ] Demo
  * [ ] Demo with HelloImgui
  * [ ] Test iPhone

* [ ] ImageNavigator
  * [X] Wheel zoom : eat event (i.e avoid scroll)
  * [N] Copy image to clipboard => voir https://github.com/dacap/clip / Non: rend compilation complexe !
  * [ ] Add param zoomCenter & zoomRatio
