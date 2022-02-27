#include "CParticleSystem.hpp"
#include "DirectX11Engine.hpp"

namespace umbra_engine
{
// An array of element descriptions to create the vertex buffer, *** This must match the Particle structure above ***
// Contents explained using the third line below as an example: that line indicates that 16 bytes into each
// vertex (into the particle structure) is a "scale" value which is a single float
D3D11_INPUT_ELEMENT_DESC ParticleElts[] =
{
	// Semantic  &  Index,   Type (eg. 1st one is float3),  Slot,   Byte Offset,  Instancing or not (not here), --"--
	{ "position",   0,       DXGI_FORMAT_R32G32B32_FLOAT,   0,      0,            D3D11_INPUT_PER_VERTEX_DATA,    0 },
	{ "alpha",      0,       DXGI_FORMAT_R32_FLOAT,         0,      12,           D3D11_INPUT_PER_VERTEX_DATA,    0 },
	{ "scale",      0,       DXGI_FORMAT_R32_FLOAT,         0,      16,           D3D11_INPUT_PER_VERTEX_DATA,    0 },
	{ "rotation",   0,       DXGI_FORMAT_R32_FLOAT,         0,      20,           D3D11_INPUT_PER_VERTEX_DATA,    0 },
};
const unsigned int NumParticleElts = sizeof(ParticleElts) / sizeof(D3D11_INPUT_ELEMENT_DESC);

CParticleSystem::CParticleSystem(int particleAmount, maths::CVector3 emitterPos, std::string textureFile, IEngine* engine)
{
	mNumberParticles = particleAmount;
	mEmitterPos = emitterPos;
	mTextureFile = textureFile;
	myEngine = engine;

	mParticlePoints.resize(mNumberParticles);
	mParticleUpdates.resize(mNumberParticles);
	mParticleDepths.resize(mNumberParticles);



	mTexture = std::make_unique<CTexture>(ETextureTypes::Diffuse);
	if (!mTexture->LoadTexture(textureFile, myEngine->GetDevice(), myEngine->GetContext()))
	{
		throw "Texture not found";
	}
	mParticleSRV = mTexture->GetTextureSRV();
	mBackBufferRenderTarget = myEngine->GetBackBufferRenderTarget();
	mDepthShaderView = myEngine->GetDepthShaderView();
	mPointSampler = myEngine->GetScene()->GetPointSampler();
}

CParticleSystem::~CParticleSystem()
{
	//if (mParticleLayout)       mParticleLayout->Release();
	//if (mParticleVertexBuffer) mParticleVertexBuffer->Release();
}

bool CParticleSystem::InitParticles()
{
	mParticlePassThruVertexShader = LoadVertexShader("ParticlePassThru_vs", myEngine);
	mParticleGeometryShader = LoadGeometryShader("ParticleRender_gs", myEngine);
	mSoftParticlePixelShader = LoadPixelShader("SoftParticle_ps", myEngine);

	if (mParticlePassThruVertexShader == nullptr || mParticleGeometryShader == nullptr ||
		mSoftParticlePixelShader == nullptr)
	{
		return false;
	}

	//*************************************************************************
	// Initialise particles

	// Create the vertex layout for the particle data structure declared near the top of the file. This step creates
	// an object (ParticleLayout) that is used to describe to the GPU the data used for each particle
	auto signature = CreateSignatureForVertexLayout(ParticleElts, NumParticleElts);
	myEngine->GetDevice()->CreateInputLayout(ParticleElts, NumParticleElts, signature->GetBufferPointer(), signature->GetBufferSize(), &mParticleLayout);


	// Set up the initial particle data
	for (auto& particle : mParticlePoints)
	{
		particle.position = mEmitterPos;
		particle.alpha = maths::Random(0.0f, 1.0f);
		particle.scale = 5.0f;
		particle.rotation = maths::Random(maths::ToRadians(0), maths::ToRadians(360));
	}
	for (auto& particleUpdate : mParticleUpdates)
	{
		particleUpdate.velocity = { maths::Random(-1.0f, 1.0f), maths::Random(2.5f, 5.0f), maths::Random(-1.0f, 1.0f) };
		particleUpdate.rotationSpeed = maths::Random(maths::ToRadians(-10), maths::ToRadians(10));
	}


	// Create the particle vertex buffer in GPU memory and copy over the contents just created (from CPU-memory)
	// We are going to update this vertex buffer every frame, so it must be defined as "dynamic" and writable (D3D11_USAGE_DYNAMIC & D3D11_CPU_ACCESS_WRITE)
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = mNumberParticles * sizeof(ParticlePoint); // Buffer size
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData; // Initial data
	initData.pSysMem = &mParticlePoints[0];
	if (FAILED(myEngine->GetDevice()->CreateBuffer(&bufferDesc, &initData, &mParticleVertexBuffer.p)))
	{
		return false;
	}

	//*************************************************************************
	return true;
}

void CParticleSystem::Render()
{
	//*************************************************************************
	// Particle rendering/update

	//////////////////////////
	// Rendering

	// Unbind the depth buffer (the NULL) as we're now going to use it as a texture instead of writing to it normally
	// Then allow access to the depth buffer as a texture in the pixel shader
	myEngine->GetContext()->OMSetRenderTargets(1, &mBackBufferRenderTarget, nullptr);
	myEngine->GetContext()->PSSetShaderResources(1, 1, &mDepthShaderView);
	myEngine->GetContext()->PSSetSamplers(1, 1, &mPointSampler);

	// Set shaders for particle rendering - the vertex shader just passes the data to the 
	// geometry shader, which generates a camera-facing 2D quad from the particle world position 
	// The pixel shader is very simple and just draws a tinted texture for each particle
	myEngine->GetContext()->VSSetShader(mParticlePassThruVertexShader, nullptr, 0);
	myEngine->GetContext()->GSSetShader(mParticleGeometryShader, nullptr, 0);
	myEngine->GetContext()->PSSetShader(mSoftParticlePixelShader, nullptr, 0);

	// Select the texture and sampler to use in the pixel shader
	myEngine->GetContext()->PSSetShaderResources(0, 1, &mParticleSRV);

	// States - alpha blending and no culling
	myEngine->GetContext()->OMSetBlendState(myEngine->GetScene()->GetAlphaBlendState(), nullptr, 0xffffff);
	myEngine->GetContext()->OMSetDepthStencilState(myEngine->GetScene()->GetDepthReadOnlyState(), 0);
	myEngine->GetContext()->RSSetState(myEngine->GetScene()->GetCullNoneState());

	// Set up particle vertex buffer / layout
	unsigned int particleVertexSize = sizeof(ParticlePoint);
	unsigned int offset = 0;
	myEngine->GetContext()->IASetVertexBuffers(0, 1, &mParticleVertexBuffer.p, &particleVertexSize, &offset);
	myEngine->GetContext()->IASetInputLayout(mParticleLayout.p);

	// Indicate that this is a point list and render it
	myEngine->GetContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	myEngine->GetContext()->Draw(mNumberParticles, 0);

	// Detach depth buffer from shader and set it back to its normal usage
	ID3D11ShaderResourceView* nullSRV = nullptr;
	myEngine->GetContext()->PSSetShaderResources(1, 1, &nullSRV);
	myEngine->GetContext()->OMSetRenderTargets(1, &mBackBufferRenderTarget, myEngine->GetDepthStencil());


	//*************************************************************************
}

void CParticleSystem::Update(float frameTime)
{
	// Particle update

	// Update particle instance data
	for (int i = 0; i < mNumberParticles; ++i)
	{
		// Decrease alpha, reset particle to new starting position when it disappears (i.e. number of particles stays constant)
		mParticlePoints[i].alpha -= 0.08f * frameTime;
		if (mParticlePoints[i].alpha <= 0.0f)
		{
			mParticlePoints[i].position = mEmitterPos;
			mParticlePoints[i].alpha = maths::Random(0.5f, 1.0f);
			mParticlePoints[i].scale = 5.0f;
			mParticlePoints[i].rotation = maths::Random(maths::ToRadians(0), maths::ToRadians(360));
		}

		// Increase scale, the particles expand as they fade
		mParticlePoints[i].scale *= pow(1.15f, frameTime);

		// Slowly rotate
		mParticlePoints[i].rotation += mParticleUpdates[i].rotationSpeed * frameTime;

		// Move particle
		mParticlePoints[i].position += mParticleUpdates[i].velocity * frameTime;
	}


	//----
	// Sort particles on camera depth

	// Recalculate the array of particle camera depths
	maths::CVector3 cameraFacing = myEngine->GetScene()->GetCamera()->WorldMatrix().GetZAxis(); // Facing direction of camera
	for (int i = 0; i < mNumberParticles; ++i)
	{
		// Depth of particle is distance from camera to particle in the direction that the camera is facing
		// Calculate this with dot product of (vector from camera position to particle position) and (camera facing vector - calculated above)
		//**** MISSING calculate particle depth using above comment
		maths::CVector3 cameraToParticle = mParticlePoints[i].position - myEngine->GetScene()->GetCamera()->Position();
		mParticleDepths[i].depth = maths::Dot(cameraFacing, cameraToParticle);

		// Store index of each particle, these will be reordered when we sort the depths and will then provide the correct order to render the particles
		mParticleDepths[i].index = i;
	}

	// Sort the particle depths. Using std::sort from <algorithm> header file. Also using a lambda function to specify the order
	// that we want things sorted in. Here putting largest depths first (draw back to front).
	// This is a modern style of C++ programming. We'll cover lambda functions in the C++11 lectures
	std::sort(mParticleDepths.begin(), mParticleDepths.end(), [](auto& a, auto& b) { return a.depth > b.depth; }); // The lambda function for a sort is given two objects
																												 // a and b (particles here) and it returns true if a
																												 // should come first, or false if b should come first


	//----
	// Pass updated particles over to GPU

	// The function "Map" gives us CPU access to the GPU vertex buffer. "Map" gives access to the buffer in CPU-memory so we can update it,
	// then when we call "Unmap" the buffer is copied back into GPU memory. We can speed this operation up since we don't actually need
	// a copy of the old contents of the buffer (just want to overwrite all the particle positions). So specify D3D11_MAP_WRITE_DISCARD,
	// which indicates that the old buffer can be discarded (no copy from GPU to CPU), and we will only write new data.
	// The buffer must have been created as "dynamic" to use "Map". Always code dynamic buffers and "Map" carefully for best performance.
	D3D11_MAPPED_SUBRESOURCE mappedData;
	myEngine->GetContext()->Map(mParticleVertexBuffer.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	// Copy current particle instance data, in order indicated by depth sorted array
	// Keep this process as fast as possible since the GPU may stall (have to wait, doing nothing) during this period
	ParticlePoint* vertexBufferData = (ParticlePoint*)mappedData.pData;
	for (int i = 0; i < mNumberParticles; ++i)
	{
		*vertexBufferData++ = mParticlePoints[mParticleDepths[i].index];
	}

	// Unlock the particle vertex buffer again so it can be used for rendering
	myEngine->GetContext()->Unmap(mParticleVertexBuffer.p, 0);
}

}