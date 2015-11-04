#ifndef _GRAPH_2_OBJ_H_
#define _GRAPH_2_OBJ_H_

#include "Globals.h"
#include "gpc.h"
#include <vector>
#include <fstream>
#include <math.h>
#include <string>

class SimpleMath
{
public:
	static void GetDiff3D(double* p1, double* p2, double* p);
	static void GetDiff2D(double* p1, double* p2, double* p);
	static void GetAdd3D(double* p1, double* p2, double* p);
	static void MakeUnitVector(double* v1);
	static void MakeUnitVector2D(double* v1);
	static double ToRad(double degrees);
	static double ToDeg(double rads);
	static void GetCrossProduct(double* vA, double* vB, double* v);
	static double GetDotProduct(double* vA, double* vB);
};

class SimpleNode
{
public:
	SimpleNode(double* pos);
	SimpleNode(double* pos, double h);
	~SimpleNode();

	void SetIdx(unsigned int idx) {m_idx = idx;}
	unsigned int GetIdx() {return m_idx;}

	void AddConectTo(SimpleNode* nei);
	double* GetPosPtr() {return m_pos;}
	unsigned int GetNumNeighbours() {return (unsigned int)m_vNei.size();}
	SimpleNode* GetNeighbour(unsigned int i) {return m_vNei.at(i);}

	bool IsInPolygon(std::vector <SimpleNode*> &vPolygon);

	void SetCanStartPolygon(bool b) {m_bCanStartPolygon = b;}
	bool GetCanStartPolygon() {return m_bCanStartPolygon;}

	void SetVisitedTmp(bool b) {m_bVisitedTmp = b;}
	bool GetVisitedTmp() {return m_bVisitedTmp;}

	SimpleNode* GetFirstNotVisitedNeighbour();

private:
	unsigned int m_idx;
	bool m_bCanStartPolygon;
	bool m_bVisitedTmp;
	double m_pos[3];
	std::vector <SimpleNode*> m_vNei;
	
};

class SimpleLink
{
public:
	SimpleLink(SimpleNode* nodeA, SimpleNode* nodeB);
	~SimpleLink();

	gpc_polygon* FillPolygonGPC(double width);
	gpc_polygon* GetPolygonGPC() {return &m_polygon;}

	double GetHeight() {return 0.5*(m_nodeA->GetPosPtr()[2] + m_nodeB->GetPosPtr()[2]);}

private:
	SimpleNode* m_nodeA;
	SimpleNode* m_nodeB;
	gpc_polygon m_polygon;
};

class SimpleGraph
{
public:
	SimpleGraph();
	~SimpleGraph();
	void AddNode(SimpleNode* node);
	void AddLink(unsigned int idxA, unsigned int idxB);

	unsigned int GetNumNodes() {return (unsigned int)m_vNode.size();}
	SimpleNode* GetNode(unsigned int i) {return m_vNode.at(i);}

	SimpleNode* GetNextLeftMost(SimpleNode* nodeA, SimpleNode* nodeB);
	std::vector <SimpleNode*> TrackPolygon(SimpleNode* nodeA, SimpleNode* nodeB);

	std::vector < std::vector<SimpleNode*> > GetGraphPolygons();

	std::vector<SimpleNode*> CreateReducedPolygon(std::vector<SimpleNode*> &vPolygon, double d, double height);

	void GetMinMaxDim(double &minX, double &minY, double &maxX, double &maxY);

	void WriteToObj_old(std::string strFilename, double dWidth, double dHeight);
	void WriteToObj(std::string strFilename, double dWidth, std::vector < std::vector <unsigned int> > &vBuilding);	

private:
	bool isNodeInPolygon(std::vector <SimpleNode*> &polyA, SimpleNode* pTest);
	bool arePolygonsEqual(std::vector <SimpleNode*> &polyA, std::vector <SimpleNode*> &polyB);

	void setWhichVerticesCanStartPolygons();

	void createBuilding(std::vector <unsigned int> &vB, std::ofstream &ofs, unsigned int &idx, double &dGroundLevel);

private:
	std::vector <SimpleNode*> m_vNode;
	std::vector <SimpleLink*> m_vLink;
	std::vector <SimpleLink*> m_vLinkRoad;
};

// GRAPH2OBJ_API void Graph2Obj_vec2(
// 								 std::vector <double*> vNode, 
// 								 std::vector <unsigned int> vLinkFrom, 
// 								 std::vector <unsigned int> vLinkTo,
// 								 std::string strFilename,
// 								 double paramDitchWidth,
// 								 double paramDitchHeight)
// {
// 	SimpleGraph* graph = new SimpleGraph();
// 
// 	for (unsigned int i=0; i<vNode.size(); i++)
// 	{
// 		graph->AddNode(new SimpleNode(vNode[i], vNode[i][2]));
// 	}
// 
// 	for (unsigned int i=0; i<vLinkFrom.size(); i++)
// 	{
// 		graph->AddLink(vLinkFrom[i], vLinkTo[i]);
// 	}
// 
// 	graph->WriteToObj(strFilename, paramDitchWidth, paramDitchHeight);
// 	delete graph;
// }

/*GRAPH2OBJ_API void Graph2Obj_vec(
	std::vector <double*> vNode, 
	std::vector <unsigned int*> vLink,
	std::vector < std::vector<unsigned int> > vBuilding,
	std::string strFilename,
	double paramDitchWidth)
{
	SimpleGraph* graph = new SimpleGraph();

	for (unsigned int i=0; i<vNode.size(); i++)
	{
		graph->AddNode(new SimpleNode(vNode[i], vNode[i][2]));
	}

	for (unsigned int i=0; i<vLink.size(); i++)
	{
		graph->AddLink(vLink[i][0], vLink[i][1]);
	}

	graph->WriteToObj(strFilename, paramDitchWidth, vBuilding);
	delete graph;
}
*/
#endif