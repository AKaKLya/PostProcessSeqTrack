// Fill out your copyright notice in the Description page of Project Settings.


#include "AkaPostTrackEditor.h"

#include "PostRuntime/Public/AkaPostTrack.h"
#include "AkaSection.h"
#include "ContentBrowserModule.h"
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

FAssetPickerConfig FAkaPostTrackEditor::CreateMatPickerConfig(FOnAssetSelected& inOnUMGAssetSelectedDelegate)
{
	FAssetPickerConfig AssetPickerConfig;

	AssetPickerConfig.OnAssetSelected = inOnUMGAssetSelectedDelegate;
	AssetPickerConfig.bAllowNullSelection = false;
	AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
	AssetPickerConfig.Filter.bRecursiveClasses = true;

	//UE 5.0
#if	ENGINE_MINOR_VERSION == 0

	AssetPickerConfig.Filter.ClassNames.Add("UserWidgetBlueprint");

	//UE 5.1 and up
#elif ENGINE_MINOR_VERSION >= 1

	AssetPickerConfig.Filter.ClassPaths.Add(UMaterialInstance::StaticClass()->GetClassPathName());
#endif

	
	AssetPickerConfig.SaveSettingsName = TEXT("SequencerAssetPicker");
	return AssetPickerConfig;
}

void FAkaPostTrackEditor::OnMatAssetSelected(const FAssetData& InAssetData)
{
	FSlateApplication::Get().DismissAllMenus();
	UMaterialInstance* SelectedUMGAsset = Cast<UMaterialInstance>(InAssetData.GetAsset());
	UMovieScene* MovieScene = GetFocusedMovieScene();
	if (!SelectedUMGAsset || !MovieScene)
		return;

	if (MovieScene->IsReadOnly())
		return;

	UAkaPostTrack* NewPostTrack = TryCreateMatSceneTrack(MovieScene, SelectedUMGAsset);


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
		PostTrack->SetDisplayName(FText::FromString(InMatInstance->GetName()));
	}


	FText SessionName = FText::FromString("Add Post Section");
	const FScopedTransaction ScopedTransaction(SessionName);

	InMovieScene->Modify();
	

	//Initialize the Evaluation Section
	PostTrack->AddNewMatSection(GetTimeForKey(),InMatInstance);


	return PostTrack;
	
}


#undef LOCTEXT_NAMESPACE
