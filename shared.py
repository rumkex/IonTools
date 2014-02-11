import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import re
import time

def update_progress(progress):
    print('\r[{0:<10}] {1}%'.format('#'*int(progress/10), int(progress)), end='')
    if int(progress) == 100:
        print()


def plot(X, Y, F, outname, title):   
    X, Y = np.meshgrid(X, Y)

    fig = plt.figure()
    ax = fig.add_subplot(111)
    #img = ax.imshow(f, vmin=0.0, vmax=2.0, interpolation='bilinear', origin='lower', aspect='auto')
    img = ax.contourf(X, Y, F, np.concatenate((np.arange(0.0, 1.0, 0.1), np.arange(1.0, 2.1, 0.2))), cmap = plt.cm.Accent, vmin=0.0, vmax=2.0)
    contours = ax.contour(img, levels=img.levels, colors = 'black', hold='on')    
    ax.set_title(title)
    ax.set_xlabel('x, км')
    ax.set_ylabel('z, км') 
    cb = fig.colorbar(img, ax=ax)
    cb.add_lines(contours)
    fig.savefig(outname, dpi=300, bbox_inches='tight')
    plt.close(fig)


def plot_data(infile, outname, title='Output'):
    hdr = infile.readline().decode('utf-8')
    infile.seek(0)
    f = np.loadtxt(infile, delimiter='\t')

    hdr = re.findall('-?[0-9]+\.?[0-9]*', hdr)
    
    y0 = float(hdr[0])
    ysize = float(hdr[1])
    xsize = y0*float(hdr[3])
    
    
    x = np.linspace(0, xsize, f.shape[1])
    y = np.linspace(0, ysize, f.shape[0])
          
    plot(x, y, f, outname, title)
