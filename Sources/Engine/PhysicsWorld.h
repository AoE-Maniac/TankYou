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
	
	// The ground plane
	PlaneCollider plane;

	TriangleCollider triangle1;
	TriangleCollider triangle2;

	TriangleMeshCollider meshCollider;

	// null terminated array of PhysicsObject pointers
	PhysicsObject** physicsObjects;

	PhysicsWorld();
	
	// Integration step
	void Update(float deltaT);
	
	// Add an object to be simulated
	void AddObject(PhysicsObject* po);

};
