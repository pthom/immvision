from enum import Enum
from .any_backend import AnyBackend
from .imgui_app_params import _ImguiAppParamsHelper


class BackendType(Enum):
    SDL = 1
    GLFW = 2


def factor_backend(backend_type: BackendType, imgui_app_params_helper: _ImguiAppParamsHelper) -> AnyBackend:
    if backend_type == BackendType.SDL:
        from .sdl_backend import SdlBackend         # this import shall not be done at top level
        return SdlBackend(imgui_app_params_helper)  # we want to run even if sdl is not installed, but glfw is
    elif backend_type == BackendType.GLFW:
        from .glfw_backend import GlfwBackend       # this import shall not be done at top level
        return GlfwBackend(imgui_app_params_helper)
    else:
        raise RuntimeError("factor_backend: unknown backend")
