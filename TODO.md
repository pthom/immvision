* [ ] Handle (& test) CV_64FC3 & 4
* [ ] simplify gl_texture
* [X] imgui powersave 
  * [ ] cleanup & doc

  * [ ] ImageNavigator
    * [ ] Layout Zoom buttons & adjust (esp if small width)
    * [ ] Text margin when big (?)
    * [ ] Handle zoom_key & adjustments_key
      * [ ] Pass ImageNavigatorParams by pointer (do not lie about lifetime)
        * [ ] Add simpler API
    * [X] Drag stops when exiting boundaries
    * [ ] Mise en page button Save (& others) / Use menus & dialogs
    * [ ] Other feature: 
      * [ ] Show channels / Grid / Display zoom level / Show alpha(?) / Copy to clipboard
      * [ ] Pixel watch  The ability to click on one or more pixels of an image and have those values appear in a list
      * [ ] Swizzling (exchange channels) / Some filters ?
    * [ ] Way to handle nice icons
    * [ ] Clean imgui_ext mess
    * [ ] Display FPS
    * [ ] Additional simpler API?
    * [ ] ClearNotUsedAnymoreTextures
    * [ ] Naming Draw_DisableDragWindow
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


