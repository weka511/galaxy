import numpy as np,matplotlib.pyplot as plt,os.path as op
from matplotlib import rc
from scipy.optimize import curve_fit

def bolzmann(n,m,beta):
    return m*np.exp(-beta*n)

def plot(name='bodies00002',ext='.csv',path='./configs',nbins=200,out='./imgs'): 
    plt.figure(figsize=(10,10))
    config = np.loadtxt(op.join(path,name+ext),delimiter=',')
    energies=[0.5*config[j,6]*(config[j,3]**2+config[j,4]**2+config[j,5]**2) for j in range(len(config))]
    n,bins,_=plt.hist(energies,bins=nbins,label='Energies')
    xx=[0.5*(bins[i]+bins[i-1]) for i in range(1,len(bins))]
    popt, pcov = curve_fit(bolzmann, xx, n, p0=(sum(n), 1e-6))
    perr = np.sqrt(np.diag(pcov))
    yy=[bolzmann(x,*popt) for x in xx]
    plt.plot( xx, yy,c='r',label=r'Bolzmann: N={0:.0f}({2:.0f}),$\beta$={1:.0f}({3:.0f})'.format(popt[0],popt[1],perr[0],perr[1]))
    plt.title(name)
    plt.legend()
    plt.savefig(op.join(out,name.replace('bodies','energy')+'png'))
    
if __name__=='__main__':
    rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
    rc('text', usetex=True)
    n0=0
    n1=2400
    step=100
    for i in range(0,n1,step):
        plot('bodies{0:05d}'.format(i))

    plt.show()