/*===============================================

	CS418 MP3 Teapot Contest
	Hao Zheng
	zheng41@illinois.edu

	Reference / Resource:
	1.Texture BMP Library for reading BMP file
		http://www.math.ucsd.edu/~sbuss/MathCG/OpenGLsoft/TextureBMP/TextureBMP.html
	2. Load OBJ
		http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

===============================================*/



#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include "RgbImage.h"
using namespace std;



/*===============================================
 All the global variables used are defined here
===============================================*/

// For Load Obj
// Numbe of Vertices and Number of Faces
#define VNUM 1204
#define FNUM 2258

/* Array for 
verteices / vertex normals / faces / faces normals 
*/
GLfloat v[VNUM][3];
GLfloat vn[VNUM][3];
int f[FNUM][3];
typedef struct{
	int* vert;//adjacenet vertices
	int size;//size of the vertices
}adjacent;



/*For Texture
texture[0] for Normal Texture, texture[1] for Environment Mapping
textureflag: 0 for Normal Texture, 1 for Environment Mapping
*/
GLuint texture[2]; 
int textureflag=0;


// For Animation / Control / Environment / Light
int nFPS = 60;
GLfloat fRotateAngle_y = 0.f;
GLfloat fRotateAngle_z = 0.f;
int lightflag=0;
float spot_y=0;

/*===============================================
 All the functions used are defined here
===============================================*/

void itexture(char *tfile,char* efile)
{   

	glClearColor(0,0,0,0); // clear color is gray	
	glEnable(GL_TEXTURE_2D); // Enable Texture
	glEnable(GL_NORMALIZE); // Make normals to be unit length
	glEnable(GL_DEPTH_TEST); //Depth Test
	glShadeModel(GL_SMOOTH); // Instead of GL_FLAT,we want our model be flat
	
	if(textureflag==0){
		/*Regulare Texture Mapping*/
		glDisable(GL_TEXTURE_GEN_S); //Disable Sphe re Map
		glDisable(GL_TEXTURE_GEN_T);	
		RgbImage TexMap(tfile); //Read the BMP Texturefile
		glGenTextures(1, &texture[0]); //Create Texture
		glBindTexture(GL_TEXTURE_2D, texture[0]); //Binding Texture and Parameterize
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		gluBuild2DMipmaps( GL_TEXTURE_2D, 3, TexMap.GetNumCols(), TexMap.GetNumRows(), GL_RGB, GL_UNSIGNED_BYTE, TexMap.ImageData() ); // Create Texture from BMP data
	

	}
	else if(textureflag==1){
		/*Environment Mapping*/
	
		RgbImage EnvMap(efile); //Read the BMP Environment file
		glGenTextures(1, &texture[1]); //Create Texture
		glBindTexture(GL_TEXTURE_2D, texture[1]); //Binding Texture and Parameterize
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); //Sphere Map
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable(GL_TEXTURE_GEN_S); //Generate Sphere Map
		glEnable(GL_TEXTURE_GEN_T);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		gluBuild2DMipmaps( GL_TEXTURE_2D, 3, EnvMap.GetNumCols(), EnvMap.GetNumRows(), GL_RGB, GL_UNSIGNED_BYTE, EnvMap.ImageData() ); // Create Texture from BMP data
	}

	


}



