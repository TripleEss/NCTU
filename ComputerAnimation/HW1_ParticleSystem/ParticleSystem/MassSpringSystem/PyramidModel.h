#ifndef PYRAMIDMODEL_H
#define PYRAMIDMODEL_H

#include <vector>
#include "CParticle.h"
#include "CSpring.h"

using namespace std;

class Pyramid{
	void InitializeParticle();
	void InitializeSpring();

	void UpdateSpringCoef(const double a_cdSpringCoef, const CSpring::enType_t a_cSpringType);
	void UpdateDamperCoef(const double a_cdDamperCoef, const CSpring::enType_t a_cSpringType);

	Vector3d ComputeSpringForce(const Vector3d &a_crPos1, const Vector3d &a_crPos2,
		const double a_cdSpringCoef, const double a_cdRestLength);
	Vector3d ComputeDamperForce(const Vector3d &a_crPos1, const Vector3d &a_crPos2,
		const Vector3d &a_crVel1, const Vector3d &a_crVel2,
		const double a_cdDamperCoef);

	int GetLayer(int particleID);

	void AddForceField(const Vector3d &a_kForce);    //add gravity
	void ComputeInternalForce();
	void HandleCollision(const double deltaT, const Vector3d& planeNormal, const Vector3d& planeRefPoint);  //collision between particles and plane

	void DrawObjectBase();
	void DrawObjectSide(int face);
	void DrawShadowPolygon(const Vector3d &a_rLightPos, const Vector3d &a_rNormalVec,
		const Vector3d &a_rVerPos1, const Vector3d &a_rVerPos2);

public:
	Pyramid();
	Pyramid(
		const Vector3d &a_kInitPos,
		const double edgeLength,
		const int numAtEdge,
		const double dSpringCoef,
		const double dDamperCoef
		);
	Pyramid::Pyramid(
		const Vector3d& a_kInitPos,
		const double edgeLength,
		const int numAtEdge,
		const double dSpringCoefStruct,
		const double dSpringCoefShear,
		const double dSpringCoefBending,
		const double dDamperCoefStruct,
		const double dDamperCoefShear,
		const double dDamperCoefBending,
		const Vector3d& a_kColorStruct,
		const Vector3d& a_kColorShear,
		const Vector3d& a_kColorBending
		);
	Pyramid(const Pyramid& a_kPyramid);
	~Pyramid();

	int ParticleNum() const;  //return number of particles in the pyramid
	int SpringNum() const;    //return number of springs in the pyramid
	int GetNumAtEdge() const; //return number of particles at edge 
	int PointMapSide(const int face, const int layer, const int num);   //return index used to access particle at face
	int PointMapBase(const int i, const int j);

	CParticle& GetParticle(int particleIdx);    //get a particle in container according to index
	vector<CParticle>& GetParticles();
	CSpring& GetSpring(int springIdx);          //get a spring in container according to index

	void SetSpringCoef(const double a_cdSpringCoef, const CSpring::enType_t a_cSpringType);
	void SetDamperCoef(const double a_cdDamperCoef, const CSpring::enType_t a_cSpringType);

	void Reset(const Vector3d &a_kOffset, const double &a_kRotate);
	void ComputeForce(const Vector3d& forceField, const double deltaT,
		const Vector3d& planeNormal, const Vector3d& planeRefPoint);

	void DrawObject();
	void DrawParticle();
	void DrawSpring(const CSpring::enType_t springType);
	void DrawShadow(const Vector3d &lightPos);

	int pidAtItsLayer(int pid);
	void initSameLayerSpring(int pid);
	void initCrossLayerSpring(int pid);

private:
	enum faces {
		f_FRONT,
		f_RIGHT,
		f_BACK,
		f_LEFT,
		f_BASE,
		f_SIZE
	};

	double m_dSpringCoefStruct;    //spring coefficient
	double m_dSpringCoefShear;
	double m_dSpringCoefBending;
	double m_dDamperCoefStruct;
	double m_dDamperCoefShear;
	double m_dDamperCoefBending;
	Vector3d m_ColorStruct;     //spring color
	Vector3d m_ColorShear;
	Vector3d m_ColorBending;

	int m_NumAtEdge;		//number of particles at pyramid's edge
	double m_EdgeLength;
	Vector3d m_InitPos;

	vector<CParticle> m_Particles;
	vector<CSpring> m_Springs;

	vector<int> m_ParticleLayers;
	vector<vector<int> > m_ParticleFaces;
};

#endif