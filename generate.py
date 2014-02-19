#!/usr/bin/env python3

from shared import update_progress, plot

from os import path, makedirs
import inspect, time

from math import sqrt, exp, sin, cos, tan, atan2, pi
import numpy as np
import matplotlib.pyplot as plt
import argparse

RMIN = 6500.
RMAX = RMIN + 1000.
PHIRANGE = pi / 6.
NSTEPS = 2048


def from_polar(r, phi):
    return r * cos(phi), r * sin(phi)


def to_polar(x, y):
    return sqrt(x ** 2 + y ** 2), atan2(y, x)


def generate(surf, num_beams, num_rays, out_filename):
    t = np.linspace(0., 1., NSTEPS)
    filedir = path.dirname(out_filename)
    if not path.exists(filedir):
        makedirs(filedir)
    if path.exists(out_filename) and path.getmtime(out_filename) > path.getmtime(__file__):
        print("Dataset is up to date, skipping.")
        return
    with open(out_filename, 'w') as out_file:
        out_file.write('diff ' + str(num_beams * num_rays) + '\n')
        rays = 0
        for pivotPhi in np.linspace(-PHIRANGE / 2, PHIRANGE / 2, num_beams):
            ax, ay = from_polar(RMIN, pivotPhi)
            for dstPhi in np.linspace(-PHIRANGE / 2, PHIRANGE / 2, num_rays):
                rays += 1
                update_progress(rays * 100 / num_beams / num_rays)
                bx, by = from_polar(RMAX, dstPhi)
                func = np.vectorize(lambda x: surf(x * ax + (1 - x) * bx, x * ay + (1 - x) * by))
                result = np.trapz(func(t), t) * sqrt((ax - bx) ** 2 + (ay - by) ** 2)
                out_file.write(str(ax) + ' ' + str(ay) + ' ')
                out_file.write(str(bx) + ' ' + str(by) + ' ')
                out_file.write(str(result) + '\n')


def main():
    w = 100.
    r0 = 6800.
    k = 0.7
    basemodel = lambda r, phi: exp(-((r - r0) ** 2) / (2 * w ** 2))
    mod = lambda x, y: 1. - exp(-((x - r0 - k * y) ** 2) / (2 * w ** 2))
    dot = lambda x, y, wx, wy: exp(- x ** 2 / (2 * wx ** 2) - y ** 2 / (2 * wy ** 2))
    perturb = lambda r, phi: \
        1.0 * dot(r - r0, phi + pi / 50, 20.0, 0.01) + \
        -0.7 * dot(r - r0 - w / 2, phi - pi / 36, 30.0, 0.01) + \
        0.5 * dot(r - r0 + w / 2, phi - pi / 64, 15.0, 0.02)

    surf1 = lambda x, y: basemodel(*to_polar(x, y))
    surf2 = lambda x, y: basemodel(*to_polar(x, y)) * mod(x, y) + perturb(*to_polar(x, y))
    datasets = [
        [surf1, 10, 128, "data/base.txt"],
        [surf2, 3,  150, "data/perturb-3beam.txt"],
        [surf2, 5,  150, "data/perturb-5beam.txt"],
        [surf2, 10, 150, "data/perturb-10beam.txt"]
    ]

    for i, set in enumerate(datasets):
        print("Generating dataset " + str(i+1) + " out of " + str(len(datasets)))
        generate(*set)

    xx = np.linspace(RMIN, RMAX, 128)
    yy = np.linspace(-PHIRANGE / 2, PHIRANGE / 2, 128)

    f = np.zeros([128, 128])
    for i in range(128):
        for j in range(128):
            f[i, j] = surf2(*from_polar(xx[i], yy[j]))

    x = np.linspace(0, RMIN*PHIRANGE, f.shape[1])
    y = np.linspace(0, RMAX-RMIN, f.shape[0])

    plot(x, y, f, 'out/model.png', 'Модель')

if __name__ == "__main__":
    main()
