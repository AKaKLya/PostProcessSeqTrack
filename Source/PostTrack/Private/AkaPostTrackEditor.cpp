// Fill out your copyright notice in the Description page of Project Settings.


#include "AkaPostTrackEditor.h"

#include "PostRuntime/Public/AkaPostTrack.h"
#include "AkaSection.h"
#include "ContentBrowserModule.h"
#include "ISequencerModule.h"
#include "SequencerUtilities.h"
#include "Styling/SlateIconFinder.h"

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

TSharedPtr<SWidget> FAkaPostTrackEditor::BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track,
	const FBuildEditWidgetParams& Params)
{
	FOnGetContent OnAddMaterialClick = FOnGetContent::CreateSP(this, &FAkaPostTrackEditor::CreateMatPickerWidget);
	auto OutlinerEditWidget =FSequencerUtilities::MakeAddButton(LOCTEXT("AddMaterial", "Post Track"),
		OnAddMaterialClick, Params.NodeIsHovered, GetSequencer());
	return OutlinerEditWidget;
}

void FAkaPostTrackEditor::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
	FText ButtonLabel = FText::FromString("Aka Post Track");
	FText ButtonToolTip = FText::FromString("Adds Post Track");
	FSlateIcon Icon = FSlateIconFinder::FindIconForClass(UMaterialInstance::StaticClass());
	FNewMenuDelegate CreateSubMenuDelegate;
	CreateSubMenuDelegate.BindSP(this, &FAkaPostTrackEditor::CreateMatPickerSubMenu);
	MenuBuilder.AddSubMenu(ButtonLabel, ButtonToolTip, CreateSubMenuDelegate, false, Icon);
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

void FAkaPostTrackEditor::CreateMatPickerSubMenu(FMenuBuilder& SubMenuBuilder)
{
	TSharedRef<SWidget> MatContentBrowserWidget = CreateMatPickerWidget();
	SubMenuBuilder.AddWidget(MatContentBrowserWidget, FText::GetEmpty(), true);
}

TSharedRef<SWidget> FAkaPostTrackEditor::CreateMatPickerWidget()
{
	FOnAssetSelected OnMatSelectedDelegate;
	OnMatSelectedDelegate.BindRaw(this, &FAkaPostTrackEditor::OnMatAssetSelected);
	
	FAssetPickerConfig MatPickerConfig = CreateMatPickerConfig(OnMatSelectedDelegate);
	
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	
	TSharedRef<SWidget> MatContentBrowserWidget = ContentBrowserModule.Get().CreateAssetPicker(MatPickerConfig);
	
	TSharedRef<SBox> AssetPickerSBoxWidget = SNew(SBox);

	AssetPickerSBoxWidget->SetWidthOverride(300.0f);
	AssetPickerSBoxWidget->SetHeightOverride(300.f);

	//Set our AssetPickerWidget as the content for the SBox
	AssetPickerSBoxWidget->SetContent(MatContentBrowserWidget);


	return AssetPickerSBoxWidget;
}

FAssetPickerConfig FAkaPostTrackEditor::CreateMatPickerConfig(FOnAssetSelected& AssetSelectedDelegate)
{
	FAssetPickerConfig AssetPickerConfig;

	AssetPickerConfig.OnAssetSelected = AssetSelectedDelegate;
	AssetPickerConfig.bAllowNullSelection = false;
	AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
	AssetPickerConfig.Filter.bRecursiveClasses = true;

	AssetPickerConfig.Filter.ClassPaths.Add(UMaterialInstance::StaticClass()->GetClassPathName());
	
	AssetPickerConfig.SaveSettingsName = TEXT("SequencerAssetPicker");
	return AssetPickerConfig;
}

void FAkaPostTrackEditor::OnMatAssetSelected(const FAssetData& InAssetData)
{
	FSlateApplication::Get().DismissAllMenus();
	UMaterialInstance* SelectedMatAsset = Cast<UMaterialInstance>(InAssetData.GetAsset());
	UMovieScene* MovieScene = GetFocusedMovieScene();
	if (!SelectedMatAsset || !MovieScene)
		return;

	if (MovieScene->IsReadOnly())
		return;

	UAkaPostTrack* NewPostTrack = TryCreateMatSceneTrack(MovieScene, SelectedMatAsset);


	if (!NewPostTrack)
		return;

	if (GetSequencer().IsValid())
	{
		GetSequencer()->OnAddTrack(NewPostTrack, FGuid());
	}
}

UAkaPostTrack* FAkaPostTrackEditor::TryCreateMatSceneTrack(UMovieScene* InMovieScene, UMaterialInstance* InMatInstance)
{
	UAkaPostTrack* PostTrack = nullptr;
	
	for (UMovieSceneTrack* Track : InMovieScene->GetTracks())
	{
		PostTrack = Cast<UAkaPostTrack>(Track);
	}

	if(!PostTrack)
	{
		FText SessionName = FText::FromString("Create Post Master Track");
		const FScopedTransaction ScopedTransaction(SessionName);
		InMovieScene->Modify();

		PostTrack = InMovieScene->AddTrack<UAkaPostTrack>();
		check(PostTrack);
		PostTrack->SetDisplayName(FText::FromString("PostProcess"));
		InMovieScene->EventHandlers.Link(PostTrack);
	}


	FText SessionName = FText::FromString("Add Post Section");
	const FScopedTransaction ScopedTransaction(SessionName);

	InMovieScene->Modify();
	

	//Initialize the Evaluation Section
	PostTrack->AddNewMatSection(GetTimeForKey(),InMatInstance,InMovieScene);

	
	return PostTrack;
	
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
