// Copyright AKaKLya 2024

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FPostTrackModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:
	FDelegateHandle PostTrackHandle;
	void AddPostProcessTracks(const AActor& SourceActor, const FGuid& Binding,TSharedPtr<class ISequencer> Sequencer);
};
