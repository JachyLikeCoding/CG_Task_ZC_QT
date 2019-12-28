#include "ScanlineZBuffer.h"

void ScanlineZBufferProcessor::initProcessor(Object &_obj) {
	obj = _obj;
	winWidth = _obj.getWinWidth();
	winHeight = _obj.getWinHeight();
	CPT.clear();
	CET.clear();
	/*CPT.resize(winHeight);
	CET.resize(winHeight);*/
	CPT.resize(int(obj.maxY + 0.5) - int(obj.minY + 0.5) + 1);
	CET.resize(int(obj.maxY + 0.5) - int(obj.minY + 0.5) + 1);
	APT.clear();
	AET.clear();
	framebuffer.clear();
	//framebuffer.resize(3 * winHeight * winWidth);
}

//build classified polygon table
void ScanlineZBufferProcessor::buildCPT() {
	//qDebug() << "obj.faces.size()：" << obj.faces.size() << endl;
	for (int i = 0; i < obj.faces.size(); i++) {
		int maxY;
		GLfloat maxZ;
		GLfloat a, b, c, d;
		int dy;
		vec3 color;
		obj.CalFace(i, a, b, c, d, maxY, maxZ, dy, color);
		obj.CalFaceEdges(i);
		ClassifiedPolygon newCP(i, a, b, c, d, dy, color);
		qDebug() << "color:" << color.x << ", "<< color.y << ", " << color.z << endl;
		//qDebug() << "maxY:" << maxY << endl;
		CPT[maxY].push_back(newCP);
	}
}


void ScanlineZBufferProcessor::buildCET() {
	//qDebug() << "obj.faces.size()：" << obj.faces.size() << endl;
	for (int i = 0; i < obj.edges.size(); i++) {
		int maxY = obj.edges[i].maxY;
		CET[maxY].push_back(obj.edges[i]);
	}
}


bool ScanlineZBufferProcessor::addPolygonToAPT(int y, QVector<ActivePolygon> &APT, QVector<QVector<ClassifiedPolygon>> &CPT) {
	if (CPT[y].empty()) {
		return false;
	}
	for (int i = 0; i < CPT[y].size(); i++){
		ClassifiedPolygon cp = CPT[y][i];
		ActivePolygon ap(cp.polygon_id, cp.a, cp.b, cp.c, cp.d, y, cp.color);
		APT.push_back(ap);
	}
	return true;
}


//如果有新的多边形加入到活化多边形表中，则把该多边形在oxy平面上的投影和扫描线相交的边加入到活化边表中
//void ScanlineZBufferProcessor::addEdgeToAET(QVector<ClassifiedEdge>& CET_y, QVector<ActiveEdge>& AET, QVector<ClassifiedPolygon>& CPT_y){
//	/*qDebug() << "CPT_y.size(): " << CPT_y.size() << endl;
//	qDebug() << "CET_y.size(): " << CET_y.size() << endl;*/
//
//	for (int p = 0; p < CPT_y.size(); p++) {
//		int leftflag(-1), rightflag(-1);
//		for (int i = 0; i < CET_y.size(); i++) {
//			if (CET_y[i].edge_polygon_id == CPT_y[p].polygon_id) {	//找到属于该多边形的边
//				//找到左边和右边组成边对
//				//qDebug() << "CET_y[" << i << "].edge_polygon_id == CPT_y[" << p << "].polygon_id = " << CET_y[i].edge_polygon_id << endl;
//				if ((leftflag == -1) || (rightflag == -1)) {
//					leftflag = rightflag = i;
//				}
//				else {
//					//qDebug() << "CET_y[i].edge_polygon_id:" << CET_y[i].edge_polygon_id << endl;
//					//qDebug() << "CET_y[leftflag].edge_polygon_id:" << CET_y[leftflag].edge_polygon_id << endl;
//					if (CET_y[i].isLeftEdge(CET_y[leftflag])) {//更新左标志
//						leftflag = i;
//					}
//					if (CET_y[rightflag].isLeftEdge(CET_y[i])) {//更新右标志
//						rightflag = i;
//					}
//				}
//			}
//		}
//		if (leftflag != rightflag) {
//			ActiveEdge *AE = new ActiveEdge(CET_y[leftflag], CET_y[rightflag], CPT_y[p]);
//			//ActiveEdge AE(CET_y[leftflag], CET_y[rightflag], CPT_y[p]);
//			AET.push_back(*AE);
//		}
//		else {
//			//qDebug() << "没有加边对!" << endl;
//		}
//	}
//}

