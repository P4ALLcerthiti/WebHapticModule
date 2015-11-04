#include "Graph2Obj.h"


void SimpleMath::GetDiff3D(double* p1, double* p2, double* p)
{
	p[0] = p1[0] - p2[0];
	p[1] = p1[1] - p2[1];
	p[2] = p1[2] - p2[2];
}

void SimpleMath::GetDiff2D(double* p1, double* p2, double* p)
{
	p[0] = p1[0] - p2[0];
	p[1] = p1[1] - p2[1];
}

void SimpleMath::GetAdd3D(double* p1, double* p2, double* p)
{
	p[0] = p1[0] + p2[0];
	p[1] = p1[1] + p2[1];
	p[2] = p1[2] + p2[2];
}

double SimpleMath::ToRad(double degrees) 
{
	return degrees / 180.0 * 3.1415926535897932384626433832795;
}

double SimpleMath::ToDeg(double rads) 
{
	return rads / 3.1415926535897932384626433832795 * 180.0;
}

void SimpleMath::GetCrossProduct(double* vA, double* vB, double* v)
{
	v[0] = vA[1]*vB[2] + vA[2] * vB[1];
	v[1] = vA[2]*vB[0] + vA[0] * vB[2];
	v[2] = vA[0]*vB[1] + vA[1] * vB[0];
}

double SimpleMath::GetDotProduct(double* vA, double* vB)
{
	return vA[0]*vB[0] + vA[1]*vB[1] + vA[2]*vB[2];
}

void SimpleMath::MakeUnitVector(double* v1)
{
	double d = sqrt(v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2]);

	if (d == 0.0)
	{
		v1[0] = 0.0;
		v1[1] = 0.0;
		v1[2] = 0.0;
	}
	else
	{
		v1[0] /= d;
		v1[1] /= d;
		v1[2] /= d;
	}
}

void SimpleMath::MakeUnitVector2D(double* v1)
{
	double d = sqrt(v1[0]*v1[0] + v1[1]*v1[1]);

	if (d == 0.0)
	{
		v1[0] = 0.0;
		v1[1] = 0.0;
	}
	else
	{
		v1[0] /= d;
		v1[1] /= d;
	}
}


SimpleNode::SimpleNode(double* pos)
{
	m_idx = -1;
	m_pos[0] = pos[0];
	m_pos[1] = pos[1];
	m_pos[2] = 0.0;
	m_bCanStartPolygon = true;
	m_bVisitedTmp = false;
}

SimpleNode::SimpleNode(double* pos, double h)
{
	m_idx = -1;
	m_pos[0] = pos[0];
	m_pos[1] = pos[1];
	m_pos[2] = h;
	m_bCanStartPolygon = true;
	m_bVisitedTmp = false;
}

SimpleNode::~SimpleNode()
{

}

void SimpleNode::AddConectTo(SimpleNode* nei)
{
	m_vNei.push_back(nei);
	nei->m_vNei.push_back(this);
}

bool SimpleNode::IsInPolygon(std::vector <SimpleNode*> &vPolygon)
{
	double pTmpA[3];
	double pTmpB[3];
	double pCrossA[3];
	double pCrossB[3];

	SimpleMath::GetDiff3D(vPolygon[vPolygon.size()-2]->m_pos, m_pos, pTmpA);
	SimpleMath::GetDiff3D(vPolygon[0]->m_pos, m_pos, pTmpB);
	SimpleMath::GetCrossProduct(pTmpA, pTmpB, pCrossA);

	for (unsigned int i=0; i<vPolygon.size()-2; i++)
	{
		SimpleMath::GetDiff3D(vPolygon[i]->m_pos, m_pos, pTmpA);
		SimpleMath::GetDiff3D(vPolygon[i+1]->m_pos, m_pos, pTmpB);
		SimpleMath::GetCrossProduct(pTmpA, pTmpB, pCrossB);

		if (SimpleMath::GetDotProduct(pCrossA, pCrossB) < 0) return false;

	}

	return true;
}

