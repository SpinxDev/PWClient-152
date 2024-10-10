#ifndef _ELEMENTPCKDIR_H_
#define _ELEMENTPCKDIR_H_

const char *	g_szPckDir[] = {
	"configs",
	"facedata",
	"gfx",
	"grasses",
	"help\\script",
	"interfaces",
	"building",
	"models",
	"sfx",
	"shaders",
	"surfaces",
	"textures",
	"trees",
	"litmodels",
	"loddata",
};

const char *	g_szNoPckDir[][2] = {
	"cursors", "", // r means recursive for param 2
	"data", "",
	"help\\voice", "r",
	"maps", "r",
	"userdata\\userpic", "",
	"userdata\\character", "r",
	"userdata\\server", "",
	"music", "r",
	"info", "",
	"bbsfiles", "r",
	".", "s",		// s means file must be in the sep files list
	"reportbugs", "s",
	"font", "",
	"physx\\v2.8.4", "",
};

const char *	g_szSepFiles[] =
{
	"dbserver.conf",
	"elementclient.exe",
	"dbghelp.dll",
	"elementskill.dll",
	"ftdriver.dll",
	"ifc22.dll",
	"immwrapper.dll",
	"speedtreert.dll",
	"unicows.dll",
	"zlibwapi.dll",
	"creportbugs.exe",
	"pwprotector.exe",
	"mfc42.dll",
	"elementlocalize.exe",
	"localizehelp.chm",
	"360inst_wanmeigj.exe",
	"migratewm2.exe",
	"dxwebsetup.exe",
	"cudart32_30_9.dll",
	"physxloader.dll",
	"microsoft.vc80.crt.manifest",
	"msvcm80.dll",
	"msvcp80.dll",
	"msvcr80.dll",
	"d3dx9_42.dll",
	"perfectworld.version",
	"gt_setup.exe",
	"gt_updater.exe",
	"unitywebplayerdevelopment.exe",
	"arcsdk.dll",
	"arcinstall.exe",
	"logiccheck.dll",
	"enablemoremem_vista_win7_32bit.bat",
	"disablemoremem_vista_win7_32bit.bat",
};

#endif
