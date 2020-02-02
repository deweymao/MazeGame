#include "stdafx.h"
#include "MazeView.h"
#include "MazeModel.h"
#include "glew.h"
#include "freeglut.h"

long g_lPreMS;
extern MazeModel g_MazeModel;
MazeView::MazeView()
{
}

MazeView::~MazeView()
{
}

bool MazeView::SetUpOpenGLView(int argc, char* argv[])
{
	glutInit(&argc, argv);							//initialize toolkit
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );	//set display mode
	int nScreenX = glutGet(GLUT_SCREEN_WIDTH);
	int nScreenY = glutGet(GLUT_SCREEN_HEIGHT);
	const int nViewWidth = 640;
	const int nViewHeight = 480;
	glutInitWindowSize(nViewWidth, nViewHeight);	//set window size
	int nViewPosX = (nScreenX - nViewWidth) >> 1;
	int nViewPosY = (nScreenY - nViewHeight) >> 1;	//calculate position of window
	glutInitWindowPosition(nViewPosX, nViewPosY);	//set window position
	glutCreateWindow("Maze");						//create window

	if (GLEW_OK != glewInit())						//initialize C++ extension lib
	{
		return false;
	}
		
	glutDisplayFunc(OnDisplay);						//register callback functions
	glutReshapeFunc(OnReshape);
	glutMouseFunc(OnClickMouse);
	glutMotionFunc(OnMouseMotion);
	glutKeyboardFunc(OnKeyboard);
	glutSpecialFunc(OnSpcialKey);
	glutIdleFunc(OnIdleTime);

	InitGLView();									//my initialization								
	glutMainLoop();									//message loop
	return true;
}

void MazeView::InitGLView()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);				//set background color black
	glColor3f(1.0f, 1.0f, 1.0f);					//set foreground color white
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);				//set world window coordinates
	g_lPreMS = GetTickCount();
}

void MazeView::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);					//clear screen 
	g_MazeModel.DisplayMaze();
	g_MazeModel.DisplayFoxPath();	
	g_MazeModel.DisplayPath();
	glFlush();										//output to display device
}

void MazeView::OnReshape(int nWidth, int nHeight)
{
	//keep nWidth/nHeight
// 	int nCalcWidth = (int)(nHeight * 4 / 3);
// 	if (nCalcWidth > nWidth)
// 	{
// 		glViewport(0, 0, nWidth, (int)(nWidth * 3 / 4));
// 	}
// 	else
// 	{
// 		glViewport(0, 0, nCalcWidth, nHeight);
// 	}
	glViewport(0, 0, nWidth, nHeight);				//set view window
}

void MazeView::OnClickMouse(int nButton, int nState, int nPtX, int nPtY)
{
}

void MazeView::OnMouseMotion(int nPtX, int nPtY)
{
}

void MazeView::OnKeyboard(unsigned char cKey, int nPtX, int nPtY)
{
	switch (cKey)
	{
	case '\r':
		{
			g_MazeModel.SolveMaze();
			glutPostRedisplay();
			break;
		}
	default:
		break;
	}
}

void MazeView::OnSpcialKey(int nKey, int nPtX, int nPtY)
{
	switch (nKey)
	{
	case GLUT_KEY_UP:
	{
		g_MazeModel.RunFox(0);
		glutPostRedisplay();
		break;
	}
	case GLUT_KEY_DOWN:
	{
		g_MazeModel.RunFox(1);
		glutPostRedisplay();
		break;
	}
	case GLUT_KEY_LEFT:
	{
		g_MazeModel.RunFox(2);
		glutPostRedisplay();
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		g_MazeModel.RunFox(3);
		glutPostRedisplay();
		break;
	}
	default:
		break;
	}
}

void MazeView::OnIdleTime()
{
	long  lCurMS = 0;;
	lCurMS = GetTickCount();
	if (lCurMS-g_lPreMS >= 500)
	{
		if (g_MazeModel.IsFoxTwinkle())
		{
			g_MazeModel.SetFoxTwinkle(false);
		}
		else
		{
			g_MazeModel.SetFoxTwinkle(true);
		}
		glutPostRedisplay();
		g_lPreMS = GetTickCount();
	}	
}



