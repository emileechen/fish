// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Agent.generated.h"

UCLASS()
class FISH_API AAgent : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	AAgent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	float sampleGaussian(float avg, float sd);
	
	
	virtual void Swim(TArray<AActor*> allNeighbors);
};
