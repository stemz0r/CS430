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
void jump(float delta_seconds);

static int win = 0;

#define PI 3.1415926535897932384
#define LEFT 0
#define RIGHT 1


/*global variables*/
int KeyDown[256], SpecialDown[256];		//arrays to store keys for press/release checks
int last_time = 0;						//variable for delta_time calculations
float proj_pos[2] = {0.0f, 0.0f};		//projectile position, initially at the origin
float proj_vel = 1.0f, AI_vel = 0.3f, player_vel = 0.01f;   //projectile velocity when fired and A.I. enemies' velocity
float player[2] = {1.1f, -0.6f};			//player position, initially at the origin
float camera[2] = {1.5f, 0.0f};			//camera position, initially at the origin
float score[2] = {1.6f, 0.7f};			//score position, initially in top right of first screen
float AI_pos[5][2] = {{5.0f, -0.7f}, {14.0f, -0.7f}, {15.0f, -0.7f}, {20.0f, -0.7f}, {28.0f, -0.7f}};		//initial enemy positions
int AI_dir[5] = {LEFT, LEFT, LEFT, LEFT, LEFT}; //initial enemy movement directions
float angle = 0.0;						//angle of the projectile between the crosshairs and the player
float overallTime = 0.0f;				//keep track of overall time elapsed for various functions
int moving = 0;							//used as a boolean variable to determine whether or not the projectile is in motion
int p_score = 0;						//player score
bool jumping = 0, dir = 0;				//direction determined by 0 (left) and 1 (right)
float last_ground_position = -0.6f;

int channel = -1;
Mix_Chunk* hit = NULL;
Mix_Chunk* miss = NULL;

/*initialize the sound effects (NEED TO CHANGE)*/
void initSounds(void)
{
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) != -1)
	{
		hit = Mix_LoadWAV("pong2.wav");
		miss = Mix_LoadWAV("pongMiss.wav");
	}
}

