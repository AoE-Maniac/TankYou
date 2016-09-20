#pragma once

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
#include "ObjLoader.h"

using namespace Kore;

#include "Collision.h"
#include "PhysicsObject.h"

class PhysicsObject;

// Handles all physically simulated objects.
class PhysicsWorld {
public:

	// Static coliiders (null terminated array of pointers)
	TriangleMeshCollider** staticColliders;

	// Dynamic objects (null terminated array of pointers)
	PhysicsObject** dynamicObjects;

	PhysicsWorld();
	
	// Integration step
	void Update(float deltaT);
	
	// Add a static collider for collisions
	void AddStaticCollider(TriangleMeshCollider* staticCollider);

	// Add a dynamic object to be simulated
	void AddDynamicObject(PhysicsObject* dynamicObject);

};