SimpleNode* SimpleNode::GetFirstNotVisitedNeighbour()
{
	for (unsigned int i=0; i<m_vNei.size(); i++)
	{
		if (!m_vNei.at(i)->m_bVisitedTmp) return m_vNei.at(i);
	}
	return NULL;
}

SimpleLink::SimpleLink(SimpleNode* nodeA, SimpleNode* nodeB)
{
	m_nodeA = nodeA;
	m_nodeB = nodeB;
	
	m_polygon.num_contours = 1;
	m_polygon.hole = new int[1];
	m_polygon.contour = new gpc_vertex_list[1];
	
	m_polygon.hole[0] = 1;
	m_polygon.contour[0].num_vertices = 4;
	m_polygon.contour[0].vertex = new gpc_vertex[4];
	
}

SimpleLink::~SimpleLink()
{
	//clear polygon
	for (int i=0; i<m_polygon.num_contours; i++)
	{
		delete [] m_polygon.contour[i].vertex;
	}
	delete [] m_polygon.hole;
	delete [] m_polygon.contour;
}

gpc_polygon* SimpleLink::FillPolygonGPC(double width)
{
	double vecA[2];
	SimpleMath::GetDiff2D(m_nodeB->GetPosPtr(), m_nodeA->GetPosPtr(), vecA);
	SimpleMath::MakeUnitVector2D(vecA);
	
	double vecB[2];
	vecB[0] = -vecA[1];
	vecB[1] = vecA[0];

	double w = 0.35 * width; // 0.35 = 0.5 / sqrt(2.0)

	double h = 0.5 * (m_nodeA->GetPosPtr()[2] + m_nodeB->GetPosPtr()[2]);

	m_polygon.contour[0].vertex[0].x = m_nodeA->GetPosPtr()[0] + (vecB[0]-vecA[0]) * w;
	m_polygon.contour[0].vertex[0].y = m_nodeA->GetPosPtr()[1] + (vecB[1]-vecA[1]) * w;
	m_polygon.contour[0].vertex[1].x = m_nodeB->GetPosPtr()[0] + (vecB[0]+vecA[0]) * w;
	m_polygon.contour[0].vertex[1].y = m_nodeB->GetPosPtr()[1] + (vecB[1]+vecA[1]) * w;

	m_polygon.contour[0].vertex[2].x = m_nodeB->GetPosPtr()[0] + (vecA[0]-vecB[0]) * w;
	m_polygon.contour[0].vertex[2].y = m_nodeB->GetPosPtr()[1] + (vecA[1]-vecB[1]) * w;
	m_polygon.contour[0].vertex[3].x = m_nodeA->GetPosPtr()[0] + (-vecA[0]-vecB[0]) * w;
	m_polygon.contour[0].vertex[3].y = m_nodeA->GetPosPtr()[1] + (-vecA[1]-vecB[1]) * w;

	m_polygon.contour[0].vertex[0].z = h;
	m_polygon.contour[0].vertex[1].z = h;
	m_polygon.contour[0].vertex[2].z = h;
	m_polygon.contour[0].vertex[3].z = h;

	return &m_polygon;
}

SimpleGraph::SimpleGraph()
{

}

SimpleGraph::~SimpleGraph()
{
	for (unsigned int i=0; i<m_vNode.size(); i++)
	{
		delete m_vNode.at(i);
	}

	for (unsigned int i=0; i<m_vLink.size(); i++)
	{
		delete m_vLink.at(i);
	}
}

void SimpleGraph::AddNode(SimpleNode* node)
{
	m_vNode.push_back(node);
}

void SimpleGraph::AddLink(unsigned int idxA, unsigned int idxB)
{
	SimpleNode* nodeA = GetNode(idxA);
	SimpleNode* nodeB = GetNode(idxB);
	nodeA->AddConectTo(nodeB);
	m_vLink.push_back(new SimpleLink(nodeA, nodeB));

	if (nodeA->GetPosPtr()[2] < 0.0 && nodeB->GetPosPtr()[2] < 0.0)
	{
		m_vLinkRoad.push_back(m_vLink.back());
	}
}

