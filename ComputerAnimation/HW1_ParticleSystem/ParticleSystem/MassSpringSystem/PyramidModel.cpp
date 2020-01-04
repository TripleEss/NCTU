#include "PyramidModel.h"
#include "glut.h"
#include <iostream>
#include <algorithm>
using namespace std;

const double g_cdK = 2500.0f;
const double g_cdD = 50.0f;

const Vector3d defaultSructColor(0.0, 1.0, 1.0);
const Vector3d defaultSheerColor(1.0, 1.0, 0.0);
const Vector3d defaultBendingColor(0.0, 1.0, 0.0);

Pyramid::Pyramid() :
	Pyramid(Vector3d::ZERO, 2.0, 10, g_cdK, g_cdD)
{
}

Pyramid::Pyramid(
	const Vector3d& a_kInitPos,
	const double edgeLength,
	const int numAtEdge,
	const double dSpringCoef,
	const double dDamperCoef
) : Pyramid(
	a_kInitPos,
	edgeLength,
	numAtEdge,
	dSpringCoef,
	dSpringCoef,
	dSpringCoef,
	dDamperCoef,
	dDamperCoef,
	dDamperCoef,
	defaultSructColor,
	defaultSheerColor,
	defaultBendingColor)
{
}

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
	const Vector3d& a_kColorBending) :
	m_NumAtEdge(numAtEdge),
	m_EdgeLength(edgeLength),
	m_InitPos(a_kInitPos),
	m_dSpringCoefStruct(dSpringCoefStruct),
	m_dSpringCoefShear(dSpringCoefShear),
	m_dSpringCoefBending(dSpringCoefBending),
	m_dDamperCoefStruct(dDamperCoefStruct),
	m_dDamperCoefShear(dDamperCoefShear),
	m_dDamperCoefBending(dDamperCoefBending),
	m_ColorStruct(a_kColorStruct),
	m_ColorShear(a_kColorShear),
	m_ColorBending(a_kColorBending)
{
	InitializeParticle();
	InitializeSpring();
}

Pyramid::Pyramid(const Pyramid &a_kPyramid) :
	m_NumAtEdge(a_kPyramid.m_NumAtEdge),
	m_EdgeLength(a_kPyramid.m_EdgeLength),
	m_InitPos(a_kPyramid.m_InitPos),
	m_dSpringCoefStruct(a_kPyramid.m_dSpringCoefStruct),
	m_dSpringCoefShear(a_kPyramid.m_dSpringCoefShear),
	m_dSpringCoefBending(a_kPyramid.m_dSpringCoefBending),
	m_dDamperCoefStruct(a_kPyramid.m_dDamperCoefStruct),
	m_dDamperCoefShear(a_kPyramid.m_dDamperCoefShear),
	m_dDamperCoefBending(a_kPyramid.m_dDamperCoefBending),
	m_ColorStruct(a_kPyramid.m_ColorStruct),
	m_ColorShear(a_kPyramid.m_ColorShear),
	m_ColorBending(a_kPyramid.m_ColorBending)
{
	InitializeParticle();
	InitializeSpring();
}

Pyramid::~Pyramid()
{
}

int Pyramid::ParticleNum() const
{
	return m_Particles.size();
}

int Pyramid::SpringNum() const
{
	return m_Springs.size();
}

int Pyramid::GetNumAtEdge() const
{
	return m_NumAtEdge;
}

int Pyramid::PointMapSide(const int face, const int layer, const int num)
{
	int pID = 0;
	for (int l = 1; l < layer; l++) {
		for (int n = 0; n < l; n++) {
			pID++;
		}
	}
	pID += num;

	return m_ParticleFaces[face][pID];
}

int Pyramid::PointMapBase(const int i, const int j) {
	return m_ParticleFaces[f_BASE][i * m_NumAtEdge + j];
}

CParticle& Pyramid::GetParticle(int particleIdx)
{
	return m_Particles[particleIdx];
}

vector<CParticle>& Pyramid::GetParticles() {
	return m_Particles;
}

