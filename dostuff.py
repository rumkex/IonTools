#!/usr/bin/env python3
from math import pi

from multiprocessing import Pool
from tempfile import NamedTemporaryFile
from subprocess import call, DEVNULL
import time
import os
import os.path as path

from shared import update_progress, plot_data

baseflags = ['--interpolate=linear', '--enable-weights']

config = {
    "base": baseflags + ['-n=128'],
    "smoothing": baseflags + ['-n=128', '--enable-aa'],
    "128x128-multigrid-3-layers": baseflags + ['-n=128', '-l=3'],
    "64x128-multigrid-3-layers": baseflags + ['-n=64x128', '-l=3'],
}

def reconstruct(filename, flags, outname):
    with NamedTemporaryFile() as temp:
        call(['bin/Solver', '-i=100', '-t=polar'] + flags + [filename] + [temp.name], stdout=DEVNULL, stderr=DEVNULL)
        plot_data(temp, outname, '')


def main():
    datafiles = [path.join('./data', f) for f in os.listdir('./data') if path.isfile(path.join('./data', f))]
    
    pool = Pool()  

    completedTasks = 0
    totalTasks = 0
    def mp_callback(task):
        nonlocal totalTasks, completedTasks
        completedTasks += 1
        update_progress(completedTasks / totalTasks * 100)        

    job_start = time.time()

    print('Generating jobs...')
    for datafile in datafiles:
        prefix = 'out/' + path.splitext(path.basename(datafile))[0] + '/'
        if not path.exists(prefix):
            os.makedirs(prefix)
        for solverType in ['ART', 'SIRT']:
            solver_flag = ['--solver=' + solverType]
            for name, cfg in config.items():
                outname = prefix + solverType + '-' + name + '.png'
                if path.exists(outname):
                    continue
                totalTasks += 1
                #reconstruct(datafile, solver_flag + cfg, outname)
                pool.apply_async(reconstruct, args = (datafile, solver_flag + cfg, outname), callback = mp_callback)

    print('Generated ' + str(totalTasks) + ' jobs.')   
    
    update_progress(0)
   
    pool.close()
    pool.join()

    if totalTasks == 0: update_progress(100)
    job_end = time.time()

    print('Took {0:1f}s'.format(job_end - job_start))

if __name__ == "__main__":
    main()
