* [ ] Post reddit r/place ?

* [ ] Deploy
  * [ ] Solidify emscripten build 
    * [ ] Separate in build script
  * [ ] hello_imgui : Check deploy assets Windows release & Debug build
  * [ ] Work on immvision_gl_loader
  * [ ] Add more warnings
  * [X] imgui powersave 
  * [ ] All in one version (minimalist header / warning make opengl possibly external)
  * [ ] Compat Eigen ?
  * [ ] AutoDetect Opencv (or Eigen) presence (via OPENCV_VERSION ?)
  * [ ] doc
  * [ ] script / hello_imgui
    * [ ] commit hello_imgui modifs (and check windows debug/release...)

* [ ] Inspector:
  * [ ] Find nice name (close to ImmDebug)
  * [?] Separate File
  * [N] add Inspector_Run() ? (requires runner in main lib...) 
  * [X] draw thumbnail in list
  * [ ] load images
  * [ ] Remove images
  * [ ] Different views : list, thumbs, etc
  * [ ] Show dates

* [ ] Viewer
  * [ ] Remove Window Title

* [ ] Image Debugger
  * [ ] Tester resistance a fichier corrompu (doit le supprimer !) 
  * [ ] LaunchViewer: use https://github.com/sheredom/subprocess.h
  * [ ] Drag Watched Pixels & reorder ? + clear ?
  * [ ] Voir https://github.com/OpenImageDebugger/OpenImageDebugger
  * [ ] Test themes
  * [ ] Make embeddable into other app => flag global sur ImDebug qui appelle inspector ?
  * [ ] Much later : make usable on iPhone & Android (idea / server: https://crowcpp.org/) 

* [ ] Demo
  * [ ] Demo with HelloImgui
  * [ ] Test iPhone

* [ ] ImageNavigator
  * [X] Wheel zoom : eat event (i.e avoid scroll)
  * [N] Copy image to clipboard => voir https://github.com/dacap/clip / Non: rend compilation complexe !
  * [ ] Add param zoomCenter & zoomRatio
