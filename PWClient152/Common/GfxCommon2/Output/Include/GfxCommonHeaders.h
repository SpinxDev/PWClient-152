// Angelica Headers


#include <memory>
#include <algorithm>
#include <AC.h>
#include <AF.h>
#include <A3D.h>

#ifdef A3D_PHYSX
#include <NxSimpleTypes.h>
#include <NxStream.h>
#endif

#ifdef _ANGELICA22
#include "FAudioEngine.h"
#include "FEventSystem.h"
#include "FEvent.h"
#include "FEventInstance.h"
#include "AGPAPerfIntegration.h"
#include "A3DEnvironment.h"
#endif

#ifdef _ANGELICA21
#include "A3DSceneRenderConfig.h"
#endif

// Self-Project Headers
#include "A3DAreaEmitter.h"
#include "A3DBoxEmitter.h"
#include "A3DCombActMan.h"
#include "A3DCombinedAction.h"
#include "A3DCombActDynData.h"
#include "A3DCurveEmitter.h"
#include "A3DCylinderEmitter.h"
#include "A3DDecalEx.h"
#include "A3DEllipsoidEmitter.h"
#include "A3DGFXCachedItemMan.h"
#include "A3DGFXContainer.h"
#include "A3DGFXEditorInterface.h"
#include "A3DGFXElement.h"
#include "A3DGFXEx.h"
#include "A3DGFXExMan.h"
#include "A3DGFXKeyPoint.h"
#include "A3DGFXLight.h"
#include "A3DGFXLightning.h"
#include "A3DGFXLightningEx.h"
#include "A3DGFXModel.h"
#include "A3DGFXModelMan.h"
#include "A3DGFXPhysDataMan.h"
#include "A3DGFXRenderSlot.h"
#include "A3DGFXRing.h"
#include "A3DGFXSound.h"
#include "A3DGFXStreamMan.h"
#include "A3DGFXSurfaceRender.h"
#include "A3DGFXTexMan.h"
#include "A3DGridDecalEx.h"
#include "A3DLtnBolt.h"
#include "A3DLtnTrail.h"
#include "A3DMultiPlaneEmitter.h"
#include "A3DParaboloid.h"
#include "A3DParticle.h"
#include "A3DParticleEmitter.h"
#include "A3DParticleSystemEx.h"
#include "A3DPointEmitter.h"
#include "A3DSkillGfxComposer.h"
#include "A3DSkillGfxEvent.h"
#include "A3DTrail.h"
#include "A3DGFXPIFuncs.h"
#include "EC_Model.h"
#include "EC_ModelMan.h"


#ifdef A3D_PHYSX
	
	#include "APhysX.h"

	#include "A3DModelPhysics.h"
	#include "A3DModelPhysSync.h"
	#include "A3DModelPhysSyncData.h"
	#include "A3DPhysActor.h"
	#include "A3DPhysElement.h"
	#include "A3DPhysEmitter.h"
	#include "A3DPhysFluidEmitter.h"
	#include "A3DPhysPointEmitter.h"
	#include "A3DSkinPhysSync.h"
	#include "A3DSkinPhysSyncData.h"

	// redefine min and max since in APhysX.h we turn off them.
	template <class T>
	T max(T a, T b) { return a > b ? a : b; }

	template <class T>
	T min(T a, T b) { return a < b ? a : b; }

#endif

#ifdef FUNCTION_ANALYSER
	#include <FunctionAnalyze.h>
#define GFX_ATTACH_FUNCTION_ANALYSER ATTACH_FUNCTION_ANALYSER
#else
#define GFX_ATTACH_FUNCTION_ANALYSER 
#endif

#ifdef _ANGELICA22
	
#define AGPA_RECORD_FUNCTION_TIME \
	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__))

#else

#define AGPA_RECORD_FUNCTION_TIME

#endif
