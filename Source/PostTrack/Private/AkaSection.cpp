// Fill out your copyright notice in the Description page of Project Settings.


#include "AkaSection.h"


#include "PostRuntime/Public/AkaPostSection.h"
#include "SequencerSectionPainter.h"

FAkaSection::FAkaSection(UMovieSceneSection& InSection)
{
	AkaPostSection = Cast<UAkaPostSection>(&InSection);

}

int32 FAkaSection::OnPaintSection(FSequencerSectionPainter& InPainter) const
{
	return InPainter.PaintSectionBackground(FLinearColor(0.2, 0.3, 0.2));
}

UMovieSceneSection* FAkaSection::GetSectionObject()
{
	return AkaPostSection;
}

FText FAkaSection::GetSectionTitle() const
{
	if(AkaPostSection)
		if(AkaPostSection->MatInstance)
			return FText::FromString(AkaPostSection->MatInstance->GetName());

	return FText();
}

float FAkaSection::GetSectionHeight() const
{
	return 40.f;
}

bool FAkaSection::IsReadOnly() const
{
	return false;
}
