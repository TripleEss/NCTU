    # -*- coding: utf-8 -*-

import logging
import os
import json
from itertools import combinations, product
logging.basicConfig(level=logging.INFO,
	format='%(asctime)s [%(levelname)s] (%(funcName)s) %(message)s',
	datefmt='%Y-%m-%d %H:%M:%S')

def powerset(elements):
	return [x for i in range(len(elements)+1) for x in combinations(elements,i)]
### EOF powerset

def pair_subsets(elements, minlen=2):
	subsets = powerset(elements)
	pairs = list(combinations(subsets,2))
	pairs = [[list(p),list(n)] for (p,n) in pairs if p!=() and n!=() and p!=tuple(elements) and n!=tuple(elements) and p!=n]#set(p)&set(n)==set()]
	pairs = sorted(pairs, key=lambda x: (len(x[0])+len(x[1]),-len(x[0])), reverse=True)
	pairs = [pair for pair in pairs if len(pair[0])+len(pair[1])>=minlen]
	return pairs

def pair_nonoverlapping_subsets(elements, minlen=2):
	subsets = powerset(elements)
	pairs = list(combinations(subsets,2))
	pairs = [[list(p),list(n)] for (p,n) in pairs if p!=() and n!=() and p!=tuple(elements) and n!=tuple(elements) and set(p)&set(n)==set()]
	pairs = sorted(pairs, key=lambda x: (len(x[0])+len(x[1]),-len(x[0])), reverse=True)
	pairs = [pair for pair in pairs if len(pair[0])+len(pair[1])>=minlen]
	return pairs

EXP = 'exp\\main'

def config_copeopi():
	exps = []
	for x in ['CT5']:
		for default_pairs,y in zip([['oaa'],['oao'],['oaa','oao']],['A','B','C']):
			for classifier in ['kNN','NB','LR','SVM','NN']:
				exp = {
					'name': '{}_copeopi_00{}{}'.format(classifier,y,x),
					'feature': {
						'type': 'copeopi',
						'init_kwargs': {},
						'reshape_kwargs': {
						}
					},
					'classifier': {
						'type': classifier,
						'init_kwargs': {},
							'fit_kwargs': {},
							'predict_kwargs': {}
					}
				}
				if classifier=='kNN':
					exp['feature']['init_kwargs']['default_pairs'] = default_pairs
					exp['feature']['init_kwargs']['min_count_mode'] = 'conf' if not x.startswith('CT') else 'count'
					exp['feature']['init_kwargs']['min_count'] = int(x[-1])
					exp['feature']['init_kwargs']['fout'] = 'copeopi{}{}'.format(y,x)
				else:
					exp['feature']['src'] = 'copeopi00{}{}'.format(y,x)
				exps.append(exp)
	logging.info('{}'.format(len(exps)))
	fout = os.path.join(EXP,'ecfg','copeopi','copeopi.ecfg')
	with open(fout, 'w', encoding='utf8') as f:
		f.write(json.dumps(exps, indent=4))

def config_copeopiANTUSD():
	exps = []
	for classifier in ['kNN','NB','LR','SVM','NN']:
		exp = {
			'name': '{}_copeopi_ANTUSD'.format(classifier),
			'feature': {
				'type': 'copeopi',
				'init_kwargs': {},
				'reshape_kwargs': {},
				'src': 'ANTUSD.copeopi'
			},
			'classifier': {
				'type': classifier,
				'init_kwargs': {},
					'fit_kwargs': {},
					'predict_kwargs': {}
				}
			}
		exps.append(exp)
	logging.info('{}'.format(len(exps)))
	fout = os.path.join(EXP,'ecfg','copeopi','copeopiANTUSD.ecfg')
	with open(fout, 'w', encoding='utf8') as f:
		f.write(json.dumps(exps, indent=4))

