#!/usr/bin/env python3
import time
import os

from shared import plotdata

debugfiles = [f for f in os.listdir('.') if f.startswith('debug-') and f.endswith('.txt')]

job_start = time.time()

for fn in debugfiles:
    with open(fn, 'rb') as infile:
        plot_data(infile, fn + '.png')

job_end = time.time()

print('Job finished in {0:1f}s'.format(job_end - job_start))
