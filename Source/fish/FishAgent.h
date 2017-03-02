// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Agent.h"

#include "GameFramework/Actor.h"
#include "FishAgent.generated.h"

//General Log
DECLARE_LOG_CATEGORY_EXTERN(YourLog, Log, All);

UCLASS()
class FISH_API AFishAgent : public AAgent
{
	GENERATED_BODY()
	
public:	

	static float timestep;


	FVector force_net;
	static float force_max;

	static int num_neighbors;
	static float body_length;
	
	// Zone of Separation
	static float radius_s;
	static float blindangle_back_s;

	// Zone of Attraction
	static float max_radius_a;				// Body length
	static float blindangle_back_a;
	static float blindangle_front_a;

	// Zone of Cohesion
	static float max_radius_c;
	float radius_c;
	static float blindangle_back_c;

	static float cruise_speed;

	// Weights
	static int weight_s;
	static int weight_a;
	static int weight_c;
	static float relaxation_time;
	static int pitch_control;
	static int roll_control;
	static float noise_factor;

	// Smoothness of radius adaptation
	static float adaptivity; 
	// Influence of a single neighbor; aka n_w
	static float neighbor_influence;

	// Sets default values for this actor's properties
	AFishAgent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
	UFUNCTION(BlueprintCallable, Category=Movement)
	void Swim(TArray<AActor*> allNeighbors) override;

	UFUNCTION(BlueprintCallable, Category=Movement)
	float CalcCohesionRadius();

	UFUNCTION(BlueprintCallable, Category=Movement)
	float GetSpeed();

	UFUNCTION(BlueprintCallable, Category=Init)
	void SetBodyLength(float bl);


};