bool loadobj(const char * filename){


	FILE *obj=fopen(filename,"r"); // Open the obj file
	if(obj==NULL){printf("Sorry,but file is invalid.\n"); return false;}

	// For Performance, Declare all temp variables will be used in futture here
	float x,y,z,nx,ny,nz;
	float vax,vay,vaz;
	float vbx,vby,vbz;
	float sx,sy,sz;
	float isquare;
	float vnorm[FNUM][3]; //Array for temp variables
	int v1; int v2; int v3; int vcount,fcount; 
	vcount=0; fcount=0; //Clear the counter
	adjacent adjfaces[VNUM];// adjacent faces, also this is a local variable,will free automatically
	char buffer[128]; // buffer for reading from file one line
	

	// Clean the array
	for(int i = 0; i < VNUM; i++)
	{
		vnorm[i][0] = 0; 		vnorm[i][1] = 0; 		vnorm[i][2] = 0;  //temp vertices arrat
		v[i][0] = 0; 	v[i][1] = 0;	v[i][2] = 0; //vertices array
		vn[i][0] = 0;	vn[i][1] = 0;	vn[i][2] = 0; //vertex normals array
		adjfaces[i].size=0;
		//Local Variable so it will free automatically after excute function
		adjfaces[i].vert=new int[FNUM];
	}
	
	// Model Loading 
	// Reference: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
	
	while(1) 
	{	
		int returnread=fscanf(obj,"%s",buffer);//read one line from buffer

		if(	returnread==EOF ) {break;}
		//Check if reading a vertices
		else if(strcmp( buffer, "v" ) == 0)
		{
			vcount++; //Increment total number of vertices
			fscanf( obj, "%f %f %f", &x,&y,&z); 
			//fscanf( obj, "%f", &x); 	fscanf( obj, "%f", &y); 	fscanf( obj, "%f", &z);
			v[vcount][0] = x; 	v[vcount][1] = y; 	v[vcount][2] = z; // read the vertices
		}
	
		else if(strcmp( buffer, "f" ) == 0)
		{
			fcount++;
			fscanf( obj, "%d %d %d", &v1,&v2,&v3); 
			int size1=0; int size2=0; int size3=0;
			size1=adjfaces[v1].size; size2=adjfaces[v2].size; size3=adjfaces[v3].size;
			 adjfaces[v1].vert[size1]=fcount;
			 adjfaces[v2].vert[size2]=fcount;
			 adjfaces[v3].vert[size3]=fcount;

			adjfaces[v1].size++; adjfaces[v2].size++; adjfaces[v3].size++;
	

						

			if(fcount<FNUM){
			f[fcount][0]=v1; f[fcount][1]=v2; f[fcount][2]=v3;	// Insert back to face array
			}
			// Calculate face normal
			// Description from document:
			//     You will need to create per-vertex normals, which you can create by setting a per-face normal,
			//     and then setting the vertex normal to be the sum of its adjacent face normals,
			//     normalized (so the resulting vertex normal is unit length).


			// Calculate per face normal
			// http://www.ehow.com/how_7819455_calculate-normal-opengl.html

			//Get two vectors
			vax=v[v2][0]-v[v1][0]; 		vay=v[v2][1]-v[v1][1]; 			vaz=v[v2][2]-v[v1][2];
			vbx=v[v3][0]-v[v1][0];		vby=v[v3][1]-v[v1][1];			vbz=v[v3][2]-v[v1][2];

			//Dot product to get normals
			nx=vay*vbz-vaz*vby;    ny=vaz*vbx-vax*vbz;   nz=vax*vby-vay*vbx;
			
			//Normalize all vectors
			isquare=nx*nx+ny*ny+nz*nz;
			
			if(fcount<FNUM){
			vnorm[fcount][0]=nx/sqrt(isquare);  vnorm[fcount][1]=ny/sqrt(isquare);	vnorm[fcount][2]=nz/sqrt(isquare);
			}
			}



	}
	//End of while loop
	//setting the vertex normal to be the sum of its adjacent face normals,
	//normalized (so the resulting vertex normal is unit length).
		for(int vertex=0;vertex<=vcount; vertex++)
	{

		//Clean sum
		sx=sy=sz=0;

		//Sum of the adjacement face normals
		for(int face=0;face<adjfaces[vertex].size;face++)
		{
			sx=vnorm[adjfaces[vertex].vert[face]][0]+sx;	     sy=vnorm[adjfaces[vertex].vert[face]][1]+sy;       sz=vnorm[adjfaces[vertex].vert[face]][2]+sz;
		}

		//normalize
		isquare=sx*sx+sy*sy+sz*sz;

		vn[vertex][0]=sx/sqrt(isquare);	vn[vertex][1]=sy/sqrt(isquare);	vn[vertex][2]=sz/sqrt(isquare);
	}


	
		return true;

}
void light()
{

	if(lightflag==0){


		//Ambinet Light 
	
		// GL_POINT / GL_LINE / GL_FILL (default)
	

		GLfloat tanamb[] = {0.2,0.15,0.1,1.0};
		GLfloat tandiff[] = {0.4,0.3,0.2,1.0};

		GLfloat seaamb[] = {0.0,0.0,0.2,1.0};
		GLfloat seadiff[] = {0.0,0.0,0.8,1.0};
		GLfloat seaspec[] = {0.5,0.5,1.0,1.0};


		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f (1.0, 1.0, 1.0);
	

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, tanamb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tandiff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tandiff);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, seaamb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, seadiff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, seaspec);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
	
		glEnable(GL_DEPTH_TEST);
		
		

	}
	else if(lightflag==1){
		//Spotlight

		// The color emitted by the material
		GLfloat ambient[4]= { 0.5f,0.5f,0.5f,1.0f }; 
		GLfloat diffuse[4]= { 0.5f,0.5f,0.5f,1.0f };
		GLfloat specular[4] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat shininess[1] = { 60.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR,specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

		// Ambient Light
		GLfloat diffuse1[4]= { 1.0, 0.0, 0, 1.0}; // Red
		GLfloat position1[4] = { 1.0, 1.0, 1.0, 0.0 }; // Ambient Light
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse1);
        glLightfv(GL_LIGHT0, GL_POSITION,position1);
 
		//Spot Light
		GLfloat ambient2[]= { 0.6, 0.6, 0, 1.0 };  // A mix of red and green
		GLfloat diffuse2[]= { 1, 0, 1, 1.0 }; // A mix of red and blue 
		GLfloat specular2[] = { 0.2, 0.8, 0.8, 1.0}; // Some kinf of yellow 
		GLfloat position2[] = { -3.0, -3.0, 3.0, 1.0  };
		GLfloat spot[4]={ 1,1.0,-1.0};  


		glLightfv(GL_LIGHT1, GL_AMBIENT, ambient2);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse2);
		glLightfv(GL_LIGHT1, GL_SPECULAR,specular2);
		glLightfv(GL_LIGHT1, GL_POSITION,position2);
	    glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION,spot);
 
		// Enable All Lighting
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_DEPTH_TEST);

	}



}

