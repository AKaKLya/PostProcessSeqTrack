// Copyright AKaKLya 2024


#include "AkaPostTrackEditor.h"

#include "PostRuntime/Public/AkaPostTrack.h"
#include "AkaSection.h"
#include "ISequencerModule.h"


#define LOCTEXT_NAMESPACE "PostTrackEditor"

FAkaPostTrackEditor::FAkaPostTrackEditor(TSharedRef<ISequencer> InSequencer) : FMovieSceneTrackEditor(InSequencer)
{
}

TSharedRef<ISequencerTrackEditor> FAkaPostTrackEditor::CreateTrackEditor(TSharedRef<ISequencer> InSequencer)
{
	return MakeShareable(new FAkaPostTrackEditor(InSequencer));
}

bool FAkaPostTrackEditor::SupportsType(TSubclassOf<UMovieSceneTrack> TrackClass) const
{
	return UAkaPostTrack::StaticClass() == TrackClass;
}

TSharedRef<ISequencerSection> FAkaPostTrackEditor::MakeSectionInterface(UMovieSceneSection& SectionObject,
	UMovieSceneTrack& Track, FGuid ObjectBinding)
{
	auto AkaSection = MakeShareable(new FAkaSection(SectionObject));
	return AkaSection;
}

void FAkaPostTrackEditor::ExtendObjectBindingTrackMenu(TSharedRef<FExtender> Extender,
	const TArray<FGuid>& ObjectBindings, const UClass* ObjectClass)
{
	if (ObjectClass->IsChildOf(APostProcessVolume::StaticClass()))
	{
		Extender->AddMenuExtension(SequencerMenuExtensionPoints::AddTrackMenu_PropertiesSection,
			EExtensionHook::Before, nullptr,
			FMenuExtensionDelegate::CreateSP(this, &FAkaPostTrackEditor::ConstructObjectBindingTrackMenu, ObjectBindings));
	}
}



void FAkaPostTrackEditor::ConstructObjectBindingTrackMenu(FMenuBuilder& MenuBuilder, TArray<FGuid> ObjectBindings)
{
	UObject* Object = GetSequencer()->FindSpawnedObjectOrTemplate(ObjectBindings[0]);
	if (!Object)
	{
		return;
	}

	APostProcessVolume* PPActor = Cast<APostProcessVolume>(Object);
	if (!PPActor)
	{
		return;
	}

	FGuid ActorID = GetSequencer()->FindObjectId(*PPActor, GetSequencer()->GetFocusedTemplateID());
	UMaterialInstance* PostMat = nullptr;
	TArray<FWeightedBlendable> PostMatArray = PPActor->Settings.WeightedBlendables.Array;
	if(PostMatArray.IsEmpty())
	{
		return;
	}

	if(auto MatObj = PostMatArray[0].Object)
	{
		if(auto DynamicMat = Cast<UMaterialInstanceDynamic>(MatObj))
		{
			PostMat = Cast<UMaterialInstance>(DynamicMat->Parent.Get());
			goto Flag1;
		}
		PostMat = Cast<UMaterialInstance>(MatObj);
	}
	else
	{
		return;
	}
		
	Flag1:
	
	if (PostMat)
	{
		MenuBuilder.BeginSection("Materials", LOCTEXT("MaterialSection", "Material Parameters"));
		{
			FUIAction AddComponentMaterialAction(FExecuteAction::CreateRaw(this, &FAkaPostTrackEditor::HandleAddComponentMaterialActionExecute, PPActor, PostMat,ActorID));
			FText AddDecalMaterialLabel = FText::Format(LOCTEXT("PostTrackEditor", "Post: {0}"), FText::FromString(PostMat->GetName()));
			FText AddDecalMaterialToolTip = FText::Format(LOCTEXT("PostTrackEditor", "Add Post material {0}"), FText::FromString(PostMat->GetName()));
			MenuBuilder.AddMenuEntry(AddDecalMaterialLabel, AddDecalMaterialToolTip, FSlateIcon(), AddComponentMaterialAction);
		}
		MenuBuilder.EndSection();
	}

}

void FAkaPostTrackEditor::HandleAddComponentMaterialActionExecute(APostProcessVolume* InPostActor,UMaterialInstance* InMatInstance,FGuid PostActorGUID)
{
	TSharedPtr<ISequencer> SequencerPtr = GetSequencer();
	UMovieScene* MovieScene = GetFocusedMovieScene();
	
	UAkaPostTrack* PostTrack = MovieScene->AddTrack<UAkaPostTrack>();
	PostTrack->SetDisplayName(FText::FromString(InMatInstance->GetName()+"_Mat"));
	
	//auto MovieScene = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene();
	
	MovieScene->EventHandlers.Link(PostTrack);
	MovieScene->Modify();
	PostTrack->AddNewMatSection(GetTimeForKey(),InMatInstance,MovieScene,PostActorGUID);
}


#undef LOCTEXT_NAMESPACE
