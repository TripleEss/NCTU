# -*- coding: utf-8 -*-

import logging
import os
import re
import json
import numpy as np
logging.basicConfig(level=logging.INFO,
	format='%(asctime)s [%(levelname)s] (%(funcName)s) %(message)s',
	datefmt='%Y-%m-%d %H:%M:%S')

import jieba
from sklearn.datasets import fetch_20newsgroups

try:
	import xml.etree.cElementTree as ET
except ImportError:
	import xml.etree.ElementTree as ET

def raw_MioChnCorp(fin):
	logging.info('START')
	file_dir = os.path.split(fin)[0]

	f = [0]*6
	for i in range(1,6):
		f[i] = open(os.path.join(file_dir,'src','data.c'+str(i)), 'w', encoding='utf8')
	fp = open(os.path.join(file_dir,'src','data.pos'), 'w', encoding='utf8')
	fn = open(os.path.join(file_dir,'src','data.neg'), 'w', encoding='utf8')

	with open(fin, 'r', encoding='utf8') as fin:
		fin.readline()
		for i_line,line in enumerate(fin):
			line = line.strip().split('\t\t')
			y = int(line[0])
			x = re.sub(r'\\', '', line[1])
			x = re.sub(r'\s+', ' ', x)

			f[y].write('{}\t{}\t{}\n'.format(i_line,x,y))
			if y in [1,2]:
				fn.write('{}\t{}\t{}\n'.format(i_line,x,y))
			elif y in [4,5]:
				fp.write('{}\t{}\t{}\n'.format(i_line,x,y))
			elif y in [3]:
				pass
			else:
				logging.warning('line #{}: y={}'.format(i_line,y))
	for i in range(1,6):
		f[i].close()
	fp.close()
	fn.close()
	logging.info('FINISH')

def raw_FudanCorp(fin_dir):
	for subset in ['train','test']:
		fout = open(os.path.join(fin_dir,'data.'+subset), 'w', encoding='utf8')
		idx = 0
		for cat in os.listdir(os.path.join(fin_dir,subset)):
			for i,file in enumerate(os.listdir(os.path.join(fin_dir,subset,cat))):
				if i<30:
					file = os.path.join(fin_dir,subset,cat,file)
					with open(file, 'r', encoding='gbk', errors='ignore') as f:
						text = re.sub(r'\s+', ' ', f.read().strip())
						fout.write('{}\t{}\t{}\n'.format(idx,text,cat))
						idx += 1
	fout.close()

def seg_FudanCorp(fin, fout):
	fout = open(fout, 'w', encoding='utf8')
	with open(fin, 'r', encoding='utf8') as f:
		for line in f:
			line = line.strip().split('\t')
			text = ' '.join(jieba.cut(line[1]))
			text = re.sub(r'\s+', ' ', text)
			fout.write('{}\t{}\t{}\n'.format(line[0],text,line[2]))
	fout.close()

def raw_FudanCorp2(fin):
	cats = ['C3-Art','C7-History','C11-Space','C19-Computer','C31-Enviornment','C32-Agriculture','C34-Economy','C38-Politics','C39-Sports']
	fout1 = open(fin.strip('.0')+'.1', 'w', encoding='utf8')
	fout2 = open(fin.strip('.0')+'.2', 'w', encoding='utf8')
	with open(fin, 'r', encoding='utf8') as f:
		for line in f:
			tag = line.strip().split('\t')[2]
			if tag in cats:
				fout1.write('{}'.format(line))
			else:
				fout2.write('{}'.format(line))
	fout1.close()
	fout2.close()

def num_FudanCorp(file,n):
	cls_sets = [
		['C3-Art','C4-Literature','C5-Education','C6-Philosophy','C7-History',
		'C11-Space','C15-Energy','C16-Electronics','C17-Communication','C19-Computer',
		'C23-Mine','C29-Transport',
		'C31-Enviornment','C32-Agriculture','C34-Economy','C35-Law','C36-Medical','C37-Military','C38-Politics','C39-Sports'],
		['C3-Art','C7-History',
		'C11-Space','C19-Computer',
		'C31-Enviornment','C32-Agriculture','C34-Economy','C38-Politics','C39-Sports'],
		['C4-Literature','C5-Education','C6-Philosophy',
		'C15-Energy','C16-Electronics','C17-Communication',
		'C23-Mine','C29-Transport',
		'C35-Law','C36-Medical','C37-Military']]

	fout = open(file+'.new', 'w', encoding='utf8')
	with open(file, 'r', encoding='utf8') as f:
		for line in f:
			line = line.strip().split('\t')
			fout.write('{}\t{}\t{}\n'.format(line[0],line[1],cls_sets[n].index(line[2])))
	fout.close()

