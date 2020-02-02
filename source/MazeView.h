/*!
 * File Name:			MazeView.h
 * Author:				Dewey Mao
 * Creation Date:		2015/6/16
 * Description:			display and interaction
 * Modifier:			
 * Modification Date:	
 * Email:				DeweyMao@foxmail.com
 */

#pragma once
class MazeView
{
public:
	MazeView();
	~MazeView();

public:
	bool SetUpOpenGLView(int argc, char* argv[]);
	
private:
	void InitGLView();
	
	//callback function
	static void OnDisplay();
	static void OnReshape(int nWidth, int nHeight);
	static void OnClickMouse(int nButton, int nState, int nPtX, int nPtY);
	static void OnMouseMotion(int nPtX, int nPtY);
	static void OnKeyboard(unsigned char cKey, int nPtX, int nPtY);
	static void OnSpcialKey(int nKey,int nPtX,int nPtY);
	static void OnIdleTime();
};

