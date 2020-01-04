# -*- coding: utf-8 -*-

import logging
import warnings
import os
import sys
import time
warnings.filterwarnings('ignore', module='gensim')

from sklearn.model_selection import GridSearchCV
from sklearn.neighbors import KNeighborsClassifier
#from sklearn.tree import DecisionTreeClassifier
from sklearn.naive_bayes import MultinomialNB, GaussianNB
from sklearn.svm import LinearSVC
from sklearn.linear_model import LogisticRegression
from sklearn.neural_network import MLPClassifier
#from sklearn.discriminant_analysis import LinearDiscriminantAnalysis

from sklearn.preprocessing import LabelEncoder
from sklearn.metrics import confusion_matrix, accuracy_score, precision_recall_fscore_support
from sklearn import preprocessing

from proc_data import *
from proc_feature import *

def set_log(flog, sh_level=logging.DEBUG, fh_level=logging.INFO):
	os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'
	logger = logging.getLogger('gensim')
	logger.setLevel(logging.ERROR)

	logger = logging.getLogger('')
	logger.handlers = []
	logger.setLevel(logging.INFO)
	fm = logging.Formatter(
		fmt='%(asctime)s [%(levelname)s] %(message)s',
		datefmt='%Y-%m-%d %H:%M:%S')
	sh = logging.StreamHandler()
	sh.setFormatter(fm)
	sh.setLevel(sh_level)
	logger.addHandler(sh)
	fh = logging.FileHandler(flog, mode='w', encoding='utf-8')
	fh.setFormatter(fm)
	fh.setLevel(fh_level)
	logger.addHandler(fh)
### EOF set_log

def report_exp(reports, fout):
	with open(fout, 'w', encoding='utf8') as f:
		for i_report,report in enumerate(reports):
			f.write('{}\n'.format(i_report)) if i_report!=len(reports)-1 else f.write('avg\n')
			if 'feature_meta' in report:
				f.write('{}\n'.format(report['feature_meta']))
			f.write('{}\n'.format(report['support']))
			f.write('{}\n'.format(report['conf_mat']))
			f.write('{}\n'.format(report['acc']))
			f.write('{}\t{}\t{}\n'.format(report['macro_p'],report['micro_p'],report['p']))
			f.write('{}\t{}\t{}\n'.format(report['macro_r'],report['micro_r'],report['r']))
			f.write('{}\t{}\t{}\n'.format(report['macro_f1'],report['micro_f1'],report['f1']))
			f.write('{}\n'.format(report['exec_time']))
### EOF report_exp

