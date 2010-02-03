/*
CS430 Project
Super Mountaineer Bros.
Nick Fleming
Jason Hooks
Anthony Palma
Adam Trainer
v0.1
*/

#ifdef WIN32
   #include <windows.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>
//#include <SDL.h>
//#include <SDL_mixer.h>
#include "SDL_mixer.h"
#include "SDL.h"


#ifdef __APPLE__
   #include <GLUT/glut.h>
   #include <OpenGL/gl.h>
   #include <OpenGL/glu.h>
#else
   #include <GL/glut.h>
   #include <GL/glu.h>
   #include <GL/gl.h>
#endif

#include <math.h>

using namespace std;
void DrawText(float x, float y, float z, void* font, char* text);
void reset(void);
void newAngle(float, int);

static int win = 0;

#define PI 3.1415926535897932384

/*global variables*/
int KeyDown[256], SpecialDown[256];		//arrays to store keys for press/release checks
int last_time = 0;						//variable for delta_time calculations
float proj_pos[2] = {0.0f, 0.0f};		//projectile position, initially at the origin
float proj_vel = 1.0f, AI_vel = 1.1f, player_vel = 1.0f;   //projectile velocity when fired and A.I. enemies' velocity
float player[2] = {0.0f, 0.0f};			//player position, initially at the origin
float angle = 0.0;						//angle of the projectile between the crosshairs and the player
float overallTime = 0.0f;				//keep track of overall time elapsed for various functions
int moving = 0;							//used as a boolean variable to determine whether or not the projectile is in motion
int p_score = 0;						//player score
bool jumping = 0;

int channel = -1;

Mix_Chunk* hit = NULL;
Mix_Chunk* miss = NULL;

/*initialize the sound effects*/
void initSounds(void)
{
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) != -1)
	{
		hit = Mix_LoadWAV("pong2.wav");
		miss = Mix_LoadWAV("pongMiss.wav");
	}
}

