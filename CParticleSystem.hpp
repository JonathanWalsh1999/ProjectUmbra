#ifndef _CPARTICLE_SYSTEM_H_
#define _CPARTICLE_SYSTEM_H_
//======================================================================================

//--------------------------------------------------------------------------------------
// Encapsulation for creating and managing a particle system
//--------------------------------------------------------------------------------------

#include "Common.hpp"
#include "ITexture.h"
#include "CTexture.h"
#include <atlbase.h>


//======================================================================================
namespace umbra_engine
{



//---------------------------------------
// Forward Declarations
//---------------------------------------
class IEngine;

class CParticleSystem
{
public:
//---------------------------------------
// Constructor / Destructor
//---------------------------------------
	CParticleSystem() = delete;
	CParticleSystem(int particleAmount, maths::CVector3 emitterPos, std::string textureFile, IEngine* engine);
	~CParticleSystem();

//---------------------------------------
// Data Access
//---------------------------------------


//---------------------------------------
// Operational Methods
//---------------------------------------
	bool InitParticles();
	void Render();
	void Update(float frameTime);

private:
//---------------------------------------
// Private Types
//---------------------------------------
	// C++ data structure for rendering a particle (stored as a single point)
	struct ParticlePoint
	{
		maths::CVector3 position; // World position of particle, the geometry shader will expand it into a camera-facing quad
		float    alpha;    // Overall transparency of particle (the particle texture can also contain per-pixel transparency)
		float    scale;    // Size of the quad created by the geometry shader from the particle point
		float    rotation;  // Rotation of the quad created by the geometry shader
	};

	// Data required to update a particle - used on the CPU only
	struct ParticleUpdate
	{
		maths::CVector3 velocity;
		float    rotationSpeed;
	};

	struct SParticleDepth
	{
		int   index;
		float depth;
	};


//---------------------------------------
// Private Member Variables
//---------------------------------------
	int mNumberParticles;
	maths::CVector3 mEmitterPos;
	std::string mTextureFile;
	std::unique_ptr<ITexture> mTexture;
	IEngine* myEngine;
	std::vector<ParticlePoint> mParticlePoints;
	std::vector<ParticleUpdate> mParticleUpdates;
	std::vector<SParticleDepth> mParticleDepths;

	// Vertex layout and buffer for the particles (rendering data only, we are not doing update on the GPU in this example)
	ID3D11RenderTargetView* mBackBufferRenderTarget = nullptr;
	ID3D11ShaderResourceView* mDepthShaderView = nullptr;
	ID3D11SamplerState* mPointSampler = nullptr;

	CComPtr<ID3D11VertexShader> mParticlePassThruVertexShader = nullptr;
	CComPtr<ID3D11GeometryShader> mParticleGeometryShader = nullptr;
	CComPtr<ID3D11PixelShader> mSoftParticlePixelShader = nullptr;
	CComPtr<ID3D11InputLayout> mParticleLayout = nullptr;
	CComPtr<ID3D11Buffer> mParticleVertexBuffer = nullptr;
	ID3D11ShaderResourceView* mParticleSRV = nullptr;

};//Class
}//Namespace
//======================================================================================
#endif//Header Guard
