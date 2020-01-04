# -*- coding: utf-8 -*-

import logging
import os
import shutil
import json
import numpy as np

from gensim.models.word2vec import LineSentence, Word2Vec
from gensim.models.doc2vec import TaggedDocument, Doc2Vec
from gensim.models.keyedvectors import KeyedVectors
from sklearn.feature_extraction.text import CountVectorizer, TfidfVectorizer
from sklearn.decomposition import TruncatedSVD, LatentDirichletAllocation
from sklearn import preprocessing

from CopeOpiVec import CopeOpiVec

DEFAULT_MINCOUNT = 5
DEFAULT_VECSIZE = 150
DEFAULT_ITER = 50

def build_char2idx(xs_list, fout=False):
	map_char2idx = {'': 0}
	for xs in xs_list:
		for x in xs:
			for ch in x:
				if ch not in map_char2idx:
					map_char2idx[ch] = len(map_char2idx)
	if fout:
		with open(fout, 'w', encoding='utf8') as f:
			for ch,idx in map_char2idx.items():
				f.write('{} {}\n'.format(ch,idx))
	return map_char2idx
### EOF build_char2idx

def build_idx2embedding(map_char2idx, map_char2embedding, fout=False):
	vocab_size = len(map_char2idx)
	embedding_size = len(list(map_char2embedding.values())[0])
	map_idx2embedding = [[0]*embedding_size]*vocab_size
	for ch,idx in map_char2idx.items():
		if ch in map_char2embedding:
			map_idx2embedding[idx] = map_char2embedding[ch]
	if fout:
		with open(fout, 'w', encoding='utf8') as f:
			for idx,embed in enumerate(map_idx2embedding):
				f.write('{} {}\n'.format(idx,embed))
	return map_idx2embedding
### EOF build_idx2embedding

def load_embedding(embd, file, **kwargs):
	if embd=='copeopi':
		return load_copeopi(file, **kwargs)
	else:
		func_name = 'load_'+embd
		return globals()[func_name](file)
### EOF load_embedding

def model_embedding(embd, xs, ys, cls_set, **kwargs):
	if embd=='copeopi':
		map_char2embedding = model_copeopi(xs, ys, cls_set, **kwargs)
	elif embd=='word2vec':
		sorted_xs = [x for x,y in sorted(zip(xs,ys), key=lambda x: x[1])]
		corpus = '\n'.join([' '.join(x) for x in sorted_xs])
		map_char2embedding = model_word2vec(corpus, **kwargs)
	elif embd=='glove':
		sorted_xs = [x for x,y in sorted(zip(xs,ys), key=lambda x: x[1])]
		corpus = '\n'.join([' '.join(x) for x in sorted_xs])
		map_char2embedding = model_glove(corpus, **kwargs)
	return map_char2embedding
### EOF model_embedding

def load_copeopi(file, **kwargs):
	logging.debug('Load CopeOpiVec: {}'.format(file))
	model, copeopi = CopeOpiVec().load(file, **kwargs)
	return copeopi
### EOF load_copeopi

def model_copeopi(xs, ys, cls_set, fout=False, **kwargs):
	model, copeopi = CopeOpiVec().fit(xs, ys, cls_set, **kwargs)
	if fout:
		model.save(fout)
	return copeopi
### EOF model_copeopi

def load_word2vec(file):
	logging.debug('Load Word2vec: {}'.format(file))

	try:
		model = KeyedVectors.load_word2vec_format(file, binary=True)
	except:
		model = KeyedVectors.load_word2vec_format(file, binary=False)
	
	word2vec = {ch:list(model[ch]) for ch in model.vocab.keys()}
	return word2vec
### EOF load_word2vec

def model_word2vec(corpus, fout=False, **kwargs):
	kwargs['size'] = kwargs.get('size',DEFAULT_VECSIZE)
	kwargs['min_count'] = kwargs.get('min_count',DEFAULT_MINCOUNT)
	kwargs['iter'] = kwargs.get('iter',DEFAULT_ITER)
	with open('corpus.tmp', 'w', encoding='utf8') as f:
		f.write(corpus)
	corpus = LineSentence('corpus.tmp')
	model = Word2Vec(corpus, **kwargs)
	word2vec = {ch:list(model[ch]) for ch in model.wv.vocab.keys()}
	os.remove('corpus.tmp')
	if fout:
		model.wv.save_word2vec_format(fout, binary=False)
	return word2vec
### EOF model_word2vec

def load_glove(file):
	logging.debug('Load GloVe: {}'.format(file))
	glove = {}
	with open(file, 'r', encoding='utf8') as f:
		for line in f:
			line = line.strip().split()
			glove[line[0]] = [float(i) for i in line[1:]]
	return glove