def raw_Yelp9(fin):
	logging.info('START')
	file_dir = os.path.split(fin)[0]

	f = [0]*6
	for i in range(1,6):
		f[i] = open(os.path.join(file_dir,'src','data.c'+str(i)), 'w', encoding='utf8')
	fp = open(os.path.join(file_dir,'src','data.pos'), 'w', encoding='utf8')
	fn = open(os.path.join(file_dir,'src','data.neg'), 'w', encoding='utf8')

	with open(fin, 'r', encoding='utf8') as fin:
		for i_line,line in enumerate(fin):
			line = json.loads(line)
			y = int(line['stars'])
			x = re.sub(r'\s+', ' ', line['text'])

			f[y].write('{}\t{}\t{}\n'.format(i_line,x,y))
			if y in [1,2]:
				fn.write('{}\t{}\t{}\n'.format(i_line,x,y))
			elif y in [4,5]:
				fp.write('{}\t{}\t{}\n'.format(i_line,x,y))
			elif y in [3]:
				pass
			else:
				logging.warning('line #{}: y={}'.format(i_line,y))
	for i in range(1,6):
		f[i].close()
	fp.close()
	fn.close()
	logging.info('FINISH')

def raw_20newsgroup(fout_dir):
	logging.info('START')
	for subset in ['all','train','test']:
		news = fetch_20newsgroups(subset=subset)
		with open(os.path.join(fout_dir,'data.'+subset+'.00'), 'w', encoding='utf8') as f:
			for i,(x,y) in enumerate(zip(news.data,news.target)):
				x = re.sub(r'\s+', ' ', x)
				x = x.strip()
				f.write('{}\t{}\t{}\n'.format(i,x,y))
		news = fetch_20newsgroups(subset=subset, remove=('headers','footers','quotes'))
		with open(os.path.join(fout_dir,'data.'+subset+'.01'), 'w', encoding='utf8') as f:
			for i,(x,y) in enumerate(zip(news.data,news.target)):
				x = re.sub(r'\s+', ' ', x)
				x = x.strip()
				f.write('{}\t{}\t{}\n'.format(i,x,y))

	cats = [
		'alt.atheism',
		'comp.graphics',
		'comp.os.ms-windows.misc',
		'comp.sys.ibm.pc.hardware',
		'comp.sys.mac.hardware',
		'comp.windows.x',
		'misc.forsale',
		'rec.autos',
		'rec.motorcycles',
		'rec.sport.baseball',
		'rec.sport.hockey',
		'sci.crypt',
		'sci.electronics',
		'sci.med',
		'sci.space',
		'soc.religion.christian',
		'talk.politics.guns',
		'talk.politics.mideast',
		'talk.politics.misc',
		'talk.religion.misc']
	for subset in ['train','test']:
		for cat in cats:
			news = fetch_20newsgroups(subset=subset, categories=[cat])
			with open(os.path.join(fout_dir,subset,cat+'.0'), 'w', encoding='utf8') as f:
				for i,(x,y) in enumerate(zip(news.data,news.target)):
					x = re.sub(r'\s+', ' ', x)
					x = x.strip()
					f.write('{}\t{}\t{}\n'.format(i,x,y))
			news = fetch_20newsgroups(subset=subset, categories=[cat], remove=('headers','footers','quotes'))
			with open(os.path.join(fout_dir,subset,cat+'.1'), 'w', encoding='utf8') as f:
				for i,(x,y) in enumerate(zip(news.data,news.target)):
					x = re.sub(r'\s+', ' ', x)
					x = x.strip()
					f.write('{}\t{}\t{}\n'.format(i,x,y))
	logging.info('FINISH')

