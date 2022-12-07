import processing.net.*;
import java.awt.Frame;
import java.awt.BorderLayout;
import controlP5.*; // http://www.sojamo.de/libraries/controlP5/
import processing.serial.*;

Server server;
char letter;

JSONObject plotterConfigJSON;

// plots
Graph LineGraph = new Graph(150, 400, 600, 200, color (20, 20, 200));

float[][] lineGraphValues = new float[6][100];
float[] lineGraphSampleNumbers = new float[100];
color[] graphColors = new color[6];

String words = "Roll 0 Pitch 0 Yaw 0";
boolean mockupSerial = true;
String topSketchPath = "";

// interface stuff
ControlP5 cp5;


int port = 10001;
void setup() {
  size(1920, 1080);
  
  // Create the font
  server = new Server(this, port);
  println("server address: " + server.ip()); //Output IP address
  textFont(createFont("Ubuntu", 36));
  
  //background(10);
  surface.setTitle("Realtime plotter");

  // set line graph colors
  graphColors[0] = color(131, 255, 20);
  graphColors[1] = color(232, 158, 12);
  graphColors[2] = color(255, 0, 0);
  graphColors[3] = color(62, 12, 232);
  graphColors[4] = color(13, 255, 243);
  graphColors[5] = color(200, 46, 232);

  // settings save file
  topSketchPath = sketchPath();
  plotterConfigJSON = loadJSONObject(topSketchPath+"/plotter_config.json");

  // gui
  cp5 = new ControlP5(this);
  
   //init charts
  setChartSettings();
  
  // build x axis values for the line graph
  for (int i=0; i<lineGraphValues.length; i++) {
    for (int k=0; k<lineGraphValues[0].length; k++) {
      lineGraphValues[i][k] = 0;
      if (i==0)
        lineGraphSampleNumbers[k] = k;
    }
  }
  
 
  
}

byte[] inBuffer = new byte[100]; // holds serial message
int i = 1; // loop variable

void draw() {
  
  Client client = server.available();
  if (client !=null) {
    String whatClientSaid = client.readString();
    if (whatClientSaid != null) {
      println(whatClientSaid); //Output message from Python
      words=whatClientSaid;
    } 
  }
  
  background(0); // Set background to black

  // Draw the letter to the center of the screen
  textSize(28);
  fill(255, 255, 255);
  text("AUV Varuna 2.0", 250, 50);
  textSize(20);
  text("Ground Control", 250, 70);
  //text("Current key: " + letter, 50, 70);
  //text("The String is " + words.length() +  " characters long", 50, 90);
  
  textSize(36);
  
  
  int s = second();  // Values from 0 - 59
  int m = minute();  // Values from 0 - 59
  int h = hour();    // Values from 0 - 23
  
  textSize(20);
  text("Local Time : "+h+":"+m+":"+s, 250,100);
  text(words, 250,200);
  //text(s, 120,200);
  
  
  //if (mockupSerial) {
  //  String myString = "";
  //  myString = mockupSerialFunction();
    
  //  //println(myString);

  //  // split the string at delimiter (space)
  //  String[] nums = split(myString, ' ');
    
  //  // count number of bars and line graphs to hide
  //  int numberOfInvisibleBars = 0;
  //  for (i=0; i<6; i++) {
  //    if (int(getPlotterConfigString("bcVisible"+(i+1))) == 0) {
  //      numberOfInvisibleBars++;
  //    }
  //  }
  //  int numberOfInvisibleLineGraphs = 0;
  //  for (i=0; i<6; i++) {
  //    if (int(getPlotterConfigString("lgVisible"+(i+1))) == 0) {
  //      numberOfInvisibleLineGraphs++;
  //    }
  //  }
  //  // build a new array to fit the data to show
  //  barChartValues = new float[6-numberOfInvisibleBars];

  //  // build the arrays for bar charts and line graphs
  //  int barchartIndex = 0;
  //  for (i=0; i<nums.length; i++) {

  //    // update barchart
  //    try {
  //      if (int(getPlotterConfigString("bcVisible"+(i+1))) == 1) {
  //        if (barchartIndex < barChartValues.length)
  //          barChartValues[barchartIndex++] = float(nums[i])*float(getPlotterConfigString("bcMultiplier"+(i+1)));
  //      }
  //      else {
  //      }
  //    }
  //    catch (Exception e) {
  //    }

  //    // update line graph
  //    try {
  //      if (i<lineGraphValues.length) {
  //        for (int k=0; k<lineGraphValues[i].length-1; k++) {
  //          lineGraphValues[i][k] = lineGraphValues[i][k+1];
  //        }

  //        lineGraphValues[i][lineGraphValues[i].length-1] = float(nums[i])*float(getPlotterConfigString("lgMultiplier"+(i+1)));
  //      }
  //    }
  //    catch (Exception e) {
  //    }
  //  }
  //}
  // draw the line graphs
  LineGraph.DrawAxis();
  for (int i=0;i<lineGraphValues.length; i++) {
    LineGraph.GraphColor = graphColors[i];
    if (int(getPlotterConfigString("lgVisible"+(i+1))) == 1)
      //LineGraph.LineGraph(lineGraphSampleNumbers, lineGraphValues[i]);
      LineGraph.LineGraph(lineGraphSampleNumbers, lineGraphValues[i]);
  }
}

void keyTyped() {
  // The variable "key" always contains the value 
  // of the most recent key pressed.
  if ((key >= 'A' && key <= 'z') || key == ' ') {
    letter = key;
    words = words + key;
    // Write the letter to the console
    println(key);
  }
}

void setChartSettings() {
  
  LineGraph.xLabel="Time";
  LineGraph.yLabel="Angle";
  LineGraph.Title="";  
  LineGraph.xDiv=20;  
  LineGraph.xMax=10; 
  LineGraph.xMin=0;  
  LineGraph.yMax=180; 
  LineGraph.yMin=-180;
}

//// get gui settings from settings file
String getPlotterConfigString(String id) {
  String r = "";
  try {
    r = plotterConfigJSON.getString(id);
  } 
  catch (Exception e) {
    r = "";
  }
  return r;
}