void drawScene(void)
{
	/*figure out how to move the ground and the background with the player*/
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		/*ground*/
		glVertex3f(-1.0f, -0.8f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glVertex3f(10.0f, -1.0f, 0.0f);
		glVertex3f(10.0f, -0.8f, 0.0f);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		/*background (will be textured later)*/
		glVertex3f(-1.0f, -0.8f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glVertex3f(10.0f, 1.0f, 0.0f);
		glVertex3f(10.0f, -0.8f, 0.0f);
	glEnd();
}

void drawPlayer(void)
{
	/*player is just a quad for now, will change later*/
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glVertex3f(-0.8f + player[0], -0.8f + player[1], 0.0f);
		glVertex3f(-0.8f + player[0], -0.4f + player[1], 0.0f);
		glVertex3f(-0.7f + player[0], -0.4f + player[1], 0.0f);
		glVertex3f(-0.7f + player[0], -0.8f + player[1], 0.0f);
	glEnd();
}

/*Artificial Intelligence code for computer paddle in 1-player games*/
void AI(float delta_seconds)
{
   //if(moving==1)//if the ball is moving
   //{
	  // if(isVelNeg==0)//if the ball is moving towards the computer paddle (on the right)
	  // {
		 //  if(rightPadY - proj_pos[1] > 0.05)
			//   rightPadY -= AI_vel * delta_seconds;
		 //  else if(rightPadY - proj_pos[1] < -0.05)
			//   rightPadY += AI_vel * delta_seconds;
	  // }
   //}
}

/*boundary testing for various aspects of the game as defined below*/
void boundaryTests(void)
{
	///*BOUNDARY TESTING (PONG BALL AND COURT)*/
 //  if(proj_pos[0] < -1.0)
 //  {
	//   Mix_PlayChannel(1, miss, 0);
	//   rightScore++;
	//   AI_vel -= 0.075;//this gives the A.I. dynamic difficulty.  if you lose a point, the A.I. paddle slows down slightly.
	//   if(AI_vel < 0.1)
	//	   AI_vel = 0.1;
	//   server = 1; //server to loser, which is left player
	//   reset();
 //  }
 //  else if(proj_pos[0] > 1.0)
 //  {
	//   Mix_PlayChannel(1, miss, 0);
	//   leftScore++;
	//   AI_vel += 0.075;//this gives the A.I. dynamic difficulty.  if you win a point, the A.I. paddle speeds up slightly.
	//   server = 0; //serve to loser, which is right player
	//   reset();
 //  }
 //  else if(proj_pos[1] < -0.85 && proj_pos[0] > -0.87 && proj_pos[0] < 0.87)
 //  {
	//   Mix_PlayChannel(-1, hit, 0);
	//   proj_pos[1] = -0.85;//assures this action is only performed once
	//   angle = (2*PI) - angle;
 //  }
 //  else if(proj_pos[1] > 0.85 && proj_pos[0] > -0.87 && proj_pos[0] < 0.87)
 //  {
	//   Mix_PlayChannel(-1, hit, 0);
	//   proj_pos[1] = 0.85;//assures this action is only performed once
	//   angle = (2*PI) - angle;
 //  }

 //  /*BOUNDARY TESTING (PADDLES AND COURT)*/
 //  if(leftPadY > 0.77)
	//   leftPadY = 0.77;
 //  else if(leftPadY < -0.77)
	//   leftPadY = -0.77;
 //  if(rightPadY > 0.77)
	//   rightPadY = 0.77;
 //  else if(rightPadY < -0.77)
	//   rightPadY = -0.77;

 //  /*BOUNDARY TESTING (PADDLES AND PONG BALL)*/
 //  //left paddle
 //  if(proj_pos[0] > -0.92 && proj_pos[0] < -0.875 && proj_pos[1] < (+0.135 + leftPadY) && proj_pos[1] > (-0.135 + leftPadY))
 //  {
	//   Mix_PlayChannel(-1, hit, 0);
	//   isVelNeg = 0;
	//   if(angle > PI)
	//	   angle -= PI;
	//   else
	//	   angle += PI;
	//   newAngle(leftPadY, 0);
	//   proj_pos[0] = -0.87;
	//   //proj_vel *= 1.0325
	//   proj_vel += 0.075;
 //  }
 //  //right paddle
 //  else if(proj_pos[0] > 0.875 && proj_pos[0] < 0.92 && proj_pos[1] < (+0.135 + rightPadY) && proj_pos[1] > (-0.135 + rightPadY))
 //  {
	//   Mix_PlayChannel(-1, hit, 0);
	//   isVelNeg = 1;
	//   if(angle > PI)
	//	   angle -= PI;
	//   else
	//	   angle += PI;
	//   newAngle(rightPadY, 1);
	//   proj_pos[0] = 0.87;
	//   //proj_vel *= 1.0325;
	//   proj_vel += 0.075;
 //  }
}

/*define what text to be drawn to the screen, including directions and scores*/
void printToScreen(void)
{
   char Score[100];
   sprintf(Score, "%d", p_score);
   glColor3f(0.0f, 1.0f, 0.0f);
   DrawText(0.7f, 0.7f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24 , Score);
}

/*generate smooth keyboard-based movement*/
void smoothMoves(float delta_seconds)
{
	/*if ( KeyDown['d'] || KeyDown['D'] )
        player[0] += player_vel * delta_seconds;
	if ( KeyDown['a'] || KeyDown['A'] )
        player[0] -= player_vel * delta_seconds;
	if(onePlayer == 0)
	{*/
	if ( SpecialDown[GLUT_KEY_RIGHT] )
		player[0] += player_vel * delta_seconds;
	if( SpecialDown[GLUT_KEY_LEFT] )
		player[0] -= player_vel * delta_seconds;
	//}
}

void jump(float delta_seconds)
{
	/*i don't freakin know*/
	jumping = 0;
}

/*display function which calls rendering functions for objects in the scene*/
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	/*RENDER OBJECTS IN SCENE*/
	drawScene();
	drawPlayer();
	/*PERFORM BOUNDARY TESTING*/
	boundaryTests();
    /*PRINT SCORES AND DIRECTIONS TO SCREEN*/
	printToScreen();

	glutSwapBuffers();
}

/*when a point is scored, reset the ball to the origin, reset the ball's velocity, and set the "is ball moving?" variable to false*/
void reset(void)
{
	/*proj_pos[0] = 0.0;
	proj_pos[1] = 0.0;
	proj_vel = 1.0;
	moving = 0;*/
}

/*change the angle when a collision occurs*/
void newAngle(float padY, int side)
{
	//float diff;
	//if(side == 0)//if we're dealing with the left paddle
	//	diff = padY - proj_pos[1];
	//else if(side == 1)//if we're dealing with the right paddle
	//	diff = proj_pos[1] - padY;
	//if(diff == 0)//avoid a divide-by-zero error
	//   diff = 0.00001;
	//float x = (0.135/diff) * 3;
	//float ang2 = (angle + (PI/x)) * -1;
	///*keep angle between 0 and 2*PI for easier calculations*/
	//if(ang2 > (2*PI))
	//	angle = ang2 - (2*PI);
	//else if(ang2 < 0)
	//	angle = ang2 + (2*PI);
	//else
	//	angle = ang2 * -1;

	///*limit the angle of the ball to +/- 60 degrees from the horizontal*/
	//if(angle > (PI/3) && angle <= PI && isVelNeg == 0)
	//	angle = PI/3;
	//else if(angle < ((5*PI)/3) && angle >= PI && isVelNeg == 0)
	//	angle = (5*PI)/3;
	//else if(angle < ((2*PI)/3) && angle >= 0 && isVelNeg == 1)
	//	angle = (2*PI)/2.75;
	//else if(angle > ((4*PI)/3) && angle <= (2*PI) && isVelNeg == 1)
	//	angle = (4*PI)/3;
}

/*update the projectile's position*/
void idle()
{
	int time = glutGet(GLUT_ELAPSED_TIME);
	int elapsed = time-last_time;
	float delta_seconds = 0.001f*elapsed;
	last_time = time;

	overallTime = time * .001f;

	//proj_pos[0] += proj_vel * moving * cos(angle)*delta_seconds;
	//proj_pos[1] += proj_vel * moving * sin(angle)*delta_seconds;

	
	//printf("player[1] = %f\n", player[1]);
	printf("overallTime = %f\n", overallTime);

	smoothMoves(delta_seconds);
	if(jumping == 1)
		jump(delta_seconds);

    glutPostRedisplay();
}

/*draw test to the screen*/
void DrawText(float x, float y, float z, void* font, char* text)
{
	int len, i;
	glRasterPos2f(x, y);
	len = (int) strlen(text);
    for (i = 0; i < len; i++) {
		glutBitmapCharacter(font, text[i]);
	}
}

/*function to utilize arrow keys for a 2-player game*/
void special_down(int key, int x, int y)
{
	SpecialDown[key] = 1;
}

/*function to check when an arrow key is released*/
void special_up(int key, int x, int y)
{
	SpecialDown[key] = 0;
}

/*create the glut window for display*/
void CreateGlutWindow()
{  
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA);

   /*WINDOW MODE (COMMEND OUT FULLSCREEN MODE AND UNCOMMENT THIS TO USE WINDOW MODE)*/
   //glutInitWindowPosition (5, 5);
   //glutInitWindowSize (1024, 768); //changed window size to 1024x768
   //win = glutCreateWindow ("Ponger");

   /*FULLSCREEN MODE (COMMENT OUT WINDOW MODE AND UNCOMMENT THIS TO USE FULLSCREEN MODE)*/
   glutGameModeString( "1024x600:32@60" ); //the settings for fullscreen mode for netbooks
	if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) 
		glutEnterGameMode();
	else
	{
		glutGameModeString("1024x768:32@60");//the settings for fullscreen mode for regular displays
		glutEnterGameMode();
	}
		
	ShowCursor(FALSE); //hides the mouse cursor
}

