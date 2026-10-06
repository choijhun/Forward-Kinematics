//
//  main.cpp
//  IK
//
//  Created by Hyun Joon Shin on 2021/06/09.
//

#include <iostream>
#define JGL2_IMPLEMENTATION
#include <JGL2/JGL.hpp>
#include <JGL2/Anim3DView.hpp>
#include "j3a.hpp"
#include "quat.hpp"
#include <filesystem>
#include <stdexcept>
using namespace JGL2;
using namespace std;
using namespace jm;

std::vector<geomObject> objs;

Anim3DView<JR::PBRRenderer>* animView;
std::vector<JR::RenderableMeshBase> meshes;


inline float toRadian(float deg) {
    return deg / 180.f * 3.14159265358979;
}


enum CHANNEL_TYPE {
    X_POSITION,
    Y_POSITION,
    Z_POSITION,
    X_ROTATION,
    Y_ROTATION,
    Z_ROTATION
};
struct Node {
    vector<Node> children;
    vector<CHANNEL_TYPE> channels;
    vec3 translation;
    quat rotation;
    vec3 offset;
    string name;
    int dataOFFset = -1;

    void update(const std::vector<float>& d) {
        int o = dataOFFset;
        translation = vec3(0);
        rotation = quat();
        for (auto c : channels) {
            switch (c) {
            case X_POSITION: translation += vec3(d[o], 0, 0) * 20; break;
            case Y_POSITION: translation += vec3(0, d[o], 0) * 20; break;
            case Z_POSITION: translation += vec3(0, 0, d[o]) * 20; break;
            case X_ROTATION: rotation = rotation * qexp(vec3(1, 0, 0) * toRadian(d[o])); break;
            case Y_ROTATION: rotation = rotation * qexp(vec3(0, 1, 0) * toRadian(d[o])); break; // 곱하기 순서
            case Z_ROTATION: rotation = rotation * qexp(vec3(0, 0, 1) * toRadian(d[o])); break;
            }
            o++;
        }
        for (auto& c : children) c.update(d);
    }
    void addChild(const Node& n) {
        children.push_back(n);
    }
    void print(ostream& os, int n = 0) const {
        for (int i = 0; i < n; i++) os << " ";
        os << name << "  <" << dataOFFset << ">" << endl;
        for (const auto& c : children)
            c.print(os, n + 1);
    }
    void render(vec3 pp = vec3(0), quat pq = quat(), bool isRoot = true) const {

        quat q = pq * rotation;

        vec3 l = (pq * quat(0, offset) * inverse(pq)).v;

        vec3 p = pp + l + translation;

        JR::drawSphere(p, 3, vec4(1, 0, 0, 1)); // 관절
        if (!isRoot)
            JR::drawCylinder(pp, p, 1.5, vec4(0, 1, 0, 1)); // 본

        for (const auto& c : children)
            c.render(p, q, false);
    }

};

struct BVH {
    Node root;
    std::vector<std::vector<float>>data;
    float frameTime;

    Node readEndSite(istream& is) {
        Node node;
        string tmp;
        is >> tmp;
        is >> node.name;
        is >> tmp;        // {
        is >> tmp;
        is >> node.offset.x >> node.offset.y >> node.offset.z;
        node.offset *= 20;
        is >> tmp;
        return node;
    }
    Node readNode(istream& is, int& offset) {
        Node node;
        node.dataOFFset = offset;
        string tmp;
        int n;
        is >> node.name;
        is >> tmp;        // {
        is >> tmp;
        is >> node.offset.x >> node.offset.y >> node.offset.z;
        node.offset *= 20;
        is >> tmp;        // CHANNELS
        is >> n;

        for (int i = 0; i < n; i++) {
            is >> tmp;
            if (tmp.compare("Xposition") == 0) node.channels.push_back(X_POSITION);
            else if (tmp.compare("Yposition") == 0) node.channels.push_back(Y_POSITION);
            else if (tmp.compare("Zposition") == 0) node.channels.push_back(Z_POSITION);
            else if (tmp.compare("Xrotation") == 0) node.channels.push_back(X_ROTATION);
            else if (tmp.compare("Yrotation") == 0) node.channels.push_back(Y_ROTATION);
            else if (tmp.compare("Zrotation") == 0) node.channels.push_back(Z_ROTATION);
            // 이후 채널 종류에 따라 push_back 처리
        }
        offset += n;
        while (true) {
            is >> tmp;
            if (tmp.compare("}") == 0) break;
            else if (tmp.compare("JOINT") == 0) {
                Node n = readNode(is, offset);
                node.addChild(n);
            }
            else if (tmp.compare("End") == 0) {
                Node n = readEndSite(is);
                node.addChild(n);
            }
        }
        return node;
    }

    void load(std::istream& is) {
        string tmp;
        is >> tmp;   // HIERARCHY
        is >> tmp;   // ROOT
        int offset = 0;
        root = readNode(is, offset);

        is >> tmp;   // Motion
        is >> tmp;   // Frame
        int n;
        is >> n;
        is >> tmp >> tmp; //Frames
        is >> frameTime;
        data.resize(n, std::vector<float>(offset, 0));
        for (auto i = 0; i < n; i++) {
            for (auto j = 0; j < offset; j++)
                is >> data[i][j];
        }
    }

    bool load(const std::filesystem::path& fn) {
        ifstream ifs(fn);
        if (!ifs.is_open()) return false;
        load(ifs);
        ifs.close();
        return true;
    }
    void update(int i) {
        root.update(data[i]);
    }
    void render() {
        root.render();
    }
    size_t frames() { return data.size(); }
};


BVH motion;

void render() {
    JR::drawQuad({ 0,0,0 }, { 0,1,0 }, { 1000,1000 }, { 1,1,1,1 });
    int i = animView->currentFrame();
    motion.update(i);
    motion.render();
}

void loadBVH(const std::string& str) {
    motion.load(str);
    if (animView) {
        animView->range(0, motion.frames() - 1);
        animView->fps(1 / motion.frameTime);
    }
}

void dndCB(Widget*, void*, const slst_t& l) {
    loadBVH(l[0]);
}

//메인 이상무
int main(int argc, const char* argv[]) {
    Window* window = new Window(800, 600, "simulation");
    window->alignment(align_t::ALL);
    animView = new Anim3DView(0, 0, 800, 600);
    animView->renderFunc(render);
    animView->cameraPos({ 0,100,500 }, true);
    loadBVH("C:/Users/choij/Downloads/BVH/BVH/BackKickA.bvh");
    animView->range(0, motion.frames() - 1);
    animView->dndCallback(dndCB);
    window->show();
    _JGL::run();
    return 0;
}