def run_exp(ecfg, xs_train, ys_train, xs_test, ys_test, cls_set):
	# [init_feature,training,total]
	exec_time = []
	# Training
	# process x
	if ecfg['feature']['type'] in ['copeopi','word2vec','glove']:
		if 'src' in ecfg['feature']:
			start_time0 = time.clock()
			map_char2embedding = load_embedding(ecfg['feature']['type'], ecfg['feature']['src'], **ecfg['feature']['init_kwargs'])
			xs_train = vectorize_embedding(xs_train, map_char2embedding)
			xs_train = reshape1d_embedding(ecfg['feature']['type'], xs_train, **ecfg['feature']['reshape_kwargs'])
			end_time0 = time.clock()
		else:
			start_time0 = time.clock()
			map_char2embedding = model_embedding(ecfg['feature']['type'], xs_train, ys_train, cls_set, **ecfg['feature']['init_kwargs'])
			xs_train = vectorize_embedding(xs_train, map_char2embedding)
			xs_train = reshape1d_embedding(ecfg['feature']['type'], xs_train, **ecfg['feature']['reshape_kwargs'])
			end_time0 = time.clock()
		xs_test = vectorize_embedding(xs_test, map_char2embedding)
		xs_test = reshape1d_embedding(ecfg['feature']['type'], xs_test, **ecfg['feature']['reshape_kwargs'])
		feature_meta = [len(xs_test[0]), len(map_char2embedding)]
		del map_char2embedding
	elif ecfg['feature']['type']=='doc2vec':
		if 'src' in ecfg['feature']:
			start_time0 = time.clock()
			model = load_doc2vec(ecfg['feature']['src'])
			xs_train = vectorize_doc2vec(xs_train, model=model)
			xs_train = preprocessing.normalize(xs_train)
			end_time0 = time.clock()
		else:
			start_time0 = time.clock()
			model,xs_train = vectorize_doc2vec(xs_train, ys=ys_train, **ecfg['feature']['init_kwargs'])
			xs_train = preprocessing.normalize(xs_train)
			end_time0 = time.clock()
		xs_test = vectorize_doc2vec(xs_test, model=model)
		xs_test = preprocessing.normalize(xs_test)
		feature_meta = [len(xs_test[0])]
		del model
	elif ecfg['feature']['type']=='ngram':
		start_time0 = time.clock()
		vectorizer,xs_train = vectorize_ngram(xs_train, **ecfg['feature']['init_kwargs'])
		xs_train = preprocessing.normalize(xs_train)
		end_time0 = time.clock()
		xs_test = vectorize_ngram(xs_test, vectorizer=vectorizer)
		xs_test = preprocessing.normalize(xs_test)
		feature_meta = [len(xs_test[0])]
		del vectorizer
	elif ecfg['feature']['type']=='tfidf':
		start_time0 = time.clock()
		vectorizer,xs_train = vectorize_tfidf(xs_train, **ecfg['feature']['init_kwargs'])
		xs_train = preprocessing.normalize(xs_train)
		end_time0 = time.clock()
		xs_test = vectorize_tfidf(xs_test, vectorizer=vectorizer)
		xs_test = preprocessing.normalize(xs_test)
		feature_meta = [len(xs_test[0])]
		del vectorizer
	elif ecfg['feature']['type']=='lsi':
		start_time0 = time.clock()
		lsi,vectorizer,xs_train = vectorize_lsi(xs_train, **ecfg['feature']['init_kwargs'])
		xs_train = preprocessing.normalize(xs_train)
		end_time0 = time.clock()
		xs_test = vectorize_lsi(xs_test, lsi=lsi, vectorizer=vectorizer)
		xs_test = preprocessing.normalize(xs_test)
		feature_meta = [len(xs_test[0])]
		del vectorizer,lsi
	xs_train = np.array(xs_train)
	xs_test = np.array(xs_test)
	# process y
	ys_enc = LabelEncoder().fit(cls_set)
	ys_train = ys_enc.transform(ys_train)
	ys_test = ys_enc.transform(ys_test)

	if ecfg['classifier']['type']=='kNN':
		ecfg['classifier']['init_kwargs']['n_neighbors'] = ecfg['classifier']['init_kwargs'].get('n_neighbors', 40)
		clf = KNeighborsClassifier(**ecfg['classifier']['init_kwargs'])
	elif ecfg['classifier']['type']=='NB':
		if ecfg['feature']['type'] in ['copeopi','word2vec','glove','lsi']:
			clf = GaussianNB(**ecfg['classifier']['init_kwargs'])
		elif  ecfg['feature']['type'] in ['ngram','tfidf']:
			clf = MultinomialNB(**ecfg['classifier']['init_kwargs'])
	elif ecfg['classifier']['type']=='LR':
		clf = LogisticRegression(**ecfg['classifier']['init_kwargs'])
	elif ecfg['classifier']['type']=='SVM':
		#param_grid = {'C': [2**i for i in range(-10,11,1)]}
		#clf = GridSearchCV(LinearSVC(), param_grid, n_jobs=-1)
		#clf.fit(xs_train, ys_train)
		#logging.info(clf.best_params_['C'])
		#ecfg['classifier']['init_kwargs']['C'] = ecfg['classifier']['init_kwargs'].get('C', clf.best_params_['C'])
		clf = LinearSVC(**ecfg['classifier']['init_kwargs'])
	elif ecfg['classifier']['type']=='NN':
		clf = MLPClassifier(**ecfg['classifier']['init_kwargs'])

	# train
	start_time1 = time.clock()
	clf.fit(xs_train, ys_train, **ecfg['classifier']['fit_kwargs'])
	end_time1 = time.clock()
	exec_time = [
		end_time0-start_time0,
		end_time1-start_time1,
		end_time0-start_time0+end_time1-start_time1
		]
	del xs_train,ys_train

	# predict
	predictions = clf.predict(xs_test, **ecfg['classifier']['predict_kwargs'])

	# Evaluate
	prf1 = precision_recall_fscore_support(ys_test, predictions, average=None, labels=ys_enc.transform(cls_set))
	macro_prf1 = precision_recall_fscore_support(ys_test, predictions, average='macro', labels=ys_enc.transform(cls_set))
	micro_prf1 = precision_recall_fscore_support(ys_test, predictions, average='micro', labels=ys_enc.transform(cls_set))
	report = { 
		'feature_meta': feature_meta,
		'support': prf1[3].tolist(),
		'conf_mat': confusion_matrix(ys_test, predictions).tolist(),
		'acc': accuracy_score(ys_test, predictions),
		'p': prf1[0].tolist(),
		'r': prf1[1].tolist(),
		'f1': prf1[2].tolist(),
		'macro_p': macro_prf1[0],
		'macro_r': macro_prf1[1],
		'macro_f1': macro_prf1[2],
		'micro_p': micro_prf1[0],
		'micro_r': micro_prf1[1],
		'micro_f1': micro_prf1[2],
		'exec_time': exec_time
		}

	logging.info('Feature meta: {}'.format(feature_meta))
	return report
