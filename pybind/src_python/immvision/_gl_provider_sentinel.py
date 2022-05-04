from . import _cpp_immvision


def create_sentinel():
    _cpp_immvision.InitGlProvider()


def destroy_sentinel():
    _cpp_immvision.ResetGlProvider()
