from enum import Enum
from .backend_any import BackendAny


class BackendType(Enum):
    SDL = 1
    GLFW = 2


def factor_backend(backend_type: BackendType) -> BackendAny:
    if backend_type == BackendType.SDL:
        from .backend_sdl import BackendSdl         # this import shall not be done at top level
        return BackendSdl()                         # we want to run even if sdl is not installed, but glfw is
    elif backend_type == BackendType.GLFW:
        from .backend_glfw import BackendGlfw       # this import shall not be done at top level
        return BackendGlfw()
    else:
        raise RuntimeError("factor_backend: unknown backend")
