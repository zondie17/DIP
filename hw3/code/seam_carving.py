import numpy as np
from imageio import imread, imwrite
from scipy.ndimage.filters import convolve
from tqdm import trange
from numba import jit


def calc_energy(img):
    filter_u = np.array([
        [1.0, 2.0, 1.0],
        [0.0, 0.0, 0.0],
        [-1.0, -2.0, -1.0],
    ])
    filter_u = np.stack([filter_u] * 3, axis=2)

    filter_v = np.array([
        [1.0, 0.0, -1.0],
        [2.0, 0.0, -2.0],
        [1.0, 0.0, -1.0],
    ])
    filter_v = np.stack([filter_v] * 3, axis=2)

    im = img.astype('float32')
    convolved = np.absolute(convolve(im, filter_u)) + np.absolute(convolve(im, filter_v))

    energy_map = convolved.sum(axis=2)

    return energy_map


@jit
def min_seam(img):
    height, width, bgr = img.shape
    energy_map = calc_energy(img)

    m = energy_map.copy()
    backtrack = np.zeros_like(m, dtype=np.int)

    for i in range(1, height):
        for j in range(0, width):
            if j == 0:
                idx = np.argmin(m[i - 1, j:j + 2])
                backtrack[i, j] = idx + j
                min_energy = m[i - 1, idx + j]
            else:
                idx = np.argmin(m[i - 1, j - 1:j + 2])
                backtrack[i, j] = idx + j - 1
                min_energy = m[i - 1, idx + j - 1]
            m[i, j] += min_energy

    return m, backtrack


@jit
def carve_column(img):
    height, width, bgr = img.shape
    m, backtrack = min_seam(img)

    mask = np.ones((height, width), dtype=np.bool)

    j = np.argmin(m[-1])
    for i in reversed(range(height)):
        mask[i, j] = False
        j = backtrack[i, j]

    mask = np.stack([mask] * 3, axis=2)

    img = img[mask].reshape((height, width - 1, 3))

    return img


def crop_c(img, scale_c):
    height, width, bgr = img.shape

    for i in trange(width - int(width * scale_c)):
        img = carve_column(img)

    return img


if __name__ == '__main__':
    infile = './4/input.jpg'
    outfile = './4/test.jpg'
    inputImg = imread(infile)
    scale = 0.6

    out = crop_c(inputImg, scale)
    imwrite(outfile, out)
