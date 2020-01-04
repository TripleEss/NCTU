#ifndef MASS_SPRING_SYSTEM_H
#define MASS_SPRING_SYSTEM_H

#include <vector>
#include <string>
#include "Vector3d.h"
#include "CParticle.h"
#include "CSpring.h"
#include "PyramidModel.h"

using std::vector;

class CMassSpringSystem
{
    public:
        enum
        {
            EXPLICIT_EULER = 0 ,
            RUNGE_KUTTA
        };
    private:
		bool m_bDrawModel;		//visibility of models
        bool m_bDrawCloth;
        bool m_bDrawStruct;      //struct stands for structural
        bool m_bDrawShear;
        bool m_bDrawBending;
        bool m_bSimulation;      //start or pause

        int m_iIntegratorType;
		int m_iModelNum;
        int m_iNumAtEdge;    
		int m_iModelID;

        double m_dDeltaT;            //delta t    
        double m_dSpringCoefStruct;
        double m_dSpringCoefShear;
        double m_dSpringCoefBending;
        double m_dDamperCoefStruct;
        double m_dDamperCoefShear;
        double m_dDamperCoefBending;
        double m_dRotate;
		double m_dModelSize;

        Vector3d m_Offset;
        Vector3d m_ForceField;      //external force field

		Vector3d m_PlaneNormal;
		Vector3d m_PlaneRefPoint;

        vector<CParticle> m_Particles;
        vector<CSpring> m_Springs;
		vector<Pyramid> m_Models;

		void InitializeModels();

		void ComputeAllForce();    //compute force of whole systems

        void Integrate();
        void ExplicitEuler();
		void RungeKutta();

        void DrawTriangle(CParticle &p1, CParticle &p2, CParticle &p3, const Vector3d color);

        Vector3d CalcTriangleNormal(CParticle &p1, CParticle &p2, CParticle &p3);

    public:
        CMassSpringSystem();
        CMassSpringSystem(const std::string &a_rcsConfigFilename);
        CMassSpringSystem(const CMassSpringSystem &a_rcMassSpringSystem);
        ~CMassSpringSystem();
        void SimulationOneTimeStep();
        void Draw();
        void DrawShadow(const Vector3d &a_rLightPos);
        void DrawShadowPolygon(const Vector3d &a_rLightPos,const Vector3d &a_rNormalVec,
                               const Vector3d &a_rVerPos1,const Vector3d &a_rVerPos2);
        void Reset();
        void SetSpringCoef(const double a_cdSpringCoef, const CSpring::enType_t a_cSpringType);
        void SetDamperCoef(const double a_cdDamperCoef, const CSpring::enType_t a_cSpringType);
        double GetSpringCoef(const CSpring::enType_t a_cSpringType);
        double GetDamperCoef(const CSpring::enType_t a_cSpringType);

        bool CheckStable();

		inline void SetModelID(const int a_iModelID){ m_iModelID = a_iModelID; }
		inline void SetRotate(const double a_cdRotate){ m_dRotate = a_cdRotate; }
		inline void SetOffset(const Vector3d &a_rOffset){ m_Offset = a_rOffset; }
		inline void SetDrawModel(const bool a_bDrawModel){ m_bDrawModel = a_bDrawModel; }
		inline void SetDrawStruct(const bool a_bDrawStruct){ m_bDrawStruct = a_bDrawStruct; }
		inline void SetDrawShear(const bool a_bDrawShear){ m_bDrawShear = a_bDrawShear; }
		inline void SetDrawBending(const bool a_bDrawBending){ m_bDrawBending = a_bDrawBending; }
		inline void SetDeltaT(const double a_cdDeltaT){ m_dDeltaT = a_cdDeltaT; }
		inline void SetIntegratorType(const int a_ciIntegratorType){ m_iIntegratorType = a_ciIntegratorType; }
		inline void SetStartSimulation(){ m_bSimulation = true; }
		inline void SetPauseSimulation(){ m_bSimulation = false; }
		inline bool IsSimulation(){ return m_bSimulation; }
		inline int GetIntegratorType(){ return m_iIntegratorType; }
		inline double GetDeltaT(){ return m_dDeltaT; }
		inline Vector3d GetPlaneNormal() const { return m_PlaneNormal; }
		inline Vector3d GetPlaneRefPoint() const { return m_PlaneRefPoint; }
};

#endif