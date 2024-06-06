import sys
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

from pyspark.sql import SparkSession, functions, types

# add more functions as necessary

def main(topic):
    #getting and formatting the data
    messages = spark.readStream.format('kafka').option('kafka.bootstrap.servers', 'node1.local:9092,node2.local:9092').option('subscribe', topic).load()
    values = messages.select(messages['value'].cast('string'))
    pieces = functions.split(values['value'], ' ')
    num_vals = values.withColumns({'x':pieces.getItem(0).cast('double'), 'y':pieces.getItem(1).cast('double')}).drop('value')
    
    #prepping columns for the calculation
    points = num_vals.withColumns({'x^2':num_vals['x']*num_vals['x'],'xy':num_vals['x']*num_vals['y'], 'y^2': num_vals['y']*num_vals['y']})
    p_sums = points.groupBy().agg(functions.sum(points['x']), functions.sum(points['y']), functions.sum(points['xy']),\
    functions.sum(points['x^2']),functions.sum(points['y^2']),functions.count(points['x']))
    
    #calculating alpha and beta
    beta_df = p_sums.withColumn('beta', p_sums['sum(xy)'] - (p_sums['sum(x)']*p_sums['sum(y)']/p_sums['count(x)']))
    beta_df = beta_df.withColumn('beta', beta_df['beta']/(beta_df['sum(x^2)'] - beta_df['sum(x)']*beta_df['sum(x)']/beta_df['count(x)']))
    alphabet = beta_df.withColumn('alpha', (beta_df['sum(y)'] - beta_df['beta']*beta_df['sum(x)'])/beta_df['count(x)'])
    
    #formatting and passing on the result 
    output_df = alphabet.select('alpha', 'beta')
    query = output_df.writeStream.outputMode("complete").format("console").start()
    query.awaitTermination(600)
    
    
    

if __name__ == '__main__':
    topic = sys.argv[1]
    spark = SparkSession.builder.appName('example code').getOrCreate()
    assert spark.version >= '3.0' # make sure we have Spark 3.0+
    spark.sparkContext.setLogLevel('WARN')
    sc = spark.sparkContext
    main(topic)
