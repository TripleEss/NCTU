#include <cmath>
#include <iostream>
#include "configFile.h"
#include "CMassSpringSystem.h"
#include "glut.h"

#pragma comment( lib, "glut32.lib" )

const double g_cdDeltaT = 0.001f;
const double g_cdK	   = 2500.0f;
const double g_cdD	   = 50.0f;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Constructor & Destructor
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMassSpringSystem::CMassSpringSystem():
	m_bDrawModel(true),
    m_bDrawStruct(false),
    m_bDrawShear(false),
    m_bDrawBending(false),
    m_bSimulation(false),

    m_iIntegratorType(EXPLICIT_EULER),
	m_iModelNum(1),
	m_iModelID(0),
	m_dModelSize(2),

    m_dDeltaT(g_cdDeltaT),
    m_dSpringCoefStruct(g_cdK),
    m_dSpringCoefShear(g_cdK),
    m_dSpringCoefBending(g_cdK),
    m_dDamperCoefStruct(g_cdD),
    m_dDamperCoefShear(g_cdD),
    m_dDamperCoefBending(g_cdD),
    m_dRotate(0.0f),

    m_Offset(Vector3d::ZERO),
    m_ForceField(Vector3d(0.0,-9.8,0.0)),

	m_PlaneNormal(Vector3d(0, 1, 0)),
	m_PlaneRefPoint(Vector3d(0, -1, 0))
{
	InitializeModels();
}

CMassSpringSystem::CMassSpringSystem(const std::string &a_rcsConfigFilename)
{
    int iIntegratorType;
    double dSpringCoef,dDamperCoef,dHeightOffset;
	double planeNormalX, planeNormalY, planeNormalZ;

    ConfigFile configFile;
    configFile.suppressWarnings(1);

	configFile.addOption("DrawModel", &m_bDrawModel);
    configFile.addOption("DrawSpringStructural",&m_bDrawStruct);
    configFile.addOption("DrawSpringShear"     ,&m_bDrawShear);
    configFile.addOption("DrawSpringBending"   ,&m_bDrawBending);
    configFile.addOption("SimulationStart"     ,&m_bSimulation);

    configFile.addOption("IntegratorType",&iIntegratorType);
	configFile.addOption("ModelNum", &m_iModelNum);
	configFile.addOption("ModelSize", &m_dModelSize);
    configFile.addOption("NumAtEdge", &m_iNumAtEdge);

    configFile.addOption("DeltaT",&m_dDeltaT);
    configFile.addOption("SpringCoef",&dSpringCoef);
    configFile.addOption("DamperCoef",&dDamperCoef);
    configFile.addOption("Rotate",&m_dRotate);
    configFile.addOption("HeightOffset",&dHeightOffset);
	
	configFile.addOption("PlaneNormalX", &planeNormalX);
	configFile.addOption("PlaneNormalY", &planeNormalY);
	configFile.addOption("PlaneNormalZ", &planeNormalZ);

    int code = configFile.parseOptions((char *)a_rcsConfigFilename.c_str());
    if(code == 1)
    {
        std::cout<<"Error in CMassSpringSystem constructor."<<std::endl;
        system("pause");
        exit(0);
    }
    m_iIntegratorType = CMassSpringSystem::EXPLICIT_EULER;
    if(iIntegratorType == 1)
    {
        m_iIntegratorType = CMassSpringSystem::RUNGE_KUTTA;
    }
	m_iModelID = 0;

    m_dSpringCoefStruct  = dSpringCoef;
    m_dSpringCoefShear   = dSpringCoef;
    m_dSpringCoefBending = dSpringCoef;
    m_dDamperCoefStruct  = dDamperCoef;
    m_dDamperCoefShear   = dDamperCoef;
    m_dDamperCoefBending = dDamperCoef;

    if(dHeightOffset<0.0 || dHeightOffset>10.0)
        dHeightOffset = 0.0;

    m_Offset       = Vector3d(0.0,dHeightOffset,0.0);
    m_ForceField   = Vector3d(0.0,-9.8,0.0);

	m_PlaneNormal = Vector3d(planeNormalX, planeNormalY, planeNormalZ);
	m_PlaneRefPoint = Vector3d(0.0, -1.0, 0.0);

	InitializeModels();
    Reset();
}

