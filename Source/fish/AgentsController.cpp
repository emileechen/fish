// Fill out your copyright notice in the Description page of Project Settings.

#include "fish.h"
#include "AgentsController.h"


// Sets default values
AAgentsController::AAgentsController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAgentsController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAgentsController::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

