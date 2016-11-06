/* =============================================================================
 * VIZA654 at Texas A&M University
 * Created by Nitin Bansal based from Ariel Chisholm's template
 * Modified Date - 10.24.2016
 * This file is supplied with an associated 'makefile'. Put both files in the same
 * directory, navigate to that directory from the Linux shell, and type 'make'.
 * It will create an executable file pr05.
 * Run ./pr05
 * The Input options are as follows
 * [Please note the error handling for this code is very rudimentary]
 * ============================================================================= */



#include <cstdlib>
#include <iostream>
#include <GL/glut.h>
#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#include <algorithm>
#include <cmath>


using namespace std;

// structure for PPM pixels

struct RGB {

      unsigned char red; unsigned char green; unsigned char blue;
};


struct ppmImage{
	int width, height, maxColor;
	RGB *pixmap;
};

float pi = 3.14;
int nsamp = 5;



// =============================================================================

// These variables will store the ppm images' width, height, and data

// =============================================================================

ppmImage inputImage;
ppmImage controlImage;
ppmImage resultImage;

class Vector2D
{

        public:
                float x, y;
                Vector2D(float x = 0, float y = 0);
                ~Vector2D() {};
                float Magnitude() const;
                Vector2D Normalize();
                float DotProduct( const Vector2D& v2 ) const;
                const Vector2D operator-( const Vector2D &v2 ) const;
};

Vector2D::Vector2D( float x, float y )
{
        this->x = x;
        this->y = y;
}

float Vector2D::Magnitude() const
{
        return sqrtf(x * x + y * y);
}

Vector2D Vector2D::Normalize()
{
        float mag = Magnitude();

        Vector2D ret(*this);

        if(mag != 0.0)
        {
                ret.x /= mag;
                ret.y /= mag;
        }

        return ret;
}

float Vector2D::DotProduct( const Vector2D &v2 ) const
{
    return (x * v2.x) + (y * v2.y);
}

const Vector2D Vector2D::operator-( const Vector2D &v2 ) const
{
    return Vector2D(*this) = Vector2D(*this)-v2;
}



// This secondary function nibbles away comment lines for a ppm file

void readComments(ifstream &streamFile){
        string str1;
        while(streamFile.peek() == '#'){

                getline(streamFile, str1);
        }
}


// =============================================================================

// readPPM(char* fileName, ppmImage &image)

// This function reads in a prescribed PPM file (by fileName) and

// stores its contents to the specified ppmImage structure

// =============================================================================