void display(void)
{

	if(textureflag==0){
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode (GL_MODELVIEW); 
		// Begin Trans Matrix
		glLoadIdentity ();
		glPushMatrix();
		gluLookAt(0.0, 0.0, 6,  /* eye is at (0,0,30) */
				  0.0, 0.0, 0.0,      /* center is at (0,0,0) */
				  0.0, 1.0, 0.);      /* up is in postivie Y direction */
		glTranslatef(0.0,0.0,-5);
		glRotatef(fRotateAngle_y, 0, 1, 0);
		glRotatef(fRotateAngle_z, 0, 0, 1);

		// Start drawing teapot
		glBegin(GL_TRIANGLES);
	
		for(int i = 1; i < FNUM; i++)
		{

			glTexCoord2f(v[f[i][0]][0],v[f[i][0]][1]);
			glNormal3f(vn[f[i][0]][0],vn[f[i][0]][1],vn[f[i][0]][2]);
			glVertex3f(v[f[i][0]][0],v[f[i][0]][1],v[f[i][0]][2]);


			glTexCoord2f(v[f[i][1]][0],v[f[i][1]][1]);
			glNormal3f(vn[f[i][1]][0],vn[f[i][1]][1],vn[f[i][1]][2]);
			glVertex3f(v[f[i][1]][0],v[f[i][1]][1],v[f[i][1]][2]);
		
			glTexCoord2f(v[f[i][2]][0],v[f[i][2]][1]);
			glNormal3f(vn[f[i][2]][0],vn[f[i][2]][1],vn[f[i][2]][2]);
			glVertex3f(v[f[i][2]][0],v[f[i][2]][1],v[f[i][2]][2]);

	
		}
		glEnd();	
		glPopMatrix ();
	}

	else if(textureflag==1)
	{

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode (GL_MODELVIEW); 
		// Begin Trans Matrix
		glLoadIdentity ();
		glPushMatrix();
		gluLookAt(0.0, 0.0, 6,  /* eye is at (0,0,30) */
				  0.0, 0.0, 0.0,      /* center is at (0,0,0) */
				  0.0, 1.0, 0.);      /* up is in postivie Y direction */
		glTranslatef(0.0,0.0,-5);
		glRotatef(fRotateAngle_y, 0, 1, 0);
		glRotatef(fRotateAngle_z, 0, 0, 1);

		// Start drawing teapot
		glBegin(GL_TRIANGLES);
	
	
		for(int i = 1; i < FNUM; i++)
		{

			glTexCoord2f(v[f[i][0]][0],v[f[i][0]][1]);
			glNormal3f(vn[f[i][0]][0],vn[f[i][0]][1],vn[f[i][0]][2]);
			glVertex3f(v[f[i][0]][0],v[f[i][0]][1],v[f[i][0]][2]);


			glTexCoord2f(v[f[i][1]][0],v[f[i][1]][1]);
			glNormal3f(vn[f[i][1]][0],vn[f[i][1]][1],vn[f[i][1]][2]);
			glVertex3f(v[f[i][1]][0],v[f[i][1]][1],v[f[i][1]][2]);
		
			glTexCoord2f(v[f[i][2]][0],v[f[i][2]][1]);
			glNormal3f(vn[f[i][2]][0],vn[f[i][2]][1],vn[f[i][2]][2]);
			glVertex3f(v[f[i][2]][0],v[f[i][2]][1],v[f[i][2]][2]);

	
		}
		glEnd();	
		glPopMatrix ();

		//Use Cube Map
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glTranslatef(0.0f, 0.0f, -10.0f);
		glBegin(GL_QUADS);
			glNormal3f( 0.0f, 0.0f, 1.0f);
			glTexCoord2f(1.0f, 0.0f); 
			glVertex3f( 10.0f, -10.0f,  10.0f);
			glTexCoord2f(1.0f, 1.0f); 
			glVertex3f( 10.0f,  10.0f,  10.0f);
			glTexCoord2f(0.0f, 1.0f); 
			glVertex3f(-10.0f,  10.0f,  10.0f);
		glEnd();
	}
	

	else if(textureflag==2)
	{
		//Using Utah Teapot
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode (GL_MODELVIEW); 
		// Begin Trans Matrix
		glLoadIdentity ();
		glPushMatrix();
		gluLookAt(0.0, 0.0, 6,  /* eye is at (0,0,30) */
				  0.0, 0.0, 0.0,      /* center is at (0,0,0) */
				  0.0, 1.0, 0.);      /* up is in postivie Y direction */
		glTranslatef(0.0,0.0,-5);
		glRotatef(fRotateAngle_y, 0, 1, 0);
		glRotatef(fRotateAngle_z, 0, 0, 1);

		// Start drawing teapot
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glutSolidTeapot(2); // Teapot size is 2
		glFlush(); 
		glDisable(GL_TEXTURE_2D);
		glPopMatrix ();

	}
	else if(textureflag==3)
	{
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode (GL_MODELVIEW); 
		// Begin Trans Matrix
		glLoadIdentity ();
		glPushMatrix();
		gluLookAt(0.0, 0.0, 6,  /* eye is at (0,0,30) */
				  0.0, 0.0, 0.0,      /* center is at (0,0,0) */
				  0.0, 1.0, 0.);      /* up is in postivie Y direction */
		glTranslatef(0.0,0.0,-5);
		glRotatef(fRotateAngle_y, 0, 1, 0);
		glRotatef(fRotateAngle_z, 0, 0, 1);

		//Blend
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_COLOR_MATERIAL);
		// Start drawing teapot
		glBegin(GL_TRIANGLES);
		// Alpha to be 0.6
		glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
		for(int i = 1; i < FNUM; i++)
		{

			glTexCoord2f(v[f[i][0]][0],v[f[i][0]][1]);
			glNormal3f(vn[f[i][0]][0],vn[f[i][0]][1],vn[f[i][0]][2]);
			glVertex3f(v[f[i][0]][0],v[f[i][0]][1],v[f[i][0]][2]);


			glTexCoord2f(v[f[i][1]][0],v[f[i][1]][1]);
			glNormal3f(vn[f[i][1]][0],vn[f[i][1]][1],vn[f[i][1]][2]);
			glVertex3f(v[f[i][1]][0],v[f[i][1]][1],v[f[i][1]][2]);
		
			glTexCoord2f(v[f[i][2]][0],v[f[i][2]][1]);
			glNormal3f(vn[f[i][2]][0],vn[f[i][2]][1],vn[f[i][2]][2]);
			glVertex3f(v[f[i][2]][0],v[f[i][2]][1],v[f[i][2]][2]);

	
		}
		glEnd();	
		glPopMatrix ();
	
	
	}
	else { textureflag==0; glutPostRedisplay(); }


	glutSwapBuffers();
}