CSpring& Pyramid::GetSpring(int springIdx)
{
	return m_Springs[springIdx];
}

void Pyramid::SetSpringCoef(const double a_cdSpringCoef, const CSpring::enType_t a_cSpringType)
{
	if (a_cSpringType == CSpring::Type_nStruct)
		m_dSpringCoefStruct = a_cdSpringCoef;
	else if (a_cSpringType == CSpring::Type_nShear)
		m_dSpringCoefShear = a_cdSpringCoef;
	else if (a_cSpringType == CSpring::Type_nBending)
		m_dSpringCoefBending = a_cdSpringCoef;
	else
		return;

	UpdateSpringCoef(a_cdSpringCoef, a_cSpringType);
}

void Pyramid::SetDamperCoef(const double a_cdDamperCoef, const CSpring::enType_t a_cSpringType)
{
	if (a_cSpringType == CSpring::Type_nStruct)
		m_dDamperCoefStruct = a_cdDamperCoef;
	else if (a_cSpringType == CSpring::Type_nShear)
		m_dDamperCoefShear = a_cdDamperCoef;
	else if (a_cSpringType == CSpring::Type_nBending)
		m_dDamperCoefBending = a_cdDamperCoef;
	else
		return;

	UpdateDamperCoef(a_cdDamperCoef, a_cSpringType);
}

void Pyramid::Reset(const Vector3d &a_kOffset, const double &a_kRotate)
{
	// new vectors
	vector<CParticle> particles;
	vector<int> particleLayers;
	vector<vector<int> > particleFaces(f_SIZE);

	const double dPI = 3.1415926f;
	double dTheta = a_kRotate * dPI / 180.0f;   //change angle from degree to radian

												// 'unitLength' is half of unit pyramid's edge length
	double unitLength = (m_EdgeLength / (m_NumAtEdge - 1.0)) * (1.0 / 2.0);
	double unitHeight = sqrt(2) * unitLength;

	for (int layer = 1; layer <= m_NumAtEdge; layer++) {

		double layerStartPosX = -unitLength * (layer - 1);
		double layerStartPosY = unitHeight * (layer - 1);
		double layerStartPosZ = -unitLength * (layer - 1);
		//cout << "(" << layerStartPosX << "," << layerStartPosY << "," << layerStartPosZ << ")" << endl;

		for (int i = 0; i < layer; i++) {
			for (int j = 0; j < layer; j++) {
				double offsetX = layerStartPosX + j * unitLength * 2;
				double offsetY = layerStartPosY;
				double offsetZ = layerStartPosZ + i * unitLength * 2;
				//cout << "(" << offsetX << "," << offsetY << "," << offsetZ << ")" << endl;

				Vector3d RotateVec(offsetX, offsetY, offsetZ);
				RotateVec.Rotate(dTheta, Vector3d(1.0f, 0.0f, 1.0f));

				CParticle Particle;
				Particle.SetPosition(m_InitPos + a_kOffset + RotateVec);
				Particle.SetForce(Vector3d::ZERO);
				Particle.SetVelocity(Vector3d::ZERO);

				particles.push_back(Particle);
				particleLayers.push_back(layer);

				int particleID = particles.size() - 1;

				if (i == 0)
					particleFaces[f_FRONT].push_back(particleID);
				if (j == 0)
					particleFaces[f_LEFT].push_back(particleID);
				if (i == layer - 1)
					particleFaces[f_BACK].push_back(particleID);
				if (j == layer - 1)
					particleFaces[f_RIGHT].push_back(particleID);
				if (layer == m_NumAtEdge)
					particleFaces[f_BASE].push_back(particleID);
			}
		}
	}

	m_Particles = particles;
	m_ParticleLayers = particleLayers;
	m_ParticleFaces = particleFaces;
}

void Pyramid::ComputeForce(const Vector3d& forceField, const double deltaT,
	const Vector3d& planeNormal, const Vector3d& planeRefPoint) {
	AddForceField(forceField);
	ComputeInternalForce();
	HandleCollision(deltaT, planeNormal, planeRefPoint);
}

