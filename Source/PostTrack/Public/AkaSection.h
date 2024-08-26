// Copyright AKaKLya 2024

#pragma once

#include "CoreMinimal.h"
#include "ISequencerSection.h"
#include "UObject/Object.h"


class UAkaPostSection;

class POSTTRACK_API FAkaSection : public ISequencerSection
{
public:
	UAkaPostSection* AkaPostSection;

	FAkaSection(UMovieSceneSection& InSection);

	//BEGIN ISequencerSection INTERFACE

	virtual int32 OnPaintSection(FSequencerSectionPainter& InPainter) const override;


	virtual UMovieSceneSection* GetSectionObject() override;

	virtual FText GetSectionTitle() const override;

	virtual float GetSectionHeight() const override;

	virtual bool IsReadOnly() const override;

	//END ISequencerSection INTERFACE
};
