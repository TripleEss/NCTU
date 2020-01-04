# -*- coding: utf-8 -*-
import logging
logging.basicConfig(level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(funcName)s: %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S')
import re
import os

import jieba
from gensim.corpora import WikiCorpus
from gensim.models import word2vec, keyedvectors
from gensim.parsing.preprocessing import (preprocess_string,
    strip_tags, strip_multiple_whitespaces, strip_punctuation, strip_numeric, remove_stopwords, strip_short, stem)


NON_ZH = re.compile(r'[^\u4E00-\u9FFF ]')
SPACES = re.compile(r'\s+')

def merge_wiki(file_in, file_out):
    wiki_corpus = WikiCorpus(file_in)
    with open(file_out, 'w', encoding='utf8') as f:
        for i,text in enumerate(wiki_corpus.get_texts()):
            f.write(' '.join(text)+'\n')
            if i%10000==0:
                logging.info('{} processed'.format(i))

def seg(file_in, file_out):
    #jieba.set_dictionary('src/jieba.dict.big')

    fout = open(file_out, 'w', encoding='utf8')
    fin = open(file_in, 'r', encoding='utf8', errors='ignore')  
    for i,line in enumerate(fin):
        fout.write(' '.join(jieba.cut(line, cut_all=False)))
        if i%10000==0:
            logging.info('{} processed'.format(i))
    fin.close()
    fout.close()

def preproc_en(file_in, file_out):
    fout = open(file_out, 'w', encoding='utf8')
    fin = open(file_in, 'r', encoding='utf8', errors='ignore')  
    for i,line in enumerate(fin):
        line = line.strip()
        line = preprocess_string(line)
        fout.write('{}\n'.format(line))
        if i%10000==0:
            logging.info('{} processed'.format(i))
    fin.close()
    fout.close()

def preproc_zh(file_in, file_out):
    fout = open(file_out, 'w', encoding='utf8')
    fin = open(file_in, 'r', encoding='utf8', errors='ignore')  
    for i,line in enumerate(fin):
        line = line.strip()
        line = NON_ZH.sub('', line)
        line = SPACES.sub(' ', line)
        fout.write('{}\n'.format(line))
        if i%10000==0:
            logging.info('{} processed'.format(i))
    fin.close()
    fout.close()

def model_word2vec(file_in, file_out):
    corpus = word2vec.LineSentence(file_in)
    model = word2vec.Word2Vec(corpus, size=200, iter=50, min_count=5)
    model.wv.save_word2vec_format(file_out, binary=True)

def model_glove(fin, fout=False, **kwargs):
    build_dir = 'glove\\build'
    tmp_dir = 'model_glove'
    f_corpus = fin
    f_vocab = os.path.join(tmp_dir,'vocab.tmp')
    f_cooccur = os.path.join(tmp_dir,'cooccurrence.bin.tmp')
    f_cooccur_shuf = os.path.join(tmp_dir,'cooccurrence.shuf.bin.tmp')
    f_glove = os.path.join(tmp_dir,'glove') if not fout else fout
    f_overflow = os.path.join(tmp_dir,'overflow')

    verbose = 0
    vector_size = 200
    min_count = 5
    iters = 50

    if not os.path.isdir(tmp_dir):
        os.mkdir(tmp_dir)
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

if __name__ == '__main__':
    #merge_wiki('zhwiki-20170801-pages-articles.xml.bz2', 'zhwiki')
    #seg('zhwiki.simp', 'zhwiki.simp.seg')
    #preproc_zh('zhwiki.simp.seg', 'zhwiki.simp. preproc')
    #preproc_en('enwiki', 'enwiki.preproc')
    #model_word2vec('enwiki.preproc', 'enwiki.word2vec.bin')
    model_glove('src\\zhwiki.simp.preproc','glove\\zhwiki.simp.glove')