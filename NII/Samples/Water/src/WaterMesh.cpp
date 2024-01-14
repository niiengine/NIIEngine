/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/


#include "WaterMesh.h"

#define ANIMATIONS_PER_SECOND 100.0f

WaterMesh::WaterMesh(const String& inMeshName, NIIf planeSize, int inComplexity)
{
	int x,y,b; // I prefer to initialize for() variables inside it, but VC doesn't like it ;(

	this->meshName = inMeshName ;
	this->complexity = inComplexity ;
	numFaces = 2 * complexity * complexity;
	numVertices = (complexity + 1) * (complexity + 1) ;
	lastTimeStamp = 0 ;
	lastAnimationTimeStamp = 0;
	lastFrameTime = 0 ;

	// initialize algorithm parameters
	PARAM_C = 0.3f ; // ripple speed
	PARAM_D = 0.4f ; // distance
	PARAM_U = 0.05f ; // viscosity
	PARAM_T = 0.13f ; // time
	useFakeNormals = false ;

	// allocate space for normal calculation
	vNormals = new Vector3[numVertices];

	// create mesh and submesh
	mesh = N_Only(Mesh).createManual(meshName, GroupDefault);
	subMesh = mesh->createSub();
	subMesh->setSelfVertex(true);

	// Vertex buffers
    VertexData * smvd;
    N_Only(GBuffer).create(smvd);
    subMesh->setVertexData(smvd);
	smvd->mOffset = 0;
	smvd->mCount = numVertices;

	VertexData * vd = smvd;

	vd->add(DT_Float3x, VT_Pos, 0, 0);
	vd->add(DT_Float3x, VT_Normals, 1, 0);
	vd->add(DT_Float2x, VT_Tex_Coord, 2, 0);

	// Prepare buffer for positions - todo: first attempt, slow
    N_Only(GBuffer).create(posVertexBuffer, 3 * sizeof(float),
        numVertices, Buffer::M_WRITE | Buffer::M_WHOLE);
	vd->attach(0, posVertexBuffer);

	// Prepare buffer for normals - write only
    N_Only(GBuffer).create(normVertexBuffer, 3 * sizeof(float),
        numVertices, Buffer::M_WRITE | Buffer::M_WHOLE);
	vd->attach(1, normVertexBuffer);

	// Prepare texture coords buffer - static one
	// todo: optimize to write directly into buffer
	float * texcoordsBufData = new float[numVertices * 2];
	for(y = 0;y <= complexity; y++)
    {
		for(x = 0;x <= complexity; x++)
        {
			texcoordsBufData[2*(y*(complexity+1)+x)+0] = (float)x / complexity ;
			texcoordsBufData[2*(y*(complexity+1)+x)+1] = 1.0f - ((float)y / (complexity)) ;
		}
	}
    N_Only(GBuffer).create(texcoordsVertexBuffer, 2 * sizeof(float),
        numVertices, Buffer::Buffer::M_DEV | Buffer::M_WRITE);
	texcoordsVertexBuffer->write(texcoordsBufData, 0, texcoordsVertexBuffer->getSize());
	delete [] texcoordsBufData;
    vd->attach(2, texcoordsVertexBuffer);

	// Prepare buffer for indices
    N_Only(GBuffer).create(indexBuffer, 16, 3 * numFaces, Buffer::Buffer::M_DEV | Buffer::M_HOST);
	unsigned short * faceVertexIndices = (unsigned short *)
		indexBuffer->lock(0, numFaces * 3 * 2, Buffer::MM_WHOLE);
	for(y = 0; y < complexity; y++)
    {
		for(x = 0; x < complexity; x++)
        {
			unsigned short * twoface = faceVertexIndices + (y * complexity + x) * 2 * 3;
			int p0 = y * (complexity + 1) + x ;
			int p1 = y * (complexity + 1) + x + 1 ;
			int p2 = (y+1)*(complexity+1) + x ;
			int p3 = (y+1)*(complexity+1) + x + 1 ;
			twoface[0]=p2; //first tri
			twoface[1]=p1;
			twoface[2]=p0;
			twoface[3]=p2; //second tri
			twoface[4]=p3;
			twoface[5]=p1;
		}
	}
	indexBuffer->unlock();
	// Set index buffer for this submesh
	subMesh->getIndexData(0)->mBuffer = indexBuffer;
	subMesh->getIndexData(0)->mOffset = 0;
	subMesh->getIndexData(0)->mCount = 3 * numFaces;

	/*	prepare vertex positions
	 *	note - we use 3 vertex buffers, since algorighm uses two last phases
	 *	to calculate the next one
	 */
	for(b=0;b<3;b++) {
		vertexBuffers[b] = new float[numVertices * 3] ;
		for(y=0;y<=complexity;y++) {
			for(x=0;x<=complexity;x++) {
				int numPoint = y*(complexity+1) + x ;
				float* vertex = vertexBuffers[b] + 3*numPoint ;
				vertex[0]=(float)(x) / (float)(complexity) * (float) planeSize ;
				vertex[1]= 0 ; // rand() % 30 ;
				vertex[2]=(float)(y) / (float)(complexity) * (float) planeSize ;
			}
		}
	}

	AABox meshBounds(0, 0, 0, planeSize, 0, planeSize);
	mesh->setAABB(meshBounds);
    mesh->setRangeFactor(0.01);

	currentBuffNumber = 0 ;
	posVertexBuffer->write(vertexBuffers[currentBuffNumber], 0, posVertexBuffer->getSize());

    mesh->load();
    mesh->ref();
}
/* ========================================================================= */
WaterMesh::~WaterMesh ()
{
 	delete[] vertexBuffers[0];
 	delete[] vertexBuffers[1];
 	delete[] vertexBuffers[2];

	delete[] vNormals;

	MeshManager::getSingleton().remove(meshName);
}
/* ========================================================================= */
void WaterMesh::push(NIIf x, NIIf y, NIIf depth, bool absolute)
{
	float *buf = vertexBuffers[currentBuffNumber]+1 ;
	// scale pressure according to time passed
	depth = depth * lastFrameTime * ANIMATIONS_PER_SECOND ;
#define _PREP(addx,addy) { \
	float *vertex=buf+3*((int)(y+addy)*(complexity+1)+(int)(x+addx)) ; \
	float diffy = y - floor(y+addy); \
	float diffx = x - floor(x+addx); \
	float dist=sqrt(diffy*diffy + diffx*diffx) ; \
	float power = 1 - dist ; \
	if (power<0)  \
		power = 0; \
	if (absolute) \
		*vertex = depth*power ;  \
	else \
		*vertex += depth*power ;  \
} /* #define */
	_PREP(0,0);
	_PREP(0,1);
	_PREP(1,0);
	_PREP(1,1);
