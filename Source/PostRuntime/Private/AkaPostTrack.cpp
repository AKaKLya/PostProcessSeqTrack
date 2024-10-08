﻿// Copyright AKaKLya 2024


#include "AkaPostTrack.h"
#include "MovieScene.h"
#include "AkaPostSection.h"
#include "IMovieScenePlayer.h"
#include "Materials/MaterialInstance.h"



bool UAkaPostTrack::SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const
{
	if (SectionClass == UAkaPostSection::StaticClass())
		return true;

	return false;
}

bool UAkaPostTrack::HasSection(const UMovieSceneSection& Section) const
{
	return AkaPostSections.Contains(&Section);
}

void UAkaPostTrack::RemoveSection(UMovieSceneSection& Section)
{
	Cast<UAkaPostSection>(&Section)->CancelMaterialLink();
	AkaPostSections.Remove(&Section);
}

void UAkaPostTrack::AddSection(UMovieSceneSection& Section)
{
	UAkaPostSection* InSection=Cast<UAkaPostSection>(&Section);

	if(InSection)
	{
		AkaPostSections.Add(&Section);
	}
		
}

void UAkaPostTrack::RemoveSectionAt(int32 SectionIndex)
{
	Cast<UAkaPostSection>(AkaPostSections[SectionIndex])->CancelMaterialLink();
	AkaPostSections.RemoveAt(SectionIndex);
}

bool UAkaPostTrack::IsEmpty() const
{
	return AkaPostSections.Num() == 0;
}

const TArray<UMovieSceneSection*>& UAkaPostTrack::GetAllSections() const
{
	return AkaPostSections;
}

bool UAkaPostTrack::SupportsMultipleRows() const
{
	return true;
}


void UAkaPostTrack::AddNewMatSection(FFrameTime InStartTime,UMaterialInstance* InMatInstance,UMovieScene* InMovieScene,FGuid InPostActorGUID)
{
	MovieScene = InMovieScene;
	PostActorGuid = InPostActorGUID;
	
	UAkaPostSection* NewSection = NewObject<UAkaPostSection>(this, NAME_None, RF_Transactional);
	
	NewSection->MatInstance = InMatInstance;
	NewSection->PostActorGuid = InPostActorGUID;
	
	
	FFrameRate FrameRate = GetTypedOuter<UMovieScene>()->GetTickResolution();
	FFrameTime DurationAsFrameTime = 2.0 * FrameRate;
	NewSection->InitialPlacementOnRow(AkaPostSections, InStartTime.FrameNumber, DurationAsFrameTime.FrameNumber.Value, INDEX_NONE);

#if WITH_EDITORONLY_DATA
	FText RowDisplayName = FText::FromString(InMatInstance->GetName());
	SetTrackRowDisplayName(RowDisplayName, NewSection->GetRowIndex());
#endif
	
	AkaPostSections.Add(NewSection);
	
}

void UAkaPostTrack::OnTrackRemovedFromBinding(UMovieSceneTrack* Track, const FGuid& InObjectBindingID)
{
	CancelMaterialLink();
}

void UAkaPostTrack::OnTrackRemoved(UMovieSceneTrack* MovieSceneTrack)
{
	for(auto Section : AkaPostSections)
	{
		if(auto PostSection = Cast<UAkaPostSection>(Section))
		{
			PostSection->CancelMaterialLink();
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("OnTrackRemoved %s"),*MovieSceneTrack->GetName());
}


void UAkaPostTrack::CancelMaterialLink()
{
	for(auto Section : AkaPostSections)
	{
		if(auto PostSection = Cast<UAkaPostSection>(Section))
		{
			PostSection->CancelMaterialLink();
		}
	}
}

#if WITH_EDITORONLY_DATA
FSlateColor UAkaPostTrack::GetLabelColor(const FMovieSceneLabelParams& LabelParams) const
{
	if (LabelParams.Player)
	{
		for (TWeakObjectPtr<> WeakObject : LabelParams.Player->FindBoundObjects(PostActorGuid, LabelParams.SequenceID))
		{
			if (Cast<APostProcessVolume>(WeakObject.Get()))
			{
				return FSlateColor(FLinearColor::White);
			}
		}
	}
	return FSlateColor(FLinearColor::Red);
}
#endif