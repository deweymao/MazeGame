/*!
 * File Name:			MazeDemo.cpp
 * Author:				Dewey Mao
 * Creation Date:		2015/6/16
 * Description:			maze based in OpenGL
 * Modifier:			
 * Modification Date:	
 * Email:				DeweyMao@foxmail.com
 */

#include "stdafx.h"
#include "MazeView.h"
#include "MazeModel.h"

MazeView	g_MazeView;									//MazeView instance only one
MazeModel	g_MazeModel;								//MazeModel instance only one 

int _tmain(int argc, _TCHAR* argv[])
{
	int nRows = 0;
	int nCols = 0;
	printf("**************Maze**************\n");
	printf("1.Press Enter to solve Maze\n");
	printf("2.Press ¡ü ¡ú ¡ý ¡û to move\n");
	printf("Please input number of rows:\n");
	scanf_s("%d", &nRows);
	printf("Please input number of columns:\n");
	scanf_s("%d", &nCols);
	printf("Processing...\n");

	//set maze model
	g_MazeModel.SetRowsAndCols(nRows, nCols);
	bool bRet = g_MazeModel.BuildModel();
	if (!bRet)
	{
		return 1;
	}
	
	//set maze view
	bRet = g_MazeView.SetUpOpenGLView(argc, (char**)argv);
	if (!bRet)
	{
		return 1;
	}

	return 0;
}

