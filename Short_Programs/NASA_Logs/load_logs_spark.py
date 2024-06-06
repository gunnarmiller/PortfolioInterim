import sys, re, uuid
from datetime import datetime
from cassandra.cluster import Cluster
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

from pyspark.sql import SparkSession, functions, types

#NASA logs datetime looks like DD/mon/YYYY:HH:MM:SS
def convert_datetime(dt_string):
    nasa_format = '%d/%b/%Y:%H:%M:%S'
    return datetime.strptime(dt_string, nasa_format)

#some of this might not be necessary: the NASA logs has the fields in a different order than the Cassandra
#table, so in addition to some necessary cleanup, this function reorders them to match.
def parse(line):
    line_re = re.compile(r'^(\S+) - - \[(\S+) [+-]\d+\] \"[A-Z]+ (\S+) HTTP/\d\.\d\" \d+ (\d+)$')
    split_line = line_re.split(line)
    if(len(split_line) == 6):
        split_line[0] = split_line[1]
        split_line[1] = str(uuid.uuid1()) 
        holder = split_line[2]
        split_line[2] = int(split_line[4])
        split_line[4] = split_line[3]
        split_line[3] = convert_datetime(holder)
        return split_line[:5]
        

def main(inputs, keyspace, table_name):

    cluster_seeds = ['node1.local', 'node2.local']
    spark = SparkSession.builder.appName('Spark Cassandra example').config('spark.cassandra.connection.host', ','.join(cluster_seeds)).getOrCreate()
    
    column_names = ["host", "id", "bytes", "datetime", "path"]
    
    logs = sc.textFile(inputs).repartition(80)
    parsed_logs = logs.map(parse).filter(lambda x: x is not None)
    logs_DF = parsed_logs.toDF(column_names)
    
    logs_DF.write.format("org.apache.spark.sql.cassandra").mode("Append").options(table = table_name, keyspace = keyspace).save()

if __name__ == '__main__':
    inputs = sys.argv[1]
    keyspace = sys.argv[2]
    table_name = sys.argv[3]
    spark = SparkSession.builder.appName('load logs spark').getOrCreate()
    assert spark.version >= '3.0' # make sure we have Spark 3.0+
    spark.sparkContext.setLogLevel('WARN')
    sc = spark.sparkContext
    main(inputs, keyspace, table_name)
