* [ ] PIP
  * [ ] imgui_fig (from imgui_datascience)
  * [ ] imgui_runner
      * [ ] change mat type if not castable (and cache)
  * [ ] Voir moderngl https://pythonawesome.com/a-python-wrapper-over-opengl-3-3-core-that-simplifies-the-creation-of-simple-graphics/
  * [Later] cf _skbuild_template: add conda & wheels
  * [Later] pyimgui
      * [ ] PR / Pull request / multisamples = 4
      * [ ] PR / Propose imgui_runner (power_save, change window size, etc)
      * [Later] imgui & pyimgui : utiliser branche docking sans powersave / ou bien branche master & official?
  * Read https://hynek.me/articles/sharing-your-labor-of-love-pypi-quick-and-dirty/
    https://setuptools.pypa.io/en/latest/index.html


 * [ ] immvision
     * [ ] ImageOld and Image are the same ? Group them ?
     * [ ] Only one signature for Image (using struct designated init as named params)
     * [ ] Navigator returns multiple values (mouse position (double), pixel color (vecdouble), ...)
           Or better, return pointer to params (helps users that did not specify them as a param, they can get it anyway)
     * [ ] Pb transcription couleurs sur mandelbrot (cf https://www.learnpythonwithrune.org/numpy-compute-mandelbrot-set-by-vectorization/) 
     *     colormaps https://matplotlib.org/3.1.0/tutorials/colors/colormaps.html#palettable
     * [ ] Compat Eigen ?
     * [ ] doc
  
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


* [ ] Viewer
  * [ ] Test themes
  * [ ] Show dates


* [ ] ImageOld Debugger
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