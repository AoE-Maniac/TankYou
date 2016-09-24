#include "pch.h"

#include <vector>
#include <algorithm>
#include <iostream>

#include <Kore/IO/FileReader.h>
#include <Kore/Math/Core.h>
#include <Kore/Math/Random.h>
#include <Kore/System.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Audio/Mixer.h>
#include <Kore/Graphics/Image.h>
#include <Kore/Graphics/Graphics.h>
#include <Kore/Log.h>

#include "Engine/Collision.h"
#include "Engine/ObjLoader.h"
#include "Engine/Particles.h"
#include "Engine/PhysicsObject.h"
#include "Engine/PhysicsWorld.h"
#include "Engine/Rendering.h"
#include "Landscape.h"

#include "Projectiles.h"
#include "Steering.h"

#include "Tank.h"

using namespace Kore;

namespace {
	const int width = 800;
	const int height = 600;

	int mouseX = width / 2;
	int mouseY = height / 2;

	double startTime;
	Shader* vertexShader;
	Shader* fragmentShader;
	Program* program;

	float cameraAngle = 0.0f;

	bool left;
	bool right;
	bool up;
	bool down;
	
	mat4 P;
	mat4 View;
	mat4 PV;

	vec3 cameraPosition;
	//vec3 targetCameraPosition;
	//vec3 oldCameraPosition;

	vec3 lookAt;
	//vec3 targetLookAt;
	//vec3 oldLookAt;

	float lightPosX;
	float lightPosY;
	float lightPosZ;

	MeshObject* sphereMesh;
	MeshObject* projectileMesh;
	PhysicsObject* spherePO;

	Projectiles* projectiles;

	PhysicsWorld physics;
	
	TextureUnit tex;
	ConstantLocation pLocation;
	ConstantLocation vLocation;
	ConstantLocation lightPosLocation;
	ConstantLocation tintLocation;

	BoxCollider boxCollider(vec3(-46.0f, -4.0f, 44.0f), vec3(10.6f, 4.4f, 4.0f));

	Texture* particleImage;
	ParticleSystem* particleSystem;
    
    Steering* move;

	double lastTime;

	MeshObject* tankTop;
	MeshObject* tankBottom;

	std::vector<Tank> tanks;

