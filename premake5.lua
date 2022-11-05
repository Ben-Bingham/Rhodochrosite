require "Dependencies/Gemstone/Lazuli/premake5"
require "Dependencies/Gemstone/Malachite/premake5"
require "Dependencies/Gemstone/Ruby/premake5"
require "Dependencies/Gemstone/Wavellite/premake5"
require "Dependencies/Gemstone/Pyrite/premake5"
require "Dependencies/Gemstone/vendor/imgui/premake5"

workspace "Rhodochrosite"
	configurations {"Debug", "Release"}
	platforms {"x64"}

	startproject "Rhodochrosite"

	filter "platforms:x64"
		architecture "x64"

	filter "configurations:Debug"
		defines {
			"MALACHITE_DEBUG",
			"LAZULI_DEBUG",
			"RUBY_DEBUG",
			"GLEW_STATIC"
		}
		symbols "On"
		
	filter "configurations:Release"
		defines { 
			"MALACHITE_RELEASE",
			"LAZULI_RELEASE",
			"RUBY_RELEASE",
			"GLEW_STATIC"
		}
		optimize "On"
	filter {}
	
	defines { "RUBY_ASSETS=\"..\\\\Dependencies\\\\Gemstone\\\\Ruby\\\\assets\"" }

	group "Rhodochrosite"
		include "Rhodochrosite"
	group ""
	
	group "Gemstone"
		project_Lazuli("Dependencies/Gemstone/")
		project_Malachite("Dependencies/Gemstone/")
		project_Ruby("Dependencies/Gemstone/")
		project_Wavellite("Dependencies/Gemstone/")
		project_Pyrite("Dependencies/Gemstone/")
	group ""

	group "Vendor"
		project_ImGui("Dependencies/Gemstone/")
	group ""
