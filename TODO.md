* [X] Recup code blit Activisu + imgui_datascience
* [X] Solve issue / mouse precision
* [X] Track init/delete/blits
* [X] Variant API / Still & animated
* [N] Variant API Fixed height, width. + doc
* [X] Skeleton doc
* [ ] Handle (& test) CV_64FC3 & 4
* [ ] simplify gl_texture
* [ ] Reduce energy branch
* [ ] ImageNavigator
  * [X] Main API
  * [X] Icon Zoom (-)
  * [X] Disable Window drag:
     * [ ] Naming Draw_DisableDragWindow
  * [X] On semble aller plus loin que le bas(!)
  * [X] Line info pixel bouge en hauteur / Ajouter type matrix (CV_8UC3 ...)
  * [X] Mettre calcul de mouseLocation_originalImage dans ZoomUtils
  * [X] colorAdjustmentsCopy and others not needed anymore
  * [X] Track Blit and creations
  * [X] Adjust widget do not appear
  * [X] Check if MakeUniqueLabel needed
    * [X] Check handle several images
  * [X] Align PixelInfo CV8U_C1 & display value
  * [ ] Mise en page button Save (& others)
  * [ ] Child window with borders
  * [ ] Way to handle nice icons
  * [ ] Clean imgui_ext mess
  * [ ] Display FPS
  * [ ] Additional simpler API?
  * [ ] Handle zoom_key & adjustments_key
  * [ ] ClearNotUsedAnymoreTextures
  * [ ] LinkedLabel is dangerous ??? (reuse same char * pointer)

      Resumé du problème:
          - ImageTexture ne peut pas être construit avant init OpenGL

          - Si on a un pointeur de Mat en interne, on est lié a sa durée de vie
            (et il faut donc garder une copie de la Mat + ImageTextureCv dans l'AppState)

          - Si on a une cv::Mat shallow copy en interne, on risque que cette copie soit désynchronisée
          (par exemple si on change la Mat dans l'AppState par operator=)

          - Si on n'a pas de stockage de Mat en interne, on est obligé de faire un blit à la construction (?)
            mais ca n'est possible que si OpenGL est initialisé.
            Si OpenGL pas initialisé, on ne pourra remplir l'image qu'apres.

          - Solution possible: init AppState en PostInit OpenGL ?

          - Autre solution: global map<cv::Mat*, ImageTextureCv> gCache; ???
              class TextureCache
              {
                  std::map<const cv::Mat *, GLuint> mTexturesMap;
                  std::map<const cv::Mat *, bool> mIsTextureValid;
              }
              TextureCache gTextureCache;
              ? + Fonction ImageCv(const cv::Mat &image, bool refreshAtEachFrame = true, const cv::Size& _size = cv::Size(0, 0));

              namespace ImageMat
              {
                  ImageFixed(...)
                  ImageAnimated(...)
                  Image(...)
                  // Clean old images in cache at each call
                  ImageCv_MousePosition_Original
                  ImageCv_ToTexture
              }

          - un mix raisonnable des deux ? (en enlevant cv::Mat de imTextureCv)