SimpleNode* SimpleGraph::GetNextLeftMost(SimpleNode* nodeA, SimpleNode* nodeB)
{
	// A -> B -> X
	double AB[3];
	SimpleMath::GetDiff3D(nodeA->GetPosPtr(), nodeB->GetPosPtr(), AB);
	SimpleMath::MakeUnitVector(AB);
	
	double atanAB = SimpleMath::ToDeg(atan2(AB[1], AB[0]));
	if (atanAB < 0) atanAB += 360.0;
	SimpleNode* leftMostNode = NULL;
	double BX[3];
	double minAngle = 720.0;

	for (unsigned int i=0; i<nodeB->GetNumNeighbours(); i++)
	{
		SimpleNode* currentNode = nodeB->GetNeighbour(i);

 		if (!currentNode->GetCanStartPolygon()) continue;
		if (currentNode->GetVisitedTmp()) continue;
		if (currentNode == nodeA) continue;

		//get vector
		SimpleMath::GetDiff3D(currentNode->GetPosPtr(), nodeB->GetPosPtr(), BX);
		SimpleMath::MakeUnitVector(BX);
		double atanBX = SimpleMath::ToDeg(atan2(BX[1], BX[0]));
		if (atanBX < 0) atanBX += 360.0;

		double currentAngle = atanAB - atanBX;
		if (currentAngle < 0.0) currentAngle += 360.0;
		if (currentAngle <= minAngle)
		{
			minAngle = currentAngle;
			leftMostNode = currentNode;
		}
	}

	return leftMostNode;
}

std::vector <SimpleNode*> SimpleGraph::TrackPolygon(SimpleNode* nodeA, SimpleNode* nodeB)
{
	std::vector <SimpleNode*> vPoly;

	SimpleNode* pA = nodeA;
	SimpleNode* pB = nodeB;
	vPoly.push_back(pA);
	vPoly.push_back(pB);

	bool bGetLeftMost = true;

	while (true)
	{
		SimpleNode* nextNode = GetNextLeftMost(pA, pB);
		if (!nextNode) break;
		if (nextNode == vPoly.front()) break;
		

		

// 		if (nextNode->GetNumNeighbours() == 1)
// 		{
// 			vPoly.push_back(nextNode);
// 			vPoly.push_back(pB);
// 			nextNode = GetNextLeftMost(nextNode, pB);
// 		}
		
		vPoly.push_back(nextNode);
//		nextNode->SetVisitedTmp(true);
		
		pA = pB;
		pB = nextNode;
	}

	for (unsigned int i=0; i<vPoly.size(); i++)
	{
		vPoly.at(i)->SetVisitedTmp(false);
	}

	return vPoly;
}

std::vector < std::vector<SimpleNode*> > SimpleGraph::GetGraphPolygons()
{
	std::vector < std::vector<SimpleNode*> > vPolygons;

	SimpleNode* nodeA = NULL;
	SimpleNode* nodeB = NULL;
	for (unsigned int i=0; i<m_vNode.size(); i++)
	{
		nodeA = m_vNode.at(i);
		if (!nodeA->GetCanStartPolygon()) continue;

		for (unsigned int j=0; j<nodeA->GetNumNeighbours(); j++)
		{
			nodeB = nodeA->GetNeighbour(j);
			if (!nodeB->GetCanStartPolygon()) continue;

			std::vector <SimpleNode*> poly = TrackPolygon(nodeA, nodeB);

			//check if to add
			bool bToAdd = poly.size()>= 3;

			for (unsigned int p=0; p<vPolygons.size(); p++)
			{
				if (arePolygonsEqual(vPolygons.at(p), poly))
				{
					if (vPolygons.at(p).size() < poly.size())
					{
						vPolygons.at(p) = poly; //replace
					}

					bToAdd = false;
					break;
				}
			}
			if (bToAdd) vPolygons.push_back(poly);
		}
	}

	return vPolygons;
}

