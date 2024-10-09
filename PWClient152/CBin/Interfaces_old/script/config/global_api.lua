--102
lua_glb = {}

function lua_glb:check_md5(mapid)
	local maps = {}

	local files = {}
	files["Maps\\a02\\a02.ecbsd"] = "B84A249DED607A0D6673E549B748438C";
	files["Maps\\a02\\a02_1.t2bk"] = "9CACEEB14333D71C34073E62696C1825";
	files["Maps\\a02\\region.clt"] = "948672B4E6FAA4A5CF7D4CDD8F5E1CBC";
	files["Maps\\a02\\bsdata\\1.dat"] = "B856CB5F4BD82CED21BC660C94E23E10";
	files["Maps\\a02\\a02.ecwld"] = "1C33EBA0F04ADB920D822B032A8F67A4";
	files["Maps\\a02\\a02.trn2"] = "469B9B68BC9CCEF4C0985932B608138E";
	maps["a02"] = files;

	local files = {}
	files["Maps\\a05\\a05.ecbsd"] = "93DC7E59BBADFC33DC27C506DB2067ED";
	files["Maps\\a05\\a05_1.t2bk"] = "04BA7EE5B4BA1D9F813AD5C7B348ABE8";
	files["Maps\\a05\\region.clt"] = "48CCF5C4990F9A8DD3419D8D28CD6620";
	files["Maps\\a05\\bsdata\\1.dat"] = "486A3C62CEB13AA71E1D760BCBE930EF";
	files["Maps\\a05\\a05.ecwld"] = "F16B591010F84C0D9D461445D4860684";
	files["Maps\\a05\\a05.trn2"] = "E8C1B2E2AF9AA0810C273367C3BE698F";
	maps["a05"] = files;

	local files = {}
	files["Maps\\a06\\a06.ecbsd"] = "824EF40670FE7EDBE14C98B411674926";
	files["Maps\\a06\\a06_1.t2bk"] = "5DBDBD1420795F1A81DEF254542E343A";
	files["Maps\\a06\\region.clt"] = "7D0777C87A4556735D4EC8CF76CCBDE1";
	files["Maps\\a06\\bsdata\\1.dat"] = "B856CB5F4BD82CED21BC660C94E23E10";
	files["Maps\\a06\\a06.ecwld"] = "197D51D41965D7CEBF70BDF7403CE725";
	files["Maps\\a06\\a06.trn2"] = "3F30A2EBDB9F1A75BB858C6403BA75E3";
	maps["a06"] = files;

	local files = {}
	files["Maps\\a07\\a07.ecbsd"] = "7FC7A9FBA19DF57D17F7EEC3C7E55419";
	files["Maps\\a07\\a07_1.t2bk"] = "48956419E2E2E435D96669F95AA7A5A0";
	files["Maps\\a07\\region.clt"] = "386A9CAFEB6F94F7EDBF4EBBF51AB1E0";
	files["Maps\\a07\\bsdata\\1.dat"] = "5975227F0B8AF4F89AA901D495B8CA57";
	files["Maps\\a07\\a07.ecwld"] = "2D751C5C3DBA4529005EFC0AAD2B8D17";
	files["Maps\\a07\\a07.trn2"] = "E8EC324286947FD3C1C8B1782BC15BFB";	
	maps["a07"] = files;

	local files = {}
	files["Maps\\a08\\a08.ecbsd"] = "1BC0572101BB0B6F18BC4E3E2EB3AE44";
	files["Maps\\a08\\a08_1.t2bk"] = "259CE34CA2D630BB7624821EB43A92D4";
	files["Maps\\a08\\region.clt"] = "8FE90B21A75791A304C6140C0A131A0A";
	files["Maps\\a08\\bsdata\\1.dat"] = "B856CB5F4BD82CED21BC660C94E23E10";
	files["Maps\\a08\\a08.ecwld"] = "9E6DB836C7C9B9933BBA9A88D2025D57";
	files["Maps\\a08\\a08.trn2"] = "BCFA49D9461AD5EED7616A0E948091AC";
	maps["a08"] = files;

	local files = {}
	files["Maps\\a09\\a09.ecbsd"] = "2D77F49B4C10005E5DF68F46517FE9F8";
	files["Maps\\a09\\a09_1.t2bk"] = "30B0149A09F9B1E3D108FB4A86FD7F45";
	files["Maps\\a09\\region.clt"] = "C5005DF274C6E1717DD7B07A5063358D";
	files["Maps\\a09\\bsdata\\1.dat"] = "486A3C62CEB13AA71E1D760BCBE930EF";
	files["Maps\\a09\\a09.ecwld"] = "74F6636063E5AC0457C9E635FF42B7A2";
	files["Maps\\a09\\a09.trn2"] = "C91B88E7B3F40BBBDE4D15E917E159DA";	
	maps["a09"] = files;

	local files = {}
	files["Maps\\a10\\a10.ecbsd"] = "6119AAAD2ED404A3B747974D51B4EF3D";
	files["Maps\\a10\\a10_1.t2bk"] = "4A74B21B859B56EEF67830E9137BBFF7";
	files["Maps\\a10\\region.clt"] = "79FE1D962E67B8347F968D600D745C67";
	files["Maps\\a10\\bsdata\\1.dat"] = "486A3C62CEB13AA71E1D760BCBE930EF";	
	files["Maps\\a10\\a10.ecwld"] = "29DBF16D0E41C7350B8CEC3D95197005";
	files["Maps\\a10\\a10.trn2"] = "AFF3F842F74906C1EB35A897CA682639";	
	maps["a10"] = files;

	local files = {}
	files["Maps\\a11\\a11.ecbsd"] = "A3FF3653054C78B2B79DC18A5B0216D4";
	files["Maps\\a11\\a11_1.t2bk"] = "6982BB7E24D68AAFB251580ED635DBC0";
	files["Maps\\a11\\region.clt"] = "8D4D81F5BF646AA9F52EA682A143DF8C";
	files["Maps\\a11\\bsdata\\1.dat"] = "FFA39FC1F900F5680B7EDB7B60A28CC3";	
	files["Maps\\a11\\a11.ecwld"] = "65CDC0B234A948884FED97CD40E25D84";
	files["Maps\\a11\\a11.trn2"] = "DDD369A35F1FA88AFD14C768989C7714";
	maps["a11"] = files;

	local files = {}
	files["Maps\\a12\\a12.ecbsd"] = "42002D25E1F277F77F13B3243AD45BDE";
	files["Maps\\a12\\a12_1.t2bk"] = "5978E4A6577A7D0BD993FD5EC6A253F4";
	files["Maps\\a12\\region.clt"] = "4AA86FEEF551A5E088D31B3FFCC5B76A";
	files["Maps\\a12\\bsdata\\1.dat"] = "486A3C62CEB13AA71E1D760BCBE930EF";	
	files["Maps\\a12\\a12.ecwld"] = "31877955BB4678253449CFBC6A2B1273";
	files["Maps\\a12\\a12.trn2"] = "5885CFA4D76EFE1B555FE2EE4D37C696";	
	maps["a12"] = files;

	local files = {}
	files["Maps\\a13\\a13.ecbsd"] = "3910A78950F3B1B3A53232C1BDBDDADB";
	files["Maps\\a13\\a13_1.t2bk"] = "FCB2CBA98AC4959D1CB6EA27F1D633F3";
	files["Maps\\a13\\region.clt"] = "0A2EC7C0028331B8FB1804E422B67099";
	files["Maps\\a13\\bsdata\\1.dat"] = "486A3C62CEB13AA71E1D760BCBE930EF";	
	files["Maps\\a13\\a13.ecwld"] = "DD39E82A21FEA64A9E27083A64F67424";
	files["Maps\\a13\\a13.trn2"] = "F0962B46F19F3CC3C72B89C504D767BA";	
	maps["a13"] = files;

	local files = {}
	files["Maps\\a14\\a14.ecbsd"] = "BCDAD91CBBABFA7F9235F69D056C9E5A";
	files["Maps\\a14\\a14_1.t2bk"] = "7AFD42539241CF7C121BCB842878D2B8";
	files["Maps\\a14\\region.clt"] = "46D29C50ACE9C151D6A230A58FBBDE06";
	files["Maps\\a14\\bsdata\\1.dat"] = "A3AF033CF09B3E06919321CD088DD9C4";	
	files["Maps\\a14\\a14.ecwld"] = "F50777BF16E0EF1AF850EA83116DE6E7";
	files["Maps\\a14\\a14.trn2"] = "C1BCE2047E81D41F45F073EDBBBBCB17";	
	maps["a14"] = files;

	local files = {}
	files["Maps\\a15\\a15.ecbsd"] = "7ECED83BFE762ABEB7CBBC35FD43D620";
	files["Maps\\a15\\a15_1.t2bk"] = "CF8472C75C7938C8DCAFC55C6BC89FF9";
	files["Maps\\a15\\region.clt"] = "5256F194CA0A60D90FEBDBFEFC3409D0";
	files["Maps\\a15\\bsdata\\1.dat"] = "B746EB48F27A0C97627A83BCF795AD29";	
	files["Maps\\a15\\a15.ecwld"] = "218C02AE1194481B0117B1B1ED62FC6C";
	files["Maps\\a15\\a15.trn2"] = "5D5C1596D08632AB210591A78F5011D7";	
	maps["a15"] = files;

	local files = {}
	files["Maps\\a16\\a16.ecbsd"] = "47F081DF188456A6CACCBC0A9A904016";
	files["Maps\\a16\\a16_1.t2bk"] = "25DFF317F051184DEB66C4B947121BFD";
	files["Maps\\a16\\region.clt"] = "A543B0ACEC5F680B583989001F05420C";
	files["Maps\\a16\\bsdata\\1.dat"] = "745C13AADC9A3A0B2F3402057F69B574";	
	files["Maps\\a16\\a16.ecwld"] = "3BFF23C24CEA009C2DA921796635F443";
	files["Maps\\a16\\a16.trn2"] = "44D0464029090B2CE36D41DCB121C2DC";	
	maps["a16"] = files;

	local files = {}
	files["Maps\\a17\\a17.ecbsd"] = "40DD578070D367BD784288E4271DEA8B";
	files["Maps\\a17\\a17_1.t2bk"] = "82057EFFC907A0082CE6407DEE57D51E";
	files["Maps\\a17\\region.clt"] = "D74636CE3D424F0DAC5665E3CA831B03";
	files["Maps\\a17\\bsdata\\1.dat"] = "19E5AFB02A2B73577F3B80B6284E6B28";	
	files["Maps\\a17\\a17.ecwld"] = "0890CB9615FE72A47F630954CA47C01F";
	files["Maps\\a17\\a17.trn2"] = "A839CFF15454A8F985C99A626DA7FA5D";	
	maps["a17"] = files;

	local files = {}
	files["Maps\\a23\\a23.ecbsd"] = "7B3E20606CCD278762E6C91967E7271D";
	files["Maps\\a23\\a23_5.t2bk"] = "1AE2D6F0BCC22646D5DE696A06F7417C";
	files["Maps\\a23\\region.clt"] = "B5778FE257DA9BD478202F033BBEF979";
	files["Maps\\a23\\bsdata\\5.dat"] = "486A3C62CEB13AA71E1D760BCBE930EF";
	files["Maps\\a23\\a23.ecwld"] = "7E11115CED1A94DB47F5D631A6DFD4F9";
	files["Maps\\a23\\a23.trn2"] = "AD1A04A13534379842818AA53B6369F8";	
	maps["a23"] = files;

	local files = {}
	files["Maps\\a24\\a24.ecbsd"] = "5C404E17276916B06F6B7C0C8892A556";
	files["Maps\\a24\\a24_1.t2bk"] = "3DF4F7E625075087A63824B285CAF68B";
	files["Maps\\a24\\region.clt"] = "C89AA26F6AC012482627FFBC013A9A8B";
	files["Maps\\a24\\bsdata\\1.dat"] = "DB10EE3B98A096F42D02E2CD5C29EB42";
	files["Maps\\a24\\a24.ecwld"] = "969436DDB9F065CD828924D389C7D20F";
	files["Maps\\a24\\a24.trn2"] = "AA8E8C64745A0FC3E7637E0A85106091";	
	maps["a24"] = files;

	local files_to_check = maps[string.lower(mapid)]
	if files_to_check == nil then
		return true
	end

	local f, m
	for f,m in pairs(files_to_check) do
		md5 = GlobalApi.lua_glb_CalcFileMd5(f)
		if string.lower(m) ~= string.lower(md5) then
			return false
		end	 
	end
	
 	return true
end