// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCameraShake.h"


UMyCameraShake::UMyCameraShake()
{
	OscillationDuration = 0.25f;
	OscillationBlendInTime = 0.05f;
	OscillationBlendOutTime = 0.05f;

	RotOscillation.Pitch.Amplitude = FMath::RandRange(5.0f, 10.0f);
	RotOscillation.Pitch.Frequency = FMath::RandRange(5.0f, 10.0f);
}

void UMyCameraShake::InitCameraShakeParam()
{
	OscillationDuration = 0.25f;
	OscillationBlendInTime = 0.05f;
	OscillationBlendOutTime = 0.05f;

	RotOscillation.Pitch.Amplitude = FMath::RandRange(5.0f, 10.0f);
	RotOscillation.Pitch.Frequency = FMath::RandRange(5.0f, 10.0f);
}

void UMyCameraShake::Test()
{
}
