// Copyright Epic Games, Inc. All Rights Reserved.

#include "PostTrack.h"

#include "AkaPostTrack.h"
#include "AkaPostTrackEditor.h"
#include "ISequencerModule.h"
#include "LevelSequence.h"
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

}

void FPostTrackModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPostTrackModule, PostTrack)