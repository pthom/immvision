import typing
import numpy as np

Size = typing.Tuple[int, int]
Pixel = typing.Tuple[float, float]

class Pq:
    ImageDisplaySize: Size = (0, 0)
    Legend: str = ""
    ZoomMatrix: np.ndarray = np.eye(3, np.float64)
    WatchedPixels: typing.List[Pixel]

