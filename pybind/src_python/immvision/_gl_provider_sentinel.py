from . import _cpp_immvision


def immvision_create_gl_sentinel():
    _cpp_immvision.InitGlProvider()


def immvision_destroy_gl_sentinel():
    _cpp_immvision.ResetGlProvider()
