#include "stdafx.h"
#include "build.h"

#include "../xrLCLight/xrdeflector.h"
#include "../xrLCLight/xrThread.h"
#include "../xrLCLight/xrLC_GlobalData.h"
#include "../xrLCLight/xrLightVertex.h"

#include "../../xrcore/xrSyncronize.h"
#include "../xrLCLight/mu_model_light.h"
xrCriticalSection	task_CS
#ifdef PROFILE_CRITICAL_SECTIONS
	(MUTEX_PROFILE_ID(task_C_S))
#endif // PROFILE_CRITICAL_SECTIONS
;

xr_vector<int>		task_pool;

class CLMThread		: public CThread
{
private:
	HASH			H;
	CDB::COLLIDER	DB;
	base_lighting	LightsSelected;
public:
	CLMThread	(u32 ID) : CThread(ID)
	{
		// thMonitor= TRUE;
		thMessages	= FALSE;
	}

	virtual void	Execute()
	{
		CDeflector* D	= 0;

		for (;;) 
		{
			// Get task
			task_CS.Enter		();
			thProgress			= 1.f - float(task_pool.size())/float(lc_global_data()->g_deflectors().size());
			if (task_pool.empty())	
			{
				task_CS.Leave		();
				return;
			}

			D					= lc_global_data()->g_deflectors()[task_pool.back()];
			task_pool.pop_back	();
			task_CS.Leave		();

			// Perform operation
			try {
				D->Light	(&DB,&LightsSelected,H);
			} catch (...)
			{
				clMsg("* ERROR: CLMThread::Execute - light");
			}
		}
	}
};
 
void	CBuild::LMapsLocal				()
{
		FPU::m64r		();
		
		mem_Compact		();

		// Randomize deflectors
 		std::random_shuffle	(lc_global_data()->g_deflectors().begin(),lc_global_data()->g_deflectors().end());
 
		for(u32 dit = 0; dit<lc_global_data()->g_deflectors().size(); dit++)	
			task_pool.push_back(dit);
   
		// Main process (4 threads)
		Status			("Lighting...");
		CThreadManager	threads;
		const	u32	thNUM	= lc_global_data()->getMaxThreads();
		CTimer	start_time;	start_time.Start();				
		for				(int L=0; L<thNUM; L++)	threads.start(xr_new<CLMThread> (L));
		threads.wait	(500);
		clMsg			("%f seconds",start_time.GetElapsed_sec());
}

void	CBuild::LMaps					()
{
	//****************************************** Lmaps
	Phase			("LIGHT: LMaps...");
	LMapsLocal();
}

void CBuild::Light()
{
	//****************************************** Implicit
	{
		FPU::m64r		();
		Phase			("LIGHT: Implicit...");
		mem_Compact		();
		ImplicitLighting();
	}
	
	LMaps		();


	//****************************************** Vertex
	FPU::m64r		();
	Phase			("LIGHT: Vertex...");
	mem_Compact		();

	LightVertex		();

//
	

	//****************************************** Starting MU
	FPU::m64r();
	Phase("LIGHT: Starting MU...");
	mem_Compact();
	Light_prepare();
 	StartMu();

 	WaitMuModelsLocalCalcLightening();
 
	//****************************************** Merge LMAPS
	{
		FPU::m64r		();
		Phase			("LIGHT: Merging lightmaps...");
		mem_Compact		();

		xrPhase_MergeLM	();
	}

	
	void XRLC_LIGHT_API IntelEmbereUNLOAD();
	IntelEmbereUNLOAD();
}

void CBuild::LightVertex	()
{
	::LightVertex();
}