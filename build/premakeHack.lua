local oldGetvpath = premake.project.getvpath

function premake.project.getvpath(prj, abspath)
		local vpath = oldGetvpath(prj, abspath)
		local rfile=io.open(abspath, "r")
		if rfile == nil or rfile == false then
			return vpath
		end
		
		local firstLine = rfile:lines();
		if firstLine == nil then
			return vpath
		end
		-- TODO:check file header and fuck the vpath
		print(abspath .. "\n")
		return vpath;
end