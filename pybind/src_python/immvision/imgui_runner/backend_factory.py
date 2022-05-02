from enum import Enum
from .any_backend import AnyBackend
from .sdl_backend import SdlBackend
from .glfw_backend import GlfwBackend
from .imgui_app_params import _ImguiAppParamsHelper


class BackendType(Enum):
    SDL = 1
    GLFW = 2


def factor_backend(backend_type: BackendType, imgui_app_params_helper: _ImguiAppParamsHelper) -> AnyBackend:
    if backend_type == BackendType.SDL:
        return SdlBackend(imgui_app_params_helper)
    elif backend_type == BackendType.GLFW:
        return GlfwBackend(imgui_app_params_helper)
    else:
        raise RuntimeError("factor_backend: unknown backend")
