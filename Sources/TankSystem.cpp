#include "Kore/pch.h"
#include "TankSystem.h"

TankSystem::TankSystem(PhysicsWorld* world, ParticleRenderer* particleRenderer, InstancedMeshObject* meshB, InstancedMeshObject* meshT, InstancedMeshObject* meshF, vec3 spawn1a, vec3 spawn1b, vec3 spawn2a, vec3 spawn2b, float delay, Projectiles* projectiles, VertexStructure** structures, Ground* grnd) :
		meshBottom(meshB),
		meshTop(meshT),
		meshFlag(meshF),
		spawnPos1a(spawn1a),
		spawnPos1b(spawn1b),
		spawnPos2a(spawn2a),
		spawnPos2b(spawn2b),
		spawnDelay(delay),
        mProjectiles(projectiles),
        particleRenderer(particleRenderer),
        world(world),
		ground(grnd) {
	tanks.reserve(MAX_TANKS);
	spawnTimer = spawnDelay;
    particleTexture = new Texture("particle.png", true);
	texture = new Texture("grey.png", true); 
	selectedTank = nullptr;
	hoveredTank = nullptr;
	
	destroyed = 0;
	deserted = 0;

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

void spawnTank(PhysicsWorld* world, std::vector<Tank*>& tanks, std::vector<Explosion*>* explosions, vec3 spawnPosa, vec3 spawnPosb, int frac, Projectiles* projectiles) {
	float a = (Kore::Random::get(0, 1000) * 1.0f / 1000);
	Tank* t1 = new Tank(frac);
    world->AddDynamicObject(t1);
	t1->SetEnemy(tanks);
    t1->setProjectile(projectiles);
	tanks.push_back(t1);
    explosions->push_back(nullptr);
	t1->SetPosition(a * spawnPosa + (1 - a) * spawnPosb);
}

void TankSystem::update(float dt) {
	if (spawnTimer <= 0 && tanks.size() <= MAX_TANKS - 2) {
		spawnTank(world, tanks, &explosions, spawnPos1a, spawnPos1b, 0, mProjectiles);
		spawnTank(world, tanks, &explosions, spawnPos2a, spawnPos2b, 1, mProjectiles);
		
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

			if (!tank->won && tank->getXPPerc() >= 1.0f) {
				tank->desert();
				deserted++;
			}
			
			vec3 pos = tank->GetPosition();
			pos.y() = ground->getHeight(pos.x(), pos.z()) + 0.5f;
			tank->SetPosition(pos);
        }
    }
    

    for( int i = 0; i < explosions.size(); i++ )
    {
        if(explosions[i] != nullptr)
        {
            explosions[i]->update(dt);
        }
    }
	
	if (selectedTank != nullptr && selectedTank->won) {
		selectedTank = nullptr;
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
		destroyed++;
		if (tanks[i]->won) {
			deserted--;
		}
    } else
    {
        if(explosions[i]->isReady() == true)
        {
            particleRenderer->removeParticleSystem(explosions[i]);
			tanks[i]->onDeath();
            delete explosions[i];
            explosions[i] = nullptr;
			for (unsigned i2 = 0; i2 < tanks.size(); ++i2) {
				tanks[i2]->RemoveEnemy(tanks[i]);
			}
			world->RemoveDynamicObject(tanks[i]);
			mProjectiles->remove(tanks[i]);
            delete tanks[i];
            tanks[i] = nullptr;
            return true;
        }
    }
    return false;
}

