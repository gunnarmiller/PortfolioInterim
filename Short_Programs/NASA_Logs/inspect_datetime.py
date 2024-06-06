import sys, re, math
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

from pyspark.sql import SparkSession, functions, types

def parse(line):
    line_re = re.compile(r'^(\S+) - - \[(\S+) [+-]\d+\] \"[A-Z]+ (\S+) HTTP/\d\.\d\" \d+ (\d+)$')
    split_line = line_re.split(line)
    if(len(split_line) == 6):
        return split_line[2]
    
def get_hostname(line):
    return line[1]

def main(inputs):
    
    column_names = ["hostname", "bytes"]
    
    logs = sc.textFile(inputs)
    parsed_logs = logs.map(parse).filter(lambda x: x is not None)
    
    print(parsed_logs.take(10))
  
    
if __name__ == '__main__':
    inputs = sys.argv[1]
    spark = SparkSession.builder.appName('example code').getOrCreate()
    assert spark.version >= '3.0' # make sure we have Spark 3.0+
    spark.sparkContext.setLogLevel('WARN')
    sc = spark.sparkContext
    main(inputs)
