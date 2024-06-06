import sys, re, math
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

from pyspark.sql import SparkSession, functions, types

# add more functions as necessary

def main(inputs, output):

    simple_schema = types.StructType([
        types.StructField('hostname', types.StringType()),
        types.StructField('request', types.LongType())
    ])

    raw_data = spark.read.csv(inputs, schema = simple_schema, sep = " ")
    raw_data.cache()
    byte_totals = raw_data.groupBy(raw_data['hostname']).agg(functions.sum(raw_data["request"])).withColumnRenamed('sum(request)', 'total bytes')
    request_totals = raw_data.groupBy(raw_data['hostname']).agg(functions.count(raw_data["request"])).withColumnRenamed('count(request)', 'total requests')
    byte_totals = byte_totals.withColumn('bytes squared', byte_totals['total bytes']*byte_totals['total bytes'])
    byte_totals = byte_totals.join(request_totals, 'hostname')
    byte_totals = byte_totals.withColumn('requests squared', byte_totals['total requests']*byte_totals['total requests'])
    byte_totals = byte_totals.withColumn('cross product', byte_totals['total requests']*byte_totals['total bytes'])
    
    num_data_points = byte_totals.count()
    num_requests = byte_totals.agg(functions.sum(byte_totals['total requests'])).first()['sum(total requests)']
    num_bytes = byte_totals.agg(functions.sum(byte_totals['total bytes'])).first()['sum(total bytes)']
    square_req_sum = byte_totals.agg(functions.sum(byte_totals['requests squared'])).first()['sum(requests squared)']
    square_byte_sum = byte_totals.agg(functions.sum(byte_totals['bytes squared'])).first()['sum(bytes squared)']
    cross_sum = byte_totals.agg(functions.sum(byte_totals['cross product'])).first()['sum(cross product)']
    print("Distinct Hosts: ", num_data_points)
    print("Total requests: ", num_requests)
    print("Total bytes: ", num_bytes)
    print("Requests Squared: ", square_req_sum)
    print("Bytes Squared: ", square_byte_sum)
    print("Sum of cross terms: ", cross_sum)
    
    numerator = num_data_points*cross_sum - num_requests*num_bytes
    left_radicand = num_data_points*square_req_sum - num_requests*num_requests
    right_radicand = num_data_points*square_byte_sum - num_bytes*num_bytes
    print(numerator)
    print(left_radicand)
    print(right_radicand)
    
    correlation_coefficient = numerator/(math.sqrt(left_radicand)*math.sqrt(right_radicand))
    r_squared = correlation_coefficient**2
    
    print("r = ", correlation_coefficient)
    print("r^2 = ", r_squared)
    
    
    

if __name__ == '__main__':
    inputs = sys.argv[1]
    output = sys.argv[2]
    spark = SparkSession.builder.appName('example code').getOrCreate()
    assert spark.version >= '3.0' # make sure we have Spark 3.0+
    spark.sparkContext.setLogLevel('WARN')
    sc = spark.sparkContext
    main(inputs, output)