def config_copeopi_advanced(expname):
	other_pairss = {
		'A3a': pair_subsets([0,1,2]),
		'A3': powerset(pair_subsets([0,1,2]))[1:],

		#'A4a': pair_nonoverlapping_subsets([1,2,3,4,5]),
		#'A4b': pair_subsets([1,2,3,4,5]),
		#'A4': powerset(pair_subsets([1,2,3,4,5]))[1:],

		#'01B1a': [[[i],[j]] for i in range(20) for j in range(i+1,20)],
		#'01B1a': [[[i],[j]] for i in range(20) for j in range(i+1,20)],

		#'01B2a': [[[i],[j]] for i in range(7) for j in range(i+1,7)],
		#'01B2b': pair_nonoverlapping_subsets([0,1,2,3,4,5,6]),

		#'01B3a': pair_nonoverlapping_subsets([0,1,2,3,4]),
		#'01B3b': pair_subsets([0,1,2,3,4]),
		
		#'01B4a': pair_subsets([0,1,2,3]),
		
		#'02B1a': [[[i],[j]] for i in range(20) for j in range(i+1,20)],
		#'02B2a': [[[i],[j]] for i in range(9) for j in range(i+1,9)],
		#'02B2b': pair_nonoverlapping_subsets([0,1,2,3,4,5,6,7,8]),
		#'02B3a': [[[i],[j]] for i in range(11) for j in range(i+1,11)]
	}
	exps = []
	for x in ['000']:#,'CF3','CF5','CT3','CT5']:
		for y,other_pairs in enumerate(other_pairss[expname]):
			for classifier in ['SVM']:#['kNN','GNB','BNB','LR','SVM','NN']:
				exp = {
					'name': '{}_copeopi_{:03d}{}'.format(classifier,y+1,x),
					'feature': {
						'type': 'copeopi',
						'init_kwargs': {},
						'reshape_kwargs': {}
					},
					'classifier': {
						'type': classifier,
						'init_kwargs': {},
						'fit_kwargs': {},
						'predict_kwargs': {}
					}
				}
				if classifier=='SVM':
					exp['feature']['init_kwargs']['min_count_mode'] = 'conf' if not x.startswith('CT') else 'count'
					exp['feature']['init_kwargs']['min_count'] = int(x[-1])
					exp['feature']['init_kwargs']['default_pairs'] = []
					exp['feature']['init_kwargs']['other_pairs'] = list(other_pairs)
					#exp['feature']['init_kwargs']['fout'] = 'copeopi{:03d}{}'.format(y+1,x)
				else:
					exp['feature']['src'] = 'copeopi{:03d}{}'.format(y+1,x)
				exps.append(exp)
	print(len(other_pairss[expname]),len(exps))
	fout = os.path.join(EXP,'ecfg','copeopi','copeopi{}.ecfg'.format(expname))
	with open(fout, 'w', encoding='utf8') as f:
		f.write(json.dumps(exps, indent=4))
	fout = os.path.join(EXP,'ecfg','copeopi','copeopi{}.vec'.format(expname))
	with open(fout, 'w', encoding='utf8') as f:
		for p in other_pairss[expname]:
			f.write('{}\n'.format(list(p)))

def config_word2vec():	
	for x in [200]:
		exps = []
		for classifier in ['kNN','NB','LR','SVM','NN']:
			exp = {
				'name': '{}_word2vec_{}'.format(classifier,x),
				'feature': {
					'type': 'word2vec',
					'init_kwargs': {},
					'reshape_kwargs': {}
					},
					'classifier': {
						'type': classifier,
						'init_kwargs': {},
							'fit_kwargs': {},
							'predict_kwargs': {}
					}
				}
			if classifier=='kNN':
				exp['feature']['init_kwargs']['size'] = x
				exp['feature']['init_kwargs']['fout'] = 'word2vec{}'.format(x)
			else:
				exp['feature']['src'] = 'word2vec{}'.format(x)
			exps.append(exp)
		logging.info('{}'.format(len(exps)))
		fout = os.path.join(EXP,'ecfg','word2vec','word2vec{}.ecfg'.format(x))
		with open(fout, 'w', encoding='utf8') as f:
			f.write(json.dumps(exps, indent=4))