/*function to check when a keyboard button is released*/
void keyboardUp(unsigned char key, int x, int y)
{
    KeyDown[key] = 0;
}

/*read and interpret keyboard input for various functions*/
void keyboard(unsigned char key, int x, int y)
{
	KeyDown[key] = 1;
    // printf("%s(key=%c (ASCII code = %d) , x=%d, y=%d)\n", __FUNCTION__, key, key, x, y);
	switch(key){ 
		case 27: //ASCII for Esc key
			/*if the Escape key is pressed, the program will exit*/
			glutLeaveGameMode();
			Mix_CloseAudio();
			Mix_FreeChunk(hit);
			exit(0);
			break;

		case 32: //ASCII for Space Bar
			/*figure out how to make character jump*/
			if(jumping == 0)
				jumping = 1;
			break;
		case 13: //ASCII for Enter
			/*?*/
			break;
	}
}

/*common glut function callbacks*/
void CreateGlutCallbacks()
{
	glutDisplayFunc   (display);
	glutIdleFunc	   (idle);
   glutSpecialFunc   (special_down);
   glutSpecialUpFunc (special_up);
   memset( KeyDown, 0, sizeof( KeyDown ) );
   memset( SpecialDown, 0, sizeof( SpecialDown ) );
   glutKeyboardFunc  (keyboard);
   glutKeyboardUpFunc( keyboardUp );
}

/*initialize background color*/
void InitOpenGL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); //sets background to white
}

/*main function*/
int main(int argc, char **argv)
{
	glutInit(&argc, argv); 
	CreateGlutWindow();
	initSounds();
	CreateGlutCallbacks();
	InitOpenGL();
  
   glutMainLoop();

	return 0;		
}