	void update() {
		double t = System::time() - startTime;
		double deltaT = t - lastTime;

		lastTime = t;
		Kore::Audio::update();
		
		Graphics::begin();
		Graphics::clear(Graphics::ClearColorFlag | Graphics::ClearDepthFlag, 0xff9999FF, 1.0f);

		// Important: We need to set the program before we set a uniform
		program->set();
		Graphics::setFloat4(tintLocation, vec4(1, 1, 1, 1));
		Graphics::setBlendingMode(SourceAlpha, Kore::BlendingOperation::InverseSourceAlpha);
		Graphics::setRenderState(BlendingState, true);

		// Reset tint for objects that should not be tinted
		Graphics::setFloat4(tintLocation, vec4(1, 1, 1, 1));

		// set the camera
		cameraAngle += 0.3f * deltaT;

		float x = 0 + 10 * Kore::cos(cameraAngle);
		float z = 0 + 10 * Kore::sin(cameraAngle);
		
		// Interpolate the camera to not follow small physics movements
		float alpha = 0.3f;

		const float cameraSpeed = 0.5f;
		if (mouseY < 50) {
			lookAt.z() -= cameraSpeed;
			lookAt.x() += cameraSpeed;
		}
		if (mouseY > height - 50) {
			lookAt.z() += cameraSpeed;
			lookAt.x() -= cameraSpeed;
		}
		if (mouseX < 50) {
			lookAt.z() += cameraSpeed;
			lookAt.x() += cameraSpeed;
		}
		if (mouseX > width - 50) {
			lookAt.z() -= cameraSpeed;
			lookAt.x() -= cameraSpeed;
		}

		cameraPosition = lookAt + vec3(-10, 5, 10);
		
		// Follow the ball with the camera
		P = mat4::Perspective(0.5f * pi, (float)width / (float)height, 0.1f, 100);
		View = mat4::lookAt(cameraPosition, lookAt, vec3(0, 1, 0)); 

		Graphics::setMatrix(pLocation, P);
		Graphics::setMatrix(vLocation, View);

		// update light pos
		lightPosX = 20 * Kore::sin(2 * t);
		lightPosY = 10;
		lightPosZ = 20 * Kore::cos(2 * t);
		Graphics::setFloat3(lightPosLocation, lightPosX, lightPosY, lightPosZ);

		// Handle inputs
		float forceX = 0.0f;
		float forceZ = 0.0f;
		if (up) forceX += 1.0f;
		if (down) forceX -= 1.0f;
		if (left) forceZ -= 1.0f;
		if (right) forceZ += 1.0f;

		// Apply inputs
		vec3 force(forceX, 0.0f, forceZ);
		force = force * 20.0f;
		//spherePO->ApplyForceToCenter(force);
        
        vec3 targetPosition = vec3(-15, 0.5f, -15);
        //vec3 velocity = move->Seek(spherePO->GetPosition(), targetPosition, 0.1f);
        vec3 velocity = move->PursueTarget(spherePO->GetPosition(), targetPosition, vec3(0.1f, 0.2f, 0.0f), vec3(0, 0, 0), 0.1f);
        spherePO->ApplyImpulse(velocity);

		// Update physics
		physics.Update(deltaT);

		// Check for game over
		bool result = spherePO->Collider.IntersectsWith(boxCollider);
		if (result) {
			// ...
		}

		// Render dynamic objects
		for (int i = 0; i < physics.currentDynamicObjects; i++) {
			PhysicsObject** currentP = &physics.dynamicObjects[i];
			(*currentP)->UpdateMatrix();
			(*currentP)->Mesh->render(mLocation, nLocation, tex);
		}
		
		std::for_each(tanks.begin(), tanks.end(), [=](Tank& tank) {
			tank.update(deltaT);
			tank.render(mLocation, nLocation, tex);
		});
		

		renderLandscape(mLocation, nLocation);

		// Render static objects
		for (int i = 0; i < physics.currentStaticColliders; i++) {
			TriangleMeshCollider** current = &physics.staticColliders[i];
			(*current)->mesh->render(mLocation, nLocation, tex);
		}

		// Update and render particles
		particleSystem->setPosition(spherePO->GetPosition());
		particleSystem->setDirection(vec3(-spherePO->Velocity.x(), 3, -spherePO->Velocity.z()));
		particleSystem->update(deltaT);
		particleSystem->render(tex, vLocation, mLocation, nLocation, tintLocation, View);

		projectiles->update(deltaT);
		projectiles->render(mLocation, nLocation, vLocation, tintLocation, tex, View);

		Graphics::end();
		Graphics::swapBuffers();
	}

	void ResetSphere(vec3 Position, vec3 Velocity) {
		spherePO->SetPosition(Position);
		spherePO->Velocity = Velocity;
	}

	void keyDown(KeyCode code, wchar_t character) {
		if (code == Key_Up) {
			up = true;
		} else if (code == Key_Down) {
			down = true;
		} else if (code == Key_Left) {
			right = true;
		} else if (code == Key_Right) {
			left = true;
		}
	}

	void keyUp(KeyCode code, wchar_t character) {
		if (code == Key_Up) {
			up = false;
		} else if (code == Key_Down) {
			down = false;
		} else if (code == Key_Left) {
			right = false;
		} else if (code == Key_Right) {
			left = false;
		}
	}

	void mouseMove(int windowId, int x, int y, int movementX, int movementY) {
		mouseX = x;
		mouseY = y;

		float screenX = (x / (float)width - 0.5f) * 2.0f;
		float screenY = (y / (float)height - 0.5f) * 2.0f;

		vec4 position = PV.Invert() * vec4(screenX, screenY, -1, 1);

		vec3 dir = vec3(position.x(), position.y(), position.z()) - cameraPosition;
		dir.normalize();

		for (int i = 0; i < physics.currentDynamicObjects; i++) {
			PhysicsObject* p = physics.dynamicObjects[i];/*
			if (p->Collider.IntersectsWith(cameraPosition, dir)) {
				log(Info, "Picky");
			}*/
		}
	}
	