void readPPM(const char* fileName, ppmImage &image){

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



//free the memory of the kernel array

void freeKernelArray(double **filter, int filterSize)

{
  for(int i = 0; i < filterSize; i++)
    delete[] filter[i];
delete[] filter;
}


float findmin(float a, float b)
{
	return (a <= b? a : b);
}



float findmax(float a ,float b)
{
  return (a >= b ? a : b); 
}


void erosion()

{
	resultImage.height = inputImage.height;
	resultImage.width = inputImage.width;
	resultImage.pixmap = new RGB[inputImage.width * inputImage.height];
	for(int x = 0; x < inputImage.height; x++) 
		for(int y = 0; y < inputImage.width; y++) 
		{ 
			int cIndex = x * resultImage.width + y;
			int filterSize = (controlImage.pixmap[cIndex].red/50) + 1;
			//Ensuring Odd Size of the Filter
			if(filterSize % 2 == 0)
				filterSize += 1;

			int min = 255;
			double **filter= new double*[filterSize];
			for(int k = 0; k< filterSize; k++)
				filter[k] = new double[filterSize];

			//kernel computation based in control image's green values
			for(int i = 0; i < filterSize; i++)
				for(int j = 0; j < filterSize; j++) 
				{
					int imageX = (x - filterSize / 2 + i + controlImage.height) % controlImage.height; 
					int imageY = (y - filterSize / 2 + j + controlImage.width) % controlImage.width; 
					int ctrlIndex = imageX * controlImage.width + imageY;
					filter[i][j] = (controlImage.pixmap[ctrlIndex].green/28) + 1;
				}
			for(int i=0;i<filterSize;i++)
				for(int j=0;j<filterSize;j++)
					min = findmin(filter[i][j], min);
			double red = inputImage.pixmap[cIndex].red * filter[filterSize/2][filterSize/2] / min;        
			double green = inputImage.pixmap[cIndex].green * filter[filterSize/2][filterSize/2] / min;
			double blue = inputImage.pixmap[cIndex].blue * filter[filterSize/2][filterSize/2] / min;
			int currIndex;
			//multiply every value of the filter with corresponding image pixel 
			for(int filterX = 0; filterX < filterSize; filterX++)
				for(int filterY = 0; filterY < filterSize; filterY++) 
				{ 
					int imageX = (x - filterSize / 2 + filterX + inputImage.height) % inputImage.height; 
					int imageY = (y - filterSize / 2 + filterY + inputImage.width) % inputImage.width; 
					currIndex = imageX * inputImage.width + imageY;
					red = findmin(inputImage.pixmap[currIndex].red * filter[filterX][filterY]/min,red); 
					green = findmin(inputImage.pixmap[currIndex].green * filter[filterX][filterY]/min, green); 
					blue = findmin(inputImage.pixmap[currIndex].blue * filter[filterX][filterY]/min, blue); 
				} 
			//truncate values smaller than zero and larger than 255 
			resultImage.pixmap[cIndex].red = red; 
			resultImage.pixmap[cIndex].green = green;
			resultImage.pixmap[cIndex].blue = blue;

			freeKernelArray(filter, filterSize);
		} 
}





void dilation()
{
	resultImage.height = inputImage.height;
	resultImage.width = inputImage.width;
	resultImage.pixmap = new RGB[inputImage.width * inputImage.height];
	for(int x = 0; x < inputImage.height; x++) 
		for(int y = 0; y < inputImage.width; y++) 
		{ 
			int cIndex = x * resultImage.width + y;
			int filterSize = (controlImage.pixmap[cIndex].red/50) + 1;
			if(filterSize % 2 == 0)
				filterSize += 1;
			int max = 0;
			double **filter= new double*[filterSize];
			for(int k = 0; k< filterSize; k++)
				filter[k] = new double[filterSize];

			//kernel computation based in control image's red values
			for(int i = 0; i < filterSize; i++)
				for(int j = 0; j < filterSize; j++) 
				{
					int imageX = (x - filterSize / 2 + i + controlImage.height) % controlImage.height; 
					int imageY = (y - filterSize / 2 + j + controlImage.width) % controlImage.width; 
					int ctrlIndex = imageX * controlImage.width + imageY;
					filter[i][j] = (controlImage.pixmap[ctrlIndex].red/28) + 1;
				}

			for(int i=0;i<filterSize;i++)
				for(int j=0;j<filterSize;j++)
					max = findmax(filter[i][j], max);

			double red = inputImage.pixmap[cIndex].red * filter[filterSize/2][filterSize/2] / max;        
			double green = inputImage.pixmap[cIndex].green * filter[filterSize/2][filterSize/2] / max;
			double blue = inputImage.pixmap[cIndex].blue * filter[filterSize/2][filterSize/2] / max;
			int currIndex;

			//multiply every value of the filter with corresponding image pixel 

			for(int filterX = 0; filterX < filterSize; filterX++)
				for(int filterY = 0; filterY < filterSize; filterY++) 
				{ 
					int imageX = (x - filterSize / 2 + filterX + inputImage.height) % inputImage.height; 
					int imageY = (y - filterSize / 2 + filterY + inputImage.width) % inputImage.width; 
					currIndex = imageX * inputImage.width + imageY;
					red = findmax(inputImage.pixmap[currIndex].red * filter[filterX][filterY]/max,red); 
					green = findmax(inputImage.pixmap[currIndex].green * filter[filterX][filterY]/max, green); 
					blue = findmax(inputImage.pixmap[currIndex].blue * filter[filterX][filterY]/max, blue); 
				} 

			//truncate values smaller than zero and larger than 255 

			resultImage.pixmap[cIndex].red = red; 
			resultImage.pixmap[cIndex].green = green;
			resultImage.pixmap[cIndex].blue = blue;
			freeKernelArray(filter, filterSize);
		} 
}



//Ellipse function - using red value of control image's pixel as theta in the parametric equation of ellipse 

double ellipse(double xLoc, double yLoc, double theta, int centerX, int centerY, double s0, double s1)

{
	//create variables to hold cosTheta and sinTheta

	float ct, st;

	ct = cos(theta);
	st = sin(theta);
	//translate 
	xLoc = xLoc - centerX;
	yLoc = yLoc - centerY;

	//rotate 
	xLoc = ct * xLoc + st * yLoc;
	yLoc = -st * xLoc + ct * yLoc;

	//scale 
	xLoc = xLoc/s0;
	yLoc = yLoc/s1;
	return pow(5, -(xLoc * xLoc + yLoc * yLoc));

}





void motionblur()
{  
	int filterSize = 11;
	double xrand, yrand;
	cout<<"Inside Motion Blur"<<endl;
	resultImage.height = inputImage.height;
	resultImage.width = inputImage.width;
	resultImage.pixmap = new RGB[inputImage.width * inputImage.height];

	for(int x = 0; x < inputImage.height; x++) 
		for(int y = 0; y < inputImage.width; y++) 
		{ 
			float red = 0.0, green = 0.0, blue = 0.0; 
			int currIndex;
			int cIndex = x * resultImage.width + y;
			//filter kernel memory allocation
			double **filter= new double*[filterSize];
			for(int k = 0; k< filterSize; k++)
				filter[k] = new double[filterSize];
			double temp, angle;
			double factor = 0.0;
			//using red value as angle from the control Image
			temp = controlImage.pixmap[cIndex].red;
			angle = (temp/255.0) * 180.0;
			//kernel computation based in control image's red values
			for(int kernely = 0; kernely < filterSize; kernely++)
				for(int kernelx = 0; kernelx < filterSize; kernelx++) 
				{
					filter[kernelx][kernely] = 0;
					xrand = rand()%1 + 0;
					yrand = rand()%1 + 0;
					for(int k = 0; k < nsamp ; k++)
						for(int l = 0; l < nsamp; l++)

						{
							//random sampling to find the points for ellipse function

							double pointx = kernelx+((k+xrand) * (1.0/nsamp));
							double pointy = kernely+((l+yrand)*(1.0/nsamp));
							filter[kernelx][kernely] += ellipse(pointx, pointy, angle, filterSize/2, filterSize/2,0.5,filterSize);
						}
					factor += filter[kernelx][kernely];
				}

			//multiply every value of the filter with corresponding image pixel 
			for(int filterX = 0; filterX < filterSize; filterX++) 
				for(int filterY = 0; filterY < filterSize; filterY++) 
				{ 
					int imageX = (x + (filterX-1)); 
					int imageY = (y + (filterY-1)); 
					//kernel location is outside conrol image..

					if((imageY > inputImage.width-1) || (imageY < 0))

					{
						//tempX = value at current pixel
						imageY = y;
					}
					if((imageX > inputImage.height-1) || (imageX < 0))
					{
						//tempY = value at current pixel

						imageX = x;
					}

					currIndex = imageX * inputImage.width + imageY;

					red += inputImage.pixmap[currIndex].red * filter[filterY][filterX]; 
					green += inputImage.pixmap[currIndex].green * filter[filterY][filterX]; 
					blue += inputImage.pixmap[currIndex].blue * filter[filterY][filterX]; 
				} 

			//normalise the rbg values         
			resultImage.pixmap[cIndex].red = red/factor;
			resultImage.pixmap[cIndex].green = green/factor;
			resultImage.pixmap[cIndex].blue = blue/factor;
			freeKernelArray(filter,filterSize);        
		} 
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

	glutCreateWindow("Project-5");

	init();

	glutReshapeFunc(windowResize);

	glutDisplayFunc(windowDisplay);

	glutMouseFunc(processMouse);

	glutMainLoop();

}



// =============================================================================

// main() Program Entry

// =============================================================================

int main(int argc, char *argv[])

{         

	int input;

	cout<<"Enter your choice  1.erosion 2.dilation 3.motion";

	cin>>input;

	switch(input)

	{
		case 1:
			readPPM("dog.ppm", inputImage);
			readPPM("construction.ppm", controlImage);
			erosion();
			break;

		case 2:
			readPPM("construction.ppm", inputImage);
			readPPM("fish.ppm", controlImage);
			dilation();
			break;
		case 3:
			readPPM("fish.ppm", inputImage);
			readPPM("dog.ppm", controlImage);
			motionblur();
			break;
		default:
			cout<<"Incorrect choice";
			exit(0);
	} 

	displayPPM(argc, argv);
	return 0; //This line never gets reached. We use it because "main" is type int.

}
