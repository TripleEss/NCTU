package u0456024;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class KMeans {
	/* Members */
	public static int k = 2;
	public static long counter = 0;
	public static int iterations = 0;
	public static int maxIterations = 20;
	public static Path corePath = new Path("/home/u0456024/hadoop/hadoop-2.7.1/etc/hadoop/core-site.xml");
	public static Path hdfsPath = new Path("/home/u0456024/hadoop/hadoop-2.7.1/etc/hadoop/hdfs-site.xml");
	public static Path tmpWorkDir = new Path("hdfs://master:9000/u0456024/kmeans/tmp");
	public static Path modelPath = new Path("hdfs://master:9000/u0456024/kmeans/data/animal.pcd");
	public static Path finalResultPath = new Path("hdfs://master:9000/u0456024/kmeans/result/result_animal.pcd");
	public static String resultPrefixPathString = new String(tmpWorkDir.toString() + "/result");
	/* Main function */
	public static void main(String[] args) throws Exception{
		// Initial
		PCD_File.init(corePath, hdfsPath, tmpWorkDir);
		PCD_File model = new PCD_File(modelPath); // Model file
		PCD_File center = new PCD_File(k, model); // Center file
		long startTime = System.currentTimeMillis();
		while(counter<k /*&& iterations<maxIterations*/){
			// Configuration
			Configuration conf = new Configuration();
			conf.addResource(corePath);
			conf.addResource(hdfsPath);
			Job job = Job.getInstance(conf, "KMeans");
			// Set input, output file
			FileInputFormat.addInputPath(job, model.getDataPath());
			FileOutputFormat.setOutputPath(job, new Path(resultPrefixPathString + iterations));
			// Set job, mapper, reducer class
			job.setJarByClass(KMeans.class);
			job.setMapperClass(KM_Mapper.class);
			job.setReducerClass(KM_Reducer.class);
			// Set mapper output class
			job.setMapOutputKeyClass(Text.class);
			job.setMapOutputValueClass(Text.class);
			// Set reducer output class
			job.setOutputKeyClass(Text.class);
			job.setOutputValueClass(Text.class);
			// Wait for mapreduce complete
			KM_Mapper.setCenters(center.read());
			job.waitForCompletion(true);
			center.write(KM_Reducer.getNewCentersAndClear());
			// Output information
			counter = job.getCounters().findCounter(KM_Reducer.Counter.CONVERGED).getValue();
			printInfo(iterations, center);
			iterations++;
		}
		long endTime = System.currentTimeMillis();
		System.out.printf("\nExecution time: %f sec.\n", (float)(endTime-startTime)/1000);
		// Get result
		PCD_File resultData = new PCD_File(new Path(resultPrefixPathString + (iterations-1) + "/part-r-00000"));
		model.write(resultData.read());
		model.save(finalResultPath);
		// Delete work directory
		resultData.delTmpSubDir();
		center.delTmpSubDir();
		model.delTmpSubDir();
		PCD_File.remove();
	}

	/* Class mapper */
	public static class KM_Mapper extends Mapper<Object, Text, Text, Text>{
		/* Members */
		protected static List<PCD_Point> centers = null;
		/* Functions */
		// Map function
		public void map(Object key, Text line, Context context) throws IOException, InterruptedException{
			// Text structure will read a line
			int minIndex = 0;
			float minDistance = Float.MAX_VALUE;
			PCD_Point point = new PCD_Point(line.toString());
			// Map: Calculate nearest center to this point
			for(int i=0;i<centers.size();i++){
				float dis = PCD_Point.calDistance(point, centers.get(i));
				if(dis<minDistance){
					minIndex = i;
					minDistance = dis;
				}
			}
			point.setRGB(centers.get(minIndex).getRGB());
			context.write(
				new Text(centers.get(minIndex).toPCDString()),
				new Text(point.toPCDString())
			);
		}
		// Set centers list
		public static int setCenters(List<PCD_Point> cent){
			centers = cent;
			return centers.size();
		}
	}

	/* Class reducer */
	public static class KM_Reducer extends Reducer<Text, Text, Text, Text>{
		/* Members */
		public enum Counter{
			CONVERGED
		}
		protected static List<PCD_Point> newCenters = new ArrayList<PCD_Point>();
		/* Functions */
		// Reduce function
		public void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
			int pointNumber = 0;
			PCD_Point oldCenter = new PCD_Point(key.toString());
			PCD_Point newCenter = new PCD_Point("0.0 0.0 0.0 " + Integer.toString(oldCenter.getRGB()));
			for(Text value : values){
				PCD_Point point = new PCD_Point(value.toString());
				newCenter.addVec(point);
				pointNumber++;
				context.write(value, new Text(""));
			}
			// Calculate new center and add to list
			if(pointNumber>0){
				newCenter.divide((float)pointNumber);
				newCenter.setRGB(oldCenter.getRGB());
				newCenters.add(newCenter);
				// Check converge
				if(PCD_Point.calDistance(oldCenter, newCenter)==0){
					context.getCounter(Counter.CONVERGED).increment(1);
				}
			}
		}
		// Get center list
		public static List<PCD_Point> getNewCentersAndClear(){
			List<PCD_Point> tmp = newCenters;
			newCenters = new ArrayList<PCD_Point>(tmp.size());
			return tmp;
		}
	}

	// Print information
	public static void printInfo(int it, PCD_File center){
		System.out.println("Iteration " + it + ": " + counter + " converged.");
		List<PCD_Point> newCenters = center.read();
		for(int i=0;i<newCenters.size();i++){
			System.out.println("    " + newCenters.get(i).toPCDString());
		}
	}
}