CMassSpringSystem::CMassSpringSystem(const CMassSpringSystem &a_rcMassSpringSystem):
	m_bDrawModel(a_rcMassSpringSystem.m_bDrawModel),
    m_bDrawStruct(a_rcMassSpringSystem.m_bDrawStruct),
    m_bDrawShear(a_rcMassSpringSystem.m_bDrawShear),
    m_bDrawBending(a_rcMassSpringSystem.m_bDrawBending),
    m_bSimulation(a_rcMassSpringSystem.m_bSimulation),

    m_iIntegratorType(a_rcMassSpringSystem.m_iIntegratorType),
	m_iModelID(a_rcMassSpringSystem.m_iModelID),
	m_iModelNum(a_rcMassSpringSystem.m_iModelNum),
	m_dModelSize(a_rcMassSpringSystem.m_dModelSize),

    m_dDeltaT(a_rcMassSpringSystem.m_dDeltaT),
    m_dSpringCoefStruct(a_rcMassSpringSystem.m_dSpringCoefStruct),
    m_dSpringCoefShear(a_rcMassSpringSystem.m_dSpringCoefShear),
    m_dSpringCoefBending(a_rcMassSpringSystem.m_dSpringCoefBending),
    m_dDamperCoefStruct(a_rcMassSpringSystem.m_dDamperCoefStruct),
    m_dDamperCoefShear(a_rcMassSpringSystem.m_dDamperCoefShear),
    m_dDamperCoefBending(a_rcMassSpringSystem.m_dDamperCoefBending),
    m_dRotate(a_rcMassSpringSystem.m_dRotate),

    m_Offset(a_rcMassSpringSystem.m_Offset),
    m_ForceField(a_rcMassSpringSystem.m_ForceField),

    m_Particles(a_rcMassSpringSystem.m_Particles),
    m_Springs(a_rcMassSpringSystem.m_Springs),

	m_PlaneNormal(a_rcMassSpringSystem.m_PlaneNormal),
	m_PlaneRefPoint(a_rcMassSpringSystem.m_PlaneRefPoint)
{

}

