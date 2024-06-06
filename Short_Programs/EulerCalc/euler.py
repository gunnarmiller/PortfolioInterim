from pyspark import SparkConf, SparkContext
from operator import add
import sys, random

calcSize = int(sys.argv[1])

conf = SparkConf().setAppName('Euler Calculator')
sc = SparkContext(conf = conf)
assert sys.version_info >= (3, 5)  # make sure we have Python 3.5+
assert sc.version >= '2.3'  # make sure we have Spark 2.3+


def sample(irrelevant):
    random.seed()
    counter = 0
    total = 0.0
    
    while(total < 1.0):
        total += random.random()
        counter += 1
        
    return counter


numbers = sc.range(calcSize, numSlices=100)
values = numbers.map(sample)
result = values.reduce(add)/calcSize
print(result)
print(result)
print(result)
print(result)
print(result)