void reshape(int w, int h)
{
	// reset viewport ( drawing screen ) size
	glViewport(0, 0, w, h);
	float fAspect = ((float)w)/h; 
	// reset OpenGL projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, fAspect, 1, 10000);
}

// keyboard function to control which mapping to show
void keyboard(unsigned char key, int x, int y)
{
	// press 't' for texture mapping
	if(key == 't')
	{
		textureflag = 0;
		itexture("t.bmp","e.bmp");
	}
	// press 'e' for environment mapping
	if(key == 'e')
	{
		textureflag = 1;
		itexture("t.bmp","e.bmp");
	}
	if(key == 'u')
	{
		textureflag = 2;
		//itexture("t.bmp","e.bmp");	
	}
	if(key == 'a')
	{
		textureflag = 3;
		//itexture("t.bmp","e.bmp");	
	}
	if(key == 's')
	{
		lightflag = 1;
		light();
	}
	if(key == 'l')
	{
		lightflag = 0;
		light();
	}
	if(key == 'p')
	{
		spot_y++;
		light();
	}



}

void timer(int v)
{
	fRotateAngle_y += 1; // change rotation angles
	fRotateAngle_z += 1; // change rotation angles
	glutPostRedisplay(); // trigger display function by sending redraw into message queue
	glutTimerFunc(1000/nFPS,timer,v); // restart timer again
}

int main(int argc, char* argv[])
{

	glutInit(&argc, (char**)argv);
	// set up for double-buffering & RGB color buffer & depth test
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
	glutInitWindowSize (1024, 768); 
	glutInitWindowPosition (260, 260);
	glutCreateWindow ((const char*)"Teapot");

	
	// Load texture
	itexture("t.bmp","e.bmp");
	// Load Object and Calculate Normal
	loadobj("teapot.obj");
	// Ambient Light or Spot Light
	light();

	// set up the call-back functions 
	glutDisplayFunc(display);  // called when drawing 
	glutReshapeFunc(reshape);  // called when change window size
	glutKeyboardFunc(keyboard); // called when received keyboard interaction
	glutTimerFunc(100,timer,nFPS); // a periodic timer. Usually used for updating animation
	glutMainLoop(); // start the main message-callback loop

	return 0;

}
