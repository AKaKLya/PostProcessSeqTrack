// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneTrackEditor.h"
#include "UObject/Object.h"
#include "IContentBrowserSingleton.h"


class UAkaPostTrack;

class POSTTRACK_API FAkaPostTrackEditor : public FMovieSceneTrackEditor
{

public:

	FAkaPostTrackEditor(TSharedRef<ISequencer>InSequencer);

	static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> InSequencer);
	

	virtual bool SupportsType(TSubclassOf<UMovieSceneTrack> TrackClass) const override;
	virtual TSharedPtr<SWidget> BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params) override;
	virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;
	virtual TSharedRef<ISequencerSection> MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding) override;
	
	virtual void ExtendObjectBindingTrackMenu(TSharedRef<FExtender> Extender, const TArray<FGuid>& ObjectBindings, const UClass* ObjectClass) override;
	
	void CreateMatPickerSubMenu(FMenuBuilder& SubMenuBuilder);
	TSharedRef<SWidget> CreateMatPickerWidget();
	FAssetPickerConfig CreateMatPickerConfig(FOnAssetSelected& AssetSelectedDelegate);
	void OnMatAssetSelected(const FAssetData& InAssetData);
	UAkaPostTrack* TryCreateMatSceneTrack(UMovieScene* InMovieScene, UMaterialInstance* InMatInstance);

	void ConstructObjectBindingTrackMenu(FMenuBuilder& MenuBuilder, TArray<FGuid> ObjectBindings);
	void HandleAddComponentMaterialActionExecute(APostProcessVolume* InPostActor,UMaterialInstance* InMatInstance);
};