void Pyramid::AddForceField(const Vector3d &a_kForce)
{
	for (unsigned int uiI = 0; uiI<m_Particles.size(); uiI++)
	{
		m_Particles[uiI].SetAcceleration(a_kForce);
	}
}

void Pyramid::ComputeInternalForce() {

	for (CSpring& spring : m_Springs) {
		
		// get the index, position, velocity of two ends of spring
		int pid_start = spring.GetSpringStartID();
		int pid_end = spring.GetSpringEndID();
		Vector3d pos_start = m_Particles[pid_start].GetPosition();
		Vector3d pos_end = m_Particles[pid_end].GetPosition();
		Vector3d vel_start = m_Particles[pid_start].GetVelocity();
		Vector3d vel_end = m_Particles[pid_end].GetVelocity();

		// compute spring force and damper force on spring
		Vector3d spring_force = ComputeSpringForce(pos_start, pos_end, spring.GetSpringCoef(), spring.GetSpringRestLength());
		Vector3d damper_force = ComputeDamperForce(pos_start, pos_end, vel_start, vel_end, spring.GetDamperCoef());
		
		// add spring force and damper force to two ends of spring
		m_Particles[pid_start].AddForce(spring_force + damper_force);
		m_Particles[pid_end].AddForce(-1 * (spring_force + damper_force));	
	}
}

Vector3d Pyramid::ComputeSpringForce(const Vector3d &a_crPos1, const Vector3d &a_crPos2,
	const double a_cdSpringCoef, const double a_cdRestLength) {

	Vector3d diffPos = a_crPos1 - a_crPos2;
	Vector3d norm_diffPos = diffPos.NormalizedCopy();

	// ks: coef of spring force
	// xa, xb: the position of two ends of spring
	// r: rest length of spring
	// spring force = -1 * ks * (|xa-xb|-r) * [(xa-xb)/|xa-xb|]
	Vector3d spring_force = -1 * a_cdSpringCoef * (diffPos.Length() - a_cdRestLength) * norm_diffPos;
	return spring_force;
}

Vector3d Pyramid::ComputeDamperForce(const Vector3d &a_crPos1, const Vector3d &a_crPos2,
	const Vector3d &a_crVel1, const Vector3d &a_crVel2, const double a_cdDamperCoef) {
	
	Vector3d diffPos = a_crPos1 - a_crPos2;
	Vector3d norm_diffPos = diffPos.NormalizedCopy();

	Vector3d diffVel = a_crVel1 - a_crVel2;

	// kd: coef of damper force
	// xa, xb: the position of two ends of spring
	// va, vb: the velocity of two ends of spring
	// damper force = -1 * kd * {[(va-vb).(xa-ab)]/|xa-ab|} * [(xa-ab)/|xa-xb|]
	Vector3d damper_force = -1 * a_cdDamperCoef * diffVel.DotProduct(norm_diffPos) * norm_diffPos;
	return damper_force;
}

