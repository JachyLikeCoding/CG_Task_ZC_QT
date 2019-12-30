#include "ScanlineZBuffer.h"
void ScanlineZBufferProcessor::initProcessor(Object& _obj) {
    obj = _obj;
    winWidth = _obj.getWinWidth();
    winHeight = _obj.getWinHeight();
    CPT.clear();
    CET.clear();
    CPT.resize(winHeight);
    CET.resize(winHeight);
    APT.clear();
    AET.clear();
    framebuffer.clear();
    framebuffer.resize(3 * winHeight * winWidth, 0);
}
// build classified polygon table
void ScanlineZBufferProcessor::buildCPT() {
    qDebug() << "buildCPT: " << obj.faces.size() << "faces" << endl;
    for (int i = 0; i < obj.faces.size(); i++) {
        int maxY;
        GLfloat maxZ;
        GLfloat a, b, c, d;
        int dy;
        vec3 color;
        obj.CalFace(i, a, b, c, d, maxY, maxZ, dy, color);
        obj.CalFaceEdges(i);
		//speed up here
		if (!obj.isBackFace.empty() && c < 0) {
			continue;
		}
        ClassifiedPolygon newCP(i, a, b, c, d, dy, color);
        qDebug() << "maxY:" << maxY << endl;
        std::cout << "i=" << i << endl;
        CPT[maxY].push_back(newCP);
    }
}
void ScanlineZBufferProcessor::buildCET() {
    // qDebug() << "obj.faces.size()：" << obj.faces.size() << endl;
    qDebug() << "buildCET：" << obj.edges.size() << "edges" << endl;
    for (int i = 0; i < obj.edges.size(); i++) {
        int maxY = obj.edges[i].maxY;
        CET[maxY].push_back(obj.edges[i]);
    }
}
bool ScanlineZBufferProcessor::addPolygonToAPT(int y, vector<ActivePolygon>& APT,
                                               vector<vector<ClassifiedPolygon>>& CPT, vector<ClassifiedEdge>& CET_y) {
    if (CPT[y].empty()) {
        return false;
    }
    for (int i = 0; i < CPT[y].size(); i++) {
        const auto& cp = CPT[y][i];
        vector<int> v;
        for (int e = 0; e < CET_y.size(); e++) {
            // qDebug() << "(i,e): " << i << ", " << e << endl;
            if (CET_y[e].edge_polygon_id == CPT[y][i].polygon_id) {  //找到属于该多边形的边
                v.push_back(e);
                if (v.size() == 2)
                    break;
            }
        }
        if (v.size() == 2) {
            const auto &e1 = CET_y[v[0]], &e2 = CET_y[v[1]];
            if (e1.x == e2.x && e1.dx == e2.dx) {
                continue;
            }
        }
		ActivePolygon ap(cp.polygon_id, cp.a, cp.b, cp.c, cp.d, cp.dy, cp.color);
        APT.push_back(ap);
    }
    return true;
}
bool ScanlineZBufferProcessor::addEdgeToAET(vector<ClassifiedEdge>& CET_y, vector<ActiveEdge>& AET,
                                            vector<ActivePolygon>& APT) {
    qDebug() << "APT.size(): " << APT.size() << endl;
    qDebug() << "CET_y.size(): " << CET_y.size() << endl;
    for (int p = AET.size(); p < APT.size(); p++) {
        int leftflag(-1), rightflag(-1);
        for (int i = 0; i < CET_y.size(); i++) {
            if (CET_y[i].edge_polygon_id == APT[p].polygon_id) {  //找到属于该多边形的边
                //找到左边和右边组成边对
                if ((leftflag == -1) || (rightflag == -1)) {
                    leftflag = rightflag = i;
                } else {
                    // qDebug() << "CET_y[i].edge_polygon_id:" << CET_y[i].edge_polygon_id << endl;
                    // qDebug() << "CET_y[leftflag].edge_polygon_id:" << CET_y[leftflag].edge_polygon_id << endl;
                    if (CET_y[i].isLeftEdge(CET_y[leftflag])) {  //更新左标志
                        leftflag = i;
                    }
                    if (CET_y[rightflag].isLeftEdge(CET_y[i])) {  //更新右标志
                        rightflag = i;
                    }
                }
            }
        }
        if (leftflag != rightflag) {
            ActiveEdge* AE = new ActiveEdge(CET_y[leftflag], CET_y[rightflag], APT[p]);
            AET.push_back(*AE);
            if (AE->xl < 0 || AE->xr > 800) {
                qDebug() << endl;
            }
        } else {
             qDebug() << "没有加边对!" << endl;
        }
    }
    return true;
}
void ScanlineZBufferProcessor::ScanlineZBuffer(Object& _obj) {
    initProcessor(_obj);
    buildCPT();
    buildCET();
    qDebug() << "minY: " << obj.minY << endl;
    qDebug() << "maxY: " << obj.maxY << endl;
    for (int y = obj.maxY; y >= obj.minY; y--) {  //每一行扫描线
        qDebug() << "----------------scanline y = " << y << "---------------------------" << endl;
        zbuffer.clear();
        coloridbuffer.clear();
        zbuffer.resize(winWidth, -INT_MAX);  //离视点最远
        coloridbuffer.resize(winWidth, -1);  //背景颜色（索引设为-1）
        //检查分类多边形表，如果有新的多边形涉及该扫描线，把它放入活化多边形表中,再把它的边放入活化边表中
        if (addPolygonToAPT(y, APT, CPT, CET[y])) {
            addEdgeToAET(CET[y], AET, APT);
            if (AET.size() != APT.size()) {
                APT.erase(APT.begin() + AET.size(), APT.end());
                qDebug() << "AET size != APT size, so erase APT!!!" << endl;
            }
        }

		if (AET.size() != APT.size()) {
			qDebug() << "APT size: " << APT.size() << endl;
			qDebug() << "AET size：" << AET.size() << endl;
            qDebug() << "AET size != APT size" << endl;
		}

        //更新缓冲区和活化多边形表和活化边表
        updateBuffer(AET, y);
        // update APT & AET
        update_APTAET(APT, AET, CET[y]);
    }
    qDebug() << "vertices number: " << obj.vertices.size() << endl;
    qDebug() << "faces number：" << obj.faces.size() << endl;
    // clear data
    clearDS();
}
bool ScanlineZBufferProcessor::update_APT(ActivePolygon& AP) {
    AP.remain_dy -= 1;
    if (AP.remain_dy < 0) {
        return false;
    } else {
		//qDebug() << "remain dy: " << AP.remain_dy << endl;
        return true;
    }
}
bool ScanlineZBufferProcessor::update_AET(ActiveEdge& AE, vector<ClassifiedEdge>& CET_y) {
    AE.dyl -= 1;
    AE.dyr -= 1;
    bool hasChange = false;
	if (AE.dyl < 0 && AE.dyr < 0) {
		qDebug() << "AE.dyl < 0 && AE.dyr < 0" << endl;
	}
    if (AE.dyl < 0) {
        int id = findNextCE(AE, AE.xl, CET_y);
        if (id != -1) {
            qDebug() << "id: " << id << endl;
            AE.changeLeftEdge(CET_y[id]);
        } 
		else
		{
            qDebug() << "id: " << id << endl;
            //return false;
        }
        hasChange = true;
    }
    if (AE.dyr < 0) {
        int id = findNextCE(AE, AE.xr, CET_y);
        if (id != -1) {
            AE.changeRightEdge(CET_y[id]);
        } 
		else {
            qDebug() << "id: " << id << endl;
            //return false;
        }
        hasChange = true;
    }
    if (hasChange) {
        return true;
    }
    AE.xl += AE.dxl;
    AE.xr += AE.dxr;
    AE.zl += (AE.dzx * AE.dxl + AE.dzy);
    return true;
}
int ScanlineZBufferProcessor::findNextCE(ActiveEdge& AE, GLfloat x, vector<ClassifiedEdge>& CET_y) {
    int id = AE.edge_polygon_id;
    for (int i = 0; i < CET_y.size(); ++i) {
        if ((CET_y[i].edge_polygon_id == id) && (abs(CET_y[i].x - x) < 1)) {
            return i;
        }
    }
    return -1;
}
bool ScanlineZBufferProcessor::update_APTAET(vector<ActivePolygon>& APT, vector<ActiveEdge>& AET,
                                             vector<ClassifiedEdge>& CET_y) {
    for (int i = 0; i < APT.size(); ++i) {
		bool isAETChanged = false;
        if (update_APT(APT[i])) {  //需要更新APT[i]
            isAETChanged = update_AET(AET[i], CET_y);
        } else {  // APT[i]要删除
            APT.erase(APT.begin() + i);
            AET.erase(AET.begin() + i);
            --i;
        }
    }
    return true;
}
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
// update z-buffer and color buffer
bool ScanlineZBufferProcessor::updateBuffer(vector<ActiveEdge>& AET, int y) {
    zbuffer.resize(winWidth, -INT_MAX);
    coloridbuffer.resize(winWidth, -1);
    for (auto iter = AET.begin(); iter != AET.end(); ++iter) {
        GLfloat zx = -INT_MAX;
        for (int x = int(iter->xl); x < std::min(int(iter->xr), 800); x++) {
            if (x < 0) {
                qDebug() << "x < 0!!!!!!! wrong here!!!!!!!!!!!!!!" << endl;
                return false;
            }
            if (x == int(iter->xl)) {
                zx = iter->zl;
            } else {
                zx += iter->dzx;
            }
            if (zx > zbuffer[x]) {
                //更新深度缓存
                zbuffer[x] = zx;
                //找到相应多边形的颜色
                coloridbuffer[x] = iter->edge_polygon_id;
                vec3 color = getColor(coloridbuffer[x], APT);
                //更新帧缓存
                framebuffer[3 * (y * winWidth + x)] = color.x;
                framebuffer[3 * (y * winWidth + x) + 1] = color.y;
                framebuffer[3 * (y * winWidth + x) + 2] = color.z;
            }
        }
    }
    return true;
}
vec3 ScanlineZBufferProcessor::getColor(int polygon_id, vector<ActivePolygon>& APT) {
    vec3 color;
    for (int i = 0; i < APT.size(); i++) {
        if (APT[i].polygon_id == polygon_id) {
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

// just for debug:
void ScanlineZBufferProcessor::test() {
    qDebug() << "CET size:" << CET.size() << endl;
    qDebug() << "CPT size:" << CPT.size() << endl;
}
void ScanlineZBufferProcessor::printAET(vector<ActiveEdge>& AET) {
    qDebug() << "AET size:" << AET.size() << "\n";
    qDebug() << "AET edge_polygon_id:  ";
    for (int i = 0; i < AET.size(); i++) {
        qDebug() << AET[i].edge_polygon_id << "\t";
    }
    qDebug() << endl;
    qDebug() << "AET dxl & dxr:  \n";
    for (int i = 0; i < AET.size(); i++) {
        qDebug() << AET[i].dxl << "\t" << AET[i].dxr << endl;
    }
    qDebug() << endl;
    qDebug() << endl;
    qDebug() << "AET xl & xr:  \n";
    for (int i = 0; i < AET.size(); i++) {
        qDebug() << AET[i].xl << "\t" << AET[i].xr << endl;
    }
}
void ScanlineZBufferProcessor::printAPT(vector<ActivePolygon>& APT) {
    qDebug() << "APT size:" << APT.size() << "\n";
    qDebug() << "APT polygon_id:  ";
    for (int i = 0; i < APT.size(); i++) {
        qDebug() << APT[i].polygon_id << "\t";
    }
    qDebug() << endl;
}