def config_glove():
	for x in [200]:
		exps = []
		for classifier in ['kNN','NB','LR','SVM','NN']:
			exp = {
				'name': '{}_glove_{}'.format(classifier,x),
				'feature': {
					'type': 'glove',
					'init_kwargs': {},
					'reshape_kwargs': {}
					},
					'classifier': {
						'type': classifier,
						'init_kwargs': {},
							'fit_kwargs': {},
							'predict_kwargs': {}
					}
				}
			if classifier=='kNN':
				exp['feature']['init_kwargs']['vector_size'] = x
				exp['feature']['init_kwargs']['fout'] = 'glove{}'.format(x)
			else:
				exp['feature']['src'] = 'glove{}'.format(x)
			exps.append(exp)
		logging.info('{}'.format(len(exps)))
		fout = os.path.join(EXP,'ecfg','glove','glove{}.ecfg'.format(x))
		with open(fout, 'w', encoding='utf8') as f:
			f.write(json.dumps(exps, indent=4))

def config_ngram():
	for x in [0.001]:
		exps = []
		for feature in ['ngram','tfidf']:
			for classifier in ['kNN','NB','LR','SVM','NN']:
				exp = {
					'name': '{}_{}_{:03d}'.format(classifier,feature,int(x*1000)),
					'feature': {
						'type': feature,
						'init_kwargs': {
							'ngram_range': [1,1],
							'min_df': x
						},
						'reshape_kwargs': {}
					},
					'classifier': {
						'type': classifier,
						'init_kwargs': {},
						'fit_kwargs': {},
						'predict_kwargs': {}
					}
				}
				exps.append(exp)
		logging.info('{}'.format(len(exps)))
		fout = os.path.join(EXP,'ecfg','ngram','ngram{:03d}.ecfg'.format(int(x*1000)))
		with open(fout, 'w', encoding='utf8') as f:
			f.write(json.dumps(exps, indent=4))

def config_lsi():
	for x in [200]:
		exps = []
		for feature in ['lsa']:
			for mat in ['ngram', 'tfidf']:
				for classifier in ['kNN','NB','LR','SVM','NN']:
					exp = {
						'name': '{}_{}_{}{:03d}'.format(classifier,feature,mat[0],int(x)),
						'feature': {
							'type': feature,
							'init_kwargs': {
								'mat': mat,
								'n_components': x
							},
							'reshape_kwargs': {}
							},
							'classifier': {
								'type': classifier,
								'init_kwargs': {},
									'fit_kwargs': {},
									'predict_kwargs': {}
							}
						}
					exps.append(exp)
		logging.info('{}'.format(len(exps)))
		fout = os.path.join(EXP,'ecfg','ngram','lsi{:03d}.ecfg'.format(int(x)))
		with open(fout, 'w', encoding='utf8') as f:
			f.write(json.dumps(exps, indent=4))

def config_dataA():
	exps = ['exp01','exp02']
	langs = ['en','zh']
	for exp,lang in zip(exps,langs):
		subexps = ['A1','A2','A3']
		work_dirs = [os.path.join(EXP,exp,subexp) for subexp in subexps]
		filess = [[os.path.join(EXP,exp,subexp,'data','data.'+subexp+'.train'), os.path.join(EXP,exp,subexp,'data','data.'+subexp+'.test')] for subexp in subexps]
		#[[os.path.join(EXP,exp,subexp,'data','data.'+subexp+'.fold'+str(i)) for i in range(10)] for subexp in subexps]
		cls_sets = [[0,1],[0,1,2],[1,2,3,4,5]]
		data = []
		for subexp,work_dir,files,cls_set in zip(subexps,work_dirs,filess,cls_sets):
			datum = {
				'mode': 'train_test_split',
				'train_size': 0.5,
				'work_dir': work_dir,
				'files': files,
				'cls_set': cls_set,
				'lang': lang,
				'preproc': {
					'tokenized': True
				}
			}
			data.append(datum)
			fout = os.path.join(EXP,exp,'dcfg',exp+'.'+subexp+'.dcfg')
			with open(fout, 'w', encoding='utf8') as f:
				f.write(json.dumps([datum], indent=4))
			fout = os.path.join('exp\\test','dcfg',exp+'.'+subexp+'.dcfg')
			with open(fout, 'w', encoding='utf8') as f:
				f.write(json.dumps([datum], indent=4))
		fout = os.path.join(EXP,exp,'dcfg',exp+'.A.dcfg')
		with open(fout, 'w', encoding='utf8') as f:
			f.write(json.dumps(data, indent=4))
		fout = os.path.join('exp\\test','dcfg',exp+'.A.dcfg')
		with open(fout, 'w', encoding='utf8') as f:
			f.write(json.dumps(data, indent=4))

