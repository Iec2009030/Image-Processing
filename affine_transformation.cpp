/*
 * Project 6
 * By Nitin Bansal
 */


#include <cstdlib>
#include <iostream>
#include <GL/glut.h>
#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <iterator>


using namespace std;

class Vector3D
{
        public:
                float x, y, z;
                void setValues(float x1, float y1, float z1);
                Vector3D(float x1,float y1, float z1);
                Vector3D();
                Vector3D operator +(const Vector3D& v) const;
                Vector3D operator /(const float& v) const;
                Vector3D operator *(const float& v) const;
                const float& operator[](int i) const;
                float& operator[](int i);
};

void Vector3D::setValues(float x1, float y1, float z1)
{
	x = x1, y = y1, z = z1;
}
Vector3D::Vector3D(float x1,float y1, float z1)
{
	x = x1, y = y1, z = z1;
}

Vector3D::Vector3D(){x=0;y=0;z=0;}
Vector3D Vector3D::operator +(const Vector3D& v) const
{
	return (Vector3D(x + v.x, y + v.y, z + v.z));
}

Vector3D Vector3D::operator /(const float& v) const
{
	return (Vector3D(x / v, y / v, z / v));
}
Vector3D Vector3D::operator *(const float& v) const
{
	return (Vector3D(x * v, y * v, z * v));
}

const float& Vector3D::operator[](int i) const
{
	switch(i){
		case 0:
			return x;
		case 1:
			return y;
		default:
			return z;
	}
}

float& Vector3D::operator[](int i)
{
	switch(i){
		case 0:
			return x;
		case 1:
			return y;
		default:
			return z;
	}
}

class matrix
{
        public:
                Vector3D row[3];
                matrix(double a11, double a12, double a13,
                                double a21, double a22, double a23,
                                double a31, double a32, double a33);
                friend Vector3D operator *(const matrix& m, const Vector3D& v);
};
                        
matrix::matrix(double a11, double a12, double a13,
                                         double a21, double a22, double a23,
                                         double a31, double a32, double a33)
{
        row[0].setValues(a11, a12, a13);
        row[1].setValues(a21, a22, a23);
        row[2].setValues(a31, a32, a33);
}

Vector3D operator*(const matrix& m, const Vector3D& v)

{
        int i, j;
        double sum;
        Vector3D result;
        for(i = 0; i < 3; i++){
                sum = 0;
                for(j = 0; j < 3; j++)
                        sum += m.row[i][j] * v[j];
                result[i] = sum;
        }
        return result;
}
    

// structure for PPM pixels

struct RGB {

      unsigned char red; unsigned char green; unsigned char blue;

};



struct ppmImage{
	int width, height, maxColor;
	RGB *pixmap;
};
float pi = 3.14;
int nsamp = 2;
int sampleSize = 25;

// =============================================================================

// These variables will store the ppm images' width, height, and data

// =============================================================================

ppmImage inputImage;
ppmImage resultImage;

// This secondary function nibbles away comment lines for a ppm file

void readComments(ifstream &streamFile){

        string nibbler;

        while(streamFile.peek() == '#'){

                getline(streamFile, nibbler);

        }

}



// =============================================================================

// readPPM(char* fileName, ppmImage &image)

//

// This function reads in a prescribed PPM file (by fileName) and

// stores its contents to the specified ppmImage structure

// =============================================================================

void readPPM(char* fileName, ppmImage &image){
	string line;
	ifstream ppmFile(fileName, ios::binary);

	if (ppmFile.is_open()){

		readComments(ppmFile);

		//read magic number

		getline(ppmFile, line);

		if(line != "P6"){

			cout<<"The PPM input file's magic number is incorrect. Try again!\n";

			ppmFile.close();

			exit(-1);

		}
		readComments(ppmFile);

		//read width and height

		getline(ppmFile, line);

		istringstream(line) >> image.width >> image.height;
		readComments(ppmFile);

		//read max color value
		getline(ppmFile, line);
        image.maxColor = atoi(line.c_str());
        if(image.maxColor > 255){

			cout<<"The PPM input file's max color value exceeded 255. Try again!\n";
			ppmFile.close();
			exit(-1);
		}
		//initialize the pixMap
        image.pixmap = new RGB[image.width * image.height];
		readComments(ppmFile);
		//read pixmap data
		for(int i = image.height - 1; i >= 0; i--){
			for(int j = 0; j < image.width; j++){
				int currentIndex = i * image.width + j;
				char rgbBytes[3];
				ppmFile.read(rgbBytes, sizeof rgbBytes);
				image.pixmap[currentIndex].red = rgbBytes[0];
			    image.pixmap[currentIndex].green = rgbBytes[1];
                image.pixmap[currentIndex].blue = rgbBytes[2];

			}
		}

		ppmFile.close();	
	}
	else{
		cout<<"The PPM input file could not be opened. Try again!\n";
		exit(-1);
	}
}


//Transformation function that transforms the image based on the transform matrix

void transform(matrix inputMatrix)

