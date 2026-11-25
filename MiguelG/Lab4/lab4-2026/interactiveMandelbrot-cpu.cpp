// Mandelbrot explorer, based on my old Julia demo plus parts of Nicolas Melot's Lab 1 code.
// CPU only! Your task: Rewrite for CUDA! Test and evaluate performance.

// Compile with the included makefile.

// Preliminary version 2014-11-30
// Cleaned a bit more 2014-12-01
// Corrected the missing glRasterPos2i 2014-12-03
// 2026: Total overhaol of the graphics to be at least a little more modern. Uses simplefont for the text.

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include "MicroGlut.h"
//#include <GLUT/glut.h>
#else
#define GL3_PROTOTYPES
//#include <GL3/gl3.h>
//#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/gl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simplefont.h"
#include "milli.h"

// Image data
	unsigned char	*pixels = NULL;
	int	 gImageWidth, gImageHeight;
// Display data
	unsigned int vertexArrayObjID;
	unsigned int vertexBufferObjID;
	unsigned int vertexArrayObjID2;
	unsigned int vertexBufferObjID2;
	GLuint shader;
	GLuint texid, minitexid;

GLfloat vertices[] = {	-1.0f,-1.0f,0.0f,
						-1.0f,1.0f,0.0f,
						1.0f,-1.0f,0.0f,
						
						1.0f,-1.0f,0.0f,
						-1.0f,1.0f,0.0f,
						1.0f,1.0f,0.0f };

GLfloat vertices2[] = {	-0.9f,-0.6f,0.0f,
						-0.9f,0.5f,0.0f,
						0.9f,-0.6f,0.0f,
						
						0.9f,-0.6f,0.0f,
						-0.9f,0.5f,0.0f,
						0.9f,0.5f,0.0f };

// Inline shaders (usually not what I do, but here I avoid too many files)
char texture_frag[] =
"#version 410\n"
"out vec4 outColor;"
"in vec2 texCoord;"
"uniform sampler2D tex;"
"void main(void)"
"{outColor = texture(tex, texCoord);}";

char texture_vert[]=
"#version 410\n"
"in  vec3 inPosition;"
"out vec2 texCoord;"
"uniform mat4 scale;"
"void main(void)"
"{"
"	texCoord = (vec2(inPosition) + vec2(1.0)) / 2;"
"	gl_Position = scale * vec4(inPosition, 1.0);"
"}";


// Init image data
void initBitmap(int width, int height)
{
	if (pixels) free(pixels);
	pixels = (unsigned char *)malloc(width * height * 4);
	gImageWidth = width;
	gImageHeight = height;
}

#define DIM 1024
#define BLOCKSIZE 16
// Select precision here! float or double!
#define MYFLOAT float

// User controlled parameters
int maxiter = 20;
MYFLOAT offsetx = -200, offsety = 0, zoom = 0;
MYFLOAT scale = 1.5;

// Complex number class
struct cuComplex
{
    MYFLOAT   r;
    MYFLOAT   i;
    
    cuComplex( MYFLOAT a, MYFLOAT b ) : r(a), i(b)  {}
    
    float magnitude2( void )
    {
        return r * r + i * i;
    }
    
    cuComplex operator*(const cuComplex& a)
    {
        return cuComplex(r*a.r - i*a.i, i*a.r + r*a.i);
    }
    
    cuComplex operator+(const cuComplex& a)
    {
        return cuComplex(r+a.r, i+a.i);
    }
};

int mandelbrot( int x, int y)
{
    MYFLOAT jx = scale * (MYFLOAT)(gImageWidth/2 - x + offsetx/scale)/(gImageWidth/2);
    MYFLOAT jy = scale * (MYFLOAT)(gImageHeight/2 - y + offsety/scale)/(gImageHeight/2);

    cuComplex c(jx, jy);
    cuComplex a(jx, jy);

    int i = 0;
    for (i=0; i<maxiter; i++)
    {
        a = a * a + c;
        if (a.magnitude2() > 1000)
            return i;
    }

    return i;
}

void computeFractal( unsigned char *ptr)
{
    // map from x, y to pixel position
    for (int x = 0; x < gImageWidth; x++)
	    for (int y = 0; y < gImageHeight; y++)
	    {
		    int offset = x + y * gImageWidth;

		    // now calculate the value at that position
		    int fractalValue = mandelbrot( x, y);
		    
		    // Colorize it
		    int red = 255 * fractalValue/maxiter;
		    if (red > 255) red = 255 - red;
		    int green = 255 * fractalValue*4/maxiter;
		    if (green > 255) green = 255 - green;
		    int blue = 255 * fractalValue*20/maxiter;
		    if (blue > 255) blue = 255 - blue;
		    
		    ptr[offset*4 + 0] = red;
		    ptr[offset*4 + 1] = green;
		    ptr[offset*4 + 2] = blue;
		    
		    ptr[offset*4 + 3] = 255;
    	}
}

char print_help = 0;

GLfloat scaleMatrix[] = {	1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f };

