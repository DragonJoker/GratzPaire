/**
See licence file in root folder, MIT.txt
*/
#pragma once

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#pragma warning( disable: 4464 )
#pragma warning( disable: 5054 )
#ifdef _WIN32
#	include <winsock2.h>
#endif
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/display.h>
#include <wx/frame.h>
#include <wx/image.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/timer.h>
#include <wx/windowptr.h>
#include <wx/wx.h>
#pragma warning( pop )

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Event/UserInput/UserInputListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Render/Ray.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