{
  resultImage.height = inputImage.height;
  resultImage.width = inputImage.width;
  resultImage.pixmap = new RGB[inputImage.width * inputImage.height];
	int x1, y1;
	int i, j;

	// loop through pixels
	for(int y = 0; y < resultImage.height ; y++) {
		for(int x = 0; x < resultImage.width; x++) {			
      double red = 0, blue = 0, green = 0;
      for(int sample = 0; sample < sampleSize; sample++){
        Vector3D newPixel(x+(1.0*rand())/RAND_MAX,y+(1.0*rand())/RAND_MAX,1);
			  Vector3D originalPixel(0,0,0);
        originalPixel = inputMatrix * newPixel;
        //normalization
			  float normalize = 1/originalPixel.z;
			  x1 = (int)(originalPixel.x * normalize);
			  y1 = (int)(originalPixel.y * normalize);
			  // warping
			  if (x1 < 0) 
			  { 
				  x1 += resultImage.width; 
			  }
			  if (x1 > resultImage.width) 
			  { 
				  x1 = x1 % resultImage.width; 
			  }
			  if (y1 < 0) 
			  { 
				  y1 += resultImage.height; 
			  }

			  if (y1 > resultImage.height) 

			  { 

				  y1 = y1 % resultImage.height; 

			  }
			  j = (y1 * resultImage.width + x1);	// original pixmap

        red += inputImage.pixmap[j].red;
        green += inputImage.pixmap[j].green;
        blue += inputImage.pixmap[j].blue;
		  }
        i = (y * resultImage.width + x);	// transform pixmap
        resultImage.pixmap[i].red = red/sampleSize;
        resultImage.pixmap[i].green = green/sampleSize;
        resultImage.pixmap[i].blue = blue/sampleSize;
    }
	}
}

//Inverse rotation matrix
void rotate(float angle)
{
  float theta = (angle * 3.1416)/180;
	matrix rotateM(	
		cos(-theta), sin(-theta), 0,
		-sin(-theta), cos(-theta),  0,
		0,    0,   1);
	transform(rotateM);
}



//Inverse translate matrix
void translate(float xM, float yM)
{
	matrix translateM(
		1, 0, -xM,
		0, 1, -yM,
		0, 0, 1);
	transform(translateM);
}



//Inverse scale matrix

void scale(float xS, float yS)

{
	matrix scaleM(
		1/xS, 0, 0,
		0, 1/yS, 0,
		0,  0, 1);
	transform(scaleM);
}



//Inverse shear matrix

void shear(float xSh, float ySh)

{
	matrix shearM(
		1, -xSh, 0,
		-ySh, 1, 0,
		0,  0, 1-(xSh * ySh));
	transform(shearM);
}



//Inverse mirror matrix

void mirror(float xM, float yM)
{
	matrix mirrorM(
		-xM, 0, 0,
		0, -yM, 0,
		0,  0, 1);
	transform(mirrorM);
}



//Inverse perspective matrix

void perspective(float a, float b)
{
	matrix perspectiveM(1, 0, 0,0, 1, 0,-a, -b, 1);
	transform(perspectiveM);
}



// =============================================================================

// OpenGL Display and Mouse Processing Functions.

//

// You can read up on OpenGL and modify these functions, as well as the commands

// in main(), to perform more sophisticated display or GUI behavior. This code

// will service the bare minimum display needs for most assignments.

// =============================================================================

static void windowResize(int w, int h)
{
        glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0,(w/2),0,(h/2),0,1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity() ;
}



static void windowDisplay(void)
{
        glClear(GL_COLOR_BUFFER_BIT);
        glRasterPos2i(0,0);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        glDrawPixels(resultImage.width, resultImage.height, GL_RGB, GL_UNSIGNED_BYTE, resultImage.pixmap);
        glFlush();
}



static void processMouse(int button, int state, int x, int y)
{
        if(state == GLUT_UP)
        exit(0);               // Exit on mouse click.
}



static void init(void)
{
        glClearColor(1,1,1,1); // Set background color.
}

vector<std::string> split(string inp)
{
  string arr[3];
  std::istringstream buf(inp);
    std::istream_iterator<std::string> beg(buf), end;
    std::vector<std::string> tokens(beg, end);
    //std::copy(tokens.begin(), tokens.end(), arr);
  return tokens;
}



// =============================================================================

// displayPPM()

//

// This function displays a globally defined PPM file 

// (that was read in and stored as a pix map)

// =============================================================================

void displayPPM(int argc, char *argv[]){
	// OpenGL Commands:
        // Once "glutMainLoop" is executed, the program loops indefinitely to all
        // glut functions.  
        glutInit(&argc, argv);
        glutInitWindowPosition(100, 100); // Where the window will display on-screen.
        glutInitWindowSize(inputImage.width, inputImage.height);
        glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
        glutCreateWindow("Nitin Bansal");
        init();
        glutReshapeFunc(windowResize);
        glutDisplayFunc(windowDisplay);
        glutMouseFunc(processMouse);
        glutMainLoop();
}



int main(int argc, char *argv[])

{        
  string choice;
  readPPM(argv[1], inputImage);
  cout<<"\n\nrotate";
  cout<<"\ntranslate";
  cout<<"\nscale";
  cout<<"\nshear";
  cout<<"\nmirror";
  cout<<"\nperspective";
  cout<<"\nEnter your choice : ";

  getline(cin, choice);
  vector<std::string> inp = split(choice);

  if(inp[0] == "rotate")
  {
    rotate(45);
  }
  else if(inp[0] == "translate")
  {
    translate(100, 150);
  }

  else if(inp[0] == "scale")

  {
    scale(1.5, 1.5);
  }

  else if(inp[0] == "shear")
  {
    shear(0.1, 0.4);
  }

  else if(inp[0] == "mirror")
  {
    mirror(1, -1);
  }

  else if(inp[0] == "perspective")
  {
    perspective(0.0005, 0.0005);
  }

    displayPPM(argc, argv);
    return 0; //This line never gets reached. We use it because "main" is type int.

}
