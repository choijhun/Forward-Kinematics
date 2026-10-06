//
//  j3a.hpp
//  IK
//
//  Created by Hyun Joon Shin on 5/8/25.
//

#ifndef j3a_h
#define j3a_h

#include <tuple>
#include <vector>
#include <jm/jm.hpp>
#include <filesystem>

inline std::string readString( std::istream& ifs ) {
	std::string ret = "";
	std::getline(ifs, ret, '\"');
	std::getline(ifs, ret, '\"');
	return ret;
}

struct geomObject {
	std::vector<jm::vec3> vertices;
	std::vector<jm::vec3> normals;
	std::vector<jm::vec2> texCoords;
	std::vector<jm::uvec3> indices;
	jm::vec4 diffColor;
	jm::vec3 specColor;
	float shininess;
	std::string diffMap;
	std::string bumpMap;
	std::string specMap;
	std::string ambOccMap;
};
inline std::vector<geomObject>
readJ3a(const std::filesystem::path& fn) {
	std::ifstream ifs( fn );
	std::vector<geomObject> ret;

	int nObjects;
	if( !ifs.is_open() ) {
		printf("J3A file cannot be open.. maybe file not found?\n");
		return ret;
	}
	
	ifs>>nObjects;
	printf("Number of objects: %d\n", nObjects );
	
	ret.resize(nObjects);
	
	for( int i=0; i<nObjects; i++ ) {
		int temp;
		int nVertices = 0, nTriangles=0;
		ifs>>ret[i].diffColor.r>>ret[i].diffColor.g>>ret[i].diffColor.b>>ret[i].diffColor.a;
		ifs>>ret[i].specColor.r>>ret[i].specColor.g>>ret[i].specColor.b>>ret[i].shininess;
		ret[i].diffMap = readString( ifs );

		ret[i].bumpMap = readString( ifs );
		ifs>>temp;
		ret[i].specMap = readString( ifs );
		ret[i].ambOccMap = readString( ifs );

		ifs>>nVertices;
		ret[i].vertices.resize(nVertices);
		ret[i].normals.resize(nVertices);
		ret[i].texCoords.resize(nVertices);

		for( int j=0; j<nVertices; j++ )
			ifs>>ret[i].vertices[j].x>>ret[i].vertices[j].y>>ret[i].vertices[j].z;
		for( int j=0; j<nVertices; j++ )
			ifs>>ret[i].normals[j].x>>ret[i].normals[j].y>>ret[i].normals[j].z;
		for( int j=0; j<nVertices; j++ )
			ifs>>ret[i].texCoords[j].x>>ret[i].texCoords[j].y;
		
		ifs>>nTriangles;
		ret[i].indices.resize(nTriangles);
		for( int j=0; j<nTriangles; j++ ) {
			ifs>>ret[i].indices[j].x>>ret[i].indices[j].y>>ret[i].indices[j].z;
			if( ifs.eof() ) printf("EOF\n");
		}
		int xxx;
		ifs>>xxx;
		if( ifs.eof() ) printf("EOF\n");
	}
	ifs.close();
	return ret;

}


#endif /* j3a_h */
