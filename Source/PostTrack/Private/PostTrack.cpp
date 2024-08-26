// Copyright AKaKLya 2024

#include "PostTrack.h"
#include "AkaPostTrack.h"
#include "AkaPostTrackEditor.h"
#include "ISequencerModule.h"
#include "LevelSequence.h"
#include "ILevelSequenceModule.h"
#define LOCTEXT_NAMESPACE "FPostTrackModule"

void FPostTrackModule::StartupModule()
{
	ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");

	SequencerModule.RegisterTrackEditor(FOnCreateTrackEditor::CreateStatic(&FAkaPostTrackEditor::CreateTrackEditor));

	auto AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	AssetEditorSubsystem->OnAssetClosedInEditor().AddLambda([this](UObject* Asset,IAssetEditorInstance* Instance)
	{
		if(ULevelSequence* SeqObj = Cast<ULevelSequence>(Asset))
		{
			UMovieScene* MovieScene = SeqObj->MovieScene;
			TArray<UMovieSceneTrack*> Tracks = MovieScene->GetTracks();
			for(auto Track : Tracks)
			{
				if(auto AkaPostTrack = Cast<UAkaPostTrack>(Track))
				{
					AkaPostTrack->CancelMaterialLink();
				}
			}
		}
	});

	ILevelSequenceModule& LevelSequenceModule = FModuleManager::LoadModuleChecked<ILevelSequenceModule>("LevelSequence");
	
	PostTrackHandle = LevelSequenceModule.OnNewActorTrackAdded().AddRaw(this,&FPostTrackModule::AddPostProcessTracks);

}

void FPostTrackModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("LevelSequence"))
	{
		ILevelSequenceModule& LevelSequenceModule = FModuleManager::LoadModuleChecked<ILevelSequenceModule>("LevelSequence");
		LevelSequenceModule.OnNewActorTrackAdded().Remove(PostTrackHandle);
	}
}

void FPostTrackModule::AddPostProcessTracks(const AActor& SourceActor, const FGuid& Binding,TSharedPtr<ISequencer> Sequencer)
{
	if (!SourceActor.IsA<APostProcessVolume>() )
	{
		return;
	}

	const APostProcessVolume* PPActorConst = Cast<APostProcessVolume>(&SourceActor);
	APostProcessVolume* PPActor = const_cast<APostProcessVolume*>(PPActorConst);
	
	if(PPActor)
	{
		const UMovieSceneSequence* Sequence = Sequencer->GetFocusedMovieSceneSequence();
		UMovieScene* MovieScene = Sequence->GetMovieScene();
	
		if (MovieScene->FindSpawnable(Binding))
		{
			return;
		}

		FGuid ActorID = Sequencer->FindObjectId(*PPActor, Sequencer->GetFocusedTemplateID());
		FString ActorName = PPActor->GetActorNameOrLabel();
		
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
		if(PostMat)
		{
			const FFrameRate FrameResolution = MovieScene->GetTickResolution();
			const FFrameTime SpawnSectionStartTime = Sequencer->GetLocalTime().ConvertTo(FrameResolution);
			
			UAkaPostTrack* PostTrack = MovieScene->AddTrack<UAkaPostTrack>();
			PostTrack->SetDisplayName(FText::FromString(ActorName+"_Mat"));
		
			MovieScene->EventHandlers.Link(PostTrack);
			MovieScene->Modify();
			PostTrack->AddNewMatSection(SpawnSectionStartTime, PostMat, MovieScene,ActorID);
		
			
		}
		
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPostTrackModule, PostTrack)