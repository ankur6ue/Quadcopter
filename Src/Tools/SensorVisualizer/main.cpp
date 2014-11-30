/*
 * OGL01Shape3D.cpp: 3D Shapes
 */
#include <windows.h>  // for MS Windows
#include <gl\gl.h>                
#include <gl\glu.h>                            
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include "../ComPortReader/Serial.h"	// Library described above
#include <string>
#include "Utils.h"

#define		SUCCESS 0
#define		FAILURE 1

extern bool        keys[256];                  
extern bool        active;                    
extern bool        fullscreen;               
extern HDC			hDC;
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)	;
int InitGL(GLvoid);
int DrawGLScene(AxAngle);								
GLvoid KillGLWindow(GLvoid);	
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag);

Serial* SetupI2C()
{
	printf("Welcome to the serial test app!\n\n");

	Serial* SP = new Serial("\\\\.\\COM16");    // adjust as needed

	if (SP->IsConnected())
		printf("We're connected");

	return SP;
}

AxAngle ConvertQuaternionToAxisAngle(float qw, float qx, float qy, float qz)
{
	AxAngle axAngle;

	axAngle.angle	= 2 * acos(qw);
	axAngle.x		= qx / sqrt(1-qw*qw);
	axAngle.y		= qy / sqrt(1-qw*qw);
	axAngle.z		= qz / sqrt(1-qw*qw);
	
	return axAngle;
}

void ConvertQuaternionToRotMatrix(float qw, float qx, float qy, float qz, float m[][3])
{
		m[0][0] = 1 - 2*qy*qy - 2*qz*qz;
		m[0][1] = 2*qx*qy - 2*qz*qw;
		m[0][2] = 2*qx*qz + 2*qy*qw;
		m[1][0] = 2*qx*qy + 2*qz*qw;
		m[1][1] = 1 - 2*qx*qx - 2*qz*qz;
		m[1][2] = 2*qy*qz - 2*qx*qw;
		m[2][0] = 2*qx*qz - 2*qy*qw;
		m[2][1] = 2*qy*qz + 2*qx*qw;
		m[2][2] = 1 - 2*qx*qx - 2*qy*qy;
}

AxAngle* toAxisAngle(float m[][3]) {
	double angle,x,y,z; // variables for result
	double epsilon = 0.01; // margin to allow for rounding errors
	double epsilon2 = 0.1; // margin to distinguish between 0 and 180 degrees
	// optional check that input is pure rotation, 'isRotationMatrix' is defined at:
	// http://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/
	if ((fabs(m[0][1]-m[1][0])< epsilon)
		&& (fabs(m[0][2]-m[2][0])< epsilon)
		&& (fabs(m[1][2]-m[2][1])< epsilon)) {
			// singularity found
			// first check for identity matrix which must have +1 for all terms
			//  in leading diagonaland zero in other terms
			if ((fabs(m[0][1]+m[1][0]) < epsilon2)
				&& (fabs(m[0][2]+m[2][0]) < epsilon2)
				&& (fabs(m[1][2]+m[2][1]) < epsilon2)
				&& (fabs(m[0][0]+m[1][1]+m[2][2]-3) < epsilon2)) {
					// this singularity is identity matrix so angle = 0
					return new AxAngle(0,1,0,0); // zero angle, arbitrary axis
			}
			// otherwise this singularity is angle = 180
			angle = 3.14;
			double xx = (m[0][0]+1)/2;
			double yy = (m[1][1]+1)/2;
			double zz = (m[2][2]+1)/2;
			double xy = (m[0][1]+m[1][0])/4;
			double xz = (m[0][2]+m[2][0])/4;
			double yz = (m[1][2]+m[2][1])/4;
			if ((xx > yy) && (xx > zz)) { // m[0][0] is the largest diagonal term
				if (xx< epsilon) {
					x = 0;
					y = 0.7071;
					z = 0.7071;
				} else {
					x = sqrt(xx);
					y = xy/x;
					z = xz/x;
				}
			} else if (yy > zz) { // m[1][1] is the largest diagonal term
				if (yy< epsilon) {
					x = 0.7071;
					y = 0;
					z = 0.7071;
				} else {
					y = sqrt(yy);
					x = xy/y;
					z = yz/y;
				}	
			} else { // m[2][2] is the largest diagonal term so base result on this
				if (zz< epsilon) {
					x = 0.7071;
					y = 0.7071;
					z = 0;
				} else {
					z = sqrt(zz);
					x = xz/z;
					y = yz/z;
				}
			}
			return new AxAngle(angle,x,y,z); // return 180 deg rotation
	}
	// as we have reached here there are no singularities so we can handle normally
	double s = sqrt((m[2][1] - m[1][2])*(m[2][1] - m[1][2])
		+(m[0][2] - m[2][0])*(m[0][2] - m[2][0])
		+(m[1][0] - m[0][1])*(m[1][0] - m[0][1])); // used to normalise
	if (fabs(s) < 0.001) s=1; 
	// prevent divide by zero, should not happen if matrix is orthogonal and should be
	// caught by singularity test above, but I've left it in just in case
	angle = cos(( m[0][0] + m[1][1] + m[2][2] - 1)/2);
	x = (m[2][1] - m[1][2])/s;
	y = (m[0][2] - m[2][0])/s;
	z = (m[1][0] - m[0][1])/s;
	return new AxAngle(angle,x,y,z);
}
void CalculateInverse(float m[][3], float invm[][3])
{
	invm[0][0] = m[0][0];
	invm[1][1] = m[1][1];
	invm[2][2] = m[2][2];
	invm[0][1] = m[1][0];
	invm[0][2] = m[2][0];
	invm[1][0] = m[0][1];
	invm[1][3] = m[3][1];
	invm[2][0] = m[0][2];
	invm[2][1] = m[1][2];
}

