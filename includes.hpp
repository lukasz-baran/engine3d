
#ifndef __includes_hpp__
#define __includes_hpp__

#define	TRUE	1
#define FALSE	0

#include <assert.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include <windows.h>

#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

//extern	bool	g_Keys[256]; /// stan klawiszy

#include "globals.hpp"
#include "resource.h"

#include "math3d\math3d.hpp"

#include "openglwrapper.hpp"
#include "exception.hpp"
#include "texturemanager.hpp"
#include "listnamed.hpp"
#include "material.hpp"
#include "object.hpp"
#include "track.hpp"
#include "keyframer.hpp"
#include "scene.hpp"
#include "loader.hpp"

#endif