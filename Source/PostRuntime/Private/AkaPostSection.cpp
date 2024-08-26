// Copyright AKaKLya 2024


#include "AkaPostSection.h"

#include "Materials/MaterialInstance.h"
#include "HelperMacro.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "IMovieScenePlayer.h"
#include "Engine/World.h"
#include "Engine/PostProcessVolume.h"
#include "Channels/MovieSceneStringChannel.h"
#include "Channels/MovieSceneFloatChannel.h"

#define LOCTEXT_NAMESPACE "PostSection"


using namespace UE::MovieScene;

#if WITH_EDITOR
struct FPostStruct
{
	FMovieSceneChannelMetaData Data[15];
	FPostStruct()
	{
		INIT_METADATA(0, 1);
		INIT_METADATA(5, 2);
		INIT_METADATA(10, 3);
		//-------------Add Vector Parameter-----Step 2---------//
		// Step1 is in UAkaPostSection
		// INIT_METADATA(15, 4);
	}
};
#endif


UAkaPostSection::UAkaPostSection(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	bRequiresRangedHook = true;
	bRequiresTriggerHooks = true;
	EvalOptions.CompletionMode = EMovieSceneCompletionMode::RestoreState;
	MatInstance= nullptr;
	
	Vector1.SetName("Post1");
	Vector2.SetName("Post2");
	Vector3.SetName("Post3");
	
	//-------------Add Vector Parameter-----Step 3---------//
	// Vector4.SetName("Post4");
}


EMovieSceneChannelProxyType UAkaPostSection::CacheChannelProxy()
{
	FMovieSceneChannelProxyData PostChannelProxyData;

#if WITH_EDITOR
	FPostStruct EditorData;
	
	
	ADD_VECTOR_DATA_EDITOR(Vector1,0);
	ADD_VECTOR_DATA_EDITOR(Vector2,5);
	ADD_VECTOR_DATA_EDITOR(Vector3,10);
	
	//-------------Add Vector Parameter-----Step 4---------//
	//ADD_VECTOR_DATA_EDITOR(Vector4, 15);
	
#else
	ADD_VECTOR_DATA(Vector1);
	ADD_VECTOR_DATA(Vector2);
	ADD_VECTOR_DATA(Vector3);
	//-------------Add Vector Parameter-----Step 5---------//
	//ADD_VECTOR_DATA(Vector4);
	
#endif
	ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(PostChannelProxyData));
	return EMovieSceneChannelProxyType::Dynamic;
}


void UAkaPostSection::Begin(IMovieScenePlayer* Player, const FEvaluationHookParams& Params) const
{
	const auto PlayerWorld= Player->GetPlaybackContext()->GetWorld();
	if(PlayerWorld->WorldType==EWorldType::PIE || PlayerWorld->WorldType == EWorldType::Game)
	{
		World = PlayerWorld;
	}
	else
	{
		World = nullptr;
	}
	
	bShouldTick = false;
	
	if(PostActor)
	{
		TArray<FWeightedBlendable> MatArray = PostActor->Settings.WeightedBlendables.Array;
	
		PostActor->Settings.WeightedBlendables.Array.Empty();
		if(MatInstance)
		{
			PostActor->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,MatInstance));
		}
		
	}
	
	PostActor = nullptr;
	for (TWeakObjectPtr<> BoundObject : Player->FindBoundObjects(PostActorGuid, Params.SequenceID))
	{
		if (APostProcessVolume* FindPostActor = Cast<APostProcessVolume>(BoundObject.Get()))
		{
			PostActor = FindPostActor;
			break;
		}
	}
	if(PostActor == nullptr)
	{
		//UE_LOG(LogTemp,Warning,TEXT("PostActor Not Found"));
		return;
	}


	if(DynamicMat == nullptr)
	{
		if(MatInstance)
		{
			auto MatName = MatInstance->GetName() + "_Animated";
			DynamicMat = UMaterialInstanceDynamic::Create(MatInstance, nullptr, FName(MatName));
		}
	}

	if(PostActor)
	{
		if(PostActor->Settings.WeightedBlendables.Array[0].Object != DynamicMat)
		{
			PostActor->Settings.WeightedBlendables.Array.Empty();
			PostActor->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,DynamicMat));
		}
		bShouldTick = true;
	}
	else
	{
		bShouldTick = false;
	}
	
	
}

inline float UAkaPostSection::GetEvaluateValue(const FMovieSceneFloatChannel& FloatChannel, const FFrameTime& Time) const
{
	float Value = 0.f;
	FloatChannel.Evaluate(Time,Value);
	return Value;
}

void UAkaPostSection::Update(IMovieScenePlayer* Player, const FEvaluationHookParams& Params) const
{
	if(bShouldTick == false)
	{
		return ;
	}
	
	if(DynamicMat)
	{
		const FFrameTime EvalTime = Params.Context.GetTime();
		SET_VECTOR_PARAMETER(Vector1,1);
		SET_VECTOR_PARAMETER(Vector2,2);
		SET_VECTOR_PARAMETER(Vector3,3);
		
		//-------------Add Vector Parameter-----Step 6---------//
		//  This is the final Step!! 
		//SET_VECTOR_PARAMETER(Vector4,4);
	}
}

void UAkaPostSection::End(IMovieScenePlayer* Player, const FEvaluationHookParams& Params) const
{
	
}

void UAkaPostSection::CancelMaterialLink()
{
	if(PostActor != nullptr)
	{
		PostActor->Settings.WeightedBlendables.Array.Empty();
		if(MatInstance)
		{
			PostActor->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,MatInstance));
		}
	}
}



UWorld* UAkaPostSection::GetWorld() const
{
	return World;
};




#undef LOCTEXT_NAMESPACE
