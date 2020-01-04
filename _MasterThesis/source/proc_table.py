# -*- coding: utf-8 -*-

import os
import numpy as np

EXP = 'exp\\main'

class EXP_EXCEL():

	def __init__(self, ftype):
		func_name = 'init_'+ftype
		getattr(self, func_name)()

	def init_copeopi(self):
		self.fout = open(os.path.join(EXP,'tbl','copeopi.tbl'), 'w', encoding='utf8')
		self.files = ['copeopi.log']
		self.exps = ['exp01','exp02']
		self.subexpss = [['A1','A2','A3','A4','B1','B2','B3','B4'],['A1','A2','A3','A4','B1','B2','B3']]
		self.cols = ['kNN','GNB','BNB','NB','ME','SVM','NN','Argmax']
		self.rows = ['copeopi_00{}{}'.format(j,i) for i in ['000','CF3','CF5','CT3','CT5'] for j in ['A','B','C']]

	def init_ngram(self):
		self.fout = open(os.path.join(EXP,'tbl','ngram.tbl'), 'w', encoding='utf8')
		self.files = ['ngram001.log','ngram003.log','ngram005.log']
		self.exps = ['exp01','exp02']
		self.subexpss = [['A1','A2','A3','A4','B1','B2','B3','B4'],['A1','A2','A3','A4','B1','B2','B3']]
		self.cols = ['kNN','GNB','BNB','NB','ME','SVM','NN']
		self.rows = ['{}_{}'.format(i,j) for i in ['ngram','tfidf'] for j in ['001','003','005']]

	def init_word2vec(self):
		self.fout = open(os.path.join(EXP,'tbl','word2vec.tbl'), 'w', encoding='utf8')
		self.files = ['word2vec120.log','word2vec140.log','word2vec160.log']
		self.exps = ['exp01','exp02']
		self.subexpss = [['A1','A2','A3','A4','B1','B2','B3','B4'],['A1','A2','A3','A4','B1','B2','B3']]
		self.cols = ['kNN','GNB','BNB','NB','ME','SVM','NN']
		self.rows = ['word2vec_{}'.format(i) for i in [120,140,160]]

	def init_glove(self):
		self.fout = open(os.path.join(EXP,'tbl','glove.tbl'), 'w', encoding='utf8')
		self.files = ['glove120.log','glove140.log','glove160.log']
		self.exps = ['exp01','exp02']
		self.subexpss = [['A1','A2','A3','A4','B1','B2','B3','B4'],['A1','A2','A3','A4','B1','B2','B3']]
		self.cols = ['kNN','GNB','BNB','NB','ME','SVM','NN']
		self.rows = ['glove_{}'.format(i) for i in [120,140,160]]

	def init_doc2vec(self):
		self.fout = open(os.path.join(EXP,'tbl','doc2vec.tbl'), 'w', encoding='utf8')
		self.files = ['doc2vec{}.log'.format(i) for i in range(10,90,10)]
		self.exps = ['exp01','exp02']
		self.subexpss = [['A1','A2','A3','A4','B1','B2','B3','B4'],['A1','A2','A3','A4','B1','B2','B3']]
		self.cols = ['kNN','GNB','BNB','NB','ME','SVM','NN']
		self.rows = ['doc2vec_{}'.format(i) for i in range(10,90,10)]

	def init_copeopiA3(self):
		self.fout = open(os.path.join(EXP,'tbl','copeopiA3.tbl'), 'w', encoding='utf8')
		self.exps = ['exp01','exp02']
		self.subexpss = [['A3'],['A3']]
		self.files = ['copeopiA3a.log']
		npairs = 16
		#self.cols = ['{}_{:03d}'.format(i,j) for i in ['kNN','DT','GNB','BNB','LR','SVM','NN','LDA'] for j in range(npairs)]
		#self.rows = ['{}'.format(j) for j in ['000','CF3','CF5','CT3','CT5']]
		self.cols = ['{:03d}'.format(i) for i in range(npairs)]
		self.rows = ['{}_{}'.format(i,j) for i in ['kNN','DT','GNB','BNB','LR','SVM','NN','LDA'] for j in ['000','CF3','CF5','CT3','CT5']]

	def init_copeopiA4(self):
		self.fout = open('exp\\main\\tbl\\copeopi01A4.tbl', 'w', encoding='utf8')
		self.exps = ['exp01']
		self.subexpss = [['A4']]
		self.files = ['copeopiA4a.log']
		npairs = 14
		self.cols2 = ['{}_{:02d}'.format(i,j) for i in ['kNN','GNB','BNB','ME','SVM','NN'] for j in range(npairs)]
		self.rows2 = ['{}'.format(j) for j in ['000','CF3','CF5','CT3','CT5']]

	def init_copeopi01B3(self):
		self.fout = open('exp\\main\\tbl\\copeopi01B3.tbl', 'w', encoding='utf8')
		self.exps = ['exp01']
		self.subexpss = [['B3']]
		self.files = ['copeopi01B3a.log']
		npairs = 86
		self.cols = ['kNN','GNB','BNB','ME','SVM','NN','Argmax']
		self.rows = ['copeopi_{:02d}{}{}'.format(i,j,k) for i in range(npairs) for j in ['000','CF3','CF5','CT3','CT5'] for k in ['m','s','n']]
		self.cols2 = ['{}_{:02d}'.format(i,j) for i in ['kNN','GNB','BNB','ME','SVM','NN'] for j in range(npairs)]
		self.rows2 = ['{}{}'.format(j,i) for i in ['m','s','n'] for j in ['000','CF3','CF5','CT3','CT5']]

	def init_copeopi01B4(self):
		self.fout = open('exp\\main\\tbl\\copeopi01B4.tbl', 'w', encoding='utf8')
		self.exps = ['exp01']
		self.subexpss = [['B4']]
		self.files = ['copeopi01B4a.log']
		npairs = 22
		self.cols = ['kNN','GNB','BNB','ME','SVM','NN','Argmax']
		self.rows = ['copeopi_{:02d}{}{}'.format(i,j,k) for i in range(npairs) for j in ['000','CF3','CF5','CT3','CT5'] for k in ['m','s','n']]
		self.cols2 = ['{}_{:02d}'.format(i,j) for i in ['kNN','GNB','BNB','ME','SVM','NN'] for j in range(npairs)]
		self.rows2 = ['{}{}'.format(j,i) for i in ['m','s','n'] for j in ['000','CF3','CF5','CT3','CT5']]

	def init_all(self):
		self.fout = open('exp\\main\\tbl\\all.tbl', 'w', encoding='utf8')
		self.exps = ['exp01','exp02']
		self.subexpss = [['A1','A2','A3','B1','B2','B3','B4'],['A1','A2','A3','B1','B2','B3']]
		self.files = [
			'copeopi.log',
			'ngram001.log',
			'word2vec200.log',
			'glove200.log',
			'lsi200.log'
		]
		self.cols = ['kNN','NB','LR','SVM','NN']
		self.rows = ['copeopi_00ACT5','copeopi_00BCT5','copeopi_00CCT5',
		'ngram_001','lsi_n200','tfidf_001','lsi_t200','word2vec_200','glove_200']

	def tbl(self):
		for exp,subexps in zip(self.exps,self.subexpss):
			self.fout.write('{}\n'.format(exp))
			for subexp in subexps:
				tbl_f1 = np.zeros((len(self.rows),len(self.cols)))
				tbl_time = np.zeros((len(self.rows),len(self.cols)))
				for file in self.files:
					try:
						f = open(os.path.join(EXP,exp,subexp,file), 'r', encoding='utf8')
					except:
						print('No {}'.format(file))
						continue
					for line in f:
						if line.find('MACRO_F1')!=-1:
							line = line.strip().split()
							f1 = line[-1]
							name = line[-2]
							col = name.split('_')[0]
							row = name[len(col)+1:]
							try:
								tbl_f1[self.rows.index(row)][self.cols.index(col)] = f1
							except:
								pass
						elif line.find('EXE_TIME')!=-1:
							line = line.strip().split()
							time = line[-2]
							name = line[-4]
							col = name.split('_')[0]
							row = name[len(col)+1:]
							try:
								tbl_time[self.rows.index(row)][self.cols.index(col)] = time
							except:
								pass
					f.close()
				self.fout.write('{}\n'.format(subexp))
				for tbl,value in zip([tbl_f1,tbl_time],['MACRO_F1','EXE_TIME']):
					self.fout.write('{}\n'.format(value))
					for row in tbl:
						for cell in row:
							self.fout.write('{} '.format(cell))
						self.fout.write('\n')
				self.fout.write('\n')
				self.sides()
	
	def tbl2(self):
		for exp,subexps in zip(self.exps,self.subexpss):
			self.fout.write('{}\n'.format(exp))
			for subexp in subexps:
				tbl_f1 = np.zeros((len(self.rows),len(self.cols)))
				tbl_time = np.zeros((len(self.rows),len(self.cols)))
				for file in self.files:
					with open(os.path.join(EXP,exp,subexp,file), 'r', encoding='utf8') as f:
						for line in f:
							if line.find('MACRO_F1')!=-1:
								line = line.strip().split()
								f1 = line[-1]
								name = line[-2]
								clf = name.split('_')[0]
								pair = name.split('_')[2][:3]
								cfct = name.split('_')[2][-3:]
								col = pair
								row = clf+'_'+cfct
								tbl_f1[self.rows.index(row)][self.cols.index(col)] = f1
							elif line.find('EXE_TIME')!=-1:
								line = line.strip().split()
								time = line[-2]
								name = line[-4]
								clf = name.split('_')[0]
								pair = name.split('_')[2][:3]
								cfct = name.split('_')[2][-3:]
								col = pair
								row = clf+'_'+cfct
								tbl_time[self.rows.index(row)][self.cols.index(col)] = time
				self.fout.write('{}\n'.format(subexp))
				for tbl,value in zip([tbl_f1,tbl_time],['MACRO_F1','EXE_TIME']):
					self.fout.write('{}\n'.format(value))
					for row in tbl:
						for cell in row:
							self.fout.write('{} '.format(cell))
						self.fout.write('\n')
				self.fout.write('\n')
				self.sides()

	def sides(self):
		with open(os.path.join(EXP,'tbl','sides.tbl'), 'w', encoding='utf8') as f:
			for col in self.cols:
				f.write('{} '.format(col))
			f.write('\n')
			for row in self.rows:
				f.write('{}\n'.format(row))

#EXP_EXCEL('ngram').tbl()
#EXP_EXCEL('word2vec').tbl()
#EXP_EXCEL('glove').tbl()
#EXP_EXCEL('doc2vec').tbl()
EXP_EXCEL('all').tbl()