#include "ObjLoader.h"

int Round(GLfloat x) {
	if (x > 0) {
		return (int)(x + 0.5);
	}
	else {
		return (int)(x - 0.5);
	}
}

void Object::initObject(const QString &objName, int _width, int _height, int _mode) {
	winWidth = _width;
	winHeight = _height;
	mode = _mode;
	view = *(new ivec3(0, 0, -1));
	bool flag = loadObj(objName);
	colorlist.resize(faces.size());
	
	if (!flag) 
	{
		qDebug() << "ERROR: File loaded fail. Please check your file '" << objName << " '.\n";
	}
}


bool Object::loadObj(const QString &objName) {

	QFile file(objName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Can't open the file!" << endl;
		return false;
	}
	QStringList list;
	
	QString line;
	GLfloat x, y, z;

	while (!file.atEnd()) {
		QByteArray line = file.readLine();
		QString str(line);
		if (str.length() < 2)
			continue;
		else {
			if (str[0] == "v" && str[1] == ' ') {
				str = str.remove(0, 1).trimmed();
				list = str.split(' ');
				GLfloat x, y, z;
				x = list[0].toFloat();
				y = list[1].toFloat();
				z = list[2].toFloat();
				originvertices.push_back(vec3(x,y,z));
				
				/*minY = minY > y ? y : minY;
				minX = minX > x ? x : minX;
				maxY = maxY > y ? maxY : y;
				maxX = maxX > x ? maxX : x;*/
			}
			else if (str[0] == 'f' && str[1] == ' ') {
				str = str.trimmed();
				list = str.split(' ');
				QVector<int> vid;
				if (list[1].contains('/')) {
					for (int i = 1; i < list.length(); i++) {
						QStringList sublist = list[i].split('/');
						if (sublist[0].toInt() > 0) {
							vid.push_back(sublist[0].toInt() - 1);
						}
						else {
							vid.push_back(sublist[0].toInt() + originvertices.size());
						}
					}
					faces.push_back(vid);
				}
				else {
					for (int i = 1; i < list.length(); i++) {
						if (list[1].toInt() > 0) {
							vid.push_back(list[i].toInt() - 1);
						}
						else {
							vid.push_back(list[i].toInt() + originvertices.size());
						}
					}
					faces.push_back(vid);
				}
			}
			else
			{
				continue;
			}
		}
	}
	Ortho = abs(minX) > abs(minY) ? abs(minX) : abs(minY);
	Ortho = Ortho > abs(maxX) ? Ortho : abs(maxX);
	Ortho = 1.5*(Ortho > abs(maxY) ? Ortho : abs(maxY));
	qDebug() << "Ortho:-----------------" << Ortho << endl;

	//just for debug:
	qDebug() << originvertices.size() << endl;
	qDebug() << faces.size() << endl;
	qDebug() << "-------------------------after change vertices:" << endl;
	getScreenPos(originvertices, vertices);

	qDebug() << "minY:-----------------" << minY << endl;
	qDebug() << "maxY:-----------------" << maxY << endl;
	qDebug() << "minX:-----------------" << minX << endl;
	qDebug() << "maxX:-----------------" << maxX << endl;
	qDebug() << "minZ:-----------------" << minZ << endl;
	qDebug() << "maxZ:-----------------" << maxZ << endl;
	return true;
}



ClassifiedEdge Object::CalEdge(int polygon_id, int v1_id, int v2_id, ClassifiedEdge edge) {
	vec3 v1 = vertices[v1_id];
	vec3 v2 = vertices[v2_id];
	edge.edge_polygon_id = polygon_id;
	edge.x = v1.y > v2.y ? v1.x : v2.x;//边的上端点的x坐标
	
	edge.dy = ((int)(v1.y + 0.5f) - (int)(v2.y + 0.5f));
	if (edge.dy == 0) {
		qDebug() << "dy = 0" << endl;
		system("pause");
	}
	edge.dx = -(v2.x - v1.x) / (v2.y - v1.y);
	edge.maxY = v1.y > v2.y ? (int)(v1.y + 0.5f) : (int)(v2.y + 0.5f);
	return edge;
}

