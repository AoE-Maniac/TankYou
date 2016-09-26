#include "Kore/pch.h"
#include "TankSystem.h"

TankSystem::TankSystem(ParticleRenderer* particleRenderer, InstancedMeshObject* meshB, InstancedMeshObject* meshT, InstancedMeshObject* meshF, vec3 spawn1a, vec3 spawn1b, vec3 spawn2a, vec3 spawn2b, float delay, Projectiles* projectiles, VertexStructure** structures) :
		meshBottom(meshB),
		meshTop(meshT),
		meshFlag(meshF),
		spawnPos1a(spawn1a),
		spawnPos1b(spawn1b),
		spawnPos2a(spawn2a),
		spawnPos2b(spawn2b),
		spawnDelay(delay),
        mProjectiles(projectiles),
        particleRenderer(particleRenderer) {
	tanks.reserve(MAX_TANKS);
	spawnTimer = spawnDelay;
    particleTexture = new Texture("particle.png", true);
	texture = new Texture("grey.png", true); 
	selectedTank = __nullptr;

	initBars(vec2(2.0f, 0.5f), structures);
}

void TankSystem::initBars(vec2 halfSize, VertexStructure** structures) {
	vbs = new VertexBuffer*[2];
	vbs[0] = new VertexBuffer(4, *structures[0], 0);
	float* vertices = vbs[0]->lock();
	setVertex(vertices, 0, -1 * halfSize.x(), -1 * halfSize.y(), 0, 0, 0);
	setVertex(vertices, 1, -1 * halfSize.x(), 1 * halfSize.y(), 0, 0, 1);
	setVertex(vertices, 2, 1 * halfSize.x(), 1 * halfSize.y(), 0, 1, 1);
	setVertex(vertices, 3, 1 * halfSize.x(), -1 * halfSize.y(), 0, 1, 0);
	vbs[0]->unlock();
	
	vbs[1] = new VertexBuffer(MAX_TANKS * 4, *structures[1], 1);

	// Set index buffer
	ib = new IndexBuffer(6);
	int* indices = ib->lock();
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;
	ib->unlock();
}

void spawnTank(std::vector<Tank*>& tanks, std::vector<Explosion*>* explosions, vec3 spawnPosa, vec3 spawnPosb, int frac, Projectiles* projectiles) {
	float a = (Kore::Random::get(0, 1000) * 1.0f / 1000);
	Tank* t1 = new Tank(frac);
	t1->SetEnemy(tanks);
    t1->setProjectile(projectiles);
	tanks.push_back(t1);
    explosions->push_back(nullptr);
	t1->SetPosition(a * spawnPosa + (1 - a) * spawnPosb);
}

void TankSystem::update(float dt) {
	if (spawnTimer <= 0 && tanks.size() <= MAX_TANKS - 2) {
		spawnTank(tanks, &explosions, spawnPos1a, spawnPos1b, 0, mProjectiles);
		spawnTank(tanks, &explosions, spawnPos2a, spawnPos2b, 1, mProjectiles);
		
		spawnTimer = spawnDelay;
	}

    std::vector<int> emptyIndices;
    for (int i = 0; i < tanks.size(); i++) {
        Tank* tank = tanks[i];
        float xpos = tank->getPosition()[0];
        float ypos = tank->getPosition()[2];
		if(tank->hp <= 0 || explosions[i] != nullptr)
        {
            if( kill(i) )
            {
                tanks.erase(tanks.begin()+i);
                explosions.erase(explosions.begin()+i);
                --i;
            }
        }
        else
        {
            tank->Integrate(dt);
            tank->update(dt);
        }
    }
    

    for( int i = 0; i < explosions.size(); i++ )
    {
        if(explosions[i] != nullptr)
        {
            explosions[i]->update(dt);
        }
    }
	
	spawnTimer -= dt;
}

bool TankSystem::kill(int i) {
    if(explosions[i] == nullptr && tanks[i] != nullptr)
    {
        explosions[i] = new Explosion(tanks[i]->getPosition(), 3.f, 10.f, 200,
                                      particleRenderer->getStructures(), particleTexture );
        particleRenderer->addParticleSystem(explosions[i]);
        Sound *winSound = new Sound("shoot_sound.wav");
        Mixer::play(winSound);
    } else
    {
        if(explosions[i]->isReady() == true)
        {
            particleRenderer->removeParticleSystem(explosions[i]);
			tanks[i]->onDeath();
            delete explosions[i];
            explosions[i] = nullptr;
            delete tanks[i];
            tanks[i] = nullptr;
            return true;
        }
    }
    return false;
}