std::vector<SimpleNode*> SimpleGraph::CreateReducedPolygon(std::vector<SimpleNode*> &vPolygon, double d, double height)
{
	std::vector <SimpleNode*> vRedu;

	double vecBA[3];
	double vecBC[3];
	double vecProgress[3];
	double p[3];
	
	for (unsigned int i=1; i<vPolygon.size(); i++)
	{
		double* pA = vPolygon.at(i-1)->GetPosPtr();
		double* pB = vPolygon.at(i)->GetPosPtr();

		if (vPolygon.at(i)->GetNumNeighbours() == 1)
		{
			vRedu.push_back(new SimpleNode(pB));
			vRedu.back()->GetPosPtr()[2] = height;
			continue;
		}

		double* pC = NULL;
		if (i+1 == vPolygon.size())
		{
			pC = vPolygon.at(1)->GetPosPtr();
		}
		else
		{
			pC = vPolygon.at(i+1)->GetPosPtr();
		}

		SimpleMath::GetDiff3D(pA, pB, vecBA);
		SimpleMath::MakeUnitVector(vecBA);
		
		SimpleMath::GetDiff3D(pC, pB, vecBC);
		SimpleMath::MakeUnitVector(vecBC);

		SimpleMath::GetAdd3D(vecBA, vecBC, vecProgress);
		vecProgress[2] = 0.0;
		SimpleMath::MakeUnitVector(vecProgress);

		double atanBA = SimpleMath::ToDeg(atan2(vecBA[1], vecBA[0]));
		double atanBC = SimpleMath::ToDeg(atan2(vecBC[1], vecBC[0]));

		if (atanBA < 0.0) atanBA += 360;
		if (atanBC < 0.0) atanBC += 360;

		//double angle = 0.5 * GetAngleBetweenVectors(vecBA, vecBC);

		double diffAngle = atanBC - atanBA;
		if (diffAngle < 0.0) diffAngle += 360.0;

		double angle = 0.5 * SimpleMath::ToRad(diffAngle);
		
		p[0] = vPolygon.at(i)->GetPosPtr()[0];
		p[1] = vPolygon.at(i)->GetPosPtr()[1];
		p[2] = vPolygon.at(i)->GetPosPtr()[2];

		double d2 = d / sin(angle);

		if (diffAngle < 180.0) //neet to do this if the polygon is not convex!
		{
			d2 *= -1.0;
		}

		p[0] += d2 * vecProgress[0];
		p[1] += d2 * vecProgress[1];
		vRedu.push_back(new SimpleNode(p));
		vRedu.back()->GetPosPtr()[2] = height;
	}

	//add again the firsts
	vRedu.push_back(new SimpleNode(vRedu.front()->GetPosPtr()));

	return vRedu;
}

void SimpleGraph::GetMinMaxDim(double &minX, double &minY, double &maxX, double &maxY)
{
	minX = 10000000.0;
	minY = 10000000.0;
	maxX = -minX;
	maxY = -minY;

	double* pos;
	for (unsigned int i=0; i<m_vNode.size(); i++)
	{
		pos = m_vNode.at(i)->GetPosPtr();

		if (pos[0] < minX) minX = pos[0];
		if (pos[0] > maxX) maxX = pos[0];
		
		if (pos[1] < minY) minY = pos[1];
		if (pos[1] > maxY) maxY = pos[1];
	}
}

