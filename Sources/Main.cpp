#include "pch.h"

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

#include "Steering.h"
#include "Projectile.h"

using namespace Kore;

namespace {
	const int width = 1024;
	const int height = 768;

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

	Projectile* projectile;

	PhysicsWorld physics;
	
	TextureUnit tex;
	ConstantLocation pLocation;
	ConstantLocation vLocation;
	ConstantLocation mLocation;
	ConstantLocation nLocation;
	ConstantLocation lightPosLocation;
	ConstantLocation tintLocation;

	BoxCollider boxCollider(vec3(-46.0f, -4.0f, 44.0f), vec3(10.6f, 4.4f, 4.0f));

	Texture* particleImage;
	ParticleSystem* particleSystem;
    
    Steering* move;

	double lastTime;

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
        
        vec3 targetPosition = vec3(-10, 5.5f, -13);
        vec3 velocity = move->Seek(spherePO->GetPosition(), targetPosition, 3.0f);
        if(!move->Arrive(spherePO->GetPosition(), targetPosition))
            spherePO->ApplyImpulse(velocity);
        else
            spherePO->ApplyImpulse(vec3(0,0,0));

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

		renderLandscape(mLocation, nLocation);

		// Render static objects
		/*for (int i = 0; i < physics.currentStaticColliders; i++) {
			TriangleMeshCollider** current = &physics.staticColliders[i];
			(*current)->mesh->render(mLocation, nLocation, tex);
		}*/

		// Update and render particles
		particleSystem->setPosition(spherePO->GetPosition());
		particleSystem->setDirection(vec3(-spherePO->Velocity.x(), 3, -spherePO->Velocity.z()));
		particleSystem->update(deltaT);
		particleSystem->render(tex, vLocation, mLocation, nLocation, tintLocation, View);

		projectile->update(deltaT);
		projectile->render(mLocation, nLocation, vLocation, tintLocation, tex, View);

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
	}
	
	void mousePress(int windowId, int button, int x, int y) {
		projectile->fire(vec3(0, 2, 0), vec3(0, 0, 1), 10);
	}

	void mouseRelease(int windowId, int button, int x, int y) {
		
	}

	void init() {
		FileReader vs("shader.vert");
		FileReader fs("shader.frag");
		vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
		fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);

		// This defines the structure of your Vertex Buffer
		VertexStructure structure;
		structure.add("pos", Float3VertexData);
		structure.add("tex", Float2VertexData);
		structure.add("nor", Float3VertexData);

		program = new Program;
		program->setVertexShader(vertexShader);
		program->setFragmentShader(fragmentShader);
		program->link(structure);

		tex = program->getTextureUnit("tex");
		pLocation = program->getConstantLocation("P");
		vLocation = program->getConstantLocation("V");
		mLocation = program->getConstantLocation("M");
		nLocation = program->getConstantLocation("N");
		lightPosLocation = program->getConstantLocation("lightPos");
		tintLocation = program->getConstantLocation("tint");
		
		sphereMesh = new MeshObject("cube.obj", "cube.png", structure);
		projectileMesh = new MeshObject("projectile.obj", "projectile.png", structure);

		spherePO = new PhysicsObject(false, 1.0f);
		spherePO->Collider.radius = 0.5f;
		spherePO->Mass = 5;
		spherePO->Mesh = sphereMesh;
		physics.AddDynamicObject(spherePO);

		ResetSphere(vec3(10, 5.5f, -10), vec3(0, 0, 0));
        
		TriangleMeshCollider* tmc = new TriangleMeshCollider();
		tmc->mesh = new MeshObject("level.obj", "level.png", structure);
		physics.AddStaticCollider(tmc);

		/*Sound* winSound;
		winSound = new Sound("sound.wav");
		Mixer::play(winSound);*/

		Graphics::setRenderState(DepthTest, true);
		Graphics::setRenderState(DepthTestCompare, ZCompareLess);

		Graphics::setTextureAddressing(tex, U, Repeat);
		Graphics::setTextureAddressing(tex, V, Repeat);

		particleImage = new Texture("particle.png", true);
		particleSystem = new ParticleSystem(spherePO->GetPosition(), vec3(0, 10, 0), 3.0f, vec4(2.5f, 0, 0, 1), vec4(0, 0, 0, 0), 10, 100, structure, particleImage);

		projectile = new Projectile(particleImage, projectileMesh, structure, &physics);

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
	options.y = 100;
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