void Pyramid::HandleCollision(const double delta_T, const Vector3d& planeNormal, const Vector3d& planeRefPoint) {
	
	static const double EPSILON = 0.01;
	double coefResist = 0.8;					// coefficient of restitution
	double coefFriction = 0.3;					// coefficient of friction

	Vector3d norm_planeNormal = planeNormal.NormalizedCopy();

	for (CParticle& particle : m_Particles) {
		Vector3d vn;
		Vector3d vt;
		
		// N: nomalized normal vector of plane
		// p: position of reference point on plane
		// x: position of particle
		// v: velocity of particle, vn: normal component of v, vt: tangential component of v
		// f: force of particle
		// kr: coef of restitution
		// kf: coef of friction

		// if particle is 1. close to plane: N.(x-p) < EPSILON
		//            and 2. heading in plane: N.v < 0
		// then doing collision response v' = -1*kr*vn + vt
		if ((norm_planeNormal.DotProduct(particle.GetPosition() - planeRefPoint) < EPSILON)
			&& (norm_planeNormal.DotProduct(particle.GetVelocity()) < 0)) {
			Vector3d vn = norm_planeNormal.DotProduct(particle.GetVelocity())*norm_planeNormal;
			Vector3d vt = particle.GetVelocity() - vn;

			particle.SetVelocity(-1 * coefResist*vn + vt);
		}

		// if particle is 1. close to plane: N.(x-p) < EPSILON
		//            and 2. pused into plane: N.f < 0
		// then there is contact force = -1*(N.f)*N on particle
		Vector3d contact_force = Vector3d(0, 0, 0);
		Vector3d friction_force = Vector3d(0, 0, 0);
		if ((norm_planeNormal.DotProduct(particle.GetPosition() - planeRefPoint) < EPSILON)
			&& (norm_planeNormal.DotProduct(particle.GetForce()) < 0)) {

			contact_force = -1 * norm_planeNormal.DotProduct(particle.GetForce())*norm_planeNormal;

			// if particle is 1. and 2.
			//            and 3. moving alone plane: N.v < EPSILON
			// then there is friction force = -1*kf*[-1*(N.f)]*vt on particle 
			if (norm_planeNormal.DotProduct(particle.GetVelocity()) < EPSILON) {
				friction_force = coefFriction* norm_planeNormal.DotProduct(particle.GetForce())*vt;
			}

			particle.AddForce(contact_force + friction_force);
		}
	}
}

void Pyramid::InitializeParticle() {
	Reset(Vector3d::ZERO, 0);
}

int Pyramid::pidAtItsLayer(int global_pid) {
	// n = GetLayer(global_pid);
	// convert global_pid to local_pid: [0,n*n-1]
	int layer = GetLayer(global_pid);

	int above_particles = 0;
	for (int i = 1; i<layer; ++i) {
		above_particles += i*i;
	}

	int local_pid = global_pid - above_particles;
	return local_pid;
}

void Pyramid::initSameLayerSpring(int pid) {
	int layer = GetLayer(pid);
	int pid_start = pid;
	Vector3d pos_start = m_Particles[pid_start].GetPosition();

	// left -> right
	int pid_end;
	Vector3d pos_end;
	int maxi = layer - pidAtItsLayer(pid) / layer;
	for (int i = 1; i < maxi; ++i) {
		pid_end = pid_start + i*layer;
		pos_end = m_Particles[pid_end].GetPosition();

		CSpring spring(pid_start, pid_end, (pos_start - pos_end).Length(), m_dSpringCoefStruct, m_dDamperCoefStruct);
		m_Springs.push_back(spring);
	}
	// top -> bottom
	maxi = layer - pidAtItsLayer(pid) % layer;
	for (int i = 1; i < maxi; ++i) {
		pid_end = pid_start + i;
		pos_end = m_Particles[pid_end].GetPosition();

		CSpring spring(pid_start, pid_end, (pos_start - pos_end).Length(), m_dSpringCoefStruct, m_dDamperCoefStruct);
		m_Springs.push_back(spring);
	}
	// top left -> bottom right
	maxi = min(layer - pidAtItsLayer(pid) / layer, layer - pidAtItsLayer(pid) % layer);
	for (int i = 1; i < maxi; ++i) {
		pid_end = pid_start + i*(layer + 1);
		pos_end = m_Particles[pid_end].GetPosition();

		CSpring spring(pid_start, pid_end, (pos_start - pos_end).Length(), m_dSpringCoefStruct, m_dDamperCoefStruct);
		m_Springs.push_back(spring);
	}
	// bottom left -> top right
	maxi = min(layer - pidAtItsLayer(pid) / layer - 1, pidAtItsLayer(pid) % layer);
	for (int i = 1; i <= maxi; ++i) {
		pid_end = pid_start + i*(layer - 1);
		pos_end = m_Particles[pid_end].GetPosition();

		CSpring spring(pid_start, pid_end, (pos_start - pos_end).Length(), m_dSpringCoefStruct, m_dDamperCoefStruct);
		m_Springs.push_back(spring);
	}
}

