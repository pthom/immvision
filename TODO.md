# imgui_bundle

Build
````
mkdir build
cd build
conan install .. -build=missing
cd ..

cd external
git clone https://github.com/pthom/imgui_bundle.git
cd imgui_bundle
git submodule update --init --recursive
````























---------

# Mandelbrot: 
Read optim & background https://theses.liacs.nl/pdf/2018-2019-JonckheereLSde.pdf / code: https://github.com/lucmans/fraccert



 * [ ] immvision
     * Add IMM_ASSERT (default = exception in release build)
     * [ ] Add coord image + custom coords
     * [Later] ToString -> ToJson / FromJson
     * [Later] Compat Eigen ?
     * [ ] doc
     * [Later] logs with spdlog
     * move fplus to internal/misc
  
 * [ ] Inspector:
   * [ ] Find nice name (close to ImmDebug ? flag global sur ImDebug qui appelle inspector ?)
   *     Build exe via pip
   * [ ] Handle params zoomCenter, zoomRatio
   * [ ] load images
   * [ ] List ImageOld
     * [ ] Different views : list, thumbs, thumbs size, etc
     * [ ] Remove one image / Remove all
   * [ ] Save inspector setup (options and window size ?)


* [ ] imgui_datascience: use sdl2?
* [ ] Deploy
    * [ ] All in one version
        * [ ] Run script at cmake time instead of build? Via build_script ? Add pre-commit ?
        * [ ] All in one immdebug
            * [ ] functions and methods inline or macro #IMMDEBUG_IMPL (test avec plusieurs fichiers qui l'incluent)
    * [ ] Github release (search actions artifacts to release : https://github.blog/changelog/2020-07-06-github-actions-manual-triggers-with-workflow_dispatch/)
    * [broken] Stats traffic longer than 14 days: https://github.com/jgehrcke/github-repo-stats
    * publish rant LowerString


* [ ] Viewer
  * [ ] Test themes
  * [ ] Show dates


* [ ] Image Debugger
  * [ ] Compat Eigen ?
  * [ ] AutoDetect Opencv (or Eigen) presence (via OPENCV_VERSION ?)
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

* [ ] Image
  * [X] Wheel zoom : eat event (i.e avoid scroll)
  * [N] Copy image to clipboard => voir https://github.com/dacap/clip / Non: rend compilation complexe !

* [?] Plugin python / lldb and gdb 


* [ ] Videos Docs
  * [ ] Exemple mandelbrot infinite zoom python
  * [ ] Exemple matplotlib