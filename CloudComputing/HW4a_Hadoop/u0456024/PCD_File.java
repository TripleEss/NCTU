package u0456024;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

public class PCD_File {
	/* Members */
	private static FileSystem fs = null;
	private static Path tmpDirPath = null;
	private static int tmpSubDirCounter = 0;
	private Path tmpSubDirPath;
	private Path tmpHeaderPath; // tmp work file for header
	private Path tmpDataPath; // tmp work file for data
	/* Center R/W functions */
	/** Initial PCD_File environment */
	public static int init(Path corePath, Path hdfsPath, Path unusedPath){
		try{
			Configuration conf = new Configuration();
			conf.addResource(corePath);
			conf.addResource(hdfsPath);
			fs = FileSystem.get(conf);
			tmpDirPath = unusedPath;
			tmpSubDirCounter = 0;
			fs.mkdirs(tmpDirPath);
			return 0;
		}catch(Exception e){
			return -1;
		}
	}
	
	/** Prepare model file */
	public PCD_File(Path modelPath){
		try{
			this.tmpSubDirPath = new Path(tmpDirPath.toString() + "/" + Integer.toString(tmpSubDirCounter));
			this.tmpHeaderPath = new Path(this.tmpSubDirPath.toString() + "/header");
			this.tmpDataPath = new Path(this.tmpSubDirPath.toString() + "/data");
			fs.mkdirs(this.tmpSubDirPath);
			tmpSubDirCounter++;
			// Load and separate header, data
			BufferedReader reader = new BufferedReader(new InputStreamReader(fs.open(modelPath)));
			BufferedWriter headerWriter = new BufferedWriter(new OutputStreamWriter(fs.create(this.tmpHeaderPath)));
			BufferedWriter dataWriter = new BufferedWriter(new OutputStreamWriter(fs.create(this.tmpDataPath)));
			String line = null;
			while((line=reader.readLine())!=null){
				char c = line.charAt(0);
				if(c=='#' || (c>='A' && c<='Z') || (c>='a' && c<='z')){ // Belongs to header
					headerWriter.append(line + "\n");
					headerWriter.flush();
				}
				else{
					dataWriter.append(line + "\n");
					dataWriter.flush();
				}
			}
			dataWriter.close();
			headerWriter.close();
			reader.close();
		}catch(Exception e){
			
		}
	}
	
	/** Get data file path */
	public Path getDataPath(){
		return this.tmpDataPath;
	}
	
	/** Read data from PCD file to ArrayList */
	public List<PCD_Point> read(){
		List<PCD_Point> pointCloud = new ArrayList<PCD_Point>();
		try{
			BufferedReader reader = new BufferedReader(new InputStreamReader(fs.open(this.tmpDataPath)));
			String line = null;
			while((line=reader.readLine())!=null){
				pointCloud.add(new PCD_Point(line));
			}
			reader.close();
		}catch(Exception e){
		
		}
		return pointCloud;
	}
	
	/** Write points to PCD file */
	public int write(List<PCD_Point> pointCloud){
		try{
			BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(fs.create(this.tmpDataPath)));
			writer.write("");
			for(int i=0;i<pointCloud.size();i++){
				writer.append(pointCloud.get(i).toPCDString() + "\n");
			}
			writer.flush();
			writer.close();
			return 0;
		}catch(Exception e){
			return -1;
		}
	}
	
	/** Output result to path */
	public int save(Path resultPath){
		try{
			String line = null;
			BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(fs.create(resultPath)));
			// Write header file
			BufferedReader headerReader = new BufferedReader(new InputStreamReader(fs.open(this.tmpHeaderPath)));
			while((line=headerReader.readLine())!=null){
				writer.append(line + "\n");
			}
			headerReader.close();
			// Write data file
			BufferedReader dataReader = new BufferedReader(new InputStreamReader(fs.open(this.tmpDataPath)));
			while((line=dataReader.readLine())!=null){
				writer.append(line + "\n");
			}
			writer.flush();
			dataReader.close();
			return 0;
		}catch(Exception e){
			return -1;
		}
	}
	
	/** Delete file under this template directory */
	public int delTmpSubDir(){
		try{
			fs.deleteOnExit(this.tmpDataPath);
			fs.deleteOnExit(this.tmpHeaderPath);
			fs.deleteOnExit(this.tmpSubDirPath);
			return 0;
		}catch(Exception e){
			return -1;
		}
	}
	
	/** Delete template directory */
	public static int remove(){
		try{
			fs.deleteOnExit(tmpDirPath);
			return 0;
		}catch(Exception e){
			return -1;
		}
	}
	
	/** Create center model:  \n
		You need to revise code in this function */
	public PCD_File(int k, PCD_File model){
		try{
			this.tmpSubDirPath = new Path(tmpDirPath.toString() + "/" + Integer.toString(tmpSubDirCounter));
			this.tmpHeaderPath = new Path(this.tmpSubDirPath.toString() + "/header");
			this.tmpDataPath = new Path(this.tmpSubDirPath.toString() + "/data");
			fs.mkdirs(this.tmpSubDirPath);
			tmpSubDirCounter++;
			// Write header file
			BufferedWriter headerWriter = new BufferedWriter(new OutputStreamWriter(fs.create(this.tmpHeaderPath)));
			headerWriter.write("Center File");
			headerWriter.write("k = " + k);
			headerWriter.flush();
			headerWriter.close();
			// Write data file
			BufferedWriter dataWriter = new BufferedWriter(new OutputStreamWriter(fs.create(this.tmpDataPath)));
			
			
			List<PCD_Point> pt = model.read();
			Collections.shuffle(pt);

			for(int i=0;i<k;i++){
				int r = (((i+1)>>0)&1)*255;
				int g = (((i+1)>>1)&1)*255;
				int b = (((i+1)>>2)&1)*255;
				int rgb = ((r<<16) | (g<<8) | (b));

				String [] center = pt.get(i).toPCDString().split(" ");	
							
				PCD_Point point = new PCD_Point(
					center[0] + " " +
					center[1] + " " +
					center[2] + " " +
					Integer.toString(rgb)
				);
				
				point.setRGB(rgb);
				dataWriter.append(point.toPCDString() + "\n");
			}
			dataWriter.flush();
			dataWriter.close();
		}catch(Exception e){

		}
	}
}
