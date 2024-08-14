// Fill out your copyright notice in the Description page of Project Settings.

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
	
	virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
	virtual bool HasSection(const UMovieSceneSection& Section) const override;
	virtual void RemoveSection(UMovieSceneSection& Section) override;
	virtual void AddSection(UMovieSceneSection& Section) override;
	virtual void RemoveSectionAt(int32 SectionIndex) override;
	virtual bool IsEmpty() const override;
	virtual const TArray<UMovieSceneSection*>& GetAllSections() const override;
	virtual bool SupportsMultipleRows() const override;
	void AddNewMatSection(FFrameTime InStartTime,UMaterialInstance* InMatInstance,UMovieScene* InMovieScene);

	virtual void OnTrackRemoved(UMovieSceneTrack*) override;
};