void Object::CalFace(int face_id, GLfloat &a, GLfloat &b, GLfloat &c, GLfloat &d, int &maxY, GLfloat &maxZ, int &dy, vec3 &color) {
	int v1_id = faces[face_id][0];//three vertices of the face
	int v2_id = faces[face_id][1];
	int v3_id = faces[face_id][2];
	//step1: calculate parameters a,b,c,d
	a = (float)((vertices[v2_id].y - vertices[v1_id].y)*(vertices[v3_id].z - vertices[v1_id].z)
		- (vertices[v2_id].z - vertices[v1_id].z)*(vertices[v3_id].y - vertices[v1_id].y));

	b = (float)((vertices[v2_id].z - vertices[v1_id].z)*(vertices[v3_id].x - vertices[v1_id].x)
		- (vertices[v2_id].x - vertices[v1_id].x)*(vertices[v3_id].z - vertices[v1_id].z));

	c = (float)((vertices[v2_id].x - vertices[v1_id].x)*(vertices[v3_id].y - vertices[v1_id].y)
		- (vertices[v2_id].y - vertices[v1_id].y)*(vertices[v3_id].x - vertices[v1_id].x));

	GLfloat sum = abs(a) + abs(b) + abs(c);
	d = (float)(0 - (a*vertices[v1_id].x + b * vertices[v1_id].y + c * vertices[v1_id].z));

	//step2: calculate maxZ and maxY
	maxZ = (vertices[v1_id].z > vertices[v2_id].z) ? vertices[v1_id].z : vertices[v2_id].z;
	maxZ = (maxZ > vertices[v3_id].z) ? maxZ : vertices[v3_id].z;

	float max_Y = (vertices[v1_id].y > vertices[v2_id].y) ? vertices[v1_id].y : vertices[v2_id].y;
	max_Y = (max_Y > vertices[v3_id].y) ? max_Y : vertices[v3_id].y;

	//step 3: calculate dy
	float minY = (vertices[v1_id].y < vertices[v2_id].y) ? vertices[v1_id].y : vertices[v2_id].y;
	minY = (minY < vertices[v3_id].y) ? minY : vertices[v3_id].y;
	
	maxY = (int)(max_Y + 0.5);
	minY = (int)(minY + 0.5);
	dy = maxY - minY;

	//step4: calculate color
	GLfloat z1 = vertices[v1_id].z;
	GLfloat z2 = vertices[v2_id].z;
	GLfloat z3 = vertices[v3_id].z;
	GLfloat z = (z1 + z2 + z3) / 3;
	GLfloat dz = maxZ - minZ;
	color.x = (z - minZ) / dz;
	color.y = (z - minZ) / dz;
	color.z = (z - minZ) / dz;
	qDebug() << face_id << " face color : " << color.x << "\t" << color.y << "\t" << color.z << endl;
}

void Object::CalFaceEdges(int face_id) {
	ClassifiedEdge edge1, edge2, edge3;

	int v1, v2, v3;// v1.y > v2.y > v3.y
	int v1_id = faces[face_id][0];//three vertices of the face
	int v2_id = faces[face_id][1];
	int v3_id = faces[face_id][2];

	if (vertices[v1_id].y < vertices[v2_id].y) {
		if (vertices[v2_id].y < vertices[v3_id].y) {
			v1 = v3_id;
			v2 = v2_id;
			v3 = v1_id;
		}else {
			if (vertices[v1_id].y < vertices[v3_id].y) {
				v1 = v2_id;
				v2 = v3_id;
				v3 = v1_id;
			}else {
				v1 = v2_id;
				v2 = v1_id;
				v3 = v3_id;
			}
		}
	}else {
		if (vertices[v1_id].y < vertices[v3_id].y) {
			v1 = v3_id;
			v2 = v1_id;
			v3 = v2_id;
		}else {
			if (vertices[v2_id].y < vertices[v3_id].y) {
				v1 = v1_id;
				v2 = v3_id;
				v3 = v2_id;
			}else {
				v1 = v1_id;
				v2 = v2_id;
				v3 = v3_id;
			}
		}
	}
	//没有把平行于扫描线的边加入分类边表
	if(vertices[v1].y != vertices[v2].y)
		edges.push_back(CalEdge(face_id, v1, v2, edge1));
	if (vertices[v1].y != vertices[v3].y)
		edges.push_back(CalEdge(face_id, v1, v3, edge2));
	if (vertices[v2].y != vertices[v3].y)
		edges.push_back(CalEdge(face_id, v2, v3, edge3));
}

//just for debug
void Object::test() {
	//test vertices of face
	qDebug() << "face count: " << faces.size() << endl;
	qDebug() << "vertices count:" << originvertices.size() << endl;
	/*for (int i = 0; i < faces.size(); i++) {
		CalFaceEdges(i);
		qDebug() << "[" << i << "]";
		for (int j = 0; j < faces[i].size(); j++) {
			qDebug() << faces[i][j] << "\t";
		} 
		qDebug() << endl;
	}*/
	//test edges of face:
	/*qDebug() << edges.size() << endl;
	for (int i = 0; i < edges.size(); i++) {
		qDebug() << "edge_polygon_id: " << edges[i].edge_polygon_id;
		qDebug() << "\tedge_dy: " << edges[i].dy << endl;
	}*/
}

