#include "pch.h"
#include "Rendering.h"

#include <Kore/Graphics/Graphics.h>

using namespace Kore;

mat4 calculateN(mat4 M) {
	return M.Invert().Transpose();
}