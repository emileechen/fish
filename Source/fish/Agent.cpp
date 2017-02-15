// Fill out your copyright notice in the Description page of Project Settings.

#include "fish.h"
#include "Agent.h"


// Sets default values
AAgent::AAgent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAgent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAgent::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

