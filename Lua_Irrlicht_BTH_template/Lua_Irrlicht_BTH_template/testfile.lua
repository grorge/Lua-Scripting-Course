print("--Loading Testfile--")

camera({-10,0,-20},{0,2,0})

table = {"id","name","type"}

-- testing the testfile

for i=1,10 
do 	
	addBox({i,i,i},1.0-i/11.0) 
end

for k,v in pairs(getNodes()) 
do 
	for kk,vv in pairs(v) 
	do 
		print(k,kk,vv)
	end 
end

camera({-10,0,0},{0,2,0})

snapshot("test1.png")

-- end of testing ]]--

addBox({1,20,1}, 10, "from testfile" )

addMesh({{1,2,1},{8,8,-8},{40,4,4},{-40,4,4},{-8,-8,-8},{-1,-12,4}})


--camera({-10,0,0},{0,2,0})

--for k,v in pairs(getNodes()) do 	print(k,v) end



--for i=1,0 do 	addBox({i,i,i},1.0-i/10.0) end

-- for k,v in pairs(getNodes()) do 	for kk,vv in pairs(v) do 		print(k,kk,vv) 	end end



print("--Finnished loading Testfile--")