void Object::ChangeScreenSize() {
	GLfloat dx = round(maxX) - round(minX);
	GLfloat dy = round(maxY) - round(minY);
	if (dx > dy) {	//winWidth不变
		winHeight = (int)((winWidth) * dy / dx);
	}
	else {	//winHeight不变
		winWidth = (int)((winHeight)* dx / dy);
	}
}

//void Object::ChangeOriginvertices() {
//	float xDis = maxX - minX;
//	float yDis = maxY - minY;
//	float xScale = (float)(xDis / winWidth);
//	float yScale = (float)(yDis / winHeight);
//	float Scale = xScale > yScale ? xScale : yScale;
//	qDebug() << "scale: " << Scale << endl;
//
//	//移到左上角
//	float xMovement = minX <= 0 ? 1.2 * abs(minX) : 0.1* abs(minX);
//	float yMovement = minY <= 0 ? 1.2 * abs(minY) : 0.1 * abs(minX);
//
//	if (Scale <= 0.01)
//		Scale = (int)(1 / Scale - 0.5) * 0.5;
//	else if (Scale < 0.5)
//		Scale = (int)(1 / Scale) * 0.6;
//	else if (Scale > 1)
//		Scale = 1.0f / (int)(Scale + 0.5);
//	else
//		Scale = 1;
//
//	qDebug() << "scale: " << Scale << endl;
//	maxY = INT_MIN;
//	minY = INT_MAX;
//	maxX = INT_MIN;
//	minX = INT_MAX;
//	vertices.clear();
//	if (Scale != 1 || xMovement != 0 || yMovement != 0) {
//		for (auto v : originvertices) {
//			//qDebug() << "v: " << v.x << ", " << v.y << ", " << v.z << endl;
//			vec3 tmpv(v.x + xMovement, v.y + yMovement, v.z);//平移
//			tmpv = tmpv * Scale;//缩放
//			tmpv.x = round(tmpv.x);
//			tmpv.y = round(tmpv.y);
//			tmpv.z = round(tmpv.z);
//
//			maxY = maxY > tmpv.y ? maxY : tmpv.y;
//			maxX = maxX > tmpv.x ? maxX : tmpv.x;
//			minY = minY < tmpv.y ? minY : tmpv.y;
//			minX = minX < tmpv.x ? minX : tmpv.x;
//			vertices.push_back(tmpv);
//			//qDebug() << "after v: " << tmpv.x << ", " << tmpv.y  << ", " << tmpv.z << endl;
//		}
//	}
//	qDebug() << "最值：" << maxY << ", " << maxX << ", " <<  minY << ", " << minX << endl;
//	if (maxY > winHeight) {
//		cerr << "Y out of range!!!!!!\n";
//	}
//	if (maxX > winWidth) {
//		cerr << "x out of range!!!!!!\n";
//	}
//}


void Object::getScreenPos(QVector<vec3> &original, QVector<ivec3> &screenVertices) {
	screenVertices.clear();
	minZ = INT_MAX;
	minY = INT_MAX;
	minX = INT_MAX;
	maxZ = INT_MIN;
	maxY = INT_MIN;
	maxX = INT_MIN;

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	for (auto v : original) {
		GLdouble x, y, z;
		gluProject(v.x, v.y, v.z, modelview, projection, viewport, &x, &y, &z);
		ivec3 screen_pos(x, y, z);
		minZ = minZ > z ? z : minZ;
		minY = minY > y ? y : minY;
		minX = minX > x ? x : minX;
		maxZ = maxZ> z ? maxZ : z;
		maxY = maxY > y ? maxY : y;
		maxX = maxX > x ? maxX : x;
		screenVertices.push_back(screen_pos);
	}
	//just for debug
	qDebug() << "before change:\n";
	for (int i = 0; i < original.size(); i++)
	{
		qDebug() << original[i].x << ", " << original[i].y << ", " << original[i].z << endl;
	}
	qDebug() << "after change:\n";
	for (int i = 0; i < screenVertices.size(); i++)
	{
		qDebug() << screenVertices[i].x << ", " << screenVertices[i].y << ", " << screenVertices[i].z << endl;
	}
}