	void mousePress(int windowId, int button, int x, int y) {
		if(tanks.empty()) {
			projectiles->fire(cameraPosition, lookAt - cameraPosition, 10);
		} else {
			vec3 p = tanks.front().getPosition();
			vec3 l = tanks.front().getTurretLookAt();
			projectiles->fire(p, l, 10);
			log(Info, "Boom! (%f, %f, %f) -> (%f, %f, %f)", p.x(), p.y(), p.z(), l.x(), l.y(), l.z());
		}
	}

	void mouseRelease(int windowId, int button, int x, int y) {
		
	}

	void init() {
		FileReader vs("shader.vert");
		FileReader fs("shader.frag");
		vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
		fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);

		// This defines the structure of your Vertex Buffer
		VertexStructure** structures = new VertexStructure*[2];
		structures[0] = new VertexStructure();
		structures[0]->add("pos", Float3VertexData);
		structures[0]->add("tex", Float2VertexData);
		structures[0]->add("nor", Float3VertexData);
		
		structures[1] = new VertexStructure();
		structures[1]->add("M", Float4x4VertexData);
		structures[1]->add("N", Float4x4VertexData);

		program = new Program;
		program->setVertexShader(vertexShader);
		program->setFragmentShader(fragmentShader);
		program->link(structures, 2);

		tex = program->getTextureUnit("tex");
		pLocation = program->getConstantLocation("P");
		vLocation = program->getConstantLocation("V");
		lightPosLocation = program->getConstantLocation("lightPos");
		tintLocation = program->getConstantLocation("tint");
		
		sphereMesh = new MeshObject("cube.obj", "cube.png", *structures[0]);
		projectileMesh = new MeshObject("projectile.obj", "projectile.png", *structures[0], PROJECTILE_SIZE);

		spherePO = new PhysicsObject(1.0f, false, false);
		spherePO->Collider.radius = 0.5f;
		spherePO->Mass = 5;
		spherePO->Mesh = sphereMesh;
		physics.AddDynamicObject(spherePO);

		ResetSphere(vec3(10, 5.5f, -10), vec3(0, 0, 0));
        
		TriangleMeshCollider* tmc = new TriangleMeshCollider();
		tmc->mesh = new MeshObject("level.obj", "level.png", *structures[0]);
		physics.AddStaticCollider(tmc);

		tankTop = new MeshObject("tank_top.obj", "cube.png", *structures[0], 10);
		tankBottom = new MeshObject("tank_bottom.obj", "tank_bottom_uv.png", *structures[0], 10);
		tanks.push_back(Tank(tankTop, tankBottom));

		/*Sound* winSound;
		winSound = new Sound("sound.wav");
		Mixer::play(winSound);*/

		Graphics::setRenderState(DepthTest, true);
		Graphics::setRenderState(DepthTestCompare, ZCompareLess);

		Graphics::setTextureAddressing(tex, U, Repeat);
		Graphics::setTextureAddressing(tex, V, Repeat);

		particleImage = new Texture("particle.png", true);
		particleSystem = new ParticleSystem(spherePO->GetPosition(), vec3(0, 10, 0), 1.0f, 3.0f, vec4(2.5f, 0, 0, 1), vec4(0, 0, 0, 0), 10, 100, structures, particleImage);

		projectiles = new Projectiles(100, particleImage, projectileMesh, structures, &physics);

		cameraPosition = spherePO->GetPosition() + vec3(-10, 5, 10);
		lookAt = spherePO->GetPosition();
        
        move = new Steering;

		createLandscape();
	}
}

int kore(int argc, char** argv) {
	Kore::System::setName("Korerorinpa");
	Kore::System::setup();

	Kore::WindowOptions options;
	options.title = "Korerorinpa";
	options.width = width;
	options.height = height;
	options.x = 100;
	options.y = 0;
	options.targetDisplay = -1;
	options.mode = WindowModeWindow;
	options.rendererOptions.depthBufferBits = 16;
	options.rendererOptions.stencilBufferBits = 8;
	options.rendererOptions.textureFormat = 0;
	options.rendererOptions.antialiasing = 0;
	Kore::System::initWindow(options);

	Kore::Mixer::init();
	Kore::Audio::init();

	init();

	Kore::System::setCallback(update);

	startTime = System::time();

	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	Mouse::the()->Move = mouseMove;
	Mouse::the()->Press = mousePress;
	Mouse::the()->Release = mouseRelease;

	Kore::System::start();

	return 0;
}