void Pyramid::initCrossLayerSpring(int pid) {
	int layer = GetLayer(pid);
	int pid_start = pid;
	Vector3d pos_start = m_Particles[pid_start].GetPosition();
	//cout << "start pid: " << pid_start << endl;

	//*** cross 1 layer: basic formula with pid_start, n = GetLayer(pid_start)
	// pid_end[cross 1 layer][top left]     = pid_start+n*n+pidAtItsLayer(pid_start)/n
	// pid_end[cross 1 layer][bottom left]  = pid_start+n*n+pidAtItsLayer(pid_start)/n+1
	// pid_end[cross 1 layer][top right]    = pid_start+n*n+pidAtItsLayer(pid_start)/n+n+1
	// pid_end[cross 1 layer][bottom right] = pid_start+n*n+pidAtItsLayer(pid_start)/n+n+2
	//
	//*** cross 2 layer: view pid_end[cross 1 layer] as pid_start and apply basic formula
	// pid_end[cross 2 layer][top left]     = pid_end[cross 1 layer][top left]'s top left
	// pid_end[cross 2 layer][bottom left]  = pid_end[cross 1 layer][bottom left]'s bottom left
	// pid_end[cross 2 layer][top right]    = pid_end[cross 1 layer][top right]'s top right
	// pid_end[cross 2 layer][bottom right] = pid_end[cross 1 layer][bottom right]'s bottom right
	// ...
	int pid_end[4] = { pid_start, pid_start, pid_start, pid_start };
	for (int n = layer; n < m_NumAtEdge; ++n) {
		// top left
		pid_end[0] += n*n + pidAtItsLayer(pid_end[0]) / n;
		// bottom left
		pid_end[1] += n*n + pidAtItsLayer(pid_end[1]) / n + 1;
		// top right
		pid_end[2] += n*n + pidAtItsLayer(pid_end[2]) / n + n + 1;
		// bottom right
		pid_end[3] += n*n + pidAtItsLayer(pid_end[3]) / n + n + 2;

		for (int i = 0; i < 4; ++i) {
			//cout << "end pid " << i << ": " << pid_end[i] << endl;
			Vector3d pos_end = m_Particles[pid_end[i]].GetPosition();

			CSpring spring(pid_start, pid_end[i], (pos_start - pos_end).Length(), m_dSpringCoefStruct, m_dDamperCoefStruct);
			m_Springs.push_back(spring);
		}
	}
}

void Pyramid::InitializeSpring() {

	for (int pid = 0; pid < m_Particles.size(); ++pid) {
		initSameLayerSpring(pid);
		initCrossLayerSpring(pid);
	}

	/*
	for (int sid = 0; sid < m_Springs.size(); ++sid) {
		cout << m_Springs[sid].GetSpringStartID() << " - " << m_Springs[sid].GetSpringEndID() << endl;
	}
	*/
	cout << "springs: " << m_Springs.size() << endl;
}

int Pyramid::GetLayer(int particleID) {
	return m_ParticleLayers[particleID];
}

void Pyramid::UpdateSpringCoef(const double a_cdSpringCoef, const CSpring::enType_t a_cSpringType) {
	double assignedCoef[3]{
		m_dSpringCoefStruct,
		m_dSpringCoefShear,
		m_dSpringCoefBending
	};

	for (unsigned int uiI = 0; uiI<m_Springs.size(); uiI++) {
		if (m_Springs[uiI].GetSpringType() == a_cSpringType) {
			m_Springs[uiI].SetSpringCoef(assignedCoef[a_cSpringType]);
		}
	}
}

void Pyramid::UpdateDamperCoef(const double a_cdDamperCoef, const CSpring::enType_t a_cSpringType) {

	// Assign coefficient index depends on spring type
	double assignedCoef[3]{
		m_dDamperCoefStruct,
		m_dDamperCoefShear,
		m_dDamperCoefBending
	};

	// Assign to corresponding type strings
	for (unsigned int uiI = 0; uiI<m_Springs.size(); uiI++) {
		if (m_Springs[uiI].GetSpringType() == a_cSpringType) {
			m_Springs[uiI].SetDamperCoef(assignedCoef[a_cSpringType]);
		}
	}
}