### EOF load_glove

def model_glove(corpus, fout=False, **kwargs):
	build_dir = 'data\\embedding\\glove\\build'
	tmp_dir = 'model_glove'
	f_corpus = os.path.join(tmp_dir,'corpus.tmp')
	f_vocab = os.path.join(tmp_dir,'vocab.tmp')
	f_cooccur = os.path.join(tmp_dir,'cooccurrence.bin.tmp')
	f_cooccur_shuf = os.path.join(tmp_dir,'cooccurrence.shuf.bin.tmp')
	f_glove = os.path.join(tmp_dir,'glove') if not fout else fout
	f_overflow = os.path.join(tmp_dir,'overflow')

	verbose = kwargs.get('verbose',0)
	vector_size = kwargs.get('vector_size',DEFAULT_VECSIZE)
	min_count = kwargs.get('min_count',DEFAULT_MINCOUNT)
	iters = kwargs.get('iter',DEFAULT_ITER)

	if not os.path.isdir(tmp_dir):
		os.mkdir(tmp_dir)
	with open(f_corpus, 'w', encoding='utf8') as f:
		f.write(corpus)
	os.system('{}\\vocab_count.exe -verbose {} -min-count {} < {} > {}'
		.format(build_dir,verbose,min_count,f_corpus,f_vocab))
	os.system('{}\\cooccur.exe -verbose {} -vocab-file {} -overflow-file {} < {} > {}'
		.format(build_dir,verbose,f_vocab,f_overflow,f_corpus,f_cooccur))
	os.system('{}\\shuffle.exe -verbose {} < {} > {}'
		.format(build_dir,verbose,f_cooccur,f_cooccur_shuf))
	os.system('{}\\glove.exe -verbose {} -vector-size {} -iter {} -save-file {} -vocab-file {} -input-file {}'
		.format(build_dir,verbose,vector_size,iters,f_glove,f_vocab,f_cooccur_shuf))
	
	if os.path.isfile(f_glove):
		os.remove(f_glove)
	os.rename(f_glove+'.txt', f_glove)
	glove = load_glove(f_glove)
	shutil.rmtree(tmp_dir)
	return glove
### EOF model_glove

def pad_sequences(x, maxlen_x, padding=0):
	padded_x = x[:maxlen_x]
	for i in range(len(x),maxlen_x):
		padded_x.append(padding)
	return padded_x
### EOF pad_sequences

def vectorize_embedding(xs, map_char2embedding, maxlen_x=-1):
	try:
		embedding_size = len(list(map_char2embedding.values())[0])
		oov_embedding = [0 for i in range(embedding_size)]
	except:
		oov_embedding = 0
	vectorized_xs = []
	for x in xs:
		vectorized_x = [map_char2embedding[ch] if ch in map_char2embedding else oov_embedding for ch in x]
		if maxlen_x>0:
			vectorized_x = pad_sequences(vectorized_x, maxlen_x, oov_embedding)
		vectorized_xs.append(vectorized_x)
	return vectorized_xs
### EOF vectorize_embedding