void PrintHelp()
{
	if (print_help)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glBindTexture(GL_TEXTURE_2D, minitexid); // Select the flat inline texture
		glBindVertexArray(vertexArrayObjID2); // Select the box
		GLint a[4];
		glGetIntegerv(GL_VIEWPORT, a);
		int portWidth = a[2];
		int portHeight = a[3];
		// Make the box still using the scaleMatrix to match the rescaling.
		scaleMatrix[0] = 512.0/portWidth;
		scaleMatrix[5] = 512.0/portHeight;
		scaleMatrix[3] = 512.0/portWidth - 1;
		scaleMatrix[7] = (1-512.0/portHeight);
		glUniformMatrix4fv(glGetUniformLocation(shader, "scale"), 1, GL_TRUE, scaleMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 6);	// draw the box
		glDisable(GL_BLEND);

		sfSetFontColor(1.f, 1.f, 1.f);
		sfDrawString(60, 170, "HELP");
		sfDrawString(60, 200, "h - Toggle Help");
		sfDrawString(60, 230, "Left click + drag - move picture");
		sfDrawString(60, 260, "Right click + drag up/down - unzoom/zoom");
		sfDrawString(60, 290, "+ - Increase max. iterations by 32");
		sfDrawString(60, 320, "- - Decrease max. iterations by 32");
		sfDrawString(60, 350, "< - zoom");
		sfDrawString(60, 380, "> - unzoom");
	}
}

	GLubyte minitex[1][1][4] =
	{
		{ {100, 50,100, 150}}
	};

// Compute fractal and display image
void Draw()
{
	char s[20];
   int elapsedTime;

	ResetMilli();
	computeFractal(pixels);
    elapsedTime = GetMicroseconds();
	printf("%d us\n", elapsedTime);
	
// Dump the whole picture onto the screen. (Old-style OpenGL but without lots of geometry that doesn't matter so much.)
	glClearColor( 0.0, 0.0, 0.0, 1.0 );

//	glClear( GL_COLOR_BUFFER_BIT );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gImageWidth, gImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glBindVertexArray(vertexArrayObjID);	// Select VAO
	scaleMatrix[0] = 1.0;
	scaleMatrix[5] = 1.0;
	scaleMatrix[3] = 0;
	scaleMatrix[7] = 0;
	glUniformMatrix4fv(glGetUniformLocation(shader, "scale"), 1, GL_TRUE, scaleMatrix);
	glDrawArrays(GL_TRIANGLES, 0, 6);	// draw object
	
	if (print_help)
		PrintHelp();
	
	sprintf(s, "%d iterations", maxiter);
	sfDrawString(10, 20, s);
	sprintf(s, "%d microseconds", elapsedTime);
	sfDrawString(10, 40, s);
	sfDrawString(-10, 20, "Press h for help");

	glutSwapBuffers();
}

char explore = 1;

static void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	initBitmap(width, height);

	glutPostRedisplay();
}

int mouse_x, mouse_y, mouse_btn;

// Mouse down
static void mouse_button(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		// Record start position
		mouse_x = x;
		mouse_y = y;
		mouse_btn = button;
	}
}

// Drag mouse
static void mouse_motion(int x, int y)
{
	if (mouse_btn == 0)
	// Ordinary mouse button - move
	{
		offsetx += (x - mouse_x)*scale;
		mouse_x = x;
		offsety += (mouse_y - y)*scale;
		mouse_y = y;
		
		glutPostRedisplay();
	}
	else
	// Alt mouse button - scale
	{
		scale *= pow(1.1, y - mouse_y);
		mouse_y = y;
		glutPostRedisplay();
	}
}

void KeyboardProc(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: /* Escape key */
	case 'q':
	case 'Q':
		exit(0);
		break;
	case '+':
		maxiter += maxiter < 1024 - 32 ? 32 : 0;
		break;
	case '-':
		maxiter -= maxiter > 0 + 32 ? 32 : 0;
		break;
	case '>':
		scale *= 1.5;
		break;
	case '<':
		scale *= 0.7;
		break;
	case 'h':
		print_help = !print_help;
		break;
	}
	glutPostRedisplay();
}

// Set up the graphics
void init()
{
	shader = sfCompileShaders(texture_vert, texture_frag);

	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);
	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);
	
	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 18*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(shader, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(shader, "inPosition"));

	// Allocate and activate Vertex Array Object 2 (minitex - flat)
	glGenVertexArrays(1, &vertexArrayObjID2);
	glBindVertexArray(vertexArrayObjID2);
	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID2);
	
	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID2);
	glBufferData(GL_ARRAY_BUFFER, 18*sizeof(GLfloat), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(shader, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(shader, "inPosition"));

	glUniform1i(glGetUniformLocation(shader, "tex"), 0); // Texture unit 0

	glGenTextures(1, &texid); // texture id
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 

	glGenTextures(1, &minitexid); // texture id
	glBindTexture(GL_TEXTURE_2D, minitexid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, minitex);
}

// Main program, inits
int main( int argc, char** argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize( DIM, DIM );
//	glutInitContextVersion(4, 1);
	glutCreateWindow("Mandelbrot explorer (CPU)");
	glutDisplayFunc(Draw);
	glutMouseFunc(mouse_button);
	glutMotionFunc(mouse_motion);
	glutKeyboardFunc(KeyboardProc);
	glutReshapeFunc(Reshape);
	
	
	init();
	initBitmap(DIM, DIM);
	
	glutMainLoop();
}