void SimpleGraph::WriteToObj_old(std::string strFilename, double dParam, double hParam)
{
	unsigned int i,j,k;

	std::vector <SimpleNode*> vUniqueVertex;
	std::vector < std::vector<SimpleNode*> > vFaces;
	unsigned int idx = 1;

	//set indices to graph nodes
	for (i=0; i<m_vNode.size(); i++)
	{
		m_vNode.at(i)->SetIdx(idx++);
		vUniqueVertex.push_back(m_vNode.at(i));
	}

	setWhichVerticesCanStartPolygons();

	std::vector < std::vector<SimpleNode*> > vPoly = GetGraphPolygons();

	for (i=0; i<vPoly.size(); i++)
	{
		std::vector<SimpleNode*> currentPolygon = vPoly.at(i);

		//get internal polygons
		std::vector<SimpleNode*> vVertex = CreateReducedPolygon(currentPolygon, dParam, hParam);
		vFaces.push_back(vVertex);

		for (k=0; k<vVertex.size()-1; k++)
		{
			vVertex.at(k)->SetIdx(idx++);
			vUniqueVertex.push_back(vVertex.at(k));

			std::vector<SimpleNode*> vQuadruplet;
			vQuadruplet.push_back(vVertex.at(k));
			vQuadruplet.push_back(currentPolygon.at(k+1));
			if (k+2 == currentPolygon.size())
			{
				vQuadruplet.push_back(currentPolygon.at(1));
			}
			else
			{
				vQuadruplet.push_back(currentPolygon.at(k+2));
			}
			vQuadruplet.push_back(vVertex.at(k+1));
			vQuadruplet.push_back(vVertex.at(k));
			vFaces.push_back(vQuadruplet);
		}

		vVertex.back()->SetIdx(vVertex.front()->GetIdx());
	}

	std::ofstream ofs(strFilename.c_str());
	for (i=0; i<vUniqueVertex.size(); i++)
	{
		ofs << "v " << vUniqueVertex[i]->GetPosPtr()[0] << " " << vUniqueVertex[i]->GetPosPtr()[1] << " " << vUniqueVertex[i]->GetPosPtr()[2] << "\n";
	}

	for (i=0; i<vFaces.size(); i++)
	{
		ofs << "f ";
		for (j=0; j<vFaces[i].size()-1; j++)
		{
			ofs << vFaces[i][j]->GetIdx() << " ";
		}
		ofs << "\n";
	}
	ofs.close();
}

