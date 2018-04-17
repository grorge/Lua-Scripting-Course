addBox(0,30,20)

bordet = "variable bordet"

for k,v in pairs(getpos()) do print(k,v) end

print("Updating position and printing")
updatepos(0,-3,0.5)
for k,v in pairs(getpos()) do print(k,v) end


print(bordet)