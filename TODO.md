* [X] Recup code blit Activisu + imgui_datascience
* [X] Solve issue / mouse precision
* [X] Track init/delete/blits
* [ ] Skeleton doc
* [ ] Handle (& test) CV_64FC3 & 4
* [ ] Variant & API / Fixed height, width. + doc
* [ ] Improve Cache (TTL & Co)
* [ ] simplify gl_texture
* [ ] Reduce energy branch

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


