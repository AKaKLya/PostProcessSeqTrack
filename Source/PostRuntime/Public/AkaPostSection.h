// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Sections/MovieSceneHookSection.h"
#include "AkaPostSection.generated.h"

/**
 * 
 */
UCLASS()
class POSTRUNTIME_API UAkaPostSection : public UMovieSceneHookSection
{
	GENERATED_BODY()

	

protected:
	UPROPERTY()
	UAkaPostSection* This = nullptr;
	
	UPROPERTY(Transient)
	UWorld* RenderingWorld=nullptr;

	UPROPERTY()
	FMovieSceneFloatChannel FloatCurve0{};
	UPROPERTY()
	FMovieSceneFloatChannel FloatCurve1{};
	UPROPERTY()
	FMovieSceneFloatChannel FloatCurve2{};
	UPROPERTY()
	FMovieSceneFloatChannel FloatCurve3{};
	UPROPERTY()
	FMovieSceneFloatChannel FloatCurve4{};
	UPROPERTY()
	FMovieSceneFloatChannel FloatCurve5{};
	UPROPERTY()
	FMovieSceneFloatChannel FloatCurve6{};
	UPROPERTY()
	FMovieSceneFloatChannel FloatCurve7{};
	UPROPERTY()
	FMovieSceneFloatChannel FloatCurve8{};
	UPROPERTY()
	FMovieSceneFloatChannel FloatCurve9{};


public:

	UAkaPostSection(const FObjectInitializer& ObjInit);
	
	UPROPERTY()
	TObjectPtr<UMaterialInstance> MatInstance;
	

	//HOOK
	virtual EMovieSceneChannelProxyType CacheChannelProxy() override;
	virtual void Begin(IMovieScenePlayer* Player, const UE::MovieScene::FEvaluationHookParams& Params) const override;
	virtual void Update(IMovieScenePlayer* Player, const UE::MovieScene::FEvaluationHookParams& Params) const override;
	virtual void End(IMovieScenePlayer* Player, const UE::MovieScene::FEvaluationHookParams& Params) const override;
	//HOOK

	virtual UWorld* GetWorld() const override;
	
	double GetPlayerTimeAsSeconds(const UE::MovieScene::FEvaluationHookParams& Params) const;
	double GetSectionLocalTime(const UE::MovieScene::FEvaluationHookParams& Params) const;
};
