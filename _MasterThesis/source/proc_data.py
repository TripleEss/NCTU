# -*- coding: utf-8 -*-

import logging
import os
import re
import json
import numpy as np

from gensim.parsing.preprocessing import (preprocess_string,
	strip_tags, strip_multiple_whitespaces, strip_punctuation, strip_numeric, remove_stopwords, strip_short, stem)

def load_config(file_dcfg, file_ecfg):
	logging.info('Load config: {} {}'.format(file_dcfg,file_ecfg))
	with open(file_dcfg, 'r', encoding='utf8') as f:
		dcfgs = json.load(f)
	with open(file_ecfg, 'r', encoding='utf8') as f:
		ecfgs = json.load(f)
	return dcfgs, ecfgs
### EOF load_config

def load_data(file):
	logging.info('Load data: {}'.format(file))
	xs, ys = [], []
	with open(file, 'r', encoding='utf8') as f:
		for line in f:
			line = line.strip().split('\t')
			xs.append(line[1])
			ys.append(int(line[-1]))
	return xs, ys
### EOF load_data

def kfold_data(xs, ys, kfold, fout_dir=False):
	index = np.random.permutation(np.arange(len(xs)))
	spts = np.linspace(0, len(xs), kfold+1, dtype=int)
	
	xs_folds,ys_folds = [],[]
	for spt0,spt1 in zip(spts,spts[1:]):
		xs_folds.append([xs[index[i]] for i in range(spts0,spts1)])
		ys_folds.append([ys[index[i]] for i in range(spts0,spts1)])
	
	if fout_dir:
		data_dir = os.path.join(fout_dir,'data')
		if not os.path.isdir(data_dir):
			os.mkdir(data_dir)
		for i_fold,(xs_fold,ys_fold) in enumerate(zip(xs_folds,ys_folds)):
			fout = os.path.join(data_dir,'data.fold'+str(i_fold))
			with open(fout, 'w', encoding='utf8') as f:
				for i_xy,(x,y) in enumerate(zip(xs_fold,ys_fold)):
					f.write('{}\t{}\t{}\n'.format(i_xy,''.join(x),y))
	return xs_folds, ys_folds
### EOF kfold_data

def train_test_split_data(xs, ys, train_size, fout_dir=None):
	index = np.random.permutation(np.arange(len(xs)))
	spt = round(train_size*len(xs))
	xs_train, ys_train, xs_test, ys_test = [], [], [], []
	for i in range(spt):
		xs_train.append(xs[index[i]])
		ys_train.append(ys[index[i]])
	for i in range(spt, len(xs)):
		xs_test.append(xs[index[i]])
		ys_test.append(ys[index[i]])

	if fout_dir:
		data_dir = os.path.join(fout_dir,'data')
		if not os.path.isdir(data_dir):
			os.mkdir(data_dir)
		fout = os.path.join(data_dir,'data.train')
		with open(fout, 'w', encoding='utf8') as f:
			for i_xy,(x,y) in enumerate(zip(xs_train,ys_train)):
				f.write('{}\t{}\t{}\n'.format(i_xy,''.join(x),y))
		fout = os.path.join(data_dir,'data.test')
		with open(fout, 'w', encoding='utf8') as f:
			for i_xy,(x,y) in enumerate(zip(xs_test,ys_test)):
				f.write('{}\t{}\t{}\n'.format(i_xy,''.join(x),y))
	return xs_train, ys_train, xs_test, ys_test
### EOF train_test_split_data

def preproc_xs(xs, language, **kargs):
	if language=='zh':
		return preproc_xs_zh(xs, **kargs)
	elif language=='en':
		return preproc_xs_en(xs, **kargs)
### EOF preproc_xs

NON_ZH = re.compile(r'[^\u4E00-\u9FFF a-zA-Z]')
def preproc_xs_zh(xs, tokenized=True, rm_stopwords=False):
	if rm_stopwords:
		stopwords = set()
		with open(rm_stopwords, 'r', encoding='utf8') as f:
			for line in f:
				stopwords.add(line.strip())
	proced_xs = []
	for x in xs:
		x = NON_ZH.sub('', x).split()
		if rm_stopwords:
			x = [ch for ch in x if ch not in stopwords]
		if x==[]:
			logging.debug('Empty Sample')
		if tokenized:
			proced_xs.append(x)
		else:
			proced_xs.append([ch0 for ch in x for ch0 in ch])
	return proced_xs
### EOF preproc_xs_zh

def preproc_xs_en(xs, tokenized=True, filters='all'):
	proced_xs = []
	for x in xs:
		if filters=='all':
			x = preprocess_string(x)
		else:
			filters = [globals()[f] for f in filters]
			x = preprocess_string(x, filters=filters)
		if x==[]:
			logging.debug('Empty Sample')
		if tokenized:
			proced_xs.append(x)
		else:
			proced_xs.append([ch0 for ch in x for ch0 in ch])
	return proced_xs
### EOF preproc_xs_en

def get_maxlen_x(xs_list):
	return max([len(x) for xs in xs_list for x in xs])
### EOF get_maxlen_x
