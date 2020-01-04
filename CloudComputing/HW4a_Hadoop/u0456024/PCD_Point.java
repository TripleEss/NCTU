package u0456024;

import java.util.StringTokenizer;

/* Class 4-dimension vector */
public class PCD_Point{
	/* Members */
	private float [] vec = new float[4];
	/* Functions */
	// Build point from PCD format string
	/** Constructor: assume line format: X Y Z RGB (No safe detection) */
	public PCD_Point(String line){
		StringTokenizer stk = new StringTokenizer(line, " ");
		for(int i=0;i<4;i++){
			try{
				String token = stk.nextToken();
				this.vec[i] = Float.parseFloat(token);
			}catch(Exception e){
				this.vec[i] = 0;
			}
		}
	}
	
	/** To PCD string format */
	public String toPCDString(){
		return Float.toString(this.vec[0]) + " " +
			Float.toString(this.vec[1]) + " " +
			Float.toString(this.vec[2]) + " " +
			Integer.toString((int)this.vec[3]);
	}
	
	/** calculate distance between 2 points */
	public static float calDistance(PCD_Point p1, PCD_Point p2){
		float x = p1.vec[0] - p2.vec[0];
		float y = p1.vec[1] - p2.vec[1];
		float z = p1.vec[2] - p2.vec[2];
		return (float)Math.sqrt(x*x + y*y + z*z);
	}
	
	/** Add a [x,y,z] vector */
	public int addVec(PCD_Point v){
		this.vec[0] = this.vec[0] + v.vec[0];
		this.vec[1] = this.vec[1] + v.vec[1];
		this.vec[2] = this.vec[2] + v.vec[2];
		return 0;
	}
	
	/** Divided by a number a */
	public int divide(float d){
		if(d==0)
			return -1;
		this.vec[0] = this.vec[0]/d;
		this.vec[1] = this.vec[1]/d;
		this.vec[2] = this.vec[2]/d;
		return 0;
	}
	
	/** Get RGB */
	public int getRGB(){
		return (int)this.vec[3];		
	}
	
	/** Set RGB */
	protected int setRGB(int rgb){
		this.vec[3] = (float)rgb;
		return 0;
	}
}