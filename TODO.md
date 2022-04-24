* [ ] imgui_datascience: use sdl2?

* [ ] Deploy
  * [ ] All in one version
    * [ ] Run script at cmake time instead of build? Via build_script ? Add pre-commit ?
    * [ ] All in one immdebug
      * [ ] functions and methods inline or macro #IMMDEBUG_IMPL (test avec plusieurs fichiers qui l'incluent)
  * [ ] Github release (search actions artifacts to release : https://github.blog/changelog/2020-07-06-github-actions-manual-triggers-with-workflow_dispatch/)
  * [broken] Stats traffic longer than 14 days: https://github.com/jgehrcke/github-repo-stats


 * [ ] PIP
   * [ ] classify code
     * [ ] Add missing signatures / cpp
     * [ ] Add nice signatures / python
   * [ ] gerer types de matrices, cf https://numpy.org/doc/stable/reference/arrays.scalars.html#sized-aliases
   * [Later] cf _skbuild_template: add conda & wheels
   * [Later] pyimgui PRs
       * [ ] Pull request / multisamples = 4
       * [ ] Propose imgui_runner
       * [ ] Propose power_save
   * [Later] imgui & pyimgui : utiliser branche docking sans powersave / ou bien branche master & official?
   * Read https://hynek.me/articles/sharing-your-labor-of-love-pypi-quick-and-dirty/
     https://setuptools.pypa.io/en/latest/index.html
   * 


 * [ ] immvision
   * [ ] Pb transcription couleurs sur mandelbrot (cf https://www.learnpythonwithrune.org/numpy-compute-mandelbrot-set-by-vectorization/) 
   *     colormaps https://matplotlib.org/3.1.0/tutorials/colors/colormaps.html#palettable
   * [ ] Compat Eigen ?
   * [ ] Image and ImageNavigator are the same ? Groupe them ?
   * [ ] Navigator returns multiple values (mouse position (double), pixel color (vecdouble), ...)
         Store these values in params? / Store Watched pixels values ?
   * [ ] doc
  
 * [ ] Inspector:
   * [ ] Find nice name (close to ImmDebug ? flag global sur ImDebug qui appelle inspector ?)
   *     Build exe via pip
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

* [ ] ImageNavigator
  * [X] Wheel zoom : eat event (i.e avoid scroll)
  * [N] Copy image to clipboard => voir https://github.com/dacap/clip / Non: rend compilation complexe !

* [?] Plugin python / lldb and gdb 
