solution "gKit2light"
	configurations { "debug", "release" }

	platforms { "x64", "x32" }
	
	includedirs { ".", "./src/gKit" }
	
	configuration "debug"
		targetdir "./bin/debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "release"
		targetdir "./bin/release"
--~ 		defines { "NDEBUG" }
--~ 		defines { "GK_RELEASE" }
		flags { "OptimizeSpeed" }

	configuration "linux"
		buildoptions { "-mtune=native -march=native" }
		buildoptions { "-std=c++11" }
		buildoptions { "-W -Wall -Wextra -Wsign-compare -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable", "-pipe" }
		buildoptions { "-flto"}
		linkoptions { "-flto"}
		links { "GLEW", "SDL2", "SDL2_image", "GL" }

	configuration { "linux", "debug" }
		buildoptions { "-g"}
		linkoptions { "-g"}
		
	configuration { "linux", "release" }
		buildoptions { "-fopenmp" }
		linkoptions { "-fopenmp" }

	configuration { "windows" }
		defines { "WIN32", "NVWIDGETS_EXPORTS", "_USE_MATH_DEFINES", "_CRT_SECURE_NO_WARNINGS" }
		defines { "NOMINMAX" } -- allow std::min() and std::max() in vc++ :(((

	configuration { "windows", "gmake", "x32" }
		buildoptions { "-std=c++11"}
		includedirs { "extern/mingw/include" }
		libdirs { "extern/mingw/lib" }
		links { "mingw32", "SDL2main", "SDL2", "SDL2_image", "opengl32", "glew32" }

	configuration { "windows", "codeblocks", "x32" }
		buildoptions { "-std=c++11"}
		includedirs { "extern/mingw/include" }
		libdirs { "extern/mingw/lib" }
		links { "mingw32", "SDL2main", "SDL2", "SDL2_image", "opengl32", "glew32" }
		
	configuration { "windows", "vs2013", "x64" }
		includedirs { "extern/visual2013/include" }
		libdirs { "extern/visual2013/lib" }
		platforms { "x64" }
		links { "opengl32", "glew32", "SDL2", "SDL2main", "SDL2_image" }

		
	configuration { "windows", "vs2015", "x64" }
		includedirs { "extern/visual2015/include" }
		libdirs { "extern/visual2015/lib" }
		links { "opengl32", "glew32", "SDL2", "SDL2main", "SDL2_image" }
		
	configuration "macosx"
		local frameworks= "-F /Library/Frameworks/"
		buildoptions { "-std=c++11" }
		defines { "GK_MACOS" }
		buildoptions { frameworks }
		linkoptions { frameworks .. " -framework OpenGL -framework SDL2 -framework SDL2_image" }


 -- description des fichiers communs
local gkit_files = { "src/gKit/*.cpp", "src/gKit/*.h" }

 -- description des projets		


project("master_meca_sim_etudiant")
    language "C++"
    kind "ConsoleApp"
    targetdir "./bin"
    files ( gkit_files )
    files { 	"src-etudiant/main.cpp",
		"src-etudiant/Viewer.cpp", 
		"src-etudiant/Viewer.h", 
		"src-etudiant/Viewer-init.cpp",
		"src-etudiant/MSS.cpp",
		"src-etudiant/MSS.h",
		"src-etudiant/Noeuds.h",
		"src-etudiant/Scene.cpp",
		"src-etudiant/Scene.h",
		"src-etudiant/ObjetSimule.cpp",
		"src-etudiant/ObjetSimule.h",
		"src-etudiant/ObjetSimuleMSS.cpp",
		"src-etudiant/ObjetSimuleMSS.h",
		"src-etudiant/ObjetSimuleParticule.cpp",
		"src-etudiant/ObjetSimuleParticule.h",
		"src-etudiant/Properties.cpp",
		"src-etudiant/Properties.h",
		"src-etudiant/Param.cpp",
		"src-etudiant/Calculs.cpp", 
		"src-etudiant/CalculsMSS.cpp", 
		"src-etudiant/CalculsParticule.cpp" }

