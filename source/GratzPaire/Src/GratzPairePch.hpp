/**
See licence file in root folder, MIT.txt
*/
#pragma once

#include <Design/Named.hpp>

#include <Engine.hpp>
#include <Cache/OverlayCache.hpp>
#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Event/UserInput/UserInputListener.hpp>
#include <Material/Material.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Miscellaneous/Ray.hpp>
#include <Overlay/Overlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Plugin/RendererPlugin.hpp>
#include <Render/RenderLoop.hpp>
#include <Render/RenderWindow.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneFileParser.hpp>
#include <Scene/SceneNode.hpp>
#include <Texture/TextureUnit.hpp>

#include <Log/Logger.hpp>
#include <Math/Angle.hpp>
#include <Math/Point.hpp>
#include <Math/Quaternion.hpp>
#include <Math/RangedValue.hpp>

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
