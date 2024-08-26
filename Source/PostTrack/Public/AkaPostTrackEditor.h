// Copyright AKaKLya 2024

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneTrackEditor.h"
#include "UObject/Object.h"


class UAkaPostTrack;

class POSTTRACK_API FAkaPostTrackEditor : public FMovieSceneTrackEditor
{
public:

	FAkaPostTrackEditor(TSharedRef<ISequencer>InSequencer);

	static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> InSequencer);
	
	virtual bool SupportsType(TSubclassOf<UMovieSceneTrack> TrackClass) const override;
	virtual TSharedRef<ISequencerSection> MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding) override;
	virtual void ExtendObjectBindingTrackMenu(TSharedRef<FExtender> Extender, const TArray<FGuid>& ObjectBindings, const UClass* ObjectClass) override;
	
	void ConstructObjectBindingTrackMenu(FMenuBuilder& MenuBuilder, TArray<FGuid> ObjectBindings);
	void HandleAddComponentMaterialActionExecute(APostProcessVolume* InPostActor,UMaterialInstance* InMatInstance,FGuid PostActorGUID);
};