### EOF run_exp

def init_feature(ecfg, xs_train, ys_train, xs_test, ys_test, cls_set):
	logging.info('Init Feature')
	# process x
	if ecfg['feature']['type'] in ['copeopi','word2vec','glove']:
		if 'src' in ecfg['feature']:
			start_time0 = time.clock()
			map_char2embedding = load_embedding(ecfg['feature']['type'], ecfg['feature']['src'], **ecfg['feature']['init_kwargs'])
			xs_train = vectorize_embedding(xs_train, map_char2embedding)
			xs_train = reshape1d_embedding(ecfg['feature']['type'], xs_train, **ecfg['feature']['reshape_kwargs'])
			end_time0 = time.clock()
		else:
			start_time0 = time.clock()
			map_char2embedding = model_embedding(ecfg['feature']['type'], xs_train, ys_train, cls_set, **ecfg['feature']['init_kwargs'])
			xs_train = vectorize_embedding(xs_train, map_char2embedding)
			xs_train = reshape1d_embedding(ecfg['feature']['type'], xs_train, **ecfg['feature']['reshape_kwargs'])
			end_time0 = time.clock()
		xs_test = vectorize_embedding(xs_test, map_char2embedding)
		xs_test = reshape1d_embedding(ecfg['feature']['type'], xs_test, **ecfg['feature']['reshape_kwargs'])
		feature_meta = [len(xs_test[0]), len(map_char2embedding)]
		del map_char2embedding
	elif ecfg['feature']['type']=='doc2vec':
		if 'src' in ecfg['feature']:
			start_time0 = time.clock()
			model = load_doc2vec(ecfg['feature']['src'])
			xs_train = vectorize_doc2vec(xs_train, model=model)
			xs_train = preprocessing.normalize(xs_train)
			end_time0 = time.clock()
		else:
			start_time0 = time.clock()
			model,xs_train = vectorize_doc2vec(xs_train, ys=ys_train, **ecfg['feature']['init_kwargs'])
			xs_train = preprocessing.normalize(xs_train)
			end_time0 = time.clock()
		xs_test = vectorize_doc2vec(xs_test, model=model)
		xs_test = preprocessing.normalize(xs_test)
		feature_meta = [len(xs_test[0])]
		del model
	elif ecfg['feature']['type']=='ngram':
		start_time0 = time.clock()
		vectorizer,xs_train = vectorize_ngram(xs_train, **ecfg['feature']['init_kwargs'])
		end_time0 = time.clock()
		xs_test = vectorize_ngram(xs_test, vectorizer=vectorizer)
		feature_meta = [len(xs_test[0])]
		del vectorizer
	elif ecfg['feature']['type']=='tfidf':
		start_time0 = time.clock()
		vectorizer,xs_train = vectorize_tfidf(xs_train, **ecfg['feature']['init_kwargs'])
		xs_train = preprocessing.normalize(xs_train)
		end_time0 = time.clock()
		xs_test = vectorize_tfidf(xs_test, vectorizer=vectorizer)
		xs_test = preprocessing.normalize(xs_test)
		feature_meta = [len(xs_test[0])]
		del vectorizer
	elif ecfg['feature']['type']=='lsi':
		start_time0 = time.clock()
		lsi,vectorizer,xs_train = vectorize_lsi(xs_train, **ecfg['feature']['init_kwargs'])
		xs_train = preprocessing.normalize(xs_train)
		end_time0 = time.clock()
		xs_test = vectorize_lsi(xs_test, lsi=lsi, vectorizer=vectorizer)
		xs_test = preprocessing.normalize(xs_test)
		feature_meta = [len(xs_test[0])]
		del vectorizer,lsi
	xs_train = np.array(xs_train)
	xs_test = np.array(xs_test)
	# process y
	ys_enc = LabelEncoder().fit(cls_set)
	ys_train = ys_enc.transform(ys_train)
	ys_test = ys_enc.transform(ys_test)

	exec_time0 = end_time0-start_time0
	return xs_train, ys_train, xs_test, ys_test, feature_meta, exec_time0
