#include "stdafx.h"
#include "MazeModel.h"
#include "freeglut.h"
#include "time.h"
#include <stack>

MazeModel::MazeModel()
	: m_nRows(0)
	, m_nCols(0)
	, m_dWallLength(0)
	, m_ptStart()
	, m_MazeFox(0)
	, m_bTwinkle(true)
{
}

MazeModel::~MazeModel()
{
}

bool MazeModel::BuildModel()
{
	if (!InitMazeNodes())
	{
		return false;
	}
	
	if (!ProcessMazeNodes())
	{
		return false;
	}
	
	if (!InitMazeFox())
	{
		return false;
	}

	return true;
}

void MazeModel::DisplayMaze()
{
	for (auto itr = m_vecNodes.begin(); itr != m_vecNodes.end();++itr)
	{
		DrawNode(*itr);
	}	
}

void MazeModel::DisplayPath()
{
	if ((int)m_vecPath.size() <= 0)
	{
		return;
	}
	for (auto itr = m_vecPath.begin(); itr != m_vecPath.end() - 1;++itr)
	{
		DrawLine(*itr, *(itr + 1),gl_rgb(1.0,0.0,0.0),1.0f);
	}
}

void MazeModel::DisplayFoxPath()
{
	if ((int)m_vecFoxPath.size() <= 0)
	{
		return;
	}
	for (auto itr = m_vecFoxPath.begin(); itr != m_vecFoxPath.end() - 1;++itr)
	{
		DrawLine(*itr, *(itr + 1),gl_rgb(0.0,1.0,0.0),4.0f);
	}
	
	if (m_bTwinkle)
	{
		DrawPoint(m_vecFoxPath[(int)m_vecFoxPath.size() - 1], gl_rgb(0.0, 1.0, 0.0), 8.0f);
	}
	else
	{
		DrawPoint(m_vecFoxPath[(int)m_vecFoxPath.size() - 1], gl_rgb(0.0, 0.0, 0.0), 8.0f);
	}
}

bool MazeModel::InitMazeNodes()
{
	if (m_nRows <= 0 || m_nCols <= 0)
	{
		return false;
	}

	CalcWallLengthAndStartPt();
	m_vecNodes.clear();
	for (int i = 0; i < m_nRows; ++i)
	{
		for (int j = 0; j < m_nCols; ++j)
		{
			maze_node n;
			n.ptNode.x = m_ptStart.x + j*m_dWallLength;
			n.ptNode.y = m_ptStart.y + i*m_dWallLength;

			//adjoin nodes
			if (i==0 && m_nRows>1)
			{
				adj_node an;
				an.nDirection = 0;//top
				an.nNodeInx = (i + 1)*m_nCols + j;
				n.vecAdjNode.push_back(an);
			}
			else if (i==(m_nRows-1) && m_nRows>1)
			{
				adj_node an;
				an.nDirection = 1;//bottom
				an.nNodeInx = (i - 1)*m_nCols + j;
				n.vecAdjNode.push_back(an);
			}
			else
			{
				adj_node anTop;
				adj_node anBtm;
				anTop.nDirection = 0;
				anTop.nNodeInx = (i + 1)*m_nCols + j;
				anBtm.nDirection = 1;
				anBtm.nNodeInx = (i - 1)*m_nCols + j;
				n.vecAdjNode.push_back(anTop);
				n.vecAdjNode.push_back(anBtm);
			}

			if (j==0 && m_nCols>1)
			{
				adj_node an;
				an.nDirection = 3;//right
				an.nNodeInx = i*m_nCols + j + 1;
				n.vecAdjNode.push_back(an);
			}
			else if (j==(m_nCols-1) && m_nCols>1)
			{
				adj_node an;
				an.nDirection = 2;//left
				an.nNodeInx = i*m_nCols + j - 1;
				n.vecAdjNode.push_back(an);
			}
			else
			{
				adj_node anLft;
				adj_node anRht;
				anLft.nDirection = 2;
				anLft.nNodeInx = i*m_nCols + j - 1;
				anRht.nDirection = 3;
				anRht.nNodeInx = i*m_nCols + j + 1;
				n.vecAdjNode.push_back(anLft);
				n.vecAdjNode.push_back(anRht);
			}
			m_vecNodes.push_back(n);
		}
	}
	SetStartAndEndNode();
	return true;
}

void MazeModel::CalcWallLengthAndStartPt()
{
	const int nViewWidth = 640;
	const int nViewHeight = 480;
	const int nMarge = 20;
	double dLen1 = (nViewWidth - 2 * nMarge) / m_nCols;
	double dLen2 = (nViewHeight - 2 * nMarge) / m_nRows;
	m_dWallLength = (dLen1 < dLen2) ? dLen1 : dLen2;
	double dX = (nViewWidth - m_nCols * m_dWallLength) / 2.0;
	double dY = (nViewHeight - m_nRows * m_dWallLength) / 2.0;
	m_ptStart.x = dX;
	m_ptStart.y = dY;
}

