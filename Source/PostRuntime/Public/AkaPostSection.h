// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneStringChannel.h"
#include "Sections/MovieSceneHookSection.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AkaPostSection.generated.h"

UCLASS()
class POSTRUNTIME_API UAkaPostSection : public UMovieSceneHookSection
{
	GENERATED_BODY()
public:
	UAkaPostSection(const FObjectInitializer& ObjInit);
	UPROPERTY()
	UMaterialInstance* MatInstance;
	
	virtual EMovieSceneChannelProxyType CacheChannelProxy() override;
	virtual UWorld* GetWorld() const override;
	
	virtual void Begin	(IMovieScenePlayer* Player, const UE::MovieScene::FEvaluationHookParams& Params) const override;
	virtual void Update	(IMovieScenePlayer* Player, const UE::MovieScene::FEvaluationHookParams& Params) const override;
	virtual void End	(IMovieScenePlayer* Player, const UE::MovieScene::FEvaluationHookParams& Params) const override;

	void CancelMaterialLink();

private:
	bool bIsCancelLink = false;
	
	UPROPERTY(Transient)
	mutable UWorld* World=nullptr;

	UPROPERTY(Transient)
	mutable UMaterialInstanceDynamic* DynamicMat = nullptr;
	
	mutable bool bShouldTick = false;

	UPROPERTY(Transient)
	mutable TArray<AActor*> ActorWithTag;
	
	UPROPERTY()
	FMovieSceneFloatChannel Vector1R{};
	UPROPERTY()
	FMovieSceneFloatChannel Vector1G{};
	UPROPERTY()
	FMovieSceneFloatChannel Vector1B{};
	UPROPERTY()
	FMovieSceneFloatChannel Vector1A{};
	
	UPROPERTY()
	FMovieSceneFloatChannel Vector2R{};
	UPROPERTY()
	FMovieSceneFloatChannel Vector2G{};
	UPROPERTY()
	FMovieSceneFloatChannel Vector2B{};
	UPROPERTY()
	FMovieSceneFloatChannel Vector2A{};
	
	UPROPERTY()
	FMovieSceneFloatChannel Vector3R{};
	UPROPERTY()
	FMovieSceneFloatChannel Vector3G{};
	UPROPERTY()
	FMovieSceneFloatChannel Vector3B{};
	UPROPERTY()
	FMovieSceneFloatChannel Vector3A{};

	UPROPERTY()
	FMovieSceneStringChannel Vector1Name;
	UPROPERTY()
	FMovieSceneStringChannel Vector2Name;
	UPROPERTY()
	FMovieSceneStringChannel Vector3Name;
	
	UPROPERTY()
	FMovieSceneStringChannel CameraTagChannel;
	
};
