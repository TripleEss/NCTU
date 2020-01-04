# -*- coding: utf-8 -*-

import logging
import json
from math import exp
from itertools import combinations, product

def powerset(elements):
	return [x for i in range(len(elements)+1) for x in combinations(elements,i)]
### EOF powerset

def pair_nonoverlapping_subsets_full(elements):
	subsets = powerset(elements)
	pairs = list(combinations(subsets,2))
	pairs = [[list(p),list(n)] for (p,n) in pairs if p!=() and n!=() and p!=tuple(elements) and n!=tuple(elements) and set(p)&set(n)==set()]
	pairs = sorted(pairs, key=lambda x: (len(x[0])+len(x[1]),-len(x[0])), reverse=True)
	return pairs

def pair_nonoverlapping_subsets(elements, level):
	subsets1 = [x for i in range(1,level+1) for x in combinations(elements,i)]
	subsets2 = [x for i in range(len(elements)-1,max(len(elements)-level-1,0),-1) for x in combinations(elements,i)]
	pairs = product(subsets1,subsets2)
	pairs = [tuple(sorted(pair, key=lambda x: (len(x),x))) for pair in pairs]
	pairs = set(pairs)
	pairs = [[list(p),list(n)] for (p,n) in pairs if set(p)&set(n)==set()]
	pairs = sorted(pairs, key=lambda x: (len(x[0]),tuple(x[0]),tuple(x[1])))
	pairs = sorted(pairs, key=lambda x: len(x[0])+len(x[1]), reverse=True)
	pairs = [[[p,n] for (p,n) in pairs if len(p)+len(n)==i and len(p)==j] for i in range(len(elements),len(elements)-level,-1) for j in range(1,level+1)]
	pairs = [pair for pair in pairs if pair!=[]]
	return pairs[:level]
### EOF pair_nonoverlapping_subsets