### EOF run_exp

def train_and_test(ecfg, xs_train, ys_train, xs_test, ys_test, cls_set):
	# init classifier
	logging.debug('Init classifier')
	if ecfg['classifier']['type']=='kNN':
		ecfg['classifier']['init_kwargs']['n_neighbors'] = ecfg['classifier']['init_kwargs'].get('n_neighbors', 40)
		clf = KNeighborsClassifier(**ecfg['classifier']['init_kwargs'])
	elif ecfg['classifier']['type']=='NB':
		if ecfg['feature']['type'] in ['copeopi','word2vec','glove','lsi']:
			clf = GaussianNB(**ecfg['classifier']['init_kwargs'])
		elif  ecfg['feature']['type'] in ['ngram','tfidf']:
			clf = MultinomialNB(**ecfg['classifier']['init_kwargs'])
	elif ecfg['classifier']['type']=='LR':
		clf = LogisticRegression(**ecfg['classifier']['init_kwargs'])
	elif ecfg['classifier']['type']=='SVM':
		#param_grid = {'C': [2**i for i in range(-10,11,1)]}
		#clf = GridSearchCV(LinearSVC(), param_grid, n_jobs=-1)
		#clf.fit(xs_train, ys_train)
		#logging.info(clf.best_params_['C'])
		#ecfg['classifier']['init_kwargs']['C'] = ecfg['classifier']['init_kwargs'].get('C', clf.best_params_['C'])
		clf = LinearSVC(**ecfg['classifier']['init_kwargs'])
	elif ecfg['classifier']['type']=='NN':
		clf = MLPClassifier(**ecfg['classifier']['init_kwargs'])

	# train
	start_time1 = time.clock()
	clf.fit(xs_train, ys_train, **ecfg['classifier']['fit_kwargs'])
	end_time1 = time.clock()
	exec_time1 = end_time1-start_time1
	del xs_train,ys_train

	# predict
	predictions = clf.predict(xs_test, **ecfg['classifier']['predict_kwargs'])

	# Evaluate
	prf1 = precision_recall_fscore_support(ys_test, predictions, average=None, labels=LabelEncoder().fit_transform(cls_set))
	macro_prf1 = precision_recall_fscore_support(ys_test, predictions, average='macro', labels=LabelEncoder().fit_transform(cls_set))
	micro_prf1 = precision_recall_fscore_support(ys_test, predictions, average='micro', labels=LabelEncoder().fit_transform(cls_set))
	report = { 
		'support': prf1[3].tolist(),
		'conf_mat': confusion_matrix(ys_test, predictions).tolist(),
		'acc': accuracy_score(ys_test, predictions),
		'p': prf1[0].tolist(),
		'r': prf1[1].tolist(),
		'f1': prf1[2].tolist(),
		'macro_p': macro_prf1[0],
		'macro_r': macro_prf1[1],
		'macro_f1': macro_prf1[2],
		'micro_p': micro_prf1[0],
		'micro_r': micro_prf1[1],
		'micro_f1': micro_prf1[2],
		}

	return report, exec_time1