void SimpleGraph::WriteToObj(std::string strFilename, double dWidth, std::vector< std::vector<unsigned int> > &vBuilding)
{
	std::ofstream ofs;
	ofs.open(strFilename.c_str());

	int i,j,k;

	//create start polygon
	gpc_polygon complexPolygon; //final polygon...
	complexPolygon.contour = NULL;
	complexPolygon.num_contours = 0;
	complexPolygon.hole = 0;
	double dRoadHeight = m_vLinkRoad.size()==0?0.0001:-m_vLinkRoad[0]->GetHeight();
//	gpc_polygon* polyA = m_vLinkRoad[0]->FillPolygonGPC(dWidth);
//	gpc_polygon* polyB = m_vLinkRoad[1]->FillPolygonGPC(dWidth);

	//this part is very slow:
//	GpcPoly::gpc_polygon_clip(GPC_UNION, polyA, polyB, &complexPolygon);

	//gpc_polygon* gpPoly;
	for (i=0; i<(int)m_vLinkRoad.size(); i++)
	{

	//	gpPoly = m_vLink[i]->FillPolygonGPC(dWidth);
	//	GpcPoly::gpc_add_contour(&complexPolygon, &gpPoly->contour[0], 1);
		GpcPoly::gpc_polygon_clip(GPC_UNION, &complexPolygon, m_vLinkRoad[i]->FillPolygonGPC(dWidth), &complexPolygon);
	}
	
	//create polygon out
	double minX, minY, maxX, maxY;
	GetMinMaxDim(minX, minY, maxX, maxY);

	gpc_polygon polygonOut;
	polygonOut.num_contours = 1;
	polygonOut.hole = new int[1];
	polygonOut.contour = new gpc_vertex_list[1];
	polygonOut.hole[0] = 1;
	polygonOut.contour[0].num_vertices = 4;
	polygonOut.contour[0].vertex = new gpc_vertex[4];
	polygonOut.contour[0].vertex[0].x = minX-dWidth;
	polygonOut.contour[0].vertex[0].y = minY-dWidth;
	polygonOut.contour[0].vertex[1].x = maxX+dWidth;
	polygonOut.contour[0].vertex[1].y = minY-dWidth;
	polygonOut.contour[0].vertex[2].x = maxX+dWidth;
	polygonOut.contour[0].vertex[2].y = maxY+dWidth;
	polygonOut.contour[0].vertex[3].x = minX-dWidth;
	polygonOut.contour[0].vertex[3].y = maxY+dWidth;

	gpc_vertex* vertexTmp;
	unsigned int idx = 1;

	if (complexPolygon.num_contours > 0)
	{
		GpcPoly::gpc_polygon_clip(GPC_DIFF, &polygonOut, &complexPolygon, &polygonOut);

		//get triangle strip of roads
		gpc_tristrip complexTriangle;
		GpcPoly::gpc_polygon_to_tristrip(&complexPolygon, &complexTriangle);

		

		ofs << "g road\n";
		for (i=0; i<complexTriangle.num_strips; i++)
		{
			for (j=0; j<complexTriangle.strip[i].num_vertices; j++)
			{
				vertexTmp = &(complexTriangle.strip[i].vertex[j]);
				vertexTmp->idx = idx++;
				ofs << "v " << vertexTmp->x << " " << vertexTmp->y << " " << 0.0 << "\n";
			}
		}

		for (i=0; i<complexTriangle.num_strips; i++)
		{
			for (j=0; j<complexTriangle.strip[i].num_vertices-2; j++)
			{
				ofs << "f ";
				for (k=0; k<3; k++)
				{
					vertexTmp = &(complexTriangle.strip[i].vertex[j+k]);
					ofs << vertexTmp->idx << " ";
				}
			
				ofs << "\n";
			}
		}

		GpcPoly::gpc_free_tristrip(&complexTriangle);
	}
	
	//get triangle strip of valeys
	ofs << "g valey\n";

	gpc_tristrip complexTriangleOut;
	GpcPoly::gpc_polygon_to_tristrip(&polygonOut, &complexTriangleOut);

	for (i=0; i<complexTriangleOut.num_strips; i++)
	{
		for (j=0; j<complexTriangleOut.strip[i].num_vertices; j++)
		{
			vertexTmp = &(complexTriangleOut.strip[i].vertex[j]);
			vertexTmp->idx = idx++;
			ofs << "v " << vertexTmp->x << " " << vertexTmp->y << " " << dRoadHeight << "\n";
		}
	}

	for (i=0; i<complexTriangleOut.num_strips; i++)
	{
		for (j=0; j<complexTriangleOut.strip[i].num_vertices-2; j++)
		{
			ofs << "f ";
			for (k=0; k<3; k++)
			{
				vertexTmp = &(complexTriangleOut.strip[i].vertex[j+k]);
				ofs << vertexTmp->idx << " ";
			}

			ofs << "\n";
		}
	}

	//create walls:
	for (i=0; i<complexPolygon.num_contours; i++)
	{
		for (j=0; j<complexPolygon.contour[i].num_vertices; j++)
		{
			vertexTmp = &(complexPolygon.contour[i].vertex[j]);
			vertexTmp->idx = idx;
			ofs << "v " << vertexTmp->x << " " << vertexTmp->y << " " << 0.0 << "\n";
			ofs << "v " << vertexTmp->x << " " << vertexTmp->y << " " << dRoadHeight << "\n";
			idx+=2;
		}
	}

	unsigned int idx2 = idx;

	for (i=0; i<complexPolygon.num_contours; i++)
	{
		for (j=0; j<complexPolygon.contour[i].num_vertices-1; j++)
		{
			vertexTmp = &(complexPolygon.contour[i].vertex[j]);
			idx2 = vertexTmp->idx;
			ofs << "f " << idx2 << " " << idx2+1 << " " << idx2+3 <<  " " << idx2+2 << "\n";
		}

		vertexTmp = &(complexPolygon.contour[i].vertex[complexPolygon.contour[i].num_vertices-1]);
		idx2 = vertexTmp->idx;
		ofs << "f " << idx2 << " " << idx2+1 << " ";

		vertexTmp = &(complexPolygon.contour[i].vertex[0]);
		idx2 = vertexTmp->idx;
		ofs << idx2+1 << " " << idx2 << "\n";
	}
	
	//add buildings
	ofs << "g building\n";
	for (unsigned int i=0; i<vBuilding.size(); i++)
	{
		createBuilding(vBuilding.at(i), ofs, idx, dRoadHeight);
	}


//*	//add peripheral walls!

	ofs << "g fence\n";

	double dWalls[5][2] = 
	{
		{minX-dWidth, minY-dWidth},
		{minX-dWidth, maxY+dWidth},
		{maxX+dWidth, maxY+dWidth},
		{maxX+dWidth, minY-dWidth},
		{minX-dWidth, minY-dWidth}
	};

	for (i=0; i<5; i++)
	{
		ofs << "v " << dWalls[i][0] << " " << dWalls[i][1] << " " << 0.0 << "\n";
		ofs << "v " << dWalls[i][0] << " " << dWalls[i][1] << " " << 1.0 << "\n";
	}

	for (i=0; i<4; i++)
	{
		ofs << "f " << idx << " " << idx+1 << " " << idx+3 << " " << idx+2 << "\n";
		idx+=2;
	}
//*/


	ofs.close();
	ofs.clear();

	//clear polygons
	for (int i=0; i<polygonOut.num_contours; i++)
	{
		delete [] polygonOut.contour[i].vertex;
	}
	delete [] polygonOut.hole;
	delete [] polygonOut.contour;

	GpcPoly::gpc_free_polygon(&complexPolygon);
	GpcPoly::gpc_free_tristrip(&complexTriangleOut);
}

