# re -> regular expression
import re 
from collections import Counter

def freq(a):
    A = re.sub(r'[^\w\s]', '', a.lower())
    word = A.split()
    word_count = Counter(word)  
    # reduce the time complexity
    print( " the frequency of the sentences is :", word_count)
freq("Mary had a little lamb Little lamb, little lamb Mary had a little lamb.Its fleece was white as snow And everywhere that Mary went Mary went,Mary went Everywhere that Mary went The lamb was sure to go")