void ScanlineZBufferProcessor::addEdgeToAET(QVector<ClassifiedEdge>& CET_y, QVector<ActiveEdge>& AET, QVector<ActivePolygon>& APT) {
	/*qDebug() << "CPT_y.size(): " << CPT_y.size() << endl;
	qDebug() << "CET_y.size(): " << CET_y.size() << endl;*/

	for (int p = 0; p < APT.size(); p++) {
		int leftflag(-1), rightflag(-1);
		for (int i = 0; i < CET_y.size(); i++) {
			if (CET_y[i].edge_polygon_id == APT[p].polygon_id) {	//找到属于该多边形的边
				//找到左边和右边组成边对
				//qDebug() << "CET_y[" << i << "].edge_polygon_id == CPT_y[" << p << "].polygon_id = " << CET_y[i].edge_polygon_id << endl;
				if ((leftflag == -1) || (rightflag == -1)) {
					leftflag = rightflag = i;
				}
				else {
					//qDebug() << "CET_y[i].edge_polygon_id:" << CET_y[i].edge_polygon_id << endl;
					//qDebug() << "CET_y[leftflag].edge_polygon_id:" << CET_y[leftflag].edge_polygon_id << endl;
					if (CET_y[i].isLeftEdge(CET_y[leftflag])) {//更新左标志
						leftflag = i;
					}
					if (CET_y[rightflag].isLeftEdge(CET_y[i])) {//更新右标志
						rightflag = i;
					}
				}
			}
		}
		if (leftflag != rightflag) {
			ActiveEdge *AE = new ActiveEdge(CET_y[leftflag], CET_y[rightflag], APT[p]);
			//ActiveEdge AE(CET_y[leftflag], CET_y[rightflag], CPT_y[p]);
			AET.push_back(*AE);
		}
		else {
			//qDebug() << "没有加边对!" << endl;
		}
	}
}


void ScanlineZBufferProcessor::ScanlineZBuffer(Object &_obj) {
	initProcessor(_obj);
	buildCPT();
	buildCET();
	qDebug() << "minY: " << obj.minY << endl;
	qDebug() << "maxY: " << obj.maxY << endl;

	//for (int y = obj.maxY; y >= obj.minY; y--) {	//每一行扫描线
	//	qDebug() << "----------------scanline y = " << y << "---------------------------" << endl;
	//	zbuffer.clear();
	//	coloridbuffer.clear();
	//	//zbuffer.resize(windowWidth, -INT_MAX);//离视点最远
	//	//coloridbuffer.resize(windowWidth, -1);//背景颜色（索引设为-1）
	//	//检查分类多边形表，如果有新的多边形涉及该扫描线，把它放入活化多边形表中,再把它的边放入活化边表中
	//	if (addPolygonToAPT(y, APT, CPT)) {
	//		//addEdgeToAET(CET[y], AET, CPT[y]);
	//		addEdgeToAET(CET[y], AET, APT);
	//	}
	//	//更新缓冲区和活化多边形表和活化边表
	//	updateBuffer(AET,y);
	//	//update APT & AET
	//	update_APTAET(APT, AET, CET[y]);
	//}
	qDebug() << "模型顶点数: " << obj.vertices.size() << endl;
	qDebug() << "模型面片数：" << obj.faces.size() << endl;
	//clear data
	clearDS();
}

bool ScanlineZBufferProcessor::update_APT(ActivePolygon &AP) {
	AP.remain_dy -= 1;
	if (AP.remain_dy < 0) {
		return false;
	}
	else
	{
		return true;
	}
}