def raw_20newsgroup2(fout_dir):
	cats = [
		['alt.atheism'],
		['comp.graphics','comp.os.ms-windows.misc','comp.sys.ibm.pc.hardware','comp.sys.mac.hardware','comp.windows.x'],
		['misc.forsale'],
		['rec.autos','rec.motorcycles','rec.sport.baseball','rec.sport.hockey'],
		['sci.crypt','sci.electronics','sci.med','sci.space'],
		['soc.religion.christian'],
		['talk.politics.guns','talk.politics.mideast','talk.politics.misc','talk.religion.misc']]
	for subset in ['train','test']:
		fout = {}
		fout['0'] =	open(os.path.join(fout_dir,'data.'+subset+'.10'), 'w', encoding='utf8')
		fout['1'] = open(os.path.join(fout_dir,'data.'+subset+'.11'), 'w', encoding='utf8')
		for file in os.listdir(os.path.join(fout_dir,subset)):
			for newy,cat in enumerate(cats):
				if file[:-2] in cat:
					tag = newy
					break
			file = os.path.join(fout_dir,subset,file)
			with open(file, 'r', encoding='utf8') as f:
				for line in f:
					line = line.strip().split('\t')
					fout[file[-1]].write('{}\t{}\t{}\n'.format(line[0],line[1],newy))
		for k,v in fout.items():
			v.close()

	cats = ['comp.graphics','comp.os.ms-windows.misc','comp.sys.ibm.pc.hardware','comp.sys.mac.hardware','comp.windows.x']
	for subset in ['train','test']:
		fout = {}
		fout['0'] =	open(os.path.join(fout_dir,'data.'+subset+'.20'), 'w', encoding='utf8')
		fout['1'] = open(os.path.join(fout_dir,'data.'+subset+'.21'), 'w', encoding='utf8')
		for file in os.listdir(os.path.join(fout_dir,subset)):
			tag = None
			for newy,cat in enumerate(cats):
				if file[:-2]==cat:
					tag = newy
					break
			if tag==None:
				continue
			file = os.path.join(fout_dir,subset,file)
			with open(file, 'r', encoding='utf8') as f:
				for line in f:
					line = line.strip().split('\t')
					fout[file[-1]].write('{}\t{}\t{}\n'.format(line[0],line[1],newy))
		for k,v in fout.items():
			v.close()

	cats = ['talk.politics.guns','talk.politics.mideast','talk.politics.misc','talk.religion.misc']
	for subset in ['train','test']:
		fout = {}
		fout['0'] =	open(os.path.join(fout_dir,'data.'+subset+'.30'), 'w', encoding='utf8')
		fout['1'] = open(os.path.join(fout_dir,'data.'+subset+'.31'), 'w', encoding='utf8')
		for file in os.listdir(os.path.join(fout_dir,subset)):
			tag = None
			for newy,cat in enumerate(cats):
				if file[:-2]==cat:
					tag = newy
					break
			if tag==None:
				continue
			file = os.path.join(fout_dir,subset,file)
			with open(file, 'r', encoding='utf8') as f:
				for line in f:
					line = line.strip().split('\t')
					fout[file[-1]].write('{}\t{}\t{}\n'.format(line[0],line[1],newy))
		for k,v in fout.items():
			v.close()

def subset_corpus(fout, subsets):
	logging.info('START')
	fout = open(fout, 'w', encoding='utf8')
	for subset in subsets:
		fin,num,tag = subset
		logging.info('Subset {} {}'.format(fin,num))
		with open(fin, 'r', encoding='utf8') as fin:
			data = [line for line in fin]
			index = np.random.permutation(np.arange(len(data)))
			for i in range(num):
				d = data[index[i]].split('\t')
				fout.write('{}\t{}\t{}\n'.format(d[0],d[1],tag))
	fout.close()
	logging.info('FINISH')

def kfold_file(fin, fout_dir, kfold):
	logging.info('START')
	fname = os.path.basename(fin)
	with open(fin, 'r', encoding='utf8') as fin:
		lines = [line for line in fin]
		index = np.random.permutation(np.arange(len(lines)))
		spts = np.linspace(0,len(lines),kfold+1, dtype=int)
		folds = []
		for i_spt,spt in enumerate(spts):
			fold = []
			if i_spt==len(spts)-1:
				break
			for i in range(int(spts[i_spt]),int(spts[i_spt+1])):
				fold.append(lines[index[i]])
			folds.append(fold)
	
	if fout_dir:
		for i_fold,fold in enumerate(folds):
			fout = os.path.join(fout_dir,fname+'.fold'+str(i_fold))
			with open(fout, 'w', encoding='utf8') as f:
				for line in fold:
					f.write('{}'.format(line))

def train_test_split_file(fin, fout_dir, train_size):
	logging.info('START')
	fname = os.path.basename(fin)
	with open(fin, 'r', encoding='utf8') as fin:
		lines = [line for line in fin]
		index = np.random.permutation(np.arange(len(lines)))
		spt = round(train_size*len(lines))
		train, test = [], []
	for i in range(spt):
		train.append(lines[index[i]])
	for i in range(spt, len(lines)):
		test.append(lines[index[i]])

	if fout_dir:
		fout = os.path.join(fout_dir,fname+'.train')
		with open(fout, 'w', encoding='utf8') as f:
			for line in train:
				f.write('{}'.format(line))
		fout = os.path.join(fout_dir,fname+'.test')
		with open(fout, 'w', encoding='utf8') as f:
			for line in test:
				f.write('{}'.format(line))
### EOF train_test_split_data

