#include "Tank.h"
#include "Kore/Math/Matrix.h"

Tank::Tank(MeshObject* top, MeshObject* bottom) : PhysicsObject(false, 100), Top(top), Bottom(bottom) {
	Mesh = bottom;
}

void Tank::render(ConstantLocation mLocation, ConstantLocation nLocation, TextureUnit tex) {
	Top->render(mLocation, nLocation, tex);
	Bottom->render(mLocation, nLocation, tex);
}

void Tank::update() {
	UpdateMatrix();
	Top->M = mat4::Translation(0,1,0) * Bottom->M;
}