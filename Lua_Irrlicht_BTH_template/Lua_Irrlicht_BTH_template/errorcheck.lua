addMesh({{0,0,0}, {5,0,0}, {0,0,5}})    -- Simple triangle in the plane
addMesh({{"orange"},{},{}})               -- Error: non-numeric coordinates
addMesh({{1,2,3,4},{},{}})               -- Error: number of components
addMesh({{1,2,3}})                       -- Error: not a valid number of vertices

camera({0,5,-20}, {0,0,0})
camera({0,5,-20}, {0,0})            -- Error: target has wrong number of coordinates
camera({0,5,-20}, {0,0,"x"})        -- Error: coordinate is not a number

snapshot("output.png")
snapshot("///")                      -- Error: file could not be opened

loadScene("test1.lua")
--loadScene("file doesn't exist")         -- Should display an error message.
--loadScene("badscene.txt")               -- If the file cannot be parsed display an error.
--[[
--]]--