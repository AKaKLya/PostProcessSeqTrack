// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneNameableTrack.h"
#include "AkaPostTrack.generated.h"

/**
 * 
 */
UCLASS()
class POSTRUNTIME_API UAkaPostTrack : public UMovieSceneNameableTrack
{
	GENERATED_BODY()
public:
	
	UPROPERTY()
	TArray<UMovieSceneSection*> AkaPostSections;
	
	virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
	virtual bool HasSection(const UMovieSceneSection& Section) const override;
	virtual void RemoveSection(UMovieSceneSection& Section) override;
	virtual void AddSection(UMovieSceneSection& Section) override;
	virtual void RemoveSectionAt(int32 SectionIndex) override;
	virtual bool IsEmpty() const override;
	virtual const TArray<UMovieSceneSection*>& GetAllSections() const override;
	virtual bool SupportsMultipleRows() const override;
	void AddNewMatSection(FFrameTime InStartTime,UMaterialInstance* InWidgetBlueprint);
	
};