### EOF run_exp


if __name__ == '__main__':

	# python main.py data.cfg exp.cfg
	if len(sys.argv)!=3:
		print('*** INPUT EXAMPLE: python main.py data.cfg exp.cfg ***')
		sys.exit()
	dcfgs,ecfgs = load_config(sys.argv[1], sys.argv[2])

	for dcfg in dcfgs:
		work_dir = dcfg['work_dir']
		set_log(os.path.join(work_dir,'{}.log'.format(os.path.splitext(os.path.basename(sys.argv[2]))[0])))
		if not os.path.isdir(os.path.join(work_dir,'results')):
			os.mkdir(os.path.join(work_dir,'results'))

		if dcfg['mode']=='cv':
			# load data
			if len(dcfg['files'])==1:
				kfold = dcfg['kfold']
				xs_folds, ys_folds = kfold_data(*load_data(dcfg['files'][0]), kfold, fout_dir=work_dir)
			else:
				kfold = len(dcfg['files'])
				xs_folds, ys_folds = zip(*[load_data(file) for file in dcfg['files']])
				xs_folds, ys_folds = list(xs_folds), list(ys_folds)
			# preproc
			xs_folds = [preproc_xs(xs_fold, dcfg['lang'], **dcfg['preproc']) for xs_fold in xs_folds]
	
			logging.info('{}'.format(','.join(['{:5d}'.format((len(xs_fold))) for xs_fold in xs_folds])))
			ys_folds = [[y for x,y in zip(xs_fold,ys_fold) if x!=[]] for xs_fold,ys_fold in zip(xs_folds,ys_folds)]
			xs_folds = [[x for x in xs_fold if x!=[]] for xs_fold in xs_folds]
			logging.info('{}'.format(','.join(['{:5d}'.format((len(xs_fold))) for xs_fold in xs_folds])))
			logging.info('{}'.format(dcfg['cls_set']))
			logging.info('{}\n'.format(list(LabelEncoder().fit_transform(dcfg['cls_set']))))
			# exps
			for id_exp,ecfg in enumerate(ecfgs):
				logging.info('START EXP {}/{}: {}'.format(id_exp+1,len(ecfgs),ecfg['name']))
				
				if 'fout' in ecfg['feature']['init_kwargs']:
					feature_fout = os.path.join(work_dir,'data',os.path.splitext(os.path.basename(ecfg['feature']['init_kwargs']['fout']))[0])

				reports = []
				for k in range(kfold):
					xs_test, ys_test = xs_folds[k], ys_folds[k]
					xs_train = [x for i,xs in enumerate(xs_folds) if i!=k for x in xs]
					ys_train = [y for i,ys in enumerate(ys_folds) if i!=k for y in ys]

					if 'fout' in ecfg['feature']['init_kwargs']:
						ecfg['feature']['init_kwargs']['fout'] = feature_fout+'.fold'+str(k)

					report = run_exp(ecfg, xs_train, ys_train, xs_test, ys_test, dcfg['cls_set'])
					reports.append(report)
					logging.info('Fold {:2d}/{}: {}'.format(k+1,kfold,report['macro_f1']))

				reports.append({
					'support': np.mean([r['support'] for r in reports], axis=0).tolist(),
					'conf_mat': np.mean([r['conf_mat'] for r in reports], axis=0).tolist(),
					'acc': np.mean([r['acc'] for r in reports]),
					'p': np.mean([r['p'] for r in reports], axis=0).tolist(),
					'r': np.mean([r['r'] for r in reports], axis=0).tolist(),
					'f1': np.mean([r['f1'] for r in reports], axis=0).tolist(),
					'macro_p': np.mean([r['macro_p'] for r in reports]),
					'macro_r': np.mean([r['macro_r'] for r in reports]),
					'macro_f1': np.mean([r['macro_f1'] for r in reports]),
					'micro_p': np.mean([r['micro_p'] for r in reports]),
					'micro_r': np.mean([r['micro_r'] for r in reports]),
					'micro_f1': np.mean([r['micro_f1'] for r in reports]),
					'exec_time': np.mean([r['exec_time'] for r in reports], axis=0).tolist()
					})
				fout = os.path.join(work_dir,'results',ecfg['name'])
				report_exp(reports, fout)
				logging.info('FINISH EXP {}/{}: {}'.format(id_exp+1,len(ecfgs),ecfg['name']))
				logging.info('MACRO_F1: {} {}'.format(ecfg['name'],reports[-1]['macro_f1']))
				logging.info('EXE_TIME: {} {} {} {}\n'.format(ecfg['name'],reports[-1]['exec_time'][0],reports[-1]['exec_time'][1],reports[-1]['exec_time'][2]))

		elif dcfg['mode']=='train_test_split':
			# load data
			if len(dcfg['files'])==1:
				xs_train, ys_train, xs_test, ys_test = train_test_split_data(*load_data(dcfg['files'][0]), dcfg['trian_size'], fout_dir=work_dir)
			else:
				xs_train, ys_train = load_data(dcfg['files'][0])
				xs_test, ys_test = load_data(dcfg['files'][1])
			# preproc
			xs_train = preproc_xs(xs_train, dcfg['lang'], **dcfg['preproc'])
			xs_test = preproc_xs(xs_test, dcfg['lang'], **dcfg['preproc'])
			
			logging.info('{:5d},{:5d}'.format(len(xs_train),len(xs_test)))
			ys_train = [y for x,y in zip(xs_train,ys_train) if x!=[]]
			xs_train = [x for x in xs_train if x!=[]]
			ys_test = [y for x,y in zip(xs_test,ys_test) if x!=[]]
			xs_test = [x for x in xs_test if x!=[]]
			logging.info('{:5d},{:5d}'.format(len(xs_train),len(xs_test)))
			logging.info('{}'.format(dcfg['cls_set']))
			logging.info('{}\n'.format(list(LabelEncoder().fit_transform(dcfg['cls_set']))))
			
			# exps
			for id_exp,ecfg in enumerate(ecfgs):
				logging.info('START EXP {}/{}: {}'.format(id_exp+1,len(ecfgs),ecfg['name']))

				if 'fout' in ecfg['feature']['init_kwargs']:
					ecfg['feature']['init_kwargs']['fout'] = os.path.join(work_dir,'data',os.path.splitext(os.path.basename(ecfg['feature']['init_kwargs']['fout']))[0])

				if id_exp==0 or ecfg['name'].split('_')[1:]!=ecfgs[id_exp-1]['name'].split('_')[1:]:
					xs_train_, ys_train_, xs_test_, ys_test_, feature_meta, exec_time0 = init_feature(ecfg, xs_train, ys_train, xs_test, ys_test, dcfg['cls_set'])

				report, exec_time1 = train_and_test(ecfg, xs_train_, ys_train_, xs_test_, ys_test_, dcfg['cls_set'])

				report['feature_meta'] = feature_meta
				report['exec_time'] = [exec_time0, exec_time1, exec_time0+exec_time1]

				fout = os.path.join(work_dir,'results',ecfg['name'])
				report_exp([report], fout)
				logging.info('Feature meta: {}'.format(feature_meta))
				logging.info('FINISH EXP {}/{}: {}'.format(id_exp+1,len(ecfgs),ecfg['name']))
				logging.info('MACRO_F1: {} {}'.format(ecfg['name'],report['macro_f1']))
				logging.info('EXE_TIME: {} {} {} {}\n'.format(ecfg['name'],report['exec_time'][0],report['exec_time'][1],report['exec_time'][2]))
			