option(WITH_XVISION "Enable XVision" OFF)
if(WITH_XVISION)
	set(XVISION_TRACKERS xvSSDAffine xvSSDGrid xvSSDGridLine xvSSDHelper xvSSDMain xvSSDPyramidAffine xvSSDPyramidRotate xvSSDPyramidRT xvSSDPyramidSE2 xvSSDPyramidTrans xvSSDRotate xvSSDRT xvSSDScaling xvSSDSE2 xvSSDTR xvSSDTrans common xvColor xvEdgeTracker)
else(WITH_XVISION)	
	set(THIRD_PARTY_DEFINITIONS ${THIRD_PARTY_DEFINITIONS} DISABLE_XVISION)
	message(STATUS "Xvision disabled")
endif(WITH_XVISION)