// Copyright Epic Games, Inc. All Rights Reserved.

#include "PostTrack.h"
#include "AkaPostTrackEditor.h"
#include "ISequencerModule.h"
#define LOCTEXT_NAMESPACE "FPostTrackModule"

void FPostTrackModule::StartupModule()
{
	ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer");

	SequencerModule.RegisterTrackEditor(FOnCreateTrackEditor::CreateStatic(&FAkaPostTrackEditor::CreateTrackEditor));
}

void FPostTrackModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPostTrackModule, PostTrack)