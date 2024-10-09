//----------------------------------------------------------
// Filename	: OperationManager.h
// Creator	: QingFeng Xin
// Date		: 2004.7.23
// Desc		: 用于所有操作的管理,任何定义的新操作都要在
//            CreateOperation()中加入相应代码，同时也必须
//            在enum中列举出所加的操作。 所有操作都是事先
//            定义的，不能动态加入,取得操作是通过索引实现。
//-----------------------------------------------------------

#ifndef _OPERATION_MANAGER_H_
#define _OPERATION_MANAGER_H_

#include "AList2.h"
#include "Operation.h"


enum
{
		OPERATION_TERRAIN_STRETCH = 0,
		OPERATION_TERRAIN_NOISE,
		OPERATION_TERRAIN_SMOOTH,
		OPERATION_TERRAIN_PAINT,
		OPERATION_TERRAIN_MASK_SMOOTH,
		OPERATION_SCENE_LIGHT_ADD,
		OPERATION_SCENE_OBJECT_SELECT,
		OPERATION_SCENE_OBJECT_MOVE,
		OPERATION_SCENE_OBJECT_ROTATE,
		OPERATION_SCENE_OBJECT_MULTISELECT,
		OPERATION_SCENE_MODEL_ADD,
		OPERATION_SCENE_OBJECT_SCALE,
		OPERATION_SCENE_WATER_ADD,
		OPERATION_TERRAIN_PLANTS,
		OPERATION_BEZIER_ADD,
		OPERATION_SCENE_AREA_ADD,
		OPERATION_BEZIER_DELETE_POINT,
		OPERATION_BEZIER_CUT_SEGMENT,
		OPERATION_BEZIER_ADD_POINT,
		OPERATION_BEZIER_DRAG,
		OPERATION_SCENE_AUDIO_ADD,
		OPERATION_SET_VERNIER,
		OPERATION_BEZIER_TEST,
		OPERATION_SCENE_DUMMY_ADD,
		OPERATION_SCENE_SPECIALLY_ADD,
		OPEAATION_SCENE_AIGENERATOR_ADD,
		OPERATION_SCENE_OBJECT_SKINMODEL_ADD,
		OPERATION_SCENE_OBJECT_CRITTER_ADD,
		OPERATION_SCENE_NPCGENERATOR_ADD,
		OPERATION_BEZIER_MERGE,
		OPERATION_SCENE_PRECINCT_ADD,
		OPERATION_TERRAIN_OBSTRUCT,
		OPERATION_SCENE_GATHER_ADD,
		OPERATION_SCENE_RANGE_ADD,
		OPERATION_SCENE_OBJECT_ROTATE_EX,
		OPERATION_SCENE_CLOUD_BOX_ADD,
		OPERATION_SCENE_INSTANCE_BOX_ADD,
		OPERATION_SCENE_OBJECT_DYNAMIC_ADD,
		OPERATION_SCENE_OBJECT_ROTATE2,
		OPERATION_TERRAIN_OBSTRUCT2,
		//Add your operation enum on below

		//end
		OPERATION_NUM,
};

/*
class COperationManager
{
public:
	COperationManager();
	~COperationManager();


	COperation* CreateOperation( int type,char* name );

	COperation* FindOperation( char* name );

	void        DeleteOperation( char *name);

	void        DeleteAllOperation();

protected:
	//Add your operation function on below
	COperation* CreateTerrainStretchOperation( char*name );

	APtrList<POPERATION>	    m_listOperation;
};
*/

class OperationContainer
{
public:
	OperationContainer();
	~OperationContainer();
	void Release();
	COperation* GetOperation(int nOperation){ return m_nOperationArray[nOperation]; };
private:
	void CreateOperation();
	
	COperation* m_nOperationArray[OPERATION_NUM];
};


extern OperationContainer g_OperationContainer;

#endif