def config_dataB01():
	exp = 'exp01'
	lang = 'en'
	subexps = ['B1','B2','B3','B4']
	work_dirs = [os.path.join(EXP,exp,subexp) for subexp in subexps]
	filess = [[os.path.join(EXP,exp,subexp,'data','data.'+subexp+'.train'), os.path.join(EXP,exp,subexp,'data','data.'+subexp+'.test')] for subexp in subexps]
	cls_sets = [
		list(range(20)),
		list(range(7)),
		list(range(5)),
		list(range(4))]
	data = []
	for subexp,work_dir,files,cls_set in zip(subexps,work_dirs,filess,cls_sets):
		datum = {
			'mode': 'train_test_split',
			'train_size': 0.6,
			'work_dir': work_dir,
			'files': files,
			'cls_set': cls_set,
			'lang': lang,
			'preproc': {
				'tokenized': True
			}

		}
		data.append(datum)
		fout = os.path.join(EXP,exp,'dcfg',exp+'.'+subexp+'.dcfg')
		with open(fout, 'w', encoding='utf8') as f:
			f.write(json.dumps([datum], indent=4))
	fout = os.path.join(EXP,exp,'dcfg',exp+'.B.dcfg')
	with open(fout, 'w', encoding='utf8') as f:
		f.write(json.dumps(data, indent=4))

def config_dataB02():
	exp = 'exp02'
	lang = 'zh'
	subexps = ['B1','B2','B3']
	work_dirs = [os.path.join(EXP,exp,subexp) for subexp in subexps]
	filess = [[os.path.join(EXP,exp,subexp,'data','data.'+subexp+'.train'), os.path.join(EXP,exp,subexp,'data','data.'+subexp+'.test')] for subexp in subexps]
	cls_sets = [
		list(range(20)),
		list(range(9)),
		list(range(11))]
	data = []
	for subexp,work_dir,files,cls_set in zip(subexps,work_dirs,filess,cls_sets):
		datum = {
			'mode': 'train_test_split',
			'train_size': 0.5,
			'work_dir': work_dir,
			'files': files,
			'cls_set': cls_set,
			'lang': lang,
			'preproc': {
				'tokenized': True
			}
		}
		data.append(datum)
		fout = os.path.join(EXP,exp,'dcfg',exp+'.'+subexp+'.dcfg')
		with open(fout, 'w', encoding='utf8') as f:
			f.write(json.dumps([datum], indent=4))
	fout = os.path.join(EXP,exp,'dcfg',exp+'.B.dcfg')
	with open(fout, 'w', encoding='utf8') as f:
		f.write(json.dumps(data, indent=4))


if __name__ == '__main__':
	config_dataA()
	#config_dataB01()
	#config_dataB02()

	config_copeopi()
	config_copeopiANTUSD()
	config_word2vec()
	config_glove()
	config_ngram()
	config_lsi()

	#config_copeopiANTUSD()

	#config_copeopi_advanced('A3a')
	#config_copeopi_advanced('A3')

	#config_copeopi_advanced('A4a')
	#config_copeopi_advanced('A4b')
	#config_copeopi_advanced('A4')

	#config_copeopi_advanced('01B1a')
	#config_copeopi_advanced('01B2a')
	#config_copeopi_advanced('01B2b')
	#config_copeopi_advanced('01B3a')
	#config_copeopi_advanced('01B3b')
	#config_copeopi_advanced('01B4a')

	#config_copeopi_advanced('02B1a')
	#config_copeopi_advanced('02B2a')
	#config_copeopi_advanced('02B2b')
	#config_copeopi_advanced('02B3a')


