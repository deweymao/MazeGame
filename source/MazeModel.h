/*!
 * File Name:			MazeModel.h
 * Author:				Dewey Mao
 * Creation Date:		2015/6/16
 * Description:			construct maze model
 * Modifier:			
 * Modification Date:	
 * Email:				DeweyMao@foxmail.com
 */

#include <vector>
#include <stack>
#pragma once

typedef struct POINT_2D{
	double x;
	double y;
	POINT_2D(){
		x = 0.0;
		y = 0.0;
	}

	POINT_2D& operator = (const POINT_2D& rhs){
		x = rhs.x;
		y = rhs.y;
		return *this;
	}

	void set(double x, double y){
		this->x = x;
		this->y = y;
	}
}pt;

typedef struct GL_RGB
{
	double r;									//[0.0,1.0]
	double g;
	double b;
	GL_RGB(){
		r = 0.0;
		g = 0.0;
		b = 0.0;
	}

	GL_RGB(double dR,double dG,double dB){
		r = dR;
		g = dB;
		b = dG;
	}
}gl_rgb;

typedef struct ADJOINED_NODE 
{
	int nNodeInx;
	int nDirection;								//0-top,1-bottom,2-left,3-right
	ADJOINED_NODE(){
		nNodeInx = -1;
		nDirection = -1;
	}
}adj_node;

typedef struct MAZE_NODE
{
	pt	 ptNode;								//left-bottom point location
	bool bTop;									//top-wall
	bool bBottom;								//bottom-wall
	bool bLeft;									//left-wall
	bool bRight;								//right-wall
	bool bVisited;								//is visited
	std::vector<adj_node> vecAdjNode;			//adjoined nodes
	MAZE_NODE(){
		bTop = true;
		bBottom = true;
		bLeft = true;
		bRight = true;
		bVisited = false;
	}
}maze_node;

typedef struct MAZE_FOX{
	int nCurInx;
	std::vector<int> vecFoxPath;
	MAZE_FOX(int nCurInx){
		this->nCurInx = nCurInx;
	}

	void Push(int nInx){
		vecFoxPath.push_back(nInx);
	}
	void Pop(){
		vecFoxPath.erase(vecFoxPath.end() - 1);
	}
	bool IsEmpty(){
		if ((int)vecFoxPath.size() <= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	int GetPreNodeInx(){
		int nPreInx = -1;
		if ((int)vecFoxPath.size() > 1)
		{
			nPreInx = vecFoxPath[(int)vecFoxPath.size() - 2];
			
		}
		return nPreInx;
	}
}maze_fox;

class MazeModel
{
public:
	MazeModel();
	~MazeModel();

public:
	inline void SetRowsAndCols(const int nRows, const int nCols){
		m_nRows = nRows;
		m_nCols = nCols;
	};
	inline bool IsFoxTwinkle(){ return m_bTwinkle; };
	inline void SetFoxTwinkle(bool bTwinkle){ m_bTwinkle = bTwinkle; };

	bool BuildModel();
	bool SolveMaze();
	bool RunFox(const int nDirection);
	void DisplayMaze();
	void DisplayPath();
	void DisplayFoxPath();

private:
	//create maze
	bool InitMazeNodes();
	void CalcWallLengthAndStartPt();
	bool ProcessMazeNodes();
	bool RandomSearchNoVisitedAdjoinedNode(const maze_node* pNode,adj_node& an);
	bool RandomSelectNoVisitedNode(int& nNodeInx);
	void PushdownWallBetween(maze_node* pCurNode,const adj_node& an);
	void SetStartAndEndNode();

	//set a maze fox
	bool InitMazeFox();
	bool StepLeft(maze_fox* pmf);
	bool StepRight(maze_fox* pmf);
	bool StepTop(maze_fox* pmf);
	bool StepBottom(maze_fox* pmf);

	//solve maze
	bool SearchAccessedAdjoinedNode(const std::vector<maze_node>& vecNodes,const maze_node* pCurNode, int& nNodeInx);
	void CalcCenterOfNode(const maze_node* pNode,pt& ptCenter);

	//draw operator
	void DrawNode(const maze_node& n);
	void DrawLine(const pt& pt1, const pt& pt2, gl_rgb color = gl_rgb(1.0, 1.0, 1.0), float fLineWidth = 2.0f);
	void DrawPoint(const pt& ptPoint, gl_rgb color = gl_rgb(1.0, 1.0, 1.0), float fPtSize = 2.0f);

private:
	int m_nRows;
	int m_nCols;
	double m_dWallLength;
	pt m_ptStart;								//start point location
	std::vector<maze_node> m_vecNodes;			//all nodes of maze
	std::vector<pt> m_vecPath;					//path of maze
	std::vector<pt> m_vecFoxPath;				//path of a maze fox
	maze_fox m_MazeFox;							//a fox of maze
	bool m_bTwinkle;							//control fox show
};

