* [ ] ImGui manual: build release!
* [ ] Deploy
  * [ ] Add warnings / gcc
  * [ ] Github release (search actions artifacts to release : https://github.blog/changelog/2020-07-06-github-actions-manual-triggers-with-workflow_dispatch/)
    * [ ] AutoDetect Opencv (or Eigen) presence (via OPENCV_VERSION ?)
  * [ ] Compat Eigen ?
  * [ ] doc
  * [ ] All in one version
    * [X] All in one immvision
    * [ ] All in one immdebug
  * [x] Stats traffic longer than 14 days: https://github.com/jgehrcke/github-repo-stats
  * [N] Static link linux. Dead end
    use ldd to see links. Read https://insanecoding.blogspot.com/2012/07/creating-portable-linux-binaries.html and https://www.systutorials.com/how-to-statically-link-c-and-c-programs-on-linux-with-gcc/
  * [N] Replace vcpkg.tgz by artifacts: see https://github.com/marketplace/actions/upload-a-build-artifact
    Parser https://api.github.com/repos/pthom/immvision/actions/artifacts
    

  
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