bool SimpleGraph::isNodeInPolygon(std::vector <SimpleNode*> &polyA, SimpleNode* pTest)
{
	for (unsigned int i=0; i<polyA.size(); i++)
	{
		if (polyA.at(i) == pTest)
		{
			return true;
		}
	}
	return false;
}

bool SimpleGraph::arePolygonsEqual(std::vector <SimpleNode*> &polyA, std::vector <SimpleNode*> &polyB)
{
	for (unsigned int i=0; i<polyB.size(); i++)
	{
		if (polyB.at(i)->GetNumNeighbours()>1 && !isNodeInPolygon(polyA, polyB.at(i)))
		{
			return false;
		}
	}

	for (unsigned int i=0; i<polyA.size(); i++)
	{
		if (polyA.at(i)->GetNumNeighbours()>1 && !isNodeInPolygon(polyB, polyA.at(i)))
		{
			return false;
		}
	}

	return true;
}

void SimpleGraph::setWhichVerticesCanStartPolygons()
{
	SimpleNode* currentNode = NULL;

	for (unsigned int i=0; i<m_vNode.size(); i++)
	{
		currentNode = m_vNode.at(i);

		if (currentNode->GetNumNeighbours() <= 1)
		{
			currentNode->SetCanStartPolygon(false);
			currentNode->SetVisitedTmp(true);

			//parse a chain until a vertex with more than two neighbours is met
			while (true)
			{
				currentNode = currentNode->GetFirstNotVisitedNeighbour();

				if (!currentNode) break; //a simple line-chain not connected to the rest graph
				if (currentNode->GetNumNeighbours() >= 3) break;

				currentNode->SetCanStartPolygon(false);
				currentNode->SetVisitedTmp(true);
			}
		}
	}

	for (unsigned int i=0; i<m_vNode.size(); i++)
	{
		m_vNode.at(i)->SetVisitedTmp(false);
	}
}

void SimpleGraph::createBuilding(std::vector<unsigned int> &vB, std::ofstream &ofs, unsigned int &idx, double &dGroundLevel)
{
	if (vB.size() <= 1) return;
	double dHeight = dGroundLevel + m_vNode[vB.front()]->GetPosPtr()[2];

	if (vB.back() != vB.front()) vB.push_back(vB.front());

	//create the two sets of vertices (up and down
	double* p1;
	for (unsigned int i=0; i<vB.size(); i++)
	{
		p1 = m_vNode.at(vB[i])->GetPosPtr();
		
		ofs << "v " << p1[0] << " " << p1[1] << " " << dGroundLevel << "\n";
		ofs << "v " << p1[0] << " " << p1[1] << " " << dHeight << "\n";
	}

	//add bottom face
	ofs << "f";
	for (unsigned int i=0; i<vB.size(); i++) ofs << " " << idx+2*i;
	ofs << "\n";

	//add top face
	ofs << "f";
	for (unsigned int i=0; i<vB.size(); i++) ofs << " " << idx+2*i+1;
	ofs << "\n";

	for (unsigned int i=0; i<vB.size()-1; i++)
	{
		ofs << "f " << idx << " " << idx+1 << " " << idx+3 << " " << idx+2 << "\n";
		idx+=2;
	}
	idx+=2;
}