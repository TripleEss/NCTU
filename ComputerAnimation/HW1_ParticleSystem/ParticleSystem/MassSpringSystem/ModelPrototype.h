#ifndef MODELPROTOTYPE_H
#define MODELPROTOTYPE_H

#include <vector>
#include "CParticle.h"
#include "CSpring.h"

using namespace std;

class ModelPrototype {
protected:
	virtual void InitializeParticle();
	virtual void InitializeSpring();

	void UpdateSpringCoef(const double a_cdSpringCoef, const CSpring::enType_t a_cSpringType);
	void UpdateDamperCoef(const double a_cdDamperCoef, const CSpring::enType_t a_cSpringType);

	Vector3d ComputeSpringForce(const Vector3d &a_crPos1, const Vector3d &a_crPos2,
		const double a_cdSpringCoef, const double a_cdRestLength);
	Vector3d ComputeDamperForce(const Vector3d &a_crPos1, const Vector3d &a_crPos2,
		const Vector3d &a_crVel1, const Vector3d &a_crVel2, const double a_cdDamperCoef);

	double m_dSpringCoefStruct;    //spring coefficient
	double m_dSpringCoefShear;
	double m_dSpringCoefBending;
	double m_dDamperCoefStruct;
	double m_dDamperCoefShear;
	double m_dDamperCoefBending;

	Vector3d m_ColorStruct;     //spring color
	Vector3d m_ColorShear;
	Vector3d m_ColorBending;

	Vector3d m_InitPos;

	vector<CParticle> m_Particles;
	vector<CSpring> m_Springs;

public:
	ModelPrototype();
	ModelPrototype(const ModelPrototype& rhs);
	~ModelPrototype();

	int ParticleNum() const;
	int SpringNum() const;

	CParticle& GetParticle(int particleIdx);    //get a particle in container according to index
	CSpring& GetSpring(int springIdx);          //get a spring in container according to index

	void SetSpringCoef(const double a_cdSpringCoef, const CSpring::enType_t a_cSpringType);
	void SetDamperCoef(const double a_cdDamperCoef, const CSpring::enType_t a_cSpringType);

	virtual void Reset(const Vector3d &a_kOffset, const double &a_kRotate);

	void AddForceField(const Vector3d &a_kForce);    //add gravity
	void ComputeInternalForce();
	void HandleCollision(const double deltaT);  //collision between particles and plane

	virtual void DrawObject();
	virtual void DrawParticle();
};

#endif