mat4 getRotM(vec3 ax, float an) {
	mat4 result = mat4::Identity();
	float cos = Kore::cos(an);
	float sin = Kore::sin(an);
	float x = ax.x();
	float y = ax.y();
	float z = ax.z();
	result[0][0] = cos + x * x * (1 - cos);
	result[0][1] = x * y *(1 - cos) - z * sin;
	result[0][2] = x * z * (1 - cos) + y * sin;
	result[1][0] = x * y *(1 - cos) + z * sin;
	result[1][1] = cos + y * y * (1 - cos);
	result[1][2] = y * z * (1 - cos) - x * sin;
	
	result[2][0] = z * x *(1 - cos) - y * sin;
	result[2][1] = z * y * (1 - cos) + x * sin;
	result[2][2] =  cos + z * z * (1 - cos);
	
	return result;
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
    int k = 0;
    for (int i = 0; i < tanks.size(); i++) {
        if(explosions[i] == nullptr)
        {
			Tank* tank = tanks[i];

			vec3 n = ground->getNormal(tank->GetPosition().x(), tank->GetPosition().z());
			vec3 axis = -vec3(0, 1, 0).cross(n);
			float angle = -Kore::acos(vec3(0, 1, 0).dot(n));
			mat4 t = Quaternion::Quaternion(axis, angle).matrix();
			t = getRotM(axis, angle);
			
			mat4 botM = tank->GetBottomM();
			vec4 col = vec4(tank->mFrac * 0.75f, 0, (1 - tank->mFrac) * 0.75f, 1);
			if (hoveredTank == tank) col = vec4(tank->mFrac * 0.25f, 0, (1 - tank->mFrac) * 0.25f, 1);
			if (tank->selected) col = vec4(Kore::max(1.0f * tank->mFrac, 0.75f), 0.25f, Kore::max(1.0f * (1 - tank->mFrac), 0.75f), 1);
			if (tank->won) col = vec4(1, 1, 1, 1);

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
            ++j;

			mat4 M = mat4::Translation(tank->GetPosition().x(), tank->GetPosition().y() + 6, tank->GetPosition().z() + 1);
			
			setMatrix(dataBars, k, 0, 36, M * modView);
			setMatrix(dataBars, k, 16, 36, calculateN(M * modView));
			setVec4(dataBars, k, 32, 36, vec4(1, 1, 1, 1));
		
			M = M * mat4::Scale(tank->getHPPerc(), 1.0f, 1.0f);

			setMatrix(dataBars, k + 1, 0, 36, M * modView);
			setMatrix(dataBars, k + 1, 16, 36, calculateN(M * modView));
			setVec4(dataBars, k + 1, 32, 36, col);

			k += 2;

			if (!tank->won) {
				M = mat4::Translation(tank->GetPosition().x(), tank->GetPosition().y() + 5, tank->GetPosition().z());
			
				setMatrix(dataBars, k, 0, 36, M * modView);
				setMatrix(dataBars, k, 16, 36, calculateN(M * modView));
				setVec4(dataBars, k, 32, 36, vec4(1, 1, 1, 1));
		
				M = M * mat4::Scale(tank->getXPPerc(), 1.0f, 1.0f);

				setMatrix(dataBars, k + 1, 0, 36, M * modView);
				setMatrix(dataBars, k + 1, 16, 36, calculateN(M * modView));
				setVec4(dataBars, k + 1, 32, 36, col);
			
				k += 2;
			}
        }
	}
    
	meshBottom->vertexBuffers[1]->unlock();
	meshTop->vertexBuffers[1]->unlock();
	meshFlag->vertexBuffers[1]->unlock();
	vbs[1]->unlock();
	
	meshBottom->render(tex, j);
	meshTop->render(tex, j);
	meshFlag->render(tex, j);
	
	Graphics::setRenderState(RenderState::DepthTest, false);
	Graphics::setTexture(tex, texture);
	Graphics::setVertexBuffers(vbs, 2);
	Graphics::setIndexBuffer(*ib);
	Graphics::drawIndexedVerticesInstanced(k);
	Graphics::setRenderState(RenderState::DepthTest, true);
}

void TankSystem::hover(vec3 cameraPosition, vec3 pickDir) {
	hoveredTank = getHitTank(cameraPosition, pickDir);
}

void TankSystem::select(vec3 cameraPosition, vec3 pickDir) {
	if (selectedTank != nullptr) {
		selectedTank->selected = false;
		selectedTank = nullptr;
	}

	selectedTank = getHitTank(cameraPosition, pickDir);
	if (selectedTank != nullptr && !selectedTank->won) {
		selectedTank->selected = true;
	}
}

void TankSystem::issueCommand(vec3 cameraPosition, vec3 pickDir) {
	if (selectedTank != nullptr && !selectedTank->won) {
		Tank* hitTank = getHitTank(cameraPosition, pickDir);

		if (hitTank == nullptr) {
			float x = (selectedTank->GetPosition().y() - cameraPosition.y()) / pickDir.y();
			vec3 pos = cameraPosition + x * pickDir;
			selectedTank->MoveToPosition(pos);
			log(Kore::LogLevel::Info, "Moving to %f, %f, %f", pos.x(), pos.y(), pos.z());
		}
		else if (hitTank->mFrac != selectedTank->mFrac || hitTank->won) {
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
