import numpy as np
import matplotlib.pyplot as plt

def Read(line,X,Y,E):
    parse = line.strip().split()
    x = float(parse[1])
    y = float(parse[2])
    e = float(parse[3])
    #print (x, y, e)
    X.append(x)
    Y.append(y)
    E.append(e)

#fig, (aU, aL) = plt.subplots(2, 1, gridspec_kw={'height_ratios': [3, 1]},figsize=(10,8))
#fig.subplots_adjust(hspace=0,wspace=0)

#aU.tick_params(labelbottom=False,direction='in')
#aL.tick_params(direction='in')




plt.figure(1,figsize=(9,6))
#plt.sca(aL)
plt.axhline(y=1,linestyle='--',linewidth=3,color='black')
plt.axvline(x=0.3,linestyle='--',linewidth=2,color='black')
plt.title(r'$1.3 < \alpha_s < 1.4$',fontsize=22)
plt.xlim([0.2,0.75])
plt.ylim([0.6,1.4])
plt.xticks([0.2,0.3,0.4,0.5,0.6,0.7],fontsize=16)
plt.yticks([0.6,0.8,1,1.2,1.4],fontsize=16)
plt.ylabel(r'$R$',fontsize=22,labelpad=12)
plt.xlabel(r"$x'$",fontsize=22)

plt.figure(2,figsize=(9,6))
#plt.sca(aU)
plt.axhline(y=1,linestyle='--',linewidth=3,color='black')
plt.axvline(x=0.4,linestyle='--',linewidth=2,color='black')
plt.title(r'$1.4 < \alpha_s < 1.5$',fontsize=22)
plt.xlim([0.2,0.75])
plt.ylim([0.6,1.4])
plt.xticks([0.2,0.3,0.4,0.5,0.6,0.7],fontsize=16)
plt.yticks([0.6,0.8,1,1.2,1.4],fontsize=16)
plt.ylabel(r'$R$',fontsize=22,labelpad=12)
plt.xlabel(r"$x'$",fontsize=22)


plt.figure(3,figsize=(9,6))
#plt.sca(aU)
plt.axhline(y=1,linestyle='--',linewidth=3,color='black')
plt.axvline(x=0.4,linestyle='--',linewidth=2,color='black')
plt.title(r'$1.5 < \alpha_s < 1.6$',fontsize=22)
plt.xlim([0.2,0.75])
plt.ylim([0.6,1.4])
plt.xticks([0.2,0.3,0.4,0.5,0.6,0.7],fontsize=16)
plt.yticks([0.6,0.8,1,1.2,1.4],fontsize=16)
plt.ylabel(r'$R$',fontsize=22,labelpad=12)
plt.xlabel(r"$x'$",fontsize=22)

files = ['xp_MCfull-asymp_ratio_asbin1.txt','xp_MCfull-asymp_ratio_asbin2.txt','xp_MCfull-asymp_ratio_asbin3.txt']
mark = ['o','s','*']
col = ['blue','red','green']


DataSimX    = [[] for j in range(len(files))]
DataSim     = [[] for j in range(len(files))]
DataSimErr  = [[] for j in range(len(files))]


for i in range(len(files)):

    with open(files[i],"r") as f:
        for line in f:
            if '#Full/Asymp Norm Ratio' in line: continue
            elif 'Full/Asymp(norm):' in line:
                Read(line,DataSimX[i],DataSim[i],DataSimErr[i])
            else: continue

        print(i)
        print(DataSimX[i])
        print(DataSim[i])


plt.figure(1)
plt.errorbar(DataSimX[0],DataSim[0],yerr=DataSimErr[0],color='blue',marker='o',linestyle='none',markersize=12,linewidth=3)
plt.savefig('mcratio_full-asymp-as-1.3-1.4.pdf',bbox_inches='tight')

plt.figure(2)
plt.errorbar(DataSimX[1],DataSim[1],yerr=DataSimErr[1],color='blue',marker='o',linestyle='none',markersize=12,linewidth=3,label=r'$1.4 < \alpha_s < 1.5')
plt.savefig('mcratio_full-asymp-as-1.4-1.5.pdf',bbox_inches='tight')
plt.figure(3)
plt.errorbar(DataSimX[2],DataSim[2],yerr=DataSimErr[2],color='blue',marker='o',linestyle='none',markersize=12,linewidth=3,label=r'$1.4 < \alpha_s < 1.5')
plt.savefig('mcratio_full-asymp-as-1.5-1.6.pdf',bbox_inches='tight')



#plt.savefig('data_as_combo.pdf',bbox_inches='tight')
plt.show()
