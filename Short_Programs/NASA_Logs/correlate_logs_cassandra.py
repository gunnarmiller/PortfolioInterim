import sys, re, math
from cassandra.cluster import Cluster
from datetime import datetime
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

from pyspark.sql import SparkSession, functions, types


def main(keyspace, table_name):

    cluster_seeds = ['node1.local', 'node2.local']
    spark = SparkSession.builder.appName('Correlate Logs Cassandra').config('spark.cassandra.connection.host', ','.join(cluster_seeds)).getOrCreate()
    logsDF = spark.read.format("org.apache.spark.sql.cassandra").options(table=table_name, keyspace=keyspace).load()
    logsDF = logsDF.drop('id','datetime','path')
    logsDF.cache()
    
    byte_totals = logsDF.groupBy(logsDF['host']).agg(functions.sum(logsDF['bytes'])).withColumnRenamed('sum(bytes)', 'total bytes')
    request_totals = logsDF.groupBy(logsDF['host']).agg(functions.count(logsDF['bytes'])).withColumnRenamed('count(bytes)', 'total requests')
    byte_totals = byte_totals.withColumn('bytes squared', byte_totals['total bytes']*byte_totals['total bytes'])
    byte_totals = byte_totals.join(request_totals, 'host')
    byte_totals = byte_totals.withColumn('requests squared', byte_totals['total requests']*byte_totals['total requests'])
    byte_totals = byte_totals.withColumn('cross product', byte_totals['total requests']*byte_totals['total bytes'])
    byte_totals.cache()
    
    num_data_points = byte_totals.count()
    num_requests = byte_totals.agg(functions.sum(byte_totals['total requests'])).first()['sum(total requests)']
    num_bytes = byte_totals.agg(functions.sum(byte_totals['total bytes'])).first()['sum(total bytes)']
    square_req_sum = byte_totals.agg(functions.sum(byte_totals['requests squared'])).first()['sum(requests squared)']
    square_byte_sum = byte_totals.agg(functions.sum(byte_totals['bytes squared'])).first()['sum(bytes squared)']
    cross_sum = byte_totals.agg(functions.sum(byte_totals['cross product'])).first()['sum(cross product)']
    
    numerator = num_data_points*cross_sum - num_requests*num_bytes
    left_radicand = num_data_points*square_req_sum - num_requests*num_requests
    right_radicand = num_data_points*square_byte_sum - num_bytes*num_bytes
    
    correlation_coefficient = numerator/(math.sqrt(left_radicand)*math.sqrt(right_radicand))
    r_squared = correlation_coefficient**2
    
    print("r = ", correlation_coefficient)
    print("r^2 = ", r_squared)
    
if __name__ == '__main__':
    keyspace = sys.argv[1]
    table_name = sys.argv[2]
    spark = SparkSession.builder.appName('example code').getOrCreate()
    assert spark.version >= '3.0' # make sure we have Spark 3.0+
    spark.sparkContext.setLogLevel('WARN')
    sc = spark.sparkContext
    main(keyspace, table_name)