def subset_A(fin_dir, fout_dir, n):
	if 1 in n:
		subset_corpus(os.path.join(fout_dir,'A1','data','data.A1.train'), [
			(os.path.join(fin_dir,'src','data.pos'),3750,1),
			(os.path.join(fin_dir,'src','data.neg'),3750,0)])
	if 2 in n:
		subset_corpus(os.path.join(fout_dir,'A2','data','data.A2.train'), [
			(os.path.join(fin_dir,'src','data.c1'),2500,0),
			(os.path.join(fin_dir,'src','data.c3'),2500,1),
			(os.path.join(fin_dir,'src','data.c5'),2500,2)])
	if 3 in n:
		subset_corpus(os.path.join(fout_dir,'A3','data','data.A3.train'), [
			(os.path.join(fin_dir,'src','data.c1'),1500,1),
			(os.path.join(fin_dir,'src','data.c2'),1500,2),
			(os.path.join(fin_dir,'src','data.c3'),1500,3),
			(os.path.join(fin_dir,'src','data.c4'),1500,4),
			(os.path.join(fin_dir,'src','data.c5'),1500,5)])
	if 4 in n:
		subset_corpus(os.path.join(fout_dir,'A4','data','data.A4'), [
			(os.path.join(fin_dir,'src','data.c5'),5000,1),
			(os.path.join(fin_dir,'src','data.c1'),5000,1),
			(os.path.join(fin_dir,'src','data.c3'),5000,0)])

def kfold_A():
	exp_dir = 'exp\\exp0-main'
	for subexp in ['exp01','exp02']:
		for expdata in ['A1','A2','A3','A4']:
			fin = os.path.join(exp_dir,subexp,expdata,'data','data.'+expdata)
			fout_dir = os.path.join(exp_dir,subexp,expdata,'data')
			kfold_file(fin, fout_dir, 10)

def ttsep_A():
	exp_dir = 'exp'
	for subexp in ['test',]:
		for expdata in ['A4']:
			fin = os.path.join(exp_dir,subexp,expdata,'data','data.'+expdata)
			fout_dir = os.path.join(exp_dir,subexp,expdata,'data')
			train_test_split_file(fin, fout_dir, 0.5)

def count(fin_dir):
	fout = open(os.path.join(fin_dir,'data.meta'),'w',encoding='utf8')
	for file in os.listdir(fin_dir):
		with open(os.path.join(fin_dir,file),'r',encoding='utf8') as f:
			n = 0
			for line in f:
				n += 1
			fout.write('{} {}\n'.format(file,n))
	fout.close()

def count2(fin_dir):
	fout = open(os.path.join(fin_dir,'data.meta'),'w',encoding='utf8')
	for cat in os.listdir(fin_dir):
		n = 0
		if os.path.isdir(os.path.join(fin_dir,cat)):
			for f in os.listdir(os.path.join(fin_dir,cat)):
				n += 1
			fout.write('{} {}\n'.format(cat,n))
	fout.close()

#count('data\\corpus\\en\\20newsgroups\\src\\train')
#count('data\\corpus\\en\\20newsgroups\\src\\test')
#count2('data\\corpus\\zh\\FudanCorp\\src\\train')
#count2('data\\corpus\\zh\\FudanCorp\\src\\test')

#raw_MioChnCorp('data\\corpus\\zh\\MioChnCorp\\src\\all.txt')
#raw_Yelp9('data\\corpus\en\\Yelp9\\src\\yelp_academic_dataset_review.json')
#raw_20newsgroup('data\\corpus\en\\20newsgroups\\src')
#raw_20newsgroup2('data\\corpus\en\\20newsgroups\\src')

#subset_A('data\\corpus\\en\\Yelp9','exp\\main\\exp01',[1,2,3])
#subset_A('data\\corpus\\zh\\MioChnCorp','exp\\main\\exp02',[1])
#ttsep_A()

#raw_FudanCorp('data\\corpus\\zh\\FudanCorp\\src')
#seg_FudanCorp('data\\corpus\\zh\\FudanCorp\\src\\data.train.1','exp\\test\\data\\data.train')
#seg_FudanCorp('data\\corpus\\zh\\FudanCorp\\src\\data.test.1','exp\\test\\data\\data.test')
#num_FudanCorp('exp\\test\\data\\data.train.2',2)
#num_FudanCorp('exp\\test\\data\\data.test.2',2)
#num_FudanCorp('exp\\main\\exp02\\B1\\data\\data.B1.train',0)
#num_FudanCorp('exp\\main\\exp02\\B1\\data\\data.B1.test',0)
#num_FudanCorp('exp\\main\\exp02\\B2\\data\\data.B2.train',1)
#num_FudanCorp('exp\\main\\exp02\\B2\\data\\data.B2.test',1)
#num_FudanCorp('exp\\main\\exp02\\B3\\data\\data.B3.train',2)
#num_FudanCorp('exp\\main\\exp02\\B3\\data\\data.B3.test',2)