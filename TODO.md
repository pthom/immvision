* [ ] Deploy
  * [X] Add more warnings: 
    * [ ] Add warnings for linux
  * [ ] Github actions / build
    * [ ] Github actions / build and deploy viewer
  * [ ] All in one version (minimalist header / warning make opengl possibly external)
    * [ ] AutoDetect Opencv (or Eigen) presence (via OPENCV_VERSION ?)
  * [ ] build script 
    * [ ] Change options
  * [ ] Bug in hello_imgui: first cmake (empty dir) fails during configure
  * [ ] hello_imgui : Check deploy assets Windows release & Debug build
  * [ ] Work on immvision_gl_loader
  * [ ] Compat Eigen ?
  * [ ] doc


* [ ] Inspector:
  * [ ] Find nice name (close to ImmDebug ? flag global sur ImDebug qui appelle inspector ?)
  * [ ] Handle params zoomCenter, zoomRatio
  * [ ] load images
  * [ ] List Image
    * [ ] Different views : list, thumbs, thumbs size, etc
    * [ ] Remove one image / Remove all
  * [ ] Save inspector setup (options and window size ?)


* [ ] Viewer
  * [ ] Test themes
  * [ ] Show dates


* [ ] Image Debugger
  * [ ] Tester resistance a fichier corrompu (doit le supprimer !) 
  * [ ] LaunchViewer: 
    * [ ] use https://github.com/sheredom/subprocess.h
    * [ ] find exe in path
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
