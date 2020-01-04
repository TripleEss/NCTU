# -*- coding: utf-8 -*-

import numpy as np

import tensorflow as tf
from keras.models import Model
from keras.layers import Input, Embedding, Conv1D, Dropout, Flatten, Dense, concatenate
#from keras.layers import MaxPooling1D, AveragePooling1D
from keras.callbacks import EarlyStopping
import keras.backend as K
from keras.engine.topology import Layer


class kMinkMaxPooling1D(Layer):
	def __init__(self, topk_min, topk_max, **kwargs):
		super(kMinkMaxPooling1D, self).__init__(**kwargs)
		self.topk_min = topk_min
		self.topk_max = topk_max

	def build(self, input_shape):
		super(kMinkMaxPooling1D, self).build(input_shape)

	def call(self, input):
		input = tf.transpose(input, perm=[0,2,1])

		kmin = None
		if self.topk_min!=0:
			kmin = -tf.nn.top_k(-input, self.topk_min, sorted=True).values 
		kmax = None
		if self.topk_max!=0:
			kmax = tf.nn.top_k(input, self.topk_max, sorted=True).values

		if kmin is not None and kmax is not None:
			return concatenate([kmin,kmax], axis=-1)
		elif kmin is not None:
			return kmin
		else:
			return kmax

	def compute_output_shape(self, input_shape):
		return (input_shape[0], self.topk_min+self.topk_max, input_shape[2])
### EOF kMinkMaxPooling1D

class NN:
	def fit(self, xs, ys, **kwargs):
		kwargs['epochs'] = kwargs.get('epochs',20)
		kwargs['callbacks'] = [EarlyStopping(monitor='val_acc', patience=2, mode='max')]
		kwargs['batch_size'] = kwargs.get('batch_size',10)
		kwargs['verbose'] = kwargs.get('verbose',2)
		#tf.Session(config=tf.ConfigProto(log_device_placement=True))
		return self.clf.fit(xs, ys, **kwargs)

	def predict(self, xs, **kwargs):
		predict_probs = self.clf.predict(xs, **kwargs)
		predictions = np.argmax(predict_probs, axis=1)
		return predictions

	def summary(self):
		self.clf.summary()

	def clear(self):
		K.clear_session()
		

class TextCNN(NN):
	def __init__(self, input_length, num_cls, map_idx2embedding,
		filters=16, filer_sizes=[2,3,4,5], topk_pooling=(3,3), embedding_trainable=False, activation='tanh'):

		inputs = Input(shape=(input_length,))
		embedding = Embedding(input_dim=len(map_idx2embedding), output_dim=len(map_idx2embedding[0]), input_length=input_length,
			weights=[map_idx2embedding], trainable=embedding_trainable)(inputs)

		grams = []
		for filter_size in filer_sizes:
			gram = Conv1D(filters, filter_size, activation=activation)(embedding)
			gram = kMinkMaxPooling1D(topk_pooling[0], topk_pooling[1])(gram)
			gram = Flatten()(gram)
			grams.append(gram)

		merged = concatenate(grams) if len(grams)>1 else grams[0]
		ouputs = Dense(num_cls, activation='softmax')(merged)
		self.clf = Model(inputs=inputs, outputs=ouputs)
		self.clf.compile(loss='categorical_crossentropy', optimizer='RMSprop', metrics=['accuracy'])
### EOF TextCNN

class SingleLayerNN(NN):
	def __init__(self, input_length, num_cls):
		inputs = Input(shape=(input_length,))
		ouputs = Dense(num_cls, activation='softmax')(inputs)
		self.clf = Model(inputs=inputs, outputs=ouputs)
		self.clf.compile(loss='categorical_crossentropy', optimizer='RMSprop', metrics=['accuracy'])
### EOF SingleLayerNN