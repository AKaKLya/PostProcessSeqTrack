// Copyright AKaKLya 2024

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneNameableTrack.h"
#include "EventHandlers/ISequenceDataEventHandler.h"
#include "AkaPostTrack.generated.h"
class UMovieScene;
class UMaterialInstance;
/**
 * 
 */
UCLASS()
class POSTRUNTIME_API UAkaPostTrack : public UMovieSceneNameableTrack
	,public	UE::MovieScene::TIntrusiveEventHandler<UE::MovieScene::ISequenceDataEventHandler>
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	TArray<UMovieSceneSection*> AkaPostSections;
	
	UPROPERTY()
	UMovieScene* MovieScene;

	UPROPERTY()
	FGuid PostActorGuid;
	
	virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
	virtual bool HasSection(const UMovieSceneSection& Section) const override;
	virtual void RemoveSection(UMovieSceneSection& Section) override;
	virtual void AddSection(UMovieSceneSection& Section) override;
	virtual void RemoveSectionAt(int32 SectionIndex) override;
	virtual bool IsEmpty() const override;
	virtual const TArray<UMovieSceneSection*>& GetAllSections() const override;
	virtual bool SupportsMultipleRows() const override;

	
	virtual void OnTrackRemovedFromBinding(UMovieSceneTrack* Track, const FGuid& InObjectBindingID)override;
	virtual void OnTrackRemoved(UMovieSceneTrack*) override;

#if WITH_EDITOR
	virtual FSlateColor GetLabelColor(const FMovieSceneLabelParams& LabelParams) const override;
#endif
	
	void AddNewMatSection(FFrameTime InStartTime,UMaterialInstance* InMatInstance,UMovieScene* InMovieScene,FGuid InPostActorGUID);
	
	void CancelMaterialLink();

};
