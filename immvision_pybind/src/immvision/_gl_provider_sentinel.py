"""
This file acts as a sentinel that will call InitGlProvider() at startup
and ResetGlProvider() at exit (so that the texture caches are cleared)

Simply import it in of your python  files, the global variable _GL_PROVIDER_SENTINEL
will handle everything at its creation/destruction.
"""

import cv2
from .cpp_immvision import InitGlProvider, ResetGlProvider

class GlProviderSentinel(object):
    def __init__(self):
        print("GlProviderSentinel __init__: calling immvision.InitGlProvider()")
        InitGlProvider()

    def __del__(self):
        print("GlProviderSentinel __del__: calling immvision.ResetGlProvider()")
        ResetGlProvider()

_GL_PROVIDER_SENTINEL = GlProviderSentinel()