bool ScanlineZBufferProcessor::update_AET(ActiveEdge &AE, QVector<ClassifiedEdge> &CET_y) {
	AE.dyl -= 1;
	AE.dyr -= 1;
	if (AE.dyl < 0) {
		int id = findNextCE(AE, AE.xl, CET_y);
		if (id != -1) {
			AE.changeLeftEdge(CET_y[id]);
		}
	}

	if (AE.dyr < 0) {
		int id = findNextCE(AE, AE.xr, CET_y);
		if (id != -1) {
			AE.changeRightEdge(CET_y[id]);
		}
	}
	//if (abs(AE.dxl) < 10 && abs(AE.dxr) < 10) {
		AE.xl += (int)AE.dxl;
		AE.xr += (int)AE.dxr;
		if (AE.xl < 0) {
			qDebug() << "AE.xl < 0!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
		}
		if (AE.xr < 0) {
			qDebug() << "AE.xr < 0!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
		}
		AE.zl += (AE.dzx * AE.dxl + AE.dzy);
	//}
		return true;
}

int ScanlineZBufferProcessor::findNextCE(ActiveEdge &AE, GLfloat x, QVector<ClassifiedEdge> &CET_y) {
	int id = AE.edge_polygon_id;
	for (int i = 0; i < CET_y.size(); ++i) {
		if ((CET_y[i].edge_polygon_id == id) && (abs(CET_y[i].x - x) < 1)){
			return i;
		}
	}
	return -1;
}

bool  ScanlineZBufferProcessor::update_APTAET(QVector<ActivePolygon> &APT, QVector<ActiveEdge> &AET, QVector<ClassifiedEdge> &CET_y) {
	for (int i = 0; i < APT.size(); ++i) {
		if (update_APT(APT[i]))
			update_AET(AET[i], CET_y);
		else
		{  
			APT.erase(APT.begin() + i);
			AET.erase(AET.begin() + i);
			--i;
		}
	}
	return true;
}

//bool  ScanlineZBufferProcessor::update_APTAET(QVector<ActivePolygon> &APT, QVector<ActiveEdge> &AET, QVector<ClassifiedEdge> &CET_y) {
//	for (int i = 0; i < APT.size(); ++i) {
//		if ((--APT[i].remain_dy < 0)) {//delete the polygon and its edge pair
//			APT.erase(APT.begin() + i);
//			AET.erase(AET.begin() + i);
//			--i;
//		}
//		else {//update AET
//			  //should find another left edge
//			if ((--AET[i].dyl) < 0) {
//				for (int e = 0; e < CET_y.size(); e++) {
//					if ((CET_y[e].edge_polygon_id == AET[i].edge_polygon_id) && (abs(CET_y[e].x - AET[i].xl)) < 1) {
//						AET[i].xl = CET_y[e].x;
//						AET[i].dxl = CET_y[e].dx;
//						AET[i].dyl = CET_y[e].dy;
//					}
//				}
//			}
//			//should find another right edge
//			if ((--AET[i].dyr) < 0) {
//				for (int e = 0; e < CET_y.size(); e++) {
//					if ((CET_y[e].edge_polygon_id == AET[i].edge_polygon_id) && (abs(CET_y[e].x - AET[i].xr)) < 1) {
//						AET[i].xr = CET_y[e].x;
//						AET[i].dxr = CET_y[e].dx;
//						AET[i].dyr = CET_y[e].dy;
//					}
//				}
//			}
//			//update AET
//			AET[i].xl += AET[i].dxl;
//			AET[i].xr += AET[i].dxr;
//			//if (abs(AET[i].dzx) < 20 && abs(AET[i].dxl) < 20 && abs(AET[i].dzy) < 20) {
//			AET[i].zl += (AET[i].dzx * AET[i].dxl + AET[i].dzy);
//
//			if (AET[i].xl < 0){
//				qDebug() << "AET[i].xl = " << AET[i].xl << endl;
//				AET[i].xl = 0;
//				cerr << "xl = 0！！！！！！！" << endl;
//			}
//			if (AET[i].xr < 0) {
//				qDebug() << "AET[i].xr = " << AET[i].xr << endl;
//				AET[i].xr = 0;
//				cerr << "xr = 0！！！！！！！" << endl;
//			}
//		}
//	}
//	return true;
//}