/*render the basic scene: ground, background, etc*/
void drawScene(void)
{
	/*figure out how to move the ground and the background with the player*/
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		/*ground*/
		glVertex3f(0.0f, -0.8f, 0.0f);
		glVertex3f(0.0f, -1.0f, 0.0f);
		glVertex3f(40.0f, -1.0f, 0.0f);
		glVertex3f(40.0f, -0.8f, 0.0f);
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

/*render the player*/
void drawPlayer(void)
{
	/*player is just a quad for now, will change later*/
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glVertex3f(-0.05f + player[0], -0.2f + player[1], 0.0f);
		glVertex3f(-0.05f + player[0], 0.2f + player[1], 0.0f);
		glVertex3f(0.05f + player[0], 0.2f + player[1], 0.0f);
		glVertex3f(0.05f + player[0], -0.2f + player[1], 0.0f);
	glEnd();
}

/*draw the platforms in the scene above the ground*/
void drawPlatforms(void)
{
	glColor3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex3f(4.0f, -0.2f, 0.0f);
		glVertex3f(4.5f, -0.2f, 0.0f);
		glVertex3f(4.5f, -0.1f, 0.0f);
		glVertex3f(4.0f, -0.1f, 0.0f);

		glVertex3f(5.0f, -0.2f, 0.0f);
		glVertex3f(5.5f, -0.2f, 0.0f);
		glVertex3f(5.5f, -0.1f, 0.0f);
		glVertex3f(5.0f, -0.1f, 0.0f);

		glVertex3f(4.5f, 0.4f, 0.0f);
		glVertex3f(5.0f, 0.4f, 0.0f);
		glVertex3f(5.0f, 0.3f, 0.0f);
		glVertex3f(4.5f, 0.3f, 0.0f);
	glEnd();

	glPushMatrix();
	glScalef(0.5f, 0.1f, 0.1f);
	glTranslatef(3.0f, 0.0f, 0.0f);
	glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCube(1);
	glPopMatrix();
}

/*draw objects and obstacles in the scene*/
void drawObjects(void)
{
	glColor3f(0.5f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex3f(10.0f, -0.8f, 0.0f);
		glVertex3f(12.0f, -0.8f, 0.0f);
		glVertex3f(12.0f, -0.55f, 0.0f);
		glVertex3f(10.0f, -0.55f, 0.0f);

		glVertex3f(18.0f, -0.8f, 0.0f);
		glVertex3f(19.0f, -0.8f, 0.0f);
		glVertex3f(19.0f, -0.35f, 0.0f);
		glVertex3f(18.0f, -0.35f, 0.0f);

		glVertex3f(25.0f, -0.8f, 0.0f);
		glVertex3f(26.0f, -0.8f, 0.0f);
		glVertex3f(26.0f, -0.35f, 0.0f);
		glVertex3f(25.0f, -0.35f, 0.0f);

		glVertex3f(32.0f, -0.8f, 0.0f);
		glVertex3f(34.0f, -0.8f, 0.0f);
		glVertex3f(34.0f, -0.55f, 0.0f);
		glVertex3f(32.0f, -0.55f, 0.0f);
	glEnd();
}

/*render the enemies*/
void drawEnemies(void)
{
	glColor3f(0.6f, 0.6f, 0.6f);
	glBegin(GL_QUADS);
		glVertex3f(AI_pos[0][0] + 0.05, AI_pos[0][1] + 0.1, 0.0f);
		glVertex3f(AI_pos[0][0] - 0.05, AI_pos[0][1] + 0.1, 0.0f);
		glVertex3f(AI_pos[0][0] - 0.05, AI_pos[0][1] - 0.1, 0.0f);
		glVertex3f(AI_pos[0][0] + 0.05, AI_pos[0][1] - 0.1, 0.0f);

		glVertex3f(AI_pos[1][0] + 0.05, AI_pos[1][1] + 0.1, 0.0f);
		glVertex3f(AI_pos[1][0] - 0.05, AI_pos[1][1] + 0.1, 0.0f);
		glVertex3f(AI_pos[1][0] - 0.05, AI_pos[1][1] - 0.1, 0.0f);
		glVertex3f(AI_pos[1][0] + 0.05, AI_pos[1][1] - 0.1, 0.0f);

		glVertex3f(AI_pos[2][0] + 0.05, AI_pos[2][1] + 0.1, 0.0f);
		glVertex3f(AI_pos[2][0] - 0.05, AI_pos[2][1] + 0.1, 0.0f);
		glVertex3f(AI_pos[2][0] - 0.05, AI_pos[2][1] - 0.1, 0.0f);
		glVertex3f(AI_pos[2][0] + 0.05, AI_pos[2][1] - 0.1, 0.0f);

		glVertex3f(AI_pos[3][0] + 0.05, AI_pos[3][1] + 0.1, 0.0f);
		glVertex3f(AI_pos[3][0] - 0.05, AI_pos[3][1] + 0.1, 0.0f);
		glVertex3f(AI_pos[3][0] - 0.05, AI_pos[3][1] - 0.1, 0.0f);
		glVertex3f(AI_pos[3][0] + 0.05, AI_pos[3][1] - 0.1, 0.0f);

		glVertex3f(AI_pos[4][0] + 0.05, AI_pos[4][1] + 0.1, 0.0f);
		glVertex3f(AI_pos[4][0] - 0.05, AI_pos[4][1] + 0.1, 0.0f);
		glVertex3f(AI_pos[4][0] - 0.05, AI_pos[4][1] - 0.1, 0.0f);
		glVertex3f(AI_pos[4][0] + 0.05, AI_pos[4][1] - 0.1, 0.0f);
	glEnd();
		
}

/*Artificial Intelligence code: enemies walk between static obstacles in the scene until they are killed*/
void AI(float delta_seconds)
{
	//enemy 1
	if(AI_pos[0][0] >= 1.0 && AI_dir[0] == LEFT)
		AI_pos[0][0] -= AI_vel * delta_seconds;
	else if(AI_pos[0][0] < 1.0 && AI_dir[0] == LEFT)
	{
		AI_dir[0] = RIGHT;
		AI_pos[0][0] = 1.0;
	}
	else if(AI_pos[0][0] <= 9.95 && AI_dir[0] == RIGHT)
		AI_pos[0][0] += AI_vel* delta_seconds;
	else if(AI_pos[0][0] > 9.95 && AI_dir[0] == RIGHT)
	{
		AI_dir[0] = LEFT;
		AI_pos[0][0] = 9.95;
	}

	//enemy 2
	if(AI_pos[1][0] >= 12.05 && AI_dir[1] == LEFT)
		AI_pos[1][0] -= AI_vel * delta_seconds;
	else if(AI_pos[1][0] < 12.05 && AI_dir[1] == LEFT)
	{
		AI_dir[1] = RIGHT;
		AI_pos[1][0] = 12.05;
	}
	else if(AI_pos[1][0] <= 17.95 && AI_dir[1] == RIGHT)
		AI_pos[1][0] += AI_vel* delta_seconds;
	else if(AI_pos[1][0] > 17.95 && AI_dir[1] == RIGHT)
	{
		AI_dir[1] = LEFT;
		AI_pos[1][0] = 17.95;
	}

	//enemy 3
	if(AI_pos[2][0] >= 12.05 && AI_dir[2] == LEFT)
		AI_pos[2][0] -= AI_vel * delta_seconds;
	else if(AI_pos[2][0] < 12.05 && AI_dir[2] == LEFT)
	{
		AI_dir[2] = RIGHT;
		AI_pos[2][0] = 12.05;
	}
	else if(AI_pos[2][0] <= 17.95 && AI_dir[2] == RIGHT)
		AI_pos[2][0] += AI_vel* delta_seconds;
	else if(AI_pos[2][0] > 17.95 && AI_dir[2] == RIGHT)
	{
		AI_dir[2] = LEFT;
		AI_pos[2][0] = 17.95;
	}

	//enemy 4
	if(AI_pos[3][0] >= 19.05 && AI_dir[3] == LEFT)
		AI_pos[3][0] -= AI_vel * delta_seconds;
	else if(AI_pos[3][0] < 19.05 && AI_dir[3] == LEFT)
	{
		AI_dir[3] = RIGHT;
		AI_pos[3][0] = 19.05;
	}
	else if(AI_pos[3][0] <= 24.95 && AI_dir[3] == RIGHT)
		AI_pos[3][0] += AI_vel* delta_seconds;
	else if(AI_pos[3][0] > 24.95 && AI_dir[3] == RIGHT)
	{
		AI_dir[3] = LEFT;
		AI_pos[3][0] = 24.95;
	}

	//enemy 5
	if(AI_pos[4][0] >= 26.05 && AI_dir[4] == LEFT)
		AI_pos[4][0] -= AI_vel * delta_seconds;
	else if(AI_pos[4][0] < 26.05 && AI_dir[4] == LEFT)
	{
		AI_dir[4] = RIGHT;
		AI_pos[4][0] = 26.05;
	}
	else if(AI_pos[4][0] <= 31.95 && AI_dir[4] == RIGHT)
		AI_pos[4][0] += AI_vel* delta_seconds;
	else if(AI_pos[4][0] > 31.95 && AI_dir[4] == RIGHT)
	{
		AI_dir[4] = LEFT;
		AI_pos[4][0] = 31.95;
	}

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
    score[0] = camera[0] + 0.6;
    char Score[100];
    sprintf_s(Score, "%d", p_score);
    glColor3f(0.0f, 1.0f, 0.0f);
    DrawText(score[0], score[1], 0.0f, GLUT_BITMAP_TIMES_ROMAN_24 , Score);
}

/*when the player approaches the side of the screen, scroll the camera*/
void moveCamera(void)
{
	if(player[0] < 0.8)
		camera[0] = 1.0;
	else
	{
		if(((camera[0] + 1) - player[0]) < 0.8)//when the player approaches the right side of the current window
			camera[0] = player[0] - 0.2;
		else if((player[0] - (camera[0] - 1)) < 0.8)//when the player approaches the left side of the current window
			camera[0] = player[0] + 0.2;
	}
		//move the camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-camera[0], 0.0f, 0.0f);
	printf("camera[0] = %f\n", camera[0]);
}

/*generate smooth keyboard-based movement*/
void smoothMoves(float delta_seconds)
{
	if ( SpecialDown[GLUT_KEY_RIGHT] ) 
	{
		dir = 1;
		if(player_vel < 1.0)
			player_vel += 2 * delta_seconds;
		player[0] += player_vel * delta_seconds;
	}
	if( SpecialDown[GLUT_KEY_LEFT] )
	{
		dir = 0;
		if(player_vel < 1.0)
			player_vel += 2 * delta_seconds;
		if(player[0] < 0.1)
			player[0] = 0.1;
		else
			player[0] -= player_vel * delta_seconds;
	}
	if( SpecialDown[GLUT_KEY_UP] )
	{
		last_ground_position = -0.6f;//will need to address blocks once we have edge detection working
		if(dir == 1){
			if(player_vel < 1.0)
				player_vel += 2 * delta_seconds;
				player[0] += player_vel * delta_seconds;
				while(player[1] <= last_ground_position + 0.5f)
					player[1] += .2 * delta_seconds;
		}
		if (dir == 0){
			if(player_vel < 1.0)
				player_vel += 2 * delta_seconds;
				player[0] -= player_vel * delta_seconds;
				while(player[1] <= last_ground_position + 0.5f)
					player[1] += .2 * delta_seconds;
				
		}


	}
	
	if(!SpecialDown[GLUT_KEY_RIGHT] && dir == 1)//player was moving right then stopped
	{
		if(player_vel > 0.02)
		{
			player_vel -= 2 * delta_seconds;
			player[0] += player_vel * delta_seconds;
		}
	}
	if(!SpecialDown[GLUT_KEY_LEFT] && dir == 0)//player was moving left then stopped
	{
		if(player_vel > 0.02)
		{
			player_vel -= 2 * delta_seconds;
			if(player[0] < 0.1)
				player[0] = 0.1;
			else
				player[0] -= player_vel * delta_seconds;
		}
	}
	if(!SpecialDown[GLUT_KEY_UP] && player[1]>-0.6f)
	{
		//eventually needs to take blocks into account, right now just the ground

			player[1] -= (.5 * delta_seconds);
	}
}

void jump(float delta_seconds)

{
	/*i don't freakin know*/
	//jumping = 0;
	

}

/*display function which calls rendering functions for objects in the scene*/
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	/*RENDER OBJECTS IN SCENE*/
	drawScene();
	drawPlatforms();
	drawObjects();
	drawPlayer();
	drawEnemies();
	moveCamera();
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
void idle(void)
{
	int time = glutGet(GLUT_ELAPSED_TIME);
	int elapsed = time-last_time;
	float delta_seconds = 0.001f*elapsed;
	last_time = time;

	overallTime = time * .001f;

	//proj_pos[0] += proj_vel * moving * cos(angle)*delta_seconds;
	//proj_pos[1] += proj_vel * moving * sin(angle)*delta_seconds;

	
	//printf("player[1] = %f\n", player[1]);

	smoothMoves(delta_seconds);
	AI(delta_seconds);
//	if(jumping == 1)
//		jump(delta_seconds);

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
   glutInitWindowPosition (5, 5);
   glutInitWindowSize (800, 600); //changed window size to 1024x768
   win = glutCreateWindow ("Super Mountaineer Brothers");

   /*FULLSCREEN MODE (COMMENT OUT WINDOW MODE AND UNCOMMENT THIS TO USE FULLSCREEN MODE)*/
 //  glutGameModeString( "1024x600:32@60" ); //the settings for fullscreen mode for netbooks
	//if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) 
	//	glutEnterGameMode();
	//else
	//{
	//	glutGameModeString("1024x768:32@60");//the settings for fullscreen mode for regular displays
	//	glutEnterGameMode();
	//}
		
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