void TankSystem::render(TextureUnit tex, mat4 View, ConstantLocation vLocation) {
	float* dataB = meshBottom->vertexBuffers[1]->lock();
	float* dataT = meshTop->vertexBuffers[1]->lock();
	float* dataF = meshFlag->vertexBuffers[1]->lock();
	float* dataBars = vbs[1]->lock();
    
	mat4 modView = View.Invert();
	modView.Set(0, 3, 0.0f);
	modView.Set(1, 3, 0.0f);
	modView.Set(2, 3, 0.0f);

    int j = 0;
    for (int i = 0; i < tanks.size(); i++) {
        if(explosions[i] == nullptr)
        {
			Tank* tank = tanks[i];
			mat4 botM = tank->GetBottomM();
			vec4 col = vec4(tank->mFrac * 0.75f, 0, (1 - tank->mFrac) * 0.75f, 1);
			if (tank->selected) col = vec4(Kore::max(1.0f * tank->mFrac, 0.75f), 0.25f, Kore::max(1.0f * (1 - tank->mFrac), 0.75f), 1);

			setMatrix(dataB, j, 0, 36, botM);
			setMatrix(dataB, j, 16, 36, calculateN(botM * View));
			setVec4(dataB, j, 32, 36, col);
		
			mat4 topM = tank->GetTopM(botM);
			setMatrix(dataT, j, 0, 36, topM);
			setMatrix(dataT, j, 16, 36, calculateN(topM * View));
			setVec4(dataT, j, 32, 36, col);
		
			mat4 flagM = tank->GetFlagM(botM);
			setMatrix(dataF, j, 0, 36, flagM);
			setMatrix(dataF, j, 16, 36, calculateN(flagM * View));
			setVec4(dataF, j, 32, 36, col);

			mat4 M = mat4::Translation(tank->GetPosition().x(), tank->GetPosition().y() + 6, tank->GetPosition().z() + 1);
			
			setMatrix(dataBars, 4 * j, 0, 36, M * modView);
			setMatrix(dataBars, 4 * j, 16, 36, calculateN(M * modView));
			setVec4(dataBars, 4 * j, 32, 36, vec4(1, 1, 1, 1));
		
			M = M * mat4::Scale(tank->getHPPerc(), 1.0f, 1.0f);

			setMatrix(dataBars, 4 * j + 1, 0, 36, M * modView);
			setMatrix(dataBars, 4 * j + 1, 16, 36, calculateN(M * modView));
			setVec4(dataBars, 4 * j + 1, 32, 36, col);

			M = mat4::Translation(tank->GetPosition().x(), tank->GetPosition().y() + 5, tank->GetPosition().z());
			
			setMatrix(dataBars, 4 * j + 2, 0, 36, M * modView);
			setMatrix(dataBars, 4 * j + 2, 16, 36, calculateN(M * modView));
			setVec4(dataBars, 4 * j + 2, 32, 36, vec4(1, 1, 1, 1));
		
			M = M * mat4::Scale(tank->getXPPerc(), 1.0f, 1.0f);

			setMatrix(dataBars, 4 * j + 3, 0, 36, M * modView);
			setMatrix(dataBars, 4 * j + 3, 16, 36, calculateN(M * modView));
			setVec4(dataBars, 4 * j + 3, 32, 36, col);
			
            ++j;
        }
	}
    
	meshBottom->vertexBuffers[1]->unlock();
	meshTop->vertexBuffers[1]->unlock();
	meshFlag->vertexBuffers[1]->unlock();
	vbs[1]->unlock();
	
	meshBottom->render(tex, j);
	meshTop->render(tex, j);
	meshFlag->render(tex, j);
	
	Graphics::setRenderState(RenderState::DepthWrite, false);
	Graphics::setTexture(tex, texture);
	Graphics::setVertexBuffers(vbs, 2);
	Graphics::setIndexBuffer(*ib);
	Graphics::drawIndexedVerticesInstanced(j * 4);
	Graphics::setRenderState(RenderState::DepthWrite, true);
}

void TankSystem::select(vec3 cameraPosition, vec3 pickDir) {
	if (selectedTank != nullptr) {
		selectedTank->selected = false;
		selectedTank = nullptr;
	}

	selectedTank = getHitTank(cameraPosition, pickDir);
	if (selectedTank != nullptr) {
		selectedTank->selected = true;
	}
}

void TankSystem::issueCommand(vec3 cameraPosition, vec3 pickDir) {
	if (selectedTank != nullptr) {
		Tank* hitTank = getHitTank(cameraPosition, pickDir);

		if (hitTank == nullptr) {
			float x = (selectedTank->GetPosition().y() - cameraPosition.y()) / pickDir.y();
			vec3 pos = cameraPosition + x * pickDir;
			selectedTank->MoveToPosition(pos);
			log(Kore::LogLevel::Info, "Moving to %f, %f, %f", pos.x(), pos.y(), pos.z());
		}
		else if (hitTank->mFrac != selectedTank->mFrac) {
			selectedTank->FollowAndAttack(hitTank);
			log(Kore::LogLevel::Info, "Attack at %f, %f, %f", hitTank->GetPosition().x(), hitTank->GetPosition().y(), hitTank->GetPosition().z());
		}
	}
}

Tank* TankSystem::getHitTank(vec3 cameraPosition, vec3 pickDir) {
	for (unsigned i = 0; i < tanks.size(); ++i) {
		if (tanks[i]->Collider.IntersectsWith(cameraPosition, pickDir)) {
			return tanks[i];
		}
	}
	return nullptr;
}