/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
   
	char incomingData[20000] = "";			// don't forget to pre-allocate memory
	//printf("%s\n",incomingData);
	unsigned int dataLength = 15000;
	int readResult = 0;
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop
	BOOL fullscreen=FALSE;							// Windowed Mode
	char c = "c"[0];
	int k = 'c';
	Serial* SP = SetupI2C();
	static int counter = 0;
	float invm[3][3];
	// Create Our OpenGL Window
	if (!CreateGLWindow("NeHe's Rotation Tutorial",640,480,16,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while(!done)									// Loop That Runs While done=FALSE
	{

		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			// Process i2c communication
			// DrawScreen(screen,h++);
			AxAngle axAngle;
			float w, x, y, z; 
			bool needDraw = false;
			if(SP->IsConnected())
			{
				memset(incomingData, 0, 20000);
				readResult = SP->ReadData(incomingData,dataLength);
				//	printf("Bytes read: (-1 means no data available) %i\n",readResult);
				if (readResult == -1) continue;
				
			//	printf("number of bytes read = %d", readResult);
				for (int i = 0; i < readResult-3; i++)
				{
					if (incomingData[i] == 'q' && incomingData[i+1] == 'u' 
						&& incomingData[i+2] == 'a' && incomingData[i+3] == 't')
					{
						sscanf(incomingData+i+4, "%f\t%f\t%f\t%f", &w, &x, &y, &z);
						float norm = sqrt(w*w + x*x + y*y + z*z);
						w = w/norm; x = x/norm; y = y/norm; z = z/norm;
						float m[3][3];
						ConvertQuaternionToRotMatrix(w,x,y,z,m);
						AxAngle* axAngle1 = toAxisAngle(m);
						delete axAngle1;
						axAngle = ConvertQuaternionToAxisAngle(w, x, y, z);
						needDraw = true;
						break;
					}
					
				//	printf("%c", incomingData[i]);
				}
				//	printf("%s",incomingData);

			//	Sleep(5);
			}

			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((needDraw && active && !DrawGLScene(axAngle)) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			{
				done=TRUE;							// ESC or DrawGLScene Signalled A Quit
			}
			else									// Not Time To Quit, Update Screen
			{
				SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1]=FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("NeHe's Rotation Tutorial",640,480,16,fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
  
//   glutMainLoop();                 // Enter the infinite event-processing loop
   return 0;
}