void Pyramid::DrawObject() {

	glPolygonMode(GL_FRONT, GL_FILL);

	for (int face = f_FRONT; face < f_BASE; face++)
		DrawObjectSide(face);

	DrawObjectBase();

	glFrontFace(GL_CCW);

}

void Pyramid::DrawParticle() {
	glPushAttrib(GL_CURRENT_BIT);
	for (int uiI = 0; uiI < m_Particles.size(); uiI++)
	{
		Vector3d Pos = m_Particles[uiI].GetPosition();

		glDisable(GL_LIGHTING);
		glPushMatrix();
		glColor3d(1.0, 0.0, 0.0);
		glPointSize(3.0);
		glBegin(GL_POINTS);
		glVertex3dv(Pos.val);
		glEnd();
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}
	glPopAttrib();
	glFrontFace(GL_CCW);
}

void Pyramid::DrawSpring(const CSpring::enType_t springType) {
	glPushAttrib(GL_CURRENT_BIT);

	for (CSpring& spring : m_Springs) {
		if (spring.GetSpringType() == springType) {
			int startID = spring.GetSpringStartID();
			int endID = spring.GetSpringEndID();

			Vector3d color = spring.GetSpringColor();
			Vector3d startPos = GetParticle(startID).GetPosition();
			Vector3d endPos = GetParticle(endID).GetPosition();

			glDisable(GL_LIGHTING);
			glPushMatrix();
			glColor3dv(color.val);
			glBegin(GL_LINES);
			glVertex3dv(startPos.val);
			glVertex3dv(endPos.val);
			glEnd();
			glPopMatrix();
			glEnable(GL_LIGHTING);
		}
	}

	glPopAttrib();
}