def reshape1d_embedding(embd, xs, mode='default', **kwargs):
	if mode=='default':
		if embd=='copeopi':
			if len(xs[0][0])==1:
				mode = 'mean'
			else:
				mode = 'norm_mean'
		elif embd=='word2vec':
			mode = 'norm_mean'
		elif embd=='glove':
			mode = 'norm_mean'
	if mode=='mean':
		reshaped_xs = [np.mean(x, axis=0) for x in xs]
	elif mode=='norm_mean':
		reshaped_xs = [np.mean(x, axis=0) for x in xs]
		reshaped_xs = preprocessing.normalize(reshaped_xs)
	elif mode=='sum':
		reshaped_xs = [np.sum(x, axis=0) for x in xs]
	elif mode=='norm_sum':
		reshaped_xs = [np.sum(x, axis=0) for x in xs]
		reshaped_xs = preprocessing.normalize(reshaped_xs)
	elif mode=='flatten':
		reshaped_xs = []
		for x in xs:
			unit = kwargs['len']//len(x[0])
			unit = max(len(x)//unit,1)
			unit = list(range(0,len(x)+1,unit))
			unit[-1] = len(x)
			reshaped_x = []
			for i,r in enumerate(unit):
				if i!=len(unit)-1:
					reshaped_x.extend(np.mean(x[unit[i]:unit[i+1]], axis=0))
			reshaped_x = pad_sequences(reshaped_x, kwargs['len'], 0)
			reshaped_xs.append(reshaped_x)
		preprocessing.normalize(reshaped_xs)

	return reshaped_xs
### EOF flatten_embedding

def load_doc2vec(file):
	logging.debug('Load Doc2vec: {}'.format(file))
	model = Doc2Vec.load(file)
	return model
### EOF load_doc2vec

def vectorize_doc2vec(xs, ys=False, model=False, fout=False, **kwargs):
	if ys:
		kwargs['size'] = kwargs.get('size',50)
		kwargs['min_count'] = kwargs.get('min_count',DEFAULT_MINCOUNT)
		kwargs['iter'] = kwargs.get('iter',DEFAULT_ITER)
		corpus = [TaggedDocument(x,[y]) for x,y in zip(xs,ys)]
		model = Doc2Vec(corpus, **kwargs)
		doc2veced_xs = [model.infer_vector(x) for x in xs]
		if fout:
			model.save(fout)
		return model, doc2veced_xs
	elif model:
		doc2veced_xs = [model.infer_vector(x) for x in xs]
		return doc2veced_xs
### EOF vectorize_doc2vec

def vectorize_ngram(xs, vectorizer=False, **kwargs):
	if not vectorizer:
		kwargs['min_df'] = kwargs.get('min_df',0.001)
		vectorizer = CountVectorizer(tokenizer=lambda x: x.split(), **kwargs)
		vectorized_xs = vectorizer.fit_transform([' '.join(x) for x in xs]).toarray()
		return vectorizer, vectorized_xs
	else:
		vectorized_xs = vectorizer.transform([' '.join(x) for x in xs]).toarray()
		return vectorized_xs
### EOF vectorize_ngram

def vectorize_tfidf(xs, vectorizer=False, **kwargs):
	if not vectorizer:
		kwargs['min_df'] = kwargs.get('min_df',0.001)
		vectorizer = TfidfVectorizer(tokenizer=lambda x: x.split(), **kwargs)
		vectorized_xs = vectorizer.fit_transform([' '.join(x) for x in xs]).toarray()
		return vectorizer, vectorized_xs
	else:
		vectorized_xs = vectorizer.transform([' '.join(x) for x in xs]).toarray()
		return vectorized_xs
### EOF vectorize_tfidf

def vectorize_lsi(xs, n_components=300, lsi=False, vectorizer=False, **kwargs):
	if not lsi:
		mat = kwargs.get('mat','ngram')
		if 'mat' in kwargs:
			del kwargs['mat']
		vectorizer,xs = globals()['vectorize_'+mat](xs)
		lsi = TruncatedSVD(n_components=n_components, **kwargs)
		vectorized_xs = lsi.fit_transform(xs)
		return lsi, vectorizer, vectorized_xs
	else:
		xs = vectorizer.transform([' '.join(x) for x in xs]).toarray()
		vectorized_xs = lsi.transform(xs)
		return vectorized_xs
### EOF vectorize_lsi

def vectorize_lda(xs, n_components=300, lda=False, vectorizer=False, **kwargs):
	if not lda:
		mat = kwargs.get('mat','ngram')
		if 'mat' in kwargs:
			del kwargs['mat']
		vectorizer,xs = globals()['vectorize_'+mat](xs)
		lda = LatentDirichletAllocation(n_components=n_components, n_jobs=-1, learning_method='batch', **kwargs)
		vectorized_xs = lda.fit_transform(xs)
		return lda, vectorizer, vectorized_xs
	else:
		xs = vectorizer.transform([' '.join(x) for x in xs]).toarray()
		vectorized_xs = lda.transform(xs)
		return vectorized_xs
### EOF vectorize_lda

'''
def load_keyedvector(file, binary=False):
	try:
		model = keyedvectors.KeyedVectors.load_word2vec_format(file, binary=True)
	except:
		model = keyedvectors.KeyedVectors.load_word2vec_format(file, binary=False)
	return model
### EOF load_keyedvector

def model_glove_python(xs, fout=False, **kwargs):
	corpus = Corpus()
	corpus.fit(xs)
	if 'no_components' not in kwargs:
		kwargs['no_components'] = 100
	glove = Glove(**kwargs)
	glove.fit(corpus.matrix, epochs=10)
	glove.add_dictionary(corpus.dictionary)

	map_char2embedding = {'<N/A>': np.zeros(len(glove.word_vectors[0]))}
	for k,v in corpus.dictionary.items():
		map_char2embedding[k] = glove.word_vectors[v]

	if fout:
		with open(fout, 'w', encoding='utf8') as f:
			f.write('{} {}\n'.format(len(map_char2embedding), len(map_char2embedding['<N/A>'])))
			for ch,vec in map_char2embedding.items():
				f.write('{}\t{}\n'.format(ch, ' '.join([str(i) for i in vec])))
	return map_char2embedding
### EOF model_glove_python
'''