CMassSpringSystem::~CMassSpringSystem(){
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Draw
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMassSpringSystem::Draw(){	
	for (Pyramid& model : m_Models){
		if (m_bDrawModel)
			model.DrawObject();
		else
			model.DrawParticle();

		if (m_bDrawStruct)
			model.DrawSpring(CSpring::Type_nStruct);
		if (m_bDrawShear)
			model.DrawSpring(CSpring::Type_nShear);
		if (m_bDrawBending)
			model.DrawSpring(CSpring::Type_nBending);
	}
}

Vector3d CMassSpringSystem::CalcTriangleNormal(CParticle &p1,CParticle &p2,CParticle &p3)
{
    Vector3d pos1 = p1.GetPosition();
    Vector3d pos2 = p2.GetPosition();
    Vector3d pos3 = p3.GetPosition();

    Vector3d v1 = pos2-pos1;
    Vector3d v2 = pos3-pos1;

    return v1.CrossProduct(v2);
}

void CMassSpringSystem::DrawShadow(const Vector3d &a_rLightPos)
{
	for (Pyramid& model : m_Models){
		model.DrawShadow(a_rLightPos);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Set and Update
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMassSpringSystem::Reset()
{ 
	for (Pyramid& model : m_Models){
		model.Reset(m_Offset, m_dRotate);
	}
}

void CMassSpringSystem::SetSpringCoef(const double a_cdSpringCoef, const CSpring::enType_t a_cSpringType)
{
	if (a_cSpringType == CSpring::Type_nStruct)
		m_dSpringCoefStruct = a_cdSpringCoef;
	else if (a_cSpringType == CSpring::Type_nShear)
		m_dSpringCoefShear = a_cdSpringCoef;
	else if (a_cSpringType == CSpring::Type_nBending)
		m_dSpringCoefBending = a_cdSpringCoef;
	else
		std::cout << "Error spring type in CMassSpringSystme SetSpringCoef" << std::endl;

	for (Pyramid& model : m_Models)
		model.SetSpringCoef(a_cdSpringCoef, a_cSpringType);
}

void CMassSpringSystem::SetDamperCoef(const double a_cdDamperCoef, const CSpring::enType_t a_cSpringType)
{
	if (a_cSpringType == CSpring::Type_nStruct)
		m_dDamperCoefStruct = a_cdDamperCoef;
	else if (a_cSpringType == CSpring::Type_nShear)
		m_dDamperCoefShear = a_cdDamperCoef;
	else if (a_cSpringType == CSpring::Type_nBending)
		m_dDamperCoefBending = a_cdDamperCoef;
	else
		std::cout << "Error spring type in CMassSpringSystme SetSpringCoef" << std::endl;

	for (Pyramid& model : m_Models)
		model.SetDamperCoef(a_cdDamperCoef, a_cSpringType);
}

double CMassSpringSystem::GetSpringCoef(const CSpring::enType_t a_cSpringType)
{
    if(a_cSpringType == CSpring::Type_nStruct)
    {
        return m_dSpringCoefStruct;
    }
    else if(a_cSpringType == CSpring::Type_nShear)
    {
        return m_dSpringCoefShear;
    }
    else if(a_cSpringType == CSpring::Type_nBending)
    {
        return m_dSpringCoefBending;
    }
    else
    {
        std::cout<<"Error spring type in CMassSpringSystme GetSpringCoef"<<std::endl;
        return -1.0;
    }
}

double CMassSpringSystem::GetDamperCoef(const CSpring::enType_t a_cSpringType)
{
    if(a_cSpringType == CSpring::Type_nStruct)
    {
        return m_dDamperCoefStruct;
    }
    else if(a_cSpringType == CSpring::Type_nShear)
    {
        return m_dDamperCoefShear;
    }
    else if(a_cSpringType == CSpring::Type_nBending)
    {
        return m_dDamperCoefBending;
    }
    else
    {
        std::cout<<"Error spring type in CMassSpringSystme GetDamperCoef"<<std::endl;
        return -1.0;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Simulation Part
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CMassSpringSystem::CheckStable(){
	for (Pyramid& model : m_Models){
		if (model.GetParticle(0).GetPosition() < Vector3d(20, 20, 20) &&
			model.GetParticle(0).GetPosition() > Vector3d(-20, -20, -20) &&
			model.GetParticle(model.ParticleNum() - 1).GetPosition() < Vector3d(20, 20, 20) &&
			model.GetParticle(model.ParticleNum() - 1).GetPosition() > Vector3d(-20, -20, -20))
			return true;
	}
	return false;
}

void CMassSpringSystem::SimulationOneTimeStep()
{
    if(m_bSimulation)
    {
	    //ComputeAllForce();
        Integrate();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Initialization
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMassSpringSystem::InitializeModels(){
	double unitL = 1.5 * m_dModelSize;
	double unitH = 0.2 * m_dModelSize;

	auto initPosition = [&](int index){
		switch (index % 8){
			case 1:
				return Vector3d(unitL, index * unitH, 0);
			case 2:
				return Vector3d(unitL / sqrt(2), index * unitH, unitL / sqrt(2));
			case 3:
				return Vector3d(0, index * unitH, unitL);
			case 4:
				return Vector3d(-unitL / sqrt(2), index * unitH, unitL / sqrt(2));
			case 5:
				return Vector3d(-unitL, index * unitH, 0);
			case 6:
				return Vector3d(-unitL / sqrt(2), index * unitH, -unitL / sqrt(2));
			case 7:
				return Vector3d(0, index * unitH, -unitL);
			case 0:
				return Vector3d(unitL / sqrt(2), index * unitH, -unitL / sqrt(2));
		}
		return Vector3d();
	};

	if (m_iModelNum > 0){
		m_Models.push_back(Pyramid(
			Vector3d::ZERO,
			m_dModelSize,
			m_iNumAtEdge,
			m_dSpringCoefStruct,
			m_dDamperCoefStruct));
	}

	for (int i = 1; i < m_iModelNum; i++){
		m_Models.push_back(Pyramid(
			initPosition(i), 
			m_dModelSize, 
			m_iNumAtEdge, 
			m_dSpringCoefStruct, 
			m_dDamperCoefStruct));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Compute Force
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMassSpringSystem::ComputeAllForce()
{
	for (Pyramid& model : m_Models){
		model.ComputeForce(m_ForceField, m_dDeltaT, m_PlaneNormal, m_PlaneRefPoint);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Integrator
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMassSpringSystem::Integrate()
{
    if(m_iIntegratorType == CMassSpringSystem::EXPLICIT_EULER)
    {
        ComputeAllForce();
	    ExplicitEuler();
    }
    else if(m_iIntegratorType == CMassSpringSystem::RUNGE_KUTTA)
    {
        RungeKutta();
    }
    else
    {
        std::cout<<"Error integrator type, use explicit Euler instead!!"<<std::endl;
        ComputeAllForce();
        ExplicitEuler();
    }
}

void CMassSpringSystem::ExplicitEuler(){

	for (Pyramid& model : m_Models){
		for (CParticle& particle : model.GetParticles()){
			// t: time step
			// x' = x + t*v
			// v' = v + t*a
			particle.AddPosition(m_dDeltaT*particle.GetVelocity());
			particle.AddVelocity(m_dDeltaT*particle.GetAcceleration());
		}
	}
}

void CMassSpringSystem::RungeKutta(){

	struct StateStep{
		Vector3d deltaVel;
		Vector3d deltaPos;
	};

	for (Pyramid& model : m_Models){
		Pyramid& origin_model = model;
		Pyramid& final_model = model;
		
		// x' = x0 + (t/6)*(v0+2*v1+2*v2+v3)
		// v' = v0 + (t/6)*(a0+2*a1+2*a2+a3)
		ComputeAllForce();
		// x0, v0, a0
		// k0 = t*v0
		for (int i = 0; i < model.GetParticles().size(); ++i) {
			Vector3d v0 = model.GetParticle(i).GetVelocity();
			Vector3d a0 = model.GetParticle(i).GetAcceleration();
			final_model.GetParticle(i).AddPosition(m_dDeltaT / 6 * v0);
			final_model.GetParticle(i).AddVelocity(m_dDeltaT / 6 * a0);
			model.GetParticle(i).AddPosition(0.5*m_dDeltaT*v0);
			model.GetParticle(i).AddVelocity(0.5*m_dDeltaT*a0);
		}
		ComputeAllForce();
		// x1 = x0 + 0.5*t*v0
		// v1 = v0 + 0.5*t*a0
		// k1 = t*v1
		for (int i = 0; i < model.GetParticles().size(); ++i) {
			Vector3d v1 = model.GetParticle(i).GetVelocity();
			Vector3d a1 = model.GetParticle(i).GetAcceleration();
			final_model.GetParticle(i).AddPosition(m_dDeltaT / 3 * v1);
			final_model.GetParticle(i).AddVelocity(m_dDeltaT / 3 * a1);
			model.GetParticle(i) = origin_model.GetParticle(i);
			model.GetParticle(i).AddPosition(0.5*m_dDeltaT*v1);
			model.GetParticle(i).AddVelocity(0.5*m_dDeltaT*a1);
		}
		ComputeAllForce();
		// x2 = x0 + 0.5*t*v1
		// v2 = v0 + 0.5*t*a1
		// k2 = t*v2
		for (int i = 0; i < model.GetParticles().size(); ++i) {
			Vector3d v2 = model.GetParticle(i).GetVelocity();
			Vector3d a2 = model.GetParticle(i).GetAcceleration();
			final_model.GetParticle(i).AddPosition(m_dDeltaT / 3 * v2);
			final_model.GetParticle(i).AddVelocity(m_dDeltaT / 3 * a2);
			model.GetParticle(i) = origin_model.GetParticle(i);
			model.GetParticle(i).AddPosition(m_dDeltaT*v2);
			model.GetParticle(i).AddVelocity(m_dDeltaT*a2);
		}
		ComputeAllForce();
		// x3 = x0 + t*v2
		// v3 = v0 + t*a2
		// k3 = t*v3
		for (int i = 0; i < model.GetParticles().size(); ++i) {
			Vector3d v3 = model.GetParticle(i).GetVelocity();
			Vector3d a3 = model.GetParticle(i).GetAcceleration();
			final_model.GetParticle(i).AddPosition(m_dDeltaT / 6 * v3);
			final_model.GetParticle(i).AddVelocity(m_dDeltaT / 6 * a3);
		}
		
		model = final_model;
	}
}


