#include "ObjLoader.h"

void Object::initObject(const QString &objName, int _width, int _height, int _mode) {
	winWidth = _width;
	winHeight = _height;
	mode = _mode;
	view = *(new ivec3(0, 0, -1));
	/*bool flag = loadObj(objName);
	colorlist.resize(faces.size());
	
	if (!flag) 
	{
		qDebug() << "ERROR: File loaded fail. Please check your file '" << objName << " '.\n";
	}*/
}


bool Object::loadObj(const QString &objName) {
	minZ = INT_MAX;
	minY = INT_MAX;
	minX = INT_MAX;
	maxZ = INT_MIN;
	maxY = INT_MIN;
	maxX = INT_MIN;
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
				minX = std::min(minX, x);
				minY = std::min(minY, y);
				minZ = std::min(minZ, z);
				maxX = std::max(maxX, x);
				maxY = std::max(maxY, y);
				maxZ = std::max(maxZ, z);
				
				originvertices.push_back(vec3(x,y,z));
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
	qDebug() << "minY:-----------------" << minY << endl;
	qDebug() << "maxY:-----------------" << maxY << endl;
	qDebug() << "minX:-----------------" << minX << endl;
	qDebug() << "maxX:-----------------" << maxX << endl;
	qDebug() << "minZ:-----------------" << minZ << endl;
	qDebug() << "maxZ:-----------------" << maxZ << endl;
	Ortho = abs(minX) > abs(minY) ? abs(minX) : abs(minY);
	Ortho = Ortho > abs(maxX) ? Ortho : abs(maxX);
	Ortho = 1.5*(Ortho > abs(maxY) ? Ortho : abs(maxY));
	qDebug() << "Ortho:-----------------" << Ortho << endl;
	//just for debug:
	qDebug() << originvertices.size() << endl;
	qDebug() << faces.size() << endl;
	colorlist.resize(faces.size());
	//getScreenPos(originvertices, vertices);
	modelUnitize();
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



//void Object::getScreenPos(QVector<vec3> &original, QVector<ivec3> &screenVertices) {
//	screenVertices.clear();
//	GLint viewport[4];
//	GLdouble modelview[16];
//	GLdouble projection[16];
//	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
//	glGetDoublev(GL_PROJECTION_MATRIX, projection);
//	glGetIntegerv(GL_VIEWPORT, viewport);
//	int length = original.size();
//	for (int i = 0; i < length; i++) {
//		GLdouble x, y, z;
//		gluProject(original[i].x, original[i].y, original[i].z, modelview, projection, viewport, &x, &y, &z);
//		ivec3 screen_pos(x, y, z);
//		minZ = std::min(screen_pos.z, int(minZ));
//		minY = std::min(screen_pos.y, int(minY));
//		minX = std::min(screen_pos.x, int(minX));
//		maxZ = std::max(screen_pos.z, int(maxZ));
//		maxY = std::max(screen_pos.y, int(maxY));
//		maxX = std::max(screen_pos.x, int(maxX));
//		screenVertices.push_back(screen_pos);
//	}
//	//just for debug
//	qDebug() << "before change:\n";
//	for (int i = 0; i < original.size(); i++)
//	{
//		qDebug() << original[i].x << ", " << original[i].y << ", " << original[i].z << endl;
//	}
//	qDebug() << "after change:\n";
//	for (int i = 0; i < screenVertices.size(); i++)
//	{
//		qDebug() << screenVertices[i].x << ", " << screenVertices[i].y << ", " << screenVertices[i].z << endl;
//	}
//	qDebug() << "minY:-----------------" << minY << endl;
//	qDebug() << "maxY:-----------------" << maxY << endl;
//	qDebug() << "minX:-----------------" << minX << endl;
//	qDebug() << "maxX:-----------------" << maxX << endl;
//	qDebug() << "minZ:-----------------" << minZ << endl;
//	qDebug() << "maxZ:-----------------" << maxZ << endl;
//}

void Object::getScreenPos(QVector<vec3> &original, QVector<ivec3> &screenVertices) {
	screenVertices.clear();
	int length = original.size();
	GLfloat scale = std::max(abs(maxY),abs(maxX));
	vec3 screen_pos;
	for (int i = 0; i < length; i++) {
		screen_pos = original[i] /scale;
		//screen_pos += (1, 1, 1);
		//screen_pos *= (winWidth / 4);
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
		minZ = std::min(screenVertices[i].z, int(minZ + 0.5));
		minY = std::min(screenVertices[i].y, int(minY + 0.5));
		minX = std::min(screenVertices[i].x, int(minX + 0.5));
		maxZ = std::max(screenVertices[i].z, int(maxZ + 0.5));
		maxY = std::max(screenVertices[i].y, int(maxY + 0.5));
		maxX = std::max(screenVertices[i].x, int(maxX + 0.5));
		qDebug() << screenVertices[i].x << ", " << screenVertices[i].y << ", " << screenVertices[i].z << endl;
	}
	qDebug() << "minY:-----------------" << minY << endl;
	qDebug() << "maxY:-----------------" << maxY << endl;
	qDebug() << "minX:-----------------" << minX << endl;
	qDebug() << "maxX:-----------------" << maxX << endl;
	qDebug() << "minZ:-----------------" << minZ << endl;
	qDebug() << "maxZ:-----------------" << maxZ << endl;
}

void Object::modelUnitize()
{
	double cx, cy, cz, w, h, d;
	double scale;

	if (originvertices.size() > 0)
	{
		w = abs(maxX) + abs(minX);
		h = abs(maxY) + abs(minY);
		d = abs(maxZ) + abs(minZ);
		//计算模型的中心
		cx = (maxX + minX) / 2.0;
		cy = (maxY + minY) / 2.0;
		cz = (maxZ + minZ) / 2.0;

		scale = 1.6 / std::max(w, std::max(h, d));
		scale = scale * 400;

		//将中心按照比例转换
		for (int i = 0; i < originvertices.size(); i++)
		{
			originvertices[i].x -= cx;
			originvertices[i].y -= cy;
			originvertices[i].z -= cz;

			originvertices[i].x += (w / 2);
			originvertices[i].y += (h / 2);

			originvertices[i].x *= scale;
			originvertices[i].y *= scale;
			originvertices[i].z *= -scale;
			vertices.push_back(originvertices[i]);
		}
	}
	for (int i = 0; i < vertices.size(); i++)
	{
		minZ = std::min(vertices[i].z, int(minZ + 0.5));
		minY = std::min(vertices[i].y, int(minY + 0.5));
		minX = std::min(vertices[i].x, int(minX + 0.5));
		maxZ = std::max(vertices[i].z, int(maxZ + 0.5));
		maxY = std::max(vertices[i].y, int(maxY + 0.5));
		maxX = std::max(vertices[i].x, int(maxX + 0.5));
		qDebug() << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << endl;
	}
	qDebug() << "minY:-----------------" << minY << endl;
	qDebug() << "maxY:-----------------" << maxY << endl;
	qDebug() << "minX:-----------------" << minX << endl;
	qDebug() << "maxX:-----------------" << maxX << endl;
	qDebug() << "minZ:-----------------" << minZ << endl;
	qDebug() << "maxZ:-----------------" << maxZ << endl;
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
