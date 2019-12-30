#include "ObjLoader.h"

void Object::initObject(const QString &objName, int _width, int _height, int _mode, bool _speedUp) {
	winWidth = _width;
	winHeight = _height;
	mode = _mode;
	isSpeedUp = _speedUp;
	view = *(new ivec3(0, 0, -1));
}


bool Object::loadObj(const QString &objName) {
	originvertices.clear();
	vertices.clear();
	faces.clear();
	edges.clear();
	colorlist.clear();
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
				vector<int> vid;
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
	qDebug() << originvertices.size() << endl;
	qDebug() << faces.size() << endl;
	colorlist.resize(faces.size());
	changeToScreenSize();
	if (isSpeedUp) {
		isBackFace.clear();
		isBackFace.resize(faces.size(), false);
	}
	return true;
}

ClassifiedEdge Object::CalEdge(int polygon_id, int v1_id, int v2_id, ClassifiedEdge edge) {
	vec3 v1 = vertices[v1_id];
	vec3 v2 = vertices[v2_id];
	edge.edge_polygon_id = polygon_id;
	edge.x = v1.y > v2.y ? v1.x : v2.x;//边的上端点的x坐标
	//edge.dy = ((int)(v1.y + 0.5f) - (int)(v2.y + 0.5f));
    edge.dy = floor(v1.y ) - ceil(v2.y);
	//TODO：DEBUG
	if (v2.y - v1.y != 0) {
		edge.dx = -(v2.x - v1.x) / (v2.y - v1.y);
	}
	else {
		qDebug() << "dy = 0" << endl;
		edge.dx = 1;
	}
	edge.maxY = std::fmaxf(v1.y, v2.y);
	return edge;
}



void Object::CalFace(int face_id, GLfloat &a, GLfloat &b, GLfloat &c, GLfloat &d, int &maxY, GLfloat &maxZ, int &dy, vec3 &color) {
	int v1_id = faces[face_id][0];//three vertices of the face
	int v2_id = faces[face_id][1];
	int v3_id = faces[face_id][2];
	//step1: calculate a,b,c,d
	vec3 vec1 = vertices[v3_id] - vertices[v2_id];
	vec3 vec2 = vertices[v1_id] - vertices[v2_id];
	a = vec1.y*vec2.z - vec1.z*vec2.y;
	b = vec1.z*vec2.x - vec1.x*vec2.z;
	c = vec1.x*vec2.y - vec1.y*vec2.x;
	GLfloat sum = abs(a) + abs(b) + abs(c);
	if (sum == 0) {
		sum = 1;
	}
	a /= sum;
	b /= sum;
	c /= sum;
	d = -a * vertices[v1_id].x - b * vertices[v1_id].y - c * vertices[v1_id].z;
	//step2: calculate color
	GLfloat p0 = abs(a);
	GLfloat p1 = abs(b);
	GLfloat p2 = abs(c);
	GLfloat costheta = p2 / sqrt(p0 + p1 + p2);
	color.x = costheta;
	color.y = costheta;
	color.z = costheta;
	//qDebug() << face_id << " face color : " << color.x << "\t" << color.y << "\t" << color.z << endl;
	//step3: calculate maxZ and maxY
	maxZ = std::fmaxf(vertices[v1_id].z, std::fmaxf(vertices[v2_id].z, vertices[v3_id].z));
	float max_Y = std::fmaxf(vertices[v1_id].y, std::fmaxf(vertices[v2_id].y, vertices[v3_id].y));
	//step 4: calculate dy
	float minY = std::fminf(vertices[v1_id].y, std::fminf(vertices[v2_id].y, vertices[v3_id].y));
	maxY = round(max_Y);
	minY = round(minY);
	dy = maxY - minY;
	//step5: judge back face 
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
	edges.push_back(CalEdge(face_id, v1, v2, edge1));
	edges.push_back(CalEdge(face_id, v1, v3, edge2));
	edges.push_back(CalEdge(face_id, v2, v3, edge3));
}

void Object::changeToScreenSize()
{
	double cx, cy, cz, w, h, d, dw, dh;
	double scale;
	if (originvertices.size() > 0)
	{
		dw = maxX - minX;
		dh = maxY - minY;
		//计算模型的中心
		cx = (maxX + minX) / 2.0;
		cy = (maxY + minY) / 2.0;
		cz = (maxZ + minZ) / 2.0;
		//qDebug() << "cx cy cz: " << cx << ", " << cy << ", " << cz << endl;
		scale = std::min(winHeight / dh, winWidth / dw);
		//qDebug() << "dw=" << dw << ", winWidth=" << winWidth << endl;
		//qDebug() << "dh=" << dh << ", winHeight=" << winHeight << endl;
		//qDebug() << "scale: " << scale << endl;
		vector<vec3> tempvertices;
		tempvertices.clear();
		tempvertices.resize(originvertices.size());
		//将中心按照比例转换
		for (int i = 0; i < originvertices.size(); i++)
		{
			tempvertices[i].x = originvertices[i].x - cx;
			tempvertices[i].y = originvertices[i].y - cy;
			tempvertices[i].z = originvertices[i].z - cz;
			tempvertices[i].x += (dw / 2);
			tempvertices[i].y += (dh / 2);
			tempvertices[i].x *= scale;
			tempvertices[i].y *= scale;
			tempvertices[i].z *= -scale;
			tempvertices[i].x *= 0.9;
            tempvertices[i].y *= 0.9;
            tempvertices[i].z *= 0.9;
			//qDebug() << "tempvertices: " << tempvertices[i].x << ", " << tempvertices[i].y << ", " << tempvertices[i].z << endl;
			vertices.push_back(tempvertices[i]);
		}
	}
	double inf_max = (INT_MAX>>1)*0.1, inf_min = (INT_MIN>>1) * 0.1;
	minZ = inf_max;
	minY = inf_max;
	minX = inf_max;
	maxZ = inf_min;
	maxY = inf_min;
	maxX = inf_min;
	for (int i = 0; i < vertices.size(); i++)
	{
		minZ = std::min(vertices[i].z, int(minZ + 0.5));
		minY = std::min(vertices[i].y, int(minY + 0.5));
		minX = std::min(vertices[i].x, int(minX + 0.5));
		maxZ = std::max(vertices[i].z, int(maxZ + 0.5));
		maxY = std::max(vertices[i].y, int(maxY + 0.5));
		maxX = std::max(vertices[i].x, int(maxX + 0.5));
		//qDebug() << "vertices: " << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << endl;
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
	for (int i = 0; i < faces.size(); i++) {
		CalFaceEdges(i);
		qDebug() << "[" << i << "]";
		for (int j = 0; j < faces[i].size(); j++) {
			qDebug() << faces[i][j] << "\t";
		}
		qDebug() << endl;
	}
	//test edges of face:
	qDebug() << edges.size() << endl;
	for (int i = 0; i < edges.size(); i++) {
		qDebug() << "edge_polygon_id: " << edges[i].edge_polygon_id;
		qDebug() << "\tedge_dy: " << edges[i].dy << endl;
	}
}