#undef _PREP
}
/* ========================================================================= */
NIIf WaterMesh::getHeight(NIIf x, NIIf y)
{
#define hat(_x,_y) buf[3*((int)_y*(complexity+1)+(int)(_x))]
	float *buf = vertexBuffers[currentBuffNumber] ;
	NIIf xa = floor(x);
	NIIf xb = xa + 1 ;
	NIIf ya = floor(y);
	NIIf yb = ya + 1 ;
	NIIf yaxavg = hat(xa,ya) * (1.0f-fabs(xa-x)) + hat(xb,ya) * (1.0f-fabs(xb-x));
	NIIf ybxavg = hat(xa,yb) * (1.0f-fabs(xa-x)) + hat(xb,yb) * (1.0f-fabs(xb-x));
	NIIf yavg = yaxavg * (1.0f-fabs(ya-y)) + ybxavg * (1.0f-fabs(yb-y)) ;
	return yavg ;
}
/* ========================================================================= */
void WaterMesh::calculateFakeNormals()
{
	int x,y;
	float *buf = vertexBuffers[currentBuffNumber] + 1;
	float *pNormals = (float*) normVertexBuffer->lock(
		0,normVertexBuffer->getSize(), Buffer::MM_WHOLE);
	for(y=1;y<complexity;y++) {
		float *nrow = pNormals + 3*y*(complexity+1);
		float *row = buf + 3*y*(complexity+1) ;
		float *rowup = buf + 3*(y-1)*(complexity+1) ;
		float *rowdown = buf + 3*(y+1)*(complexity+1) ;
		for(x=1;x<complexity;x++) {
			NIIf xdiff = row[3*x+3] - row[3*x-3] ;
			NIIf ydiff = rowup[3*x] - rowdown[3*x-3] ;
			Vector3 norm(xdiff,30,ydiff);
			norm.normalise();
			nrow[3*x+0] = norm.x;
			nrow[3*x+1] = norm.y;
			nrow[3*x+2] = norm.z;
		}
	}
	normVertexBuffer->unlock();
}
/* ========================================================================= */
void WaterMesh::calculateNormals()
{
	int i,x,y;
	float * buf = NULL;
	// zero normals
	for(i = 0; i < numVertices; i++) 
    {
		vNormals[i] = Vector3::ZERO;
	}
	// first, calculate normals for faces, add them to proper vertices
	buf = vertexBuffers[currentBuffNumber] ;
	unsigned short * vinds = (unsigned short *)indexBuffer->lock(
		0, indexBuffer->getSize(), Buffer::MM_READ);
	float * pNormals = (float *)normVertexBuffer->lock(
		0, normVertexBuffer->getSize(), Buffer::MM_WHOLE);
	for(i = 0; i < numFaces; i++) 
    {
		int p0 = vinds[3*i] ;
		int p1 = vinds[3*i+1] ;
		int p2 = vinds[3*i+2] ;
		Vector3 v0(buf[3*p0], buf[3*p0+1], buf[3*p0+2]);
		Vector3 v1(buf[3*p1], buf[3*p1+1], buf[3*p1+2]);
		Vector3 v2(buf[3*p2], buf[3*p2+1], buf[3*p2+2]);
		Vector3 diff1 = v2 - v1 ;
		Vector3 diff2 = v0 - v1 ;
		Vector3 fn = diff1.crossProduct(diff2);
		vNormals[p0] += fn ;
		vNormals[p1] += fn ;
		vNormals[p2] += fn ;
	}
	// now normalize vertex normals
	for(y = 0; y <= complexity; y++) 
    {
		for(x = 0; x <= complexity; x++) 
        {
			int numPoint = y*(complexity+1) + x ;
			Vector3 n = vNormals[numPoint] ;
			n.normalise() ;
			float * normal = pNormals + 3*numPoint ;
			normal[0]=n.x;
			normal[1]=n.y;
			normal[2]=n.z;
		}
	}
	indexBuffer->unlock();
	normVertexBuffer->unlock();
}
/* ========================================================================= */
void WaterMesh::updateMesh(NIIf timeSinceLastFrame)
{
	int x, y ;

	lastFrameTime = timeSinceLastFrame ;
	lastTimeStamp += timeSinceLastFrame ;

	// do rendering to get ANIMATIONS_PER_SECOND
	while(lastAnimationTimeStamp <= lastTimeStamp) {

		// switch buffer numbers
		currentBuffNumber = (currentBuffNumber + 1) % 3 ;
		float *buf = vertexBuffers[currentBuffNumber] + 1 ; // +1 for Y coordinate
		float *buf1 = vertexBuffers[(currentBuffNumber+2)%3] + 1 ;
		float *buf2 = vertexBuffers[(currentBuffNumber+1)%3] + 1;

		/* we use an algorithm from
		 * http://collective.valve-erc.com/index.php?go=water_simulation
		 * The params could be dynamically changed every frame ofcourse
		 */
		double C = PARAM_C; // ripple speed
		double D = PARAM_D; // distance
		double U = PARAM_U; // viscosity
		double T = PARAM_T; // time
		NIIf TERM1 = ( 4.0f - 8.0f*C*C*T*T/(D*D) ) / (U*T+2) ;
		NIIf TERM2 = ( U*T-2.0f ) / (U*T+2.0f) ;
		NIIf TERM3 = ( 2.0f * C*C*T*T/(D*D) ) / (U*T+2) ;
		for(y=1;y<complexity;y++) { // don't do anything with border values
			float *row = buf + 3*y*(complexity+1) ;
			float *row1 = buf1 + 3*y*(complexity+1) ;
			float *row1up = buf1 + 3*(y-1)*(complexity+1) ;
			float *row1down = buf1 + 3*(y+1)*(complexity+1) ;
			float *row2 = buf2 + 3*y*(complexity+1) ;
			for(x=1;x<complexity;x++) {
				row[3*x] = TERM1 * row1[3*x]
					+ TERM2 * row2[3*x]
					+ TERM3 * ( row1[3*x-3] + row1[3*x+3] + row1up[3*x]+row1down[3*x] ) ;
			}
		}

		lastAnimationTimeStamp += (1.0f / ANIMATIONS_PER_SECOND);
	}

	if (useFakeNormals) {
		calculateFakeNormals();
	} else {
		calculateNormals();
	}

	// set vertex buffer
	posVertexBuffer->write(vertexBuffers[currentBuffNumber], 0, posVertexBuffer->getSize());
}
