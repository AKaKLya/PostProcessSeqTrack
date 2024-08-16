// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneStringChannel.h"
#include "Sections/MovieSceneHookSection.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AkaPostSection.generated.h"

USTRUCT()
struct FAkaVectorStruct
{
	GENERATED_BODY()
public: 
	UPROPERTY()
	FMovieSceneStringChannel Name;

	UPROPERTY()
	FMovieSceneFloatChannel R{};

	UPROPERTY()
	FMovieSceneFloatChannel G{};

	UPROPERTY()
	FMovieSceneFloatChannel B{};

	UPROPERTY()
	FMovieSceneFloatChannel A{};

	void SetName(const FString& InName)
	{
		this->Name.SetDefault(InName);
	}
	
	FAkaVectorStruct()
	{
		R.SetDefault(1.0f);
		G.SetDefault(1.0f);
		B.SetDefault(1.0f);
		A.SetDefault(1.0f);
	}
};


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
	FMovieSceneStringChannel ActorTagChannel;
	
	UPROPERTY()
	FAkaVectorStruct Vector1;

	UPROPERTY()
	FAkaVectorStruct Vector2;

	UPROPERTY()
	FAkaVectorStruct Vector3;
	
	//-------------Change this to Add Vector--------------//
	//UPROPERTY()
	//FAkaVectorStruct Vector4;
	
};