class CopeOpiVec():
	def fit(self, xs, ys, cls_set, min_count_mode='conf', min_count=0, default_pairs=['oaa'], other_pairs=[], conf_on=True):
		self.CLS_SET = cls_set
		# count
		self.CNT_ALL = {c:0 for c in self.CLS_SET}
		self.CNT = {}
		if min_count_mode=='count':
			self._CNT_ALL = {c:0 for c in self.CLS_SET}
			self._CNT = {}
		self._update_count(xs, ys, min_count_mode=min_count_mode, min_count=min_count)
		# conf
		self._update_conf(min_count=min_count)
		# pairs
		self.PAIRS = []
		self._update_pairs(default_pairs=default_pairs, other_pairs=other_pairs)
		# copeopi
		self._update_copeopi()
		if conf_on:
			return self, self.CONF_COPEOPI
		else:
			return self, self.COPEOPI

	def update(self, xs, ys, min_count_mode='conf', min_count=0):
		# count
		self._update_count(xs, ys, min_count_mode=min_count_mode, min_count=min_count)
		# conf
		self._update_conf(min_count=min_count)
		# copeopi
		self._update_copeopi()
		if conf_on:
			return self, self.CONF_COPEOPI
		else:
			return self, self.COPEOPI

	def extend(self, default_pairs=['oaa'], other_pairs=[], conf_on=True):
		# pairs
		self._update_pairs(default_pairs=default_pairs, other_pairs=other_pairs)
		# copeopi
		self._update_copeopi()
		if conf_on:
			return self, self.CONF_COPEOPI
		else:
			return self, self.COPEOPI

	def reshape(self, default_pairs=['oaa'], other_pairs=[], conf_on=True):
		# pairs
		self.PAIRS = []
		self._update_pairs(default_pairs=default_pairs, other_pairs=other_pairs)
		# copeopi
		self._update_copeopi()
		if conf_on:
			return self, self.CONF_COPEOPI
		else:
			return self, self.COPEOPI

	def _update_count(self, xs, ys, min_count_mode='conf', min_count=0):
		if min_count_mode=='conf':
			for x,y in zip(xs,ys):
				self.CNT_ALL[y] += len(x)
				for ch in x:
					if ch=='':
						continue
					if ch not in self.CNT:
						self.CNT[ch] = {c:0 for c in self.CLS_SET}
					self.CNT[ch][y] += 1
		elif min_count_mode=='count':
			for x,y in zip(xs,ys):
				self._CNT_ALL[y] += len(x)
				for ch in x:
					if ch=='':
						continue
					if ch not in self._CNT:
						self._CNT[ch] = {c:0 for c in self.CLS_SET}
					self._CNT[ch][y] += 1	
			self.CNT_ALL = dict(self._CNT_ALL)
			self.CNT = {}
			for ch in self._CNT.keys():
				cnt = sum(self._CNT[ch].values())
				if cnt<min_count:
					self.CNT_ALL = {c:cnt-self._CNT[ch][c] for c,cnt in self.CNT_ALL.items()}
				else:
					self.CNT[ch] = self._CNT[ch]

		self.CNT = dict(sorted(self.CNT.items(), key=lambda x: sum(x[1].values()), reverse=True))

	def _update_conf(self, min_count=0):
		avg_cntc = sum(self.CNT_ALL.values())/len(self.CNT)/len(self.CLS_SET)
		self.CONF = {ch:1.0 for ch in self.CNT.keys()}
		n = 0
		for ch in self.CNT.keys():
			cnt = sum(self.CNT[ch].values())
			cntc = max(self.CNT[ch].values())
			if cnt<min_count:
				n += 1
				self.CONF[ch] = 0
			elif cntc<avg_cntc:
				self.CONF[ch] = 1/(1+3*exp(-4*cntc/avg_cntc))

		logging.info('{}-{}={}'.format(len(self.CNT),n,len(self.CNT)-n))

	def oaa_pairs(self, elements):
		if len(elements)==2:
			return [[[elements[0]],[elements[1]]]]
		return [[[p],[n for n in elements if n!=p]] for p in elements]

	def oao_pairs(self, elements):
		return [[[p],[n]] for i_p,p in enumerate(elements) for n in elements[i_p+1:]]

	def _update_pairs(self, default_pairs=['oaa'], other_pairs=[]):
		for (p,n) in other_pairs:
			for c in set(p)|set(n):
				if c not in self.CLS_SET:
					raise ValueError('Classes in other_pairs should be in {}, but {} is given.'.format(self.CLS_SET,c))

		for pairs in default_pairs:
			pairs = getattr(self,pairs+'_pairs')(self.CLS_SET)
			for pair in pairs:
				if pair not in self.PAIRS:
					self.PAIRS.append(pair)
		for pair in other_pairs:
			if pair not in self.PAIRS:
				self.PAIRS.append(pair)
		logging.debug('CopeOpiVec({})\n{}'.format(len(self.PAIRS),'\n'.join([str(p) for p in self.PAIRS])))

	def _update_copeopi(self):
		self.COPEOPI = {ch:[] for ch in self.CNT.keys()}
		self.CONF_COPEOPI = {ch:[] for ch in self.CNT.keys()}
		for pair in self.PAIRS:
			cls_pos,cls_neg = pair
			fp_all = sum([cnt for c,cnt in self.CNT_ALL.items() if c in cls_pos])
			fn_all = sum([cnt for c,cnt in self.CNT_ALL.items() if c in cls_neg])
			for ch in self.CNT.keys():
				fp_ch = sum([cnt for c,cnt in self.CNT[ch].items() if c in cls_pos])
				fn_ch = sum([cnt for c,cnt in self.CNT[ch].items() if c in cls_neg])
				if fp_ch+fn_ch>0:
					cope = (fp_ch/fp_all-fn_ch/fn_all)/(fp_ch/fp_all+fn_ch/fn_all)
				else:
					cope = 0
				self.COPEOPI[ch].append(cope)
				self.CONF_COPEOPI[ch].append(cope*self.CONF[ch])

	def most_similar(self, target):
		if target not in self.COPEOPI:
			raise KeyError('{} is not found.'.format(target))

	def save(self, fout):
		with open(fout, 'w', encoding='utf8') as f:
			f.write('{}\n'.format(json.dumps(self.CLS_SET)))
			f.write('{}\t{}\n'.format(len(self.PAIRS),json.dumps(self.PAIRS)))
			f.write('{}\n'.format(json.dumps(self.CNT_ALL)))
			for ch in self.CNT.keys():
				f.write('{}\t{}\t{}\t{}\t{}\n'.format(ch, self.COPEOPI[ch], self.CONF[ch], self.CONF_COPEOPI[ch], json.dumps(self.CNT[ch])))

	def load(self, file, conf_on=True):
		self.CNT = {}
		self.CONF = {}
		self.COPEOPI = {}
		self.CONF_COPEOPI ={}
		with open(file, 'r', encoding='utf8') as f:
			f.readline()
			f.readline()
			#self.CLS_SET = json.loads(f.readline().strip())
			#self.PAIRS = json.loads(f.readline().split('\t')[1])
			#self.CNT_ALL = json.loads(f.readline())
			for line in f:
				line = line.strip().split('\t')
				ch = line[0]
				self.COPEOPI[ch] = json.loads(line[1])
				self.CONF[ch] = json.loads(line[2])
				self.CONF_COPEOPI[ch] = [json.loads(line[3])]
				#self.CNT[ch] = json.loads(line[4])
		if conf_on:
			return self, self.CONF_COPEOPI
		else:
			return self, self.COPEOPI
### EOF CopeOpiVec