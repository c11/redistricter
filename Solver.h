#ifndef SOLVER_H
#define SOLVER_H

#include "District2.h"

#include <stdio.h>
#include "LinearInterpolate.h"

class Adjacency;
class DistrictSet;
class GeoData;
class GrabIntermediateStorage;
class Node;
template<class T> class LastNMinMax;
class BinaryStatLogger;

// TODO separate application state and solution state!
class Solver {
public:
	int districts;
	
	double totalpop;
	double districtPopTarget;
	
	Node* nodes;
	int* allneigh;
	POPTYPE* winner;
	DistrictSet* (*districtSetFactory)(Solver*);
	DistrictSet* dists;
	
	const char* inputname;
	int generations;
	char* dumpname;
	char* loadname;
	enum initModeEnum {
		initWithNewDistricts = 1,
		initWithOldDistricts
	} initMode;
	
	char* solutionLogPrefix;
	int solutionLogInterval;
	int solutionLogCountdown;
	
#if WITH_PNG
	char* pngLogPrefix;
	int pngLogInterval;
	int pngLogCountdown;
#endif
	
	FILE* statLog;
	int statLogInterval;
	int statLogCountdown;
	BinaryStatLogger* pbStatLog;
	
	char* distfname;
	char* coordfname;
	
#if WITH_PNG
	char* pngname;
	int pngWidth, pngHeight;
#endif
	
	GeoData* gd;
	GeoData* (*geoFact)(const char*);
	
	uint32_t numEdges;
	int32_t* edgeData;
	
	//POPTYPE* sorti;
	
#define POS_TYPE int
	POS_TYPE minx, miny, maxx, maxy;
	double viewportRatio;
	
	int gencount;

	FILE* blaf;
	
	int runDutySeconds;
	int sleepDutySeconds;
	
	LastNMinMax<double>* recentKmpp;
	LastNMinMax<double>* recentSpread;
	int giveupSteps;
	double recentKmppGiveupFraction;
	double recentSpreadGiveupFraction;
	bool nonProgressGiveup() const;

	Solver();
	~Solver();
	
	int handleArgs( int argc, const char** argv );
	void load();
	// If filename is null, add ".links" to inputname
	void readLinksFile(const char* filename);
	bool readLinksFileData(const char* data, size_t len);
	void readLinksBin();
	/** Binary Format:
	[GeoData.h binary format for GeoData::writeBin(int,char*)
	uint32_t numEdges;
	int32_t edges[numEdges*2]; // pairs of indecies into GeoData
	*/
	int writeBin( const char* fname );
#if HAVE_PROTOBUF
	int writeProtobuf( const char* fname );
#endif
	void initNodes();
	void allocSolution();
	int saveZSolution( const char* filename );
	int loadZSolution( const char* filename );
	int loadCsvSolution( const char* filename );
	void initSolution();
	void initSolutionFromOldCDs();
	void init();
	static const char* getSetFactoryName(int index);
	void setFactoryByIndex(int index);
	int megaInit();
	int step();
#if WITH_PNG
	void doPNG();
	void doPNG(POPTYPE* soln, const char* outname);
	//void doPNG_r( unsigned char* data, unsigned char** rows, int pngWidth, int pngHeight, const char* pngname );
#endif /* WITH_PNG */

	void printDistricts(const char* filename);
	void printDistricts() {
		printDistricts(distfname);
	}
	/* to dumpname */
	int saveSolution();
	
	/* draw stuff */
	double dcx, dcy;
	double zoom;
	int showLinks;

	POPTYPE* renumber;

	void calculateAdjacency(Adjacency*);
	static void calculateAdjacency_r(Adjacency* it, int numPoints, int districts,
									 const POPTYPE* winner, const Node* nodes);
	void californiaRenumber();
	void nullRenumber();

	LinearInterpolate popRatioFactor;

	void* _point_draw_array;
	//void* _link_draw_array;
	int lastGenDrawn;
	unsigned long vertexBufferObject, colorBufferObject, linesBufferObject, vertexIndexObject;
#ifndef DRAW_HISTORY_LEN
#define DRAW_HISTORY_LEN 10
#endif
	int64_t usecDrawTimeHistory[DRAW_HISTORY_LEN];
	double fps;
	int drawHistoryPos;
	void recordDrawTime();
	
	void initGL();
	void drawGL();
	void nudgeLeft();
	void nudgeRight();
	void nudgeUp();
	void nudgeDown();
	void zoomIn();
	void zoomOut();
	void zoomAll();
	void centerOnPoint( int index );
	int main( int argc, const char** argv );
		
	inline int setDist( POPTYPE d, int i ) {
		return (*dists)[(d)].add( this, (i), (d) );
	}
	
	int debugDistrictNumber;

	double maxSpreadFraction;
	double maxSpreadAbsolute;
	
	SolverStats* getDistrictStats();
	int getDistrictStats( char* str, int len );
	
	static const char* argHelp;
};

class SolverStats {
public:
	int generation;
	double avgPopDistToCenterOfDistKm;
	double poptotal;
	double popavg;
	double popstd;
	double popmin;
	double popmax;
	double popmed;
	int mindist;
	int maxdist;
	int meddist;
	int nod;
	double nodpop;
	SolverStats* next;

	SolverStats();
	SolverStats( int geni, double pd, double pa, double ps, double pmi, double pma, double pme,
		int mid, int mad, int med, int noDist, double noDistPop, SolverStats* n = NULL );

	int toString( char* str, int len );
};

// describe which districts border which other districts for coloring purposes
class Adjacency {
public:
	// pairs of district indecies which border each other.
	POPTYPE* adjacency;
	// number of pairs in list
	int adjlen;
	// size of allocation in nuber of pairs
	int adjcap;
	
	Adjacency();
	~Adjacency();
};

DistrictSet* NearestNeighborDistrictSetFactory(Solver* sov);
DistrictSet* District2SetFactory(Solver* sov);

int parseArgvFromFile(const char* filename, char*** argvP);

#endif /* SOLVER_H */
