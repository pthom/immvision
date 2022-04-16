* [ ] imgui_datascience: use sdl2?

* [ ] Deploy
  * [ ] All in one version
    * [ ] Run script at cmake time instead of build? Via build_script ? Add pre-commit ?
    * [X] All in one immvision
    * [ ] All in one immdebug
      * [ ] functions and methods inline or macro #IMMDEBUG_IMPL (test avec plusieurs fichiers qui l'incluent)
  * [ ] Github release (search actions artifacts to release : https://github.blog/changelog/2020-07-06-github-actions-manual-triggers-with-workflow_dispatch/)
  * [x] Stats traffic longer than 14 days: https://github.com/jgehrcke/github-repo-stats
  * [N] Static link linux. Dead end
    use ldd to see links. Read https://insanecoding.blogspot.com/2012/07/creating-portable-linux-binaries.html and https://www.systutorials.com/how-to-statically-link-c-and-c-programs-on-linux-with-gcc/
  * [N] Replace vcpkg.tgz by artifacts: see https://github.com/marketplace/actions/upload-a-build-artifact
    Parser https://api.github.com/repos/pthom/immvision/actions/artifacts

* [ ] Python module
  * [ ] use power_save
  * [ ] les icones sont moches
  * [ ] Provide imgui_runner in the package
  * [X] rename native module to cpp_immvision
  * [ ] classify code & add missing signatures
  * [ ] CI
    * [ ] test fresh install sous docker
  * [ ] publish opencv_converter: add unit tests
  * [N?] pyimgui / version official

* [ ] immvision
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
  * [ ] Add param zoomCenter & zoomRatio


    ImGuiWindow* outer_window = GetCurrentWindow();
    const ImVec2 avail_size = GetContentRegionAvail();
    ImVec2 actual_outer_size = CalcItemSize(outer_size, ImMax(avail_size.x, 1.0f), use_child_window ? ImMax(avail_size.y, 1.0f) : 0.0f);
    ImRect outer_rect(outer_window->DC.CursorPos, outer_window->DC.CursorPos + actual_outer_size);

    ...
    // For non-scrolling tables, WorkRect == OuterRect == InnerRect.
    // But at this point we do NOT have a correct value for .Max.y (unless a height has been explicitly passed in). It will only be updated in EndTable().
    table->WorkRect = table->OuterRect = table->InnerRect = outer_rect;