bool MazeModel::ProcessMazeNodes()
{
	srand((unsigned int)time(NULL));//use system time initialize random seek,set only one time
	maze_node* pCurNode = &m_vecNodes[0];
	pCurNode->bVisited = true;
	std::stack<maze_node*> staVisitedNode;//DFS
	staVisitedNode.push(pCurNode);

	int nCount = 1;
	while (nCount < (int)m_vecNodes.size())
	{
		adj_node an;
		if (RandomSearchNoVisitedAdjoinedNode(pCurNode, an))
		{
			PushdownWallBetween(pCurNode, an);
			pCurNode = &m_vecNodes[an.nNodeInx];
			pCurNode->bVisited = true;
			staVisitedNode.push(pCurNode);
			nCount++;
		}
		else
		{
			staVisitedNode.pop();
			if (!staVisitedNode.empty())
			{
				pCurNode = staVisitedNode.top();
			}
			else
			{
				int nNodeInx = 0;
				if (RandomSelectNoVisitedNode(nNodeInx))
				{
					pCurNode = &m_vecNodes[nNodeInx];
					pCurNode->bVisited = true;
					staVisitedNode.push(pCurNode);
					nCount++;
				}
				else
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool MazeModel::RandomSearchNoVisitedAdjoinedNode(const maze_node* pNode, adj_node& an)
{
	int nCount = 0;
	std::vector<adj_node> vecAns;
	for (auto itr = pNode->vecAdjNode.begin();itr!=pNode->vecAdjNode.end(); ++itr)
	{
		if (!m_vecNodes[itr->nNodeInx].bVisited)
		{
			vecAns.push_back(*itr);
			nCount++;
		}
	}

	if (nCount == 0)
	{
		return false;
	}
	else
	{
		int nInx = rand() % nCount;//[0,nCount-1]
		an = vecAns[nInx];
		return true;
	}
}

bool MazeModel::RandomSelectNoVisitedNode(int& nNodeInx)
{
	int nCount = 0;
	std::vector<int> vecNodeInx;
	for (int i = 0; i < (int)m_vecNodes.size();++i)
	{
		if (!m_vecNodes[i].bVisited)
		{
			vecNodeInx.push_back(i);
			nCount++;
		}
	}

	if (nCount == 0)
	{
		return false;
	}
	else
	{
		int nInx = rand() % nCount;//[0,nCount-1]
		nNodeInx = vecNodeInx[nInx];
		return true;
	}
}

void MazeModel::PushdownWallBetween(maze_node* pCurNode, const adj_node& an)
{
	if (an.nDirection == 0)
	{
		pCurNode->bTop = false;
		m_vecNodes[an.nNodeInx].bBottom = false;
	}
	else if (an.nDirection == 1)
	{
		pCurNode->bBottom = false;
		m_vecNodes[an.nNodeInx].bTop = false;
	}
	else if (an.nDirection == 2)
	{
		pCurNode->bLeft = false;
		m_vecNodes[an.nNodeInx].bRight = false;
	}
	else if (an.nDirection == 3)
	{
		pCurNode->bRight = false;
		m_vecNodes[an.nNodeInx].bLeft = false;
	}
}

void MazeModel::SetStartAndEndNode()
{
	m_vecNodes[0].bLeft = false;
	m_vecNodes[(int)m_vecNodes.size() - 1].bRight = false;
}

bool MazeModel::SolveMaze()
{
	std::vector<maze_node> vecNodes;
	for (auto itr = m_vecNodes.begin(); itr != m_vecNodes.end();++itr)
	{
		maze_node n = *itr;
		n.bVisited = false;
		vecNodes.push_back(n);
	}
	vecNodes[0].bLeft = true;
	vecNodes[(int)vecNodes.size() - 1].bRight = true;					//build wall of start and end node

	std::stack<maze_node*> staPathNode;
	maze_node* pCurNode = &vecNodes[0];
	pCurNode->bVisited = true;
	staPathNode.push(pCurNode);

	while (pCurNode != &vecNodes[(int)vecNodes.size()-1])				//DFS
	{
		int nNodeInx = 0;
		if (SearchAccessedAdjoinedNode(vecNodes,pCurNode,nNodeInx))
		{
			pCurNode = &vecNodes[nNodeInx];
			pCurNode->bVisited = true;
			staPathNode.push(pCurNode);
		}
		else
		{
			staPathNode.pop();
			if (!staPathNode.empty())
			{
				pCurNode = staPathNode.top();
			}
			else
			{
				return false;				
			}
		}
	}

	m_vecPath.clear();
	while (!staPathNode.empty())
	{
		maze_node* pNode = staPathNode.top();
		pt ptCenter;
		CalcCenterOfNode(pNode, ptCenter);
		m_vecPath.push_back(ptCenter);
		staPathNode.pop();
	}
	std::reverse(m_vecPath.begin(), m_vecPath.end());
	return true;
}

bool MazeModel::SearchAccessedAdjoinedNode(const std::vector<maze_node>& vecNodes, const maze_node* pCurNode, int& nNodeInx)
{
	if (!pCurNode->bTop)
	{
		for (auto itr = pCurNode->vecAdjNode.begin(); itr != pCurNode->vecAdjNode.end();++itr)
		{
			if ((itr->nDirection==0) && !vecNodes[itr->nNodeInx].bVisited)
			{
				nNodeInx = itr->nNodeInx;
				return true;
			}
		}
	}
	if (!pCurNode->bBottom)
	{
		for (auto itr = pCurNode->vecAdjNode.begin(); itr != pCurNode->vecAdjNode.end(); ++itr)
		{
			if ((itr->nDirection == 1) && !vecNodes[itr->nNodeInx].bVisited)
			{
				nNodeInx = itr->nNodeInx;
				return true;
			}
		}
	}
	if (!pCurNode->bLeft)
	{
		for (auto itr = pCurNode->vecAdjNode.begin(); itr != pCurNode->vecAdjNode.end(); ++itr)
		{
			if ((itr->nDirection == 2) && !vecNodes[itr->nNodeInx].bVisited)
			{
				nNodeInx = itr->nNodeInx;
				return true;
			}
		}
	}
	if (!pCurNode->bRight)
	{
		for (auto itr = pCurNode->vecAdjNode.begin(); itr != pCurNode->vecAdjNode.end(); ++itr)
		{
			if ((itr->nDirection == 3) && !vecNodes[itr->nNodeInx].bVisited)
			{
				nNodeInx = itr->nNodeInx;
				return true;
			}
		}
	}
	return false;
}

void MazeModel::CalcCenterOfNode(const maze_node* pNode, pt& ptCenter)
{
	ptCenter.x = pNode->ptNode.x + 0.5*m_dWallLength;
	ptCenter.y = pNode->ptNode.y + 0.5*m_dWallLength;
}

void MazeModel::DrawNode(const maze_node& n)
{
	if (n.bTop)
	{
		pt pt1;
		pt pt2;
		pt1.x = n.ptNode.x;
		pt1.y = n.ptNode.y + m_dWallLength;
		pt2.x = n.ptNode.x + m_dWallLength;
		pt2.y = n.ptNode.y + m_dWallLength;
		DrawLine(pt1, pt2);
	}
	if (n.bBottom)
	{
		pt pt1;
		pt pt2;
		pt1 = n.ptNode;
		pt2.x = n.ptNode.x + m_dWallLength;
		pt2.y = n.ptNode.y;
		DrawLine(pt1, pt2);
	}
	if (n.bLeft)
	{
		pt pt1;
		pt pt2;
		pt1 = n.ptNode;
		pt2.x = n.ptNode.x;
		pt2.y = n.ptNode.y + m_dWallLength;
		DrawLine(pt1, pt2);
	}
	if (n.bRight)
	{
		pt pt1;
		pt pt2;
		pt1.x = n.ptNode.x + m_dWallLength;
		pt1.y = n.ptNode.y;
		pt2.x = n.ptNode.x + m_dWallLength;
		pt2.y = n.ptNode.y + m_dWallLength;
		DrawLine(pt1, pt2);
	}
}

void MazeModel::DrawLine(const pt& pt1, const pt& pt2, gl_rgb color, float fLineWidth)
{
	glColor3d(color.r,color.g,color.b);//set line color
	glLineWidth(fLineWidth);//set line width
	glBegin(GL_LINES);
	glVertex2d(pt1.x,pt1.y);
	glVertex2d(pt2.x, pt2.y);
	glEnd();
}

void MazeModel::DrawPoint(const pt& ptPoint, gl_rgb color/*=gl_rgb(1.0, 1.0, 1.0)*/, float fPtSize /*= 2.0f*/)
{
	glColor3d(color.r, color.g, color.b);//set point color
	glPointSize(fPtSize);//set point size
	glBegin(GL_POINTS);
	glVertex2d(ptPoint.x, ptPoint.y);
	glEnd();
}

bool MazeModel::InitMazeFox()
{
	if ((int)m_vecNodes.size() <= 0)
	{
		return false;
	}

	m_MazeFox = maze_fox(0);
	m_MazeFox.Push(0);
	pt ptCenter;
	CalcCenterOfNode(&m_vecNodes[0], ptCenter);
	m_vecFoxPath.push_back(ptCenter);
	return true;
}

bool MazeModel::RunFox(const int nDirection)
{	
	if (nDirection == 0)
	{
		StepTop(/*m_pMazeFox*/&m_MazeFox);
	}
	else if (nDirection == 1)
	{
		StepBottom(/*m_pMazeFox*/&m_MazeFox);
	}
	else if (nDirection == 2)
	{
		StepLeft(/*m_pMazeFox*/&m_MazeFox);
	}
	else if (nDirection == 3)
	{
		StepRight(/*m_pMazeFox*/&m_MazeFox);
	}
	return true;
}

bool MazeModel::StepLeft(maze_fox* pmf)
{
	maze_node n = m_vecNodes[pmf->nCurInx];
	if (!n.bLeft)
	{
		int nNxtInx = -1;
		for (auto itr = n.vecAdjNode.begin(); itr != n.vecAdjNode.end();++itr)
		{
			if (itr->nDirection == 2)
			{
				nNxtInx = itr->nNodeInx;
				break;
			}
		}
		if (nNxtInx != -1)
		{
			if (pmf->GetPreNodeInx() != nNxtInx)
			{
				pmf->nCurInx = nNxtInx;
				pmf->Push(nNxtInx);
				pt ptCenter;
				CalcCenterOfNode(&m_vecNodes[nNxtInx], ptCenter);
				m_vecFoxPath.push_back(ptCenter);
			}
			else
			{
				pmf->nCurInx = nNxtInx;
				if (!pmf->IsEmpty())
				{
					pmf->Pop();
					m_vecFoxPath.erase(m_vecFoxPath.end() - 1);
				}		
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool MazeModel::StepRight(maze_fox* pmf)
{
	maze_node n = m_vecNodes[pmf->nCurInx];
	if (!n.bRight)
	{
		int nNxtInx = -1;
		for (auto itr = n.vecAdjNode.begin(); itr != n.vecAdjNode.end(); ++itr)
		{
			if (itr->nDirection == 3)
			{
				nNxtInx = itr->nNodeInx;
				break;
			}
		}
		if (nNxtInx != -1)
		{
			if (pmf->GetPreNodeInx() != nNxtInx)
			{
				pmf->nCurInx = nNxtInx;
				pmf->Push(nNxtInx);
				pt ptCenter;
				CalcCenterOfNode(&m_vecNodes[nNxtInx], ptCenter);
				m_vecFoxPath.push_back(ptCenter);
			}
			else
			{
				pmf->nCurInx = nNxtInx;
				if (!pmf->IsEmpty())
				{
					pmf->Pop();
					m_vecFoxPath.erase(m_vecFoxPath.end() - 1);
				}
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool MazeModel::StepTop(maze_fox* pmf)
{
	maze_node n = m_vecNodes[pmf->nCurInx];
	if (!n.bTop)
	{
		int nNxtInx = -1;
		for (auto itr = n.vecAdjNode.begin(); itr != n.vecAdjNode.end(); ++itr)
		{
			if (itr->nDirection == 0)
			{
				nNxtInx = itr->nNodeInx;
				break;
			}
		}
		if (nNxtInx != -1)
		{
			if (pmf->GetPreNodeInx() != nNxtInx)
			{
				pmf->nCurInx = nNxtInx;
				pmf->Push(nNxtInx);
				pt ptCenter;
				CalcCenterOfNode(&m_vecNodes[nNxtInx], ptCenter);
				m_vecFoxPath.push_back(ptCenter);
			}
			else
			{
				pmf->nCurInx = nNxtInx;
				if (!pmf->IsEmpty())
				{
					pmf->Pop();
					m_vecFoxPath.erase(m_vecFoxPath.end() - 1);
				}
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool MazeModel::StepBottom(maze_fox* pmf)
{
	maze_node n = m_vecNodes[pmf->nCurInx];
	if (!n.bBottom)
	{
		int nNxtInx = -1;
		for (auto itr = n.vecAdjNode.begin(); itr != n.vecAdjNode.end(); ++itr)
		{
			if (itr->nDirection == 1)
			{
				nNxtInx = itr->nNodeInx;
				break;
			}
		}
		if (nNxtInx != -1)
		{
			if (pmf->GetPreNodeInx() != nNxtInx)
			{
				pmf->nCurInx = nNxtInx;
				pmf->Push(nNxtInx);
				pt ptCenter;
				CalcCenterOfNode(&m_vecNodes[nNxtInx], ptCenter);
				m_vecFoxPath.push_back(ptCenter);
			}
			else
			{
				pmf->nCurInx = nNxtInx;
				if (!pmf->IsEmpty())
				{
					pmf->Pop();
					m_vecFoxPath.erase(m_vecFoxPath.end() - 1);
				}
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

