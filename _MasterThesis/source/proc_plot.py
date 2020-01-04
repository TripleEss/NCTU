# -*- coding: utf-8 -*-

import os
import sys
import json

import numpy as np

import matplotlib.pyplot as plt
import matplotlib.colors
import matplotlib.cm
import matplotlib.transforms



class CV_Report:
    #plt.rcParams['text.usetex'] = True
    DATA_LABELS = {
        'acc': 'Accuracy',
        'micro_p': 'Micro Precision',
        'micro_r': 'Micro Recall',
        'micro_f1': 'Micro F1-Score',
        'macro_p': 'Macro Precision',
        'macro_r': 'Macro Recall',
        'macro_f1': 'Macro F1-Score'
    }

    def __init__(self, files, kfold):
        self.file_labels = []
        self.data = []
        for file in files:
            label, datum = self.load_result(file, kfold)
            self.file_labels.append(label)
            self.data.append(datum)

    @classmethod
    def load_result(cls, file, kfold):
        label = os.path.split(file)[1]
        data = []
        with open(file, 'r', encoding='utf8') as f:
            for i in range(kfold+1):
                datum = {}
                datum['id'] = f.readline()
                datum['support'] = json.loads(f.readline())
                datum['conf_mat'] = json.loads(f.readline())
                datum['acc'] = float(f.readline())
                for v in ['p','r','f1']:
                    values = f.readline().split('\t')
                    datum['macro_'+v] = float(values[0])
                    datum['micro_'+v] = float(values[1])
                    datum[v] = json.loads(values[2])
                f.readline()
                data.append(datum)
        return label, data

    @classmethod
    def get_data(cls, cvs, dname, avg=False):
        data = []
        for cv in cvs:
            if avg:
                data.append(float(cv[len(cv)-1][dname]))
            else:
                cv = cv[:-1]
                datum = []
                for fold in cv:
                    datum.append(float(fold[dname]))
                data.append(datum)
        return data

    @classmethod
    def plt_cv(cls, labels, data, dname, fig_prop={}, plt_props={}, legend_prop={}, fout=False):
        x = np.arange(1,11)
        ys = cls.get_data(data, dname)

        plt.figure(**fig_prop)
        plt.xlabel('k-th Fold')
        plt.ylabel(cls.DATA_LABELS[dname])
        plt.xticks(x)

        for i,(y,label,plt_prop) in enumerate(zip(ys,labels,plt_props)):
            plt.plot(x, y, label=label, **plt_prop)

        if legend_prop:
            plt.legend(**legend_prop)
        
        if fout:
            plt.savefig(fout, bbox_inches='tight')
        plt.show()
        plt.clf()

    @classmethod
    def plt_avg(cls, labels, data, dnames, fig_prop={}, plt_props={}, legend_prop=False, fout=False):
        fig,axs = plt.subplots(1,len(dnames), sharey=True, **fig_prop)
        plt.tight_layout()
        if len(dnames)==1:
            axs = [axs]
        for dname,ax in zip(dnames,axs):
            ys = cls.get_data(data, dname, avg=True)

            ax.set_xlabel('Avg. '+cls.DATA_LABELS[dname])
            ax.set_xticks([])
            ax.set_xlim(0,1)
            for i,(y,label,plt_prop) in enumerate(zip(ys,labels,plt_props)):
                ax.plot([0.25,0.75], [y,y], label=label, **plt_prop)

            
            #ax.plot([0,1], [np.mean(ys),np.mean(ys)], color='gray', linestyle='dashed')

        if legend_prop:
            axs[len(dnames)//2].legend(**legend_prop)

        if fout:
            plt.savefig(fout, bbox_inches='tight')
        plt.show()
        plt.clf()

    @classmethod
    def tbl_avg(cls, labels, data, dnames, tbl_prop={}, fout=False):
        mat = []
        col_labels = []
        for dname in dnames:
            subdata = cls.get_data(data, dname, avg=True)
            subdata.extend([np.mean(subdata)])
            mat.append(subdata)
            col_labels.append('Avg. '+cls.DATA_LABELS[dname])
        mat = list(map(list,zip(*mat)))
        mat = [['{:.4f}'.format(cell) for cell in row] for row in mat]
        labels.extend([''])

        tbl_prop['cellColours'].append(['white']*len(dnames))

        tbl = plt.table(cellText=mat, rowLabels=labels, colLabels=col_labels, **tbl_prop)

        plt.axis('off')
        plt.gcf().canvas.draw()
        pts = tbl.get_window_extent(plt.gcf()._cachedRenderer).get_points()
        pts[0,:] -= 10
        pts[1,:] += 10
        nbbox = matplotlib.transforms.Bbox.from_extents(pts/plt.gcf().dpi)

        if fout:
            plt.savefig(fout, bbox_inches=nbbox)
        plt.clf()


class Exp00_Report(CV_Report):
    def __init__(self, files, kfold):
        super().__init__(files, kfold)

        colors =  ['C0','C1','C2','C3','C4','C5','C6','C7','C8','C9']

        self.data_labels = []
        self.data_styles = []
        for i,label in enumerate(self.file_labels):
            data_style = {}
            label = label.split('_')
            optimizer = label[0]
            activation = label[1]
            self.data_labels.append(label)
            self.data_styles.append({'color': colors[i%10]})

    def plt_cv(self, dname, legend_on=True, fout=False):
        fig_prop = {
        }
        legend_prop = {
            'ncol': 5,
        }
        if not legend_on:
            legend_prop = False

        for i in range(0,len(self.data),10):
            super().plt_cv(self.data_labels[i:i+10], self.data[i:i+10], dname, fig_prop=fig_prop, plt_props=self.data_styles[i:i+10], legend_prop=legend_prop, fout=fout)

    def plt_avg(self, dnames, legend_on=False, fout=False):
        fig_prop = {
        }
        legend_prop = {
            'ncol': 5,
        }
        super().plt_avg(self.data_labels, self.data, dnames, fig_prop=fig_prop, plt_props=self.data_styles, legend_prop=legend_prop, fout=fout)

    def tbl_exp(self, dname, fout=False):
        col_labels = ['KNN','MultinomialNB','LinearSVC','LR','']
        row_labels = ['copeopi','word2vec','doc2vec',
            'ngram(1)','ngram(2)','ngram(3)','ngram(1,2)','ngram(2,3)','ngram(1,2,3)',
            'tfidf(1)','tfidf(2)','tfidf(3)','tfidf(1,2)','tfidf(2,3)','tfidf(1,2,3)',''
            ]
        mat = [[0]*len(col_labels) for i in range(len(row_labels))]
        for label,data in zip(self.data_labels,self.data):
            try:
                mat[row_labels.index(label[1])][col_labels.index(label[0])] = self.get_data([data], dname, avg=True)[0]
            except:
                continue
        for i in range(len(row_labels)-1):
            mat[i][len(col_labels)-1] = np.mean(mat[i][:len(col_labels)-1])
        for i in range(len(col_labels)-1):
            mat[len(row_labels)-1][i] = np.mean([r[i] for r in mat[:len(row_labels)-1]])
        mat[len(row_labels)-1][len(col_labels)-1] = ''
        for i in range(len(row_labels)):
            for j in range(len(col_labels)):
                if i==len(row_labels)-1 and j==len(col_labels)-1:
                    break
                mat[i][j] = '{:.4f}'.format(mat[i][j])

        tbl = plt.table(cellText=mat, rowLabels=row_labels, colLabels=col_labels, cellLoc='center')
        tbl.scale(3,3)

        plt.axis('off')
        plt.gcf().canvas.draw()
        pts = tbl.get_window_extent(plt.gcf()._cachedRenderer).get_points()
        pts[0,:] -= 10
        pts[1,:] += 10
        nbbox = matplotlib.transforms.Bbox.from_extents(pts/plt.gcf().dpi)

        if fout:
            plt.savefig(fout, bbox_inches=nbbox)
        plt.clf()

class Exp20_Report(CV_Report):
    def __init__(self, files, kfold):
        super().__init__(files, kfold)

        colors = {
            'KNN': 'C0',
            'GaussianNB': 'C1',
            'MultinomialNB': 'C2',
            'BernoulliNB': 'C3',
            'SVC': 'C4',
            'NuSVC': 'C5',
            'LinearSVC': 'C6',
            'LR': 'C7',
            'TextCNN': 'C8'
            }
        #colors =  ['C0','C1','C2','C3','C4','C5','C6','C7']
        linestyles = [['solid','dashed'],['dashdot','dotted']]

        self.data_labels = []
        self.data_styles = []
        for i,label in enumerate(self.file_labels):
            data_style = {}
            label = label.split('_')
            classifier = label[0]
            if len(label[1])==3:
                tokenized = int(label[1][0])
                rm_nonalpha = int(label[1][1])
                rm_stopwords = int(label[1][2])
                data_style['color'] = colors[classifier] if tokenized else matplotlib.colors.to_rgba(colors[classifier],0.3)
                data_style['linestyle'] = linestyles[rm_nonalpha][rm_stopwords]
            elif len(label[1])==4:
                tokenized = label[1][0]
                di_stem = label[1][1]
                rm_nonalpha = label[1][2]
                rm_stopwords = label[1][3]
                data_style['color'] = colors[i%8]
            self.data_labels.append(label)
            self.data_styles.append(data_style)

    def plt_cv(self, dname, legend_on=True, fout=False):
        fig_prop = {
        }
        legend_prop = {
            'ncol': 4,
        }
        if not legend_on:
            legend_prop = False

        for i in range(0,len(self.data),4):
            super().plt_cv(self.data_labels[i:i+4], self.data[i:i+4], dname, fig_prop=fig_prop, plt_props=self.data_styles[i:i+4], legend_prop=legend_prop, fout=fout)

    def plt_avg(self, dnames, legend_on=False, fout=False):
        fig_prop = {
        }
        legend_prop = {
            'ncol': 4,
        }
        for i in range(0,len(self.data),4):
            super().plt_avg(self.data_labels[i:i+4], self.data[i:i+4], dnames, fig_prop=fig_prop, plt_props=self.data_styles[i:i+4], legend_prop=legend_prop, fout=fout)



class Exp40_Report(CV_Report):
    def __init__(self, files, kfold):
        super().__init__(files, kfold)

        colors =  ['C0','C1','C2','C3','C4','C5','C6','C7','C8','C9']

        self.data_labels = []
        self.data_styles = []
        for i,label in enumerate(self.file_labels):
            data_style = {}
            label = label.split('_')
            optimizer = label[0]
            activation = label[1]
            self.data_labels.append(label)
            self.data_styles.append({'color': colors[i%10]})

    def plt_cv(self, dname, legend_on=True, fout=False):
        fig_prop = {
        }
        legend_prop = {
            'ncol': 5,
        }
        if not legend_on:
            legend_prop = False

        for i in range(0,len(self.data),10):
            super().plt_cv(self.data_labels[i:i+10], self.data[i:i+10], dname, fig_prop=fig_prop, plt_props=self.data_styles[i:i+10], legend_prop=legend_prop, fout=fout)

    def plt_avg(self, dnames, legend_on=False, fout=False):
        fig_prop = {
        }
        legend_prop = {
            'ncol': 5,
        }
        super().plt_avg(self.data_labels, self.data, dnames, fig_prop=fig_prop, plt_props=self.data_styles, legend_prop=legend_prop, fout=fout)

    def tbl_exp(self, dname, fout=False):
        mat = [[0]*8 for i in range(11)]
        col_labels = ['SGD','RMSprop','Adagrad','Adadelta','Adam','Adamax','Nadam','']
        row_labels = ['softmax','elu','selu','softplus','softsign','relu','tanh','sigmoid','hard','linear','']
        for label,data in zip(self.data_labels,self.data):
            mat[row_labels.index(label[1])][col_labels.index(label[0])] = self.get_data([data], dname, avg=True)[0]
        print(mat)
        for i in range(10):
            mat[i][7] = np.mean(mat[i][:7])
        for i in range(7):
            mat[10][i] = np.mean([r[i] for r in mat[:10]])
        mat[10][7] = ''
        for i in range(11):
            for j in range(8):
                if i!=10 or j!=7:
                    mat[i][j] = '{:.4f}'.format(mat[i][j])

        tbl = plt.table(cellText=mat, rowLabels=row_labels, colLabels=col_labels, cellLoc='center')

        plt.axis('off')
        plt.gcf().canvas.draw()
        pts = tbl.get_window_extent(plt.gcf()._cachedRenderer).get_points()
        pts[0,:] -= 10
        pts[1,:] += 10
        nbbox = matplotlib.transforms.Bbox.from_extents(pts/plt.gcf().dpi)

        if fout:
            plt.savefig(fout, bbox_inches=nbbox)
        plt.clf()




class CV_Report1:
    #plt.rcParams['text.usetex'] = True
    DATA_LABELS = {
        'acc': 'Accuracy',
        'micro_p': 'Micro Precision',
        'micro_r': 'Micro Recall',
        'micro_f1': 'Micro F1-Score',
        'macro_p': 'Macro Precision',
        'macro_r': 'Macro Recall',
        'macro_f1': 'Macro F1-Score'
    }

    def __init__(self, files, kfold):
        self.file_labels = []
        self.data = []
        for file in files:
            label, datum = self.load_result(file, kfold)
            self.file_labels.append(label)
            self.data.append(datum)

    @classmethod
    def load_result(cls, file, kfold):
        label = file.split('-')[-1]
        data = [{}]*(kfold+1)
        with open(file, 'r', encoding='utf8') as f:
            for i in range(kfold):
                datum = {}
                datum['id'] = json.loads(f.readline())
                datum['conf_mat'] = json.loads(f.readline())
                datum['acc'] = json.loads(f.readline())
                datum['p'],datum['r'],datum['f1'],datum['support'] = json.loads(f.readline())
                datum['micro_p'],datum['micro_r'],datum['micro_f1'] = json.loads(f.readline())
                datum['macro_p'],datum['macro_r'],datum['macro_f1'] = json.loads(f.readline())
                data[i] = datum
                f.readline()
            datum = {}
            datum['id'] = f.readline().strip()
            datum['acc'] = json.loads(f.readline())
            datum['p'],datum['r'],datum['f1'],datum['support'] = json.loads(f.readline())
            datum['micro_p'],datum['micro_r'],datum['micro_f1'] = json.loads(f.readline())
            datum['macro_p'],datum['macro_r'],datum['macro_f1'] = json.loads(f.readline())
            data[kfold] = datum
        return label, data

    @classmethod
    def get_data(cls, cvs, dname, avg=False):
        data = []
        for cv in cvs:
            if avg:
                data.append(cv[len(cvs)][dname])
            else:
                cv = cv[:-1]
                datum = []
                for fold in cv:
                    datum.append(fold[dname])
                data.append(datum)
        return data

    @classmethod
    def plt_cv(cls, labels, data, dname, fig_prop={}, plt_props={}, legend_prop={}, fout=False):
        x = np.arange(1,11)
        ys = cls.get_data(data, dname)

        plt.figure(**fig_prop)
        plt.xlabel('k-th Fold')
        plt.ylabel(cls.DATA_LABELS[dname])
        plt.xticks(x)

        for i,(y,label) in enumerate(zip(ys,labels)):
            plt_prop = {}
            for k,v in plt_props.items():
                plt_prop[k] = v[i%len(v)]
            plt.plot(x, y, label=label, **plt_prop)

        if legend_prop:
            plt.legend(**legend_prop)
        
        if fout:
            plt.savefig(fout, bbox_inches='tight')
        plt.clf()

    @classmethod
    def plt_avg(cls, labels, data, dnames, fig_prop={}, plt_props={}, legend_prop=False, fout=False):
        fig,axs = plt.subplots(1,len(dnames), sharey=True, **fig_prop)
        plt.tight_layout()
        if len(dnames)==1:
            axs = [axs]
        for dname,ax in zip(dnames,axs):
            ys = cls.get_data(data, dname, avg=True)
            ax.set_xlabel('Avg. '+cls.DATA_LABELS[dname])
            ax.set_xticks([])
            ax.set_xlim(0,1)
            for i,(y,label) in enumerate(zip(ys,labels)):
                plt_prop = {}
                for k,v in plt_props.items():
                    plt_prop[k] = v[i%len(v)]
                ax.plot([0.25,0.75], [y,y], label=label, **plt_prop)
            ax.plot([0,1], [np.mean(ys),np.mean(ys)], color='gray', linestyle='dashed')

        if legend_prop:
            axs[len(dnames)//2].legend(**legend_prop)

        if fout:
            plt.savefig(fout, bbox_inches='tight')
        plt.clf()

    @classmethod
    def tbl_avg(cls, labels, data, dnames, tbl_prop={}, fout=False):
        mat = []
        col_labels = []
        for dname in dnames:
            subdata = cls.get_data(data, dname, avg=True)
            subdata.extend([np.mean(subdata)])
            mat.append(subdata)
            col_labels.append('Avg. '+cls.DATA_LABELS[dname])
        mat = list(map(list,zip(*mat)))
        mat = [['{:.4f}'.format(cell) for cell in row] for row in mat]
        labels.extend([''])

        tbl_prop['cellColours'].append(['white']*len(dnames))

        tbl = plt.table(cellText=mat, rowLabels=labels, colLabels=col_labels, **tbl_prop)

        plt.axis('off')
        plt.gcf().canvas.draw()
        pts = tbl.get_window_extent(plt.gcf()._cachedRenderer).get_points()
        pts[0,:] -= 10
        pts[1,:] += 10
        nbbox = matplotlib.transforms.Bbox.from_extents(pts/plt.gcf().dpi)

        if fout:
            plt.savefig(fout, bbox_inches=nbbox)
        plt.clf()

class Exp3_Report(CV_Report1):
    def __init__(self, files, kfold):
        super().__init__(files, kfold)

        self.data_labels = [
            'Char-CopeOpi',
            'Confidenced Char-CopeOpi',
            'Word-CopeOpi',
            'Confidenced Word-CopeOpi'
            ]

        self.data_styles = {
            'color': ['r','r','b','b'],
            'linestyle': ['dashed','solid']
        }

    def plt_exp(self, dname, legend_on=True, fout=False):
        data_sizes = set()
        for label in self.file_labels:
            data_size = int(label[:2])
            data_sizes.add(data_size)
        data_sizes = list(data_sizes)

        ys = np.zeros((4,len(data_sizes)))
        for label,datum in zip(self.file_labels,self.data):
            data_size = int(label[:2])
            tokenized = label[-2]
            confidence_on = label[-1]
            if tokenized=='0' and confidence_on=='0':
                ys[0][data_sizes.index(data_size)] = datum[-1][dname]
            elif tokenized=='0' and confidence_on=='1':
                ys[1][data_sizes.index(data_size)] = datum[-1][dname]
            elif tokenized=='1' and confidence_on=='0':
                ys[2][data_sizes.index(data_size)] = datum[-1][dname]
            else:
                ys[3][data_sizes.index(data_size)] = datum[-1][dname]

        plt.figure()
        plt.xlabel('Data size')
        plt.ylabel(super().DATA_LABELS[dname])
        plt.xticks(data_sizes)
        for i,(y,label) in enumerate(zip(ys,self.data_labels)):
            plt_prop = {}
            for k,v in self.data_styles.items():
                plt_prop[k] = v[i%len(v)]
            plt.plot(data_sizes, y, label=label, **plt_prop)

        if legend_on:
            plt.legend()
        
        if fout:
            plt.savefig(fout, bbox_inches='tight')
        plt.clf()

class Exp7_Report(CV_Report1):
    def __init__(self, files, kfold):
        super().__init__(files, kfold)

        self.data_labels = []
        for label in self.file_labels:
            confidence_on = label[0]
            embedding_trainable = label[1]
            if confidence_on=='0' and embedding_trainable=='0':
                label = 'Non-Trainable CopeOpi'
            elif confidence_on=='0' and embedding_trainable=='1':
                label = 'Trainable CopeOpi'
            elif confidence_on=='1' and embedding_trainable=='0':
                label = 'Non-Trainable confidenced CopeOpi'
            elif confidence_on=='1' and embedding_trainable=='1':
                label = 'Trainable confidenced CopeOpi'
            self.data_labels.append(label)

        self.data_styles = {
            'color': ['r','r','b','b'],
            'linestyle': ['dashed','solid']
        }

    def plt_cv(self, dname, legend_on=True, fout=False):
        fig_prop = {
            'figsize': (8,6)
        }
        plt_props = {
            'color': self.data_styles['color'],
            'linestyle': self.data_styles['linestyle']
        }
        legend_prop = {
            'ncol': 2,
            'bbox_to_anchor': (0,1,1,1),
            'loc': 'lower center'
        }
        if not legend_on:
            legend_prop = False
        super().plt_cv(self.data_labels, self.data, dname, fig_prop=fig_prop, plt_props=plt_props, legend_prop=legend_prop, fout=fout)

    def plt_avg(self, dnames, legend_on=False, fout=False):
        fig_prop = {
            'figsize': (2*len(dnames),6)
        }
        plt_props = {
            'color': self.data_styles['color'],
            'linestyle': self.data_styles['linestyle'],
        }
        legend_prop = {
            'ncol': 2,
            'bbox_to_anchor': (0,1,1,1) if len(dnames)%2 else (-1.25,1,2.25,1),
            'loc': 'lower center'
        }
        if not legend_on:
            legend_prop = False  
        super().plt_avg(self.data_labels, self.data, dnames, fig_prop=fig_prop, plt_props=plt_props, legend_prop=legend_prop, fout=fout)

    def tbl_avg(self, dnames, fout=False):
        tbl_prop = {
            'cellLoc': 'center',
            'cellColours': [[matplotlib.colors.to_rgba(c,0.3)]*len(dnames) for c in self.data_styles['color']],
            'loc': 'center'
        }
        super().tbl_avg(self.data_labels, self.data, dnames, tbl_prop=tbl_prop, fout=fout)

class Exp8_Report(CV_Report1):
    def __init__(self, files, kfold):
        super().__init__(files, kfold)

        self.data_labels = self.file_labels

        self.data_styles = {
            'color': ['C0','C1','C2','C3','C4','C5','C6','C7','C8','C9'],
        }

    def plt_cv(self, dname, legend_on=True, fout=False):
        fig_prop = {
            'figsize': (8,6)
        }
        plt_props = {
            'color': self.data_styles['color'],
            'linestyle': ['dashdot'],
            'linewidth': [0.5],
            'marker': ['o'],
            'markersize': [5]
        }
        legend_prop = {
            'ncol': 5,
            'bbox_to_anchor': (0,1,1,1),
            'loc': 'lower center',
            'mode': 'expand'
        }
        if not legend_on:
            legend_prop = False
        super().plt_cv(self.data_labels, self.data, dname, fig_prop=fig_prop, plt_props=plt_props, legend_prop=legend_prop, fout=fout)

    def plt_avg(self, dnames, legend_on=False, fout=False):
        fig_prop = {
            'figsize': (2*len(dnames),6)
        }
        plt_props = {
            'color': self.data_styles['color'],
        }
        legend_prop = {
            'ncol': 5,
            'bbox_to_anchor': (0,1,1,1) if len(dnames)%2 else (-1.25,1,2.25,1),
            'loc': 'lower center'
        }
        if not legend_on:
            legend_prop = False  
        super().plt_avg(self.data_labels, self.data, dnames, fig_prop=fig_prop, plt_props=plt_props, legend_prop=legend_prop, fout=fout)

    def tbl_avg(self, dnames, fout=False):
        tbl_prop = {
            'cellLoc': 'center',
            'cellColours': [[matplotlib.colors.to_rgba(c,0.3)]*len(dnames) for c in self.data_styles['color']],
            'loc': 'center'
        }
        super().tbl_avg(self.data_labels, self.data, dnames, tbl_prop=tbl_prop, fout=fout)

class Exp9_Report(CV_Report1):
    def __init__(self, files, kfold):
        super().__init__(files, kfold)

        self.data_labels = [
            'Char-CopeOpi',
            'Confidenced Char-CopeOpi',
            'Word-CopeOpi',
            'Confidenced Word-CopeOpi'
            ]

        self.data_styles = {
            'color': ['r','r','b','b'],
            'linestyle': ['dashed','solid']
        }

    def plt_exp(self, dname, legend_on=True, fout=False):
        x = [i*0.1 for i in range(1,10)]
        y = np.zeros(len(x))
        for label,datum in zip(self.file_labels,self.data):
            dropout_rate = int(label[-1])
            y[dropout_rate-1] = datum[-1][dname]

        plt.figure()
        plt.xlabel('Dropout Rate')
        plt.ylabel(super().DATA_LABELS[dname])
        plt.xticks(x)
        plt.plot(x, y)

        if legend_on:
            plt.legend()
        
        if fout:
            plt.savefig(fout, bbox_inches='tight')
        plt.clf()

class Exp10_Report(CV_Report1):
    def __init__(self, files, kfold):
        super().__init__(files, kfold)

        self.data_labels = self.file_labels

        self.data_styles = {
            'color': ['C0','C1','C2','C3','C4','C5','C6','C7'],#['red','darkorange','gold','green','blue','deepskyblue','magenta'],
        }

    def plt_cv(self, dname, legend_on=True, fout=False):
        fig_prop = {
            'figsize': (8,6)
        }
        plt_props = {
            'color': self.data_styles['color'],
            'linestyle': ['dashdot'],
            'linewidth': [0.5],
            'marker': ['o'],
            'markersize': [5]
        }
        legend_prop = {
            'ncol': 4,
            'bbox_to_anchor': (0,1,1,1),
            'loc': 'lower center',
            'mode': 'expand'
        }
        if not legend_on:
            legend_prop = False
        super().plt_cv(self.data_labels, self.data, dname, fig_prop=fig_prop, plt_props=plt_props, legend_prop=legend_prop, fout=fout)

    def plt_avg(self, dnames, legend_on=False, fout=False):
        fig_prop = {
            'figsize': (2*len(dnames),6)
        }
        plt_props = {
            'color': self.data_styles['color']
        }
        legend_prop = {
            'ncol': 4,
            'bbox_to_anchor': (0,1,1,1) if len(dnames)%2 else (-1.25,1,2.25,1),
            'loc': 'lower center'
        }
        if not legend_on:
            legend_prop = False  
        super().plt_avg(self.data_labels, self.data, dnames, fig_prop=fig_prop, plt_props=plt_props, legend_prop=legend_prop, fout=fout)

    def tbl_avg(self, dnames, fout=False):
        tbl_prop = {
            'cellLoc': 'center',
            'cellColours': [[matplotlib.colors.to_rgba(c,0.3)]*len(dnames) for c in self.data_styles['color']],
            'loc': 'center'
        }
        super().tbl_avg(self.data_labels, self.data, dnames, tbl_prop=tbl_prop, fout=fout)

class CopeOpi_Report():
    def __init__(self, file):
        self.copeopi0 = {}
        self.copeopi1 = {}
        with open(file, 'r', encoding='utf8') as f:
            f.readline()
            f.readline()
            f.readline()
            for line in f:
                line = line.strip().split('\t')
                self.copeopi0[line[0]] = json.loads(line[1])
                self.copeopi1[line[0]] = json.loads(line[3])

    def hst_copeopi(self, copeopi0=True, copeopi1=True):
        if copeopi0 and copeopi1:
            cope0 = np.array(list(self.copeopi0.values()))
            cope0 = cope0.transpose()

            cope1 = np.array(list(self.copeopi1.values()))
            cope1 = cope1.transpose()
            for d0,d1 in zip(cope0,cope1):
                plt.xlabel('CopeOpi Scores')
                plt.ylabel('Number of Words')
                plt.hist([d0,d1],label=['COP','CF-COP'],bins=50)
                plt.legend()
                plt.show()




if __name__ == '__main__':
   x = CopeOpi_Report('exp\\copeopiSA(EN)(A)').hst_copeopi()