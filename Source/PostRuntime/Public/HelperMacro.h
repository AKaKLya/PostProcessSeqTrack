// Copyright AKaKLya 2024
#pragma once
#include "Channels/MovieSceneChannelEditorData.h"


#define INIT_VECTOR_VALUE(INDEX) \
Vector##INDEX##Name.SetDefault("Post" #INDEX); \
Vector##INDEX##R.SetDefault(1.f); \
Vector##INDEX##G.SetDefault(1.f); \
Vector##INDEX##B.SetDefault(1.f); \
Vector##INDEX##A.SetDefault(1.f)

#define ADD_VECTOR_DATA(Vector) \
PostChannelProxyData.Add(Vector.Name);\
PostChannelProxyData.Add(Vector.R);\
PostChannelProxyData.Add(Vector.G);\
PostChannelProxyData.Add(Vector.B);\
PostChannelProxyData.Add(Vector.A)

#define SET_VECTOR_PARAMETER(Vector,INDEX) \
const float Vec##INDEX##R = GetEvaluateValue(Vector.R, EvalTime); \
const float Vec##INDEX##G = GetEvaluateValue(Vector.G, EvalTime); \
const float Vec##INDEX##B = GetEvaluateValue(Vector.B, EvalTime); \
const float Vec##INDEX##A = GetEvaluateValue(Vector.A, EvalTime); \
const FName Vec##INDEX = FName(*Vector.Name.Evaluate(EvalTime)); \
DynamicMat->SetVectorParameterValue(Vec##INDEX, FLinearColor(Vec##INDEX##R, Vec##INDEX##G, Vec##INDEX##B, Vec##INDEX##A));


#if WITH_EDITOR

#define ADD_VECTOR_DATA_EDITOR(Vector,DataIndex) \
PostChannelProxyData.Add(Vector.Name, EditorData.Data[DataIndex], TMovieSceneExternalValue<FString>()); \
PostChannelProxyData.Add(Vector.R, EditorData.Data[DataIndex + 1], TMovieSceneExternalValue<float>()); \
PostChannelProxyData.Add(Vector.G, EditorData.Data[DataIndex + 2], TMovieSceneExternalValue<float>()); \
PostChannelProxyData.Add(Vector.B, EditorData.Data[DataIndex + 3], TMovieSceneExternalValue<float>()); \
PostChannelProxyData.Add(Vector.A, EditorData.Data[DataIndex + 4], TMovieSceneExternalValue<float>())

inline void MetaDataInitial(FMovieSceneChannelMetaData& Data,FString InName, FString InDisplayText, FString InGroup,int Sort)
{
	Data.Group = FText::FromString(InGroup);
	Data.Name = *InName;
	Data.DisplayText = FText::FromString(InDisplayText);
	Data.SortOrder = Sort;
}

#define INIT_METADATA(DataIndex, VectorIndex) \
\
MetaDataInitial(Data[DataIndex],	 "Vector" #VectorIndex "Name", "Name",	"Vector" #VectorIndex, DataIndex); \
MetaDataInitial(Data[DataIndex + 1], "Vector" #VectorIndex "R",	 "R",		"Vector" #VectorIndex, DataIndex + 1); \
MetaDataInitial(Data[DataIndex + 2], "Vector" #VectorIndex "G",	 "G",		"Vector" #VectorIndex, DataIndex + 2); \
MetaDataInitial(Data[DataIndex + 3], "Vector" #VectorIndex "B",	 "B",		"Vector" #VectorIndex, DataIndex + 3); \
MetaDataInitial(Data[DataIndex + 4], "Vector" #VectorIndex "A",	 "A",		"Vector" #VectorIndex, DataIndex + 4)
#endif