void Pyramid::DrawShadow(const Vector3d &lightPos) {
	float fKa[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float fKd[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float fKs[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float fKe[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, fKa);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, fKd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fKs);
	glMaterialfv(GL_FRONT, GL_EMISSION, fKe);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

	// Draw sides shadow
	for (int face = f_FRONT; face < f_BASE; face++) {
		glPushAttrib(GL_CURRENT_BIT);
		glPushMatrix();

		for (int layer = 1; layer < m_NumAtEdge; layer++) {
			for (int num = 0; num < layer; num++) {
				Vector3d V1 = GetParticle(PointMapSide(face, layer, num)).GetPosition();
				Vector3d V2 = GetParticle(PointMapSide(face, layer + 1, num)).GetPosition();
				Vector3d V3 = GetParticle(PointMapSide(face, layer + 1, num + 1)).GetPosition();

				Vector3d N1 = (V2 - V1).CrossProduct(V3 - V2).NormalizedCopy();

				DrawShadowPolygon(lightPos, N1, V1, V2);
				DrawShadowPolygon(lightPos, N1, V2, V3);
				DrawShadowPolygon(lightPos, N1, V3, V1);

				if (num + 1 < layer) {
					Vector3d V4 = GetParticle(PointMapSide(face, layer, num + 1)).GetPosition();
					Vector3d N2 = (V3 - V1).CrossProduct(V4 - V3).NormalizedCopy();

					DrawShadowPolygon(lightPos, N2, V1, V3);
					DrawShadowPolygon(lightPos, N2, V3, V4);
					DrawShadowPolygon(lightPos, N2, V4, V1);
				}
			}
		}

		glPopMatrix();
		glPopAttrib();
	}

	// Draw base shadow
	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();

	for (int j = 1; j < m_NumAtEdge; j++) {
		for (int i = 1; i < m_NumAtEdge; i++) {
			Vector3d V1 = GetParticle(PointMapBase(i, j)).GetPosition();
			Vector3d V2 = GetParticle(PointMapBase(i, j - 1)).GetPosition();
			Vector3d V3 = GetParticle(PointMapBase(i - 1, j - 1)).GetPosition();
			Vector3d V4 = GetParticle(PointMapBase(i - 1, j)).GetPosition();

			Vector3d N1 = (V2 - V1).CrossProduct(V3 - V2);
			Vector3d N2 = (V4 - V3).CrossProduct(V1 - V4);

			N1.NormalizedCopy();
			N2.NormalizedCopy();

			DrawShadowPolygon(lightPos, N1, V1, V2);

			DrawShadowPolygon(lightPos, N1, V2, V3);
			DrawShadowPolygon(lightPos, N1, V3, V1);
			DrawShadowPolygon(lightPos, N2, V3, V4);
			DrawShadowPolygon(lightPos, N2, V4, V1);
			DrawShadowPolygon(lightPos, N2, V1, V3);
		}
	}

	glPopMatrix();
	glPopAttrib();

}

void Pyramid::DrawShadowPolygon(const Vector3d &lightPos, const Vector3d &normalVec,
	const Vector3d &verPos1, const Vector3d &verPos2) {
	Vector3d lightVec = (verPos1 - lightPos).NormalizedCopy();
	Vector3d shadowPos1 = verPos1 + (verPos1 - lightPos) * 5.0f;
	Vector3d shadowPos2 = verPos2 + (verPos2 - lightPos) * 5.0f;

	if (normalVec.DotProduct(lightVec) <= 0.0f)
	{
		glBegin(GL_QUADS);
		glVertex3dv(verPos1.val);
		glVertex3dv(shadowPos1.val);
		glVertex3dv(shadowPos2.val);
		glVertex3dv(verPos2.val);
		glEnd();
	}
	else
	{
		glBegin(GL_QUADS);
		glVertex3dv(verPos1.val);
		glVertex3dv(verPos2.val);
		glVertex3dv(shadowPos2.val);
		glVertex3dv(shadowPos1.val);
		glEnd();
	}
}

void Pyramid::DrawObjectSide(int face) {
	double dFaceFactor = 1;
	vector<vector<Vector3d> > Normal(m_NumAtEdge + 1);
	vector<vector<int> > iCounter(m_NumAtEdge + 1);

	// reset buffers
	for (int layer = 1; layer <= m_NumAtEdge; layer++) {
		Normal[layer] = vector<Vector3d>(layer, Vector3d::ZERO);
		iCounter[layer] = vector<int>(layer, 0);
	}

	auto AccNormal = [&](int base_i, int base_j, int a_i, int a_j, int b_i, int b_j) {
		Vector3d base = m_Particles[PointMapSide(face, base_i, base_j)].GetPosition();
		Vector3d a = m_Particles[PointMapSide(face, a_i, a_j)].GetPosition();
		Vector3d b = m_Particles[PointMapSide(face, b_i, b_j)].GetPosition();

		Vector3d R1 = a - base;
		Vector3d R2 = b - base;
		Vector3d R3 = R1.CrossProduct(R2);

		R3 *= dFaceFactor;
		R3.Normalize();
		Normal[base_i][base_j] += R3;
		iCounter[base_i][base_j]++;
		Normal[a_i][a_j] += R3;
		iCounter[a_i][a_j]++;
		Normal[b_i][b_j] += R3;
		iCounter[b_i][b_j]++;
	};

	// process triangles, accumulate normals for Gourad shading 
	for (int layer = 1; layer < m_NumAtEdge; layer++) {
		for (int num = 0; num < layer; num++) { // process block (layer, num)
			AccNormal(layer, num, layer + 1, num, layer + 1, num + 1);
			AccNormal(layer + 1, num, layer, num, layer + 1, num + 1);
			AccNormal(layer + 1, num + 1, layer, num, layer + 1, num);
		}
	}

	for (int layer = 1; layer <= m_NumAtEdge; layer++)
		for (int num = 0; num < layer; num++) // process block (layer, num)
			if (iCounter[layer][num] != 0)
				Normal[layer][num] /= iCounter[layer][num];

	float fKa[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float fKd[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	float fKs[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float fKe[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, fKa);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, fKd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fKs);
	glMaterialfv(GL_FRONT, GL_EMISSION, fKe);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();
	//glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, face + 14);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// the actual rendering 

	for (int layer = 1; layer < m_NumAtEdge; layer++) {
		double interval = (double)m_NumAtEdge - 1;
		auto glCoord = [&](int l, int n) {
			Vector3d vertex = m_Particles[PointMapSide(face, l, n)].GetPosition();
			glNormal3dv(Normal[l][n].val);
			glTexCoord2d((1.0 / 2.0) * ((m_NumAtEdge - l) / interval) + n / interval, (l - 1) / interval);
			glVertex3dv(vertex.val);
		};

		for (int num = 0; num < layer; num++) {
			glBegin(GL_TRIANGLES);
			glCoord(layer, num);
			glCoord(layer + 1, num);
			glCoord(layer + 1, num + 1);
			glEnd();
		}

		for (int num = 0; num < layer - 1; num++) {
			glBegin(GL_TRIANGLES);
			glCoord(layer, num);
			glCoord(layer + 1, num + 1);
			glCoord(layer, num + 1);
			glEnd();
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	glPopMatrix();
	glPopAttrib();
}

void Pyramid::DrawObjectBase() {
	// f_BASE
	double dFaceFactor = 1;
	glFrontFace(GL_CCW);

	// reset buffers
	vector<vector<Vector3d> > Normal(m_NumAtEdge, vector<Vector3d>(m_NumAtEdge, Vector3d::ZERO));
	vector<vector<int> > iCounter(m_NumAtEdge, vector<int>(m_NumAtEdge, 0));

	auto AccNormal = [&](int base_i, int base_j, int a_i, int a_j, int b_i, int b_j) {
		Vector3d base = m_Particles[PointMapBase(base_i, base_j)].GetPosition();
		Vector3d a = m_Particles[PointMapBase(a_i, a_j)].GetPosition();
		Vector3d b = m_Particles[PointMapBase(b_i, b_j)].GetPosition();

		Vector3d R1 = a - base;
		Vector3d R2 = b - base;
		Vector3d R3 = R1.CrossProduct(R2);

		R3 *= dFaceFactor;
		R3.Normalize();
		Normal[base_i][base_j] += R3;
		iCounter[base_i][base_j]++;
		Normal[a_i][a_j] += R3;
		iCounter[a_i][a_j]++;
		Normal[b_i][b_j] += R3;
		iCounter[b_i][b_j]++;
	};

	for (int i = 0; i < m_NumAtEdge - 1; i++) {
		for (int j = 0; j < m_NumAtEdge - 1; j++) { // process block (i,j)
			AccNormal(i, j, i + 1, j, i, j + 1);
			AccNormal(i + 1, j + 1, i, j + 1, i + 1, j);
		}
	}

	for (int j = 0; j < m_NumAtEdge; j++)
		for (int i = 0; i < m_NumAtEdge; i++)
			if (iCounter[i][j] != 0)
				Normal[i][j] /= iCounter[i][j];

	float fKa[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float fKd[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	float fKs[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float fKe[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, fKa);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, fKd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fKs);
	glMaterialfv(GL_FRONT, GL_EMISSION, fKe);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0f);

	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();
	//glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, f_BASE + 14);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	double interval = (double)m_NumAtEdge - 1;
	for (int j = 1; j < m_NumAtEdge; j++) {
		for (int i = 1; i < m_NumAtEdge; i++) {

			auto glCoord = [&](int v_i, int v_j) {
				glNormal3dv(Normal[v_i][v_j].val);
				glTexCoord2d((double)v_i / interval, (double)v_j / interval);
				glVertex3dv(m_Particles[PointMapBase(v_i, v_j)].GetPosition().val);
			};

			glBegin(GL_QUADS);
			glCoord(i, j);
			glCoord(i, j - 1);
			glCoord(i - 1, j - 1);
			glCoord(i - 1, j);
			glEnd();
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	glPopMatrix();
	glPopAttrib();
}