void ScanlineZBufferProcessor::resizeWindow(int width, int height) {
	winHeight = height;
	winWidth = width;
	CPT.clear();
	CET.clear();
	CPT.resize(height);
	CET.resize(height);
	zbuffer.clear();
	zbuffer.resize(width);
}

//update z-buffer and color buffer
bool ScanlineZBufferProcessor::updateBuffer(QVector<ActiveEdge> &AET, int y) {
	/*zbuffer.resize(winWidth, -INT_MAX);
	coloridbuffer.resize(winWidth, -1);*/

	QVector<ActiveEdge>::iterator iter;
	for(iter = AET.begin(); iter != AET.end(); ++iter){
	//for (int i = 0; i < AET.size(); i++) {
		GLfloat zx = -INT_MAX;
		for (int x = int(iter->xl); x < iter->xr; x++) {
			if (x < 0) {
				qDebug() << "x < 0!!!!!!! wrong here!!!!!!!!!!!!!!" << endl;
				return false;
			}
			if (x == int(iter->xl)) {
				zx = iter->zl;
			}
			else {
				zx += iter->dzx;
			}
			
			if (zx > zbuffer[x]) {
				/*qDebug() << "zx > zbuffer" << endl;*/
				//更新深度缓存
				zbuffer[x] = zx;
				//找到相应多边形的颜色
				coloridbuffer[x] = iter->edge_polygon_id;
				vec3 color = getColor(coloridbuffer[x], APT);
				//更新帧缓存
				framebuffer[3 * (y * winWidth + x )] = color.x;
				framebuffer[3 * (y * winWidth + x ) + 1] = color.y;
				framebuffer[3 * (y * winWidth + x ) + 2] = color.z;
			}
		}
	}
	return true;
}



vec3 ScanlineZBufferProcessor::getColor(int polygon_id,  QVector<ActivePolygon> &APT) {
	vec3 color;
	for (int i = 0; i < APT.size(); i++) {
		if (APT[i].polygon_id == polygon_id)
		{
			color = APT[i].color;
		}
	}
	return color;
}

void ScanlineZBufferProcessor::clearDS() {
	AET.clear();
	APT.clear();
	CET.clear();
	CPT.clear();
}


//just for debug:
void ScanlineZBufferProcessor::test() {
	qDebug() << "CET size:" << CET.size() << endl;
	qDebug() << "CPT size:" << CPT.size() << endl;
}


void ScanlineZBufferProcessor::printAET(QVector<ActiveEdge> &AET) {
	qDebug() << "AET size:" << AET.size() << "\n";
	qDebug() << "AET edge_polygon_id:  ";
	for (int i = 0; i < AET.size(); i++) {
		qDebug() << AET[i].edge_polygon_id << "\t";
	}
	qDebug() << endl;
	qDebug() << "AET dxl & dxr:  \n";
	for (int i = 0; i < AET.size(); i++) {
		qDebug() << AET[i].dxl << "\t" << AET[i].dxr <<endl;
	}
	qDebug() << endl;
	qDebug() << endl;
	qDebug() << "AET xl & xr:  \n";
	for (int i = 0; i < AET.size(); i++) {
		qDebug() << AET[i].xl << "\t" << AET[i].xr << endl;
	}
}

void ScanlineZBufferProcessor::printAPT(QVector<ActivePolygon> &APT) {
	qDebug() << "APT size:" << APT.size() << "\n";
	qDebug() << "APT polygon_id:  ";
	for (int i = 0; i < APT.size(); i++) {
		qDebug() << APT[i].polygon_id << "\t";
	}
	qDebug() << endl;
}