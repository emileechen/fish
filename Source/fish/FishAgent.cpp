// Fill out your copyright notice in the Description page of Project Settings.

#include "fish.h"
#include "FishAgent.h"

//General Log
DEFINE_LOG_CATEGORY(YourLog);

float AFishAgent::timestep = 0.05;
int AFishAgent::num_neighbors = 100;
float AFishAgent::adaptivity = 0.3;
float AFishAgent::neighbor_influence = 0.1;

float AFishAgent::body_length = 3;	// cm/BL

// 2 BL/s = 2 * BL/s * s/tick = 2 BL/tick
float AFishAgent::cruise_speed = 2 * AFishAgent::body_length;

// Zone of Separation
float AFishAgent::radius_s = 2 * AFishAgent::body_length;
float AFishAgent::blindangle_back_s = 60;	// degrees

// Zone of Attraction
float AFishAgent::max_radius_a = 5 * AFishAgent::body_length;
float AFishAgent::blindangle_back_a = 60;	// degrees
float AFishAgent::blindangle_front_a = 60;

// Zone of Cohesion
float AFishAgent::max_radius_c = 15 * AFishAgent::body_length;
float AFishAgent::blindangle_back_c = 90;

// Weights
int AFishAgent::weight_s = 10;
int AFishAgent::weight_a = 5;
int AFishAgent::weight_c = 9;
float AFishAgent::relaxation_time = 0.2;	// s = s * s/tick = /tick
int AFishAgent::pitch_control = 2;
int AFishAgent::roll_control = 5;
float AFishAgent::noise_factor = 0.5;

float AFishAgent::force_max = 3 * FMath::Square(AFishAgent::body_length);		// cm^2 / s^2

// Sets default values
AFishAgent::AFishAgent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	force_net = GetActorForwardVector();

	// force_net = FVector(1.0, 1.0, 0.0);
	// force_net = FVector(FMath::RandRange((float)-1, (float)1.0), FMath::RandRange((float)-1, (float)1.0), 0.0);
	
	// UE_LOG(YourLog,Warning,TEXT("force net starts as %s"), *force_net.ToString());
}

void AFishAgent::SetBodyLength(float bl) {
	AFishAgent::body_length = bl;
	AFishAgent::radius_s *= bl;
	AFishAgent::max_radius_a *= bl;
	AFishAgent::max_radius_c *= bl;
	AFishAgent::force_max *= bl;

	radius_c = AFishAgent::max_radius_c;
}

// Called when the game starts or when spawned
void AFishAgent::BeginPlay()
{
	Super::BeginPlay();

	// FTimerHandle TimerHandle;
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFishAgent::TimeTick, AFishAgent::timestep, true);
}

// Called every frame
void AFishAgent::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

float AFishAgent::GetSpeed() {
	return force_net.Size();
}

// Calculate adapted perception radius for cohesion forces
float AFishAgent::CalcCohesionRadius() {
	// float s = adaptivity;

	// // density-dependent term
	// float R_i = max_radius_c - (neighbor_influence * num_neighbors);
	// float lerp = FMath::Lerp(radius_c, R_i, s);
	// UE_LOG(YourLog,Warning,TEXT("R_i=%f, radius_s=%f, lerp=%f"), R_i, radius_s, lerp);
	// radius_c = fmax(radius_s, lerp);

	// UE_LOG(YourLog,Warning,TEXT("radius_c is %f"), radius_c);

	// return radius_c;


	UE_LOG(YourLog,Warning,TEXT("radius_c is %f"), max_radius_c);

	return max_radius_c;
}


// a = forward or back vector, b = vector to neighbour, angle = max limit of angle from a
bool AFishAgent::CheckWithinAngle(FVector a, FVector b, float angle) {
	float dot = FVector::DotProduct(a.GetSafeNormal2D(), b.GetSafeNormal2D());		// a.b = cos(theta)
	float rad = FMath::DegreesToRadians(angle);
	float limit = FMath::Cos(rad);
	UE_LOG(YourLog,Warning,TEXT("a = %s; b = %s, lim = %f, dot = %f, limit = %f"), *a.ToString(), *b.ToString(), angle, dot, limit);
	// returns true if in the blind angle
	return dot > limit;
}


void AFishAgent::Swim(TArray<AActor*> allNeighbors) {
	float delta_time = FApp::GetDeltaTime();	// seconds elapsed	s/tick
	float bl = AFishAgent::body_length;		// cm/BL

	FVector myPos = GetActorLocation();
	float mySpeed = GetSpeed() * delta_time;	// cm/s -> cm/tick

	UE_LOG(YourLog,Warning,TEXT("self at %s"), *myPos.ToString());
	UE_LOG(YourLog,Warning,TEXT("speed is %f"), mySpeed);

	int num_s = 0;
	int num_a = 0;
	int num_c = 0;
	FVector dir_s = FVector(0.0);
	FVector dir_a = FVector(0.0);
	FVector dir_c = FVector(0.0);

	num_neighbors = allNeighbors.Num();
	UE_LOG(YourLog,Warning,TEXT("%d Neighbors in radius:"), num_neighbors);

	for (AActor* neighbor : allNeighbors) {

		// Do not consider yourself as a neighbour
		FVector pos = neighbor->GetActorLocation();
		if (pos == myPos) continue;


		UE_LOG(YourLog,Warning,TEXT("-------"));
		UE_LOG(YourLog,Warning,TEXT("neighbor at %s"), *pos.ToString());

		FVector diff_vector = pos - myPos;		// vector pointing towards neighbour

		// float dot = FVector::DotProduct(force_net, diff_vector);
		// float diff_cos = dot / (mySpeed * diff_vector.Size());
		// UE_LOG(YourLog,Warning,TEXT("neighbor dot is %f"), dot);
		// UE_LOG(YourLog,Warning,TEXT("neighbor angle is %f"), diff_cos);

		float distance = diff_vector.Size();
		UE_LOG(YourLog,Warning,TEXT("neighbor is %f away"), distance);

		// if (diff_cos < blindangle_back_a) {
		// 	UE_LOG(YourLog,Warning,TEXT("blind angle at %f"), diff_cos);
		// 	continue;
		// }

		// SEPARATION
		if (distance < radius_s) {	// If neighbour is in the separation radius
			UE_LOG(YourLog,Warning,TEXT("Repulsion"));
			num_s += 1;
			dir_s += diff_vector / FMath::Square(distance);
		}

		// ALIGNMENT
		else if (distance < max_radius_a) {
			UE_LOG(YourLog,Warning,TEXT("Alignment"));
			if (AFishAgent::CheckWithinAngle(-1 * force_net, diff_vector, blindangle_back_a)) {
				UE_LOG(YourLog,Warning,TEXT("blind angle back"));
				continue;
			} else if (~ AFishAgent::CheckWithinAngle(force_net, diff_vector, blindangle_front_a)) {
				UE_LOG(YourLog,Warning,TEXT("blind angle front"));
				continue;
			}
			num_a += 1;
			dir_a += neighbor->GetActorForwardVector();
		}

		// COHESION
		else {
			UE_LOG(YourLog,Warning,TEXT("Cohesion"));
			if (AFishAgent::CheckWithinAngle(-1 * force_net, diff_vector, blindangle_back_c)) {
				UE_LOG(YourLog,Warning,TEXT("blind angle back"));
				continue;
			}
			num_c += 1;
			dir_c += diff_vector.GetUnsafeNormal();
		}
	}

	UE_LOG(YourLog,Warning,TEXT("-------"));

	FVector e_x = GetActorForwardVector();
	UE_LOG(YourLog,Warning,TEXT("forward vector: %s"), *e_x.ToString());

	FVector force_s;
	FVector force_a;
	FVector force_c;

	if (num_s == 0) {	// No fish in repulsion radius
		force_s = FVector(0);
	} else {
		dir_s = - (1.0 / (float)num_s) * dir_s;
		force_s = weight_s * dir_s.GetSafeNormal();
	}
	
	if (num_a == 0) {	// No fish in alignment radius
		force_a = FVector(0);
	} else {
		dir_a = - (1.0 / (float)num_a) * dir_a;
		force_a = weight_a * (dir_a - e_x).GetSafeNormal();
	}
	
	if (num_c == 0) {	// No fish in cohesion radius
		force_c = FVector(0);
	} else {
		dir_c = - (1.0 / (float)num_c) * dir_c;
		force_c = weight_c * dir_c.GetSafeNormal();
	}
	
	FVector force_speed = (1 / relaxation_time) * (cruise_speed - mySpeed) * e_x;

	FVector z = FVector(0, 0, 1);
	FVector force_pc = -pitch_control * FVector::DotProduct(e_x, z) * z;
	FVector force_rc = -roll_control * FVector::DotProduct(GetActorRightVector(), z) * z;

	FVector force_rand = FVector(0);

	UE_LOG(YourLog,Warning,TEXT("separation:	dir: %s,	force: %s"), *dir_s.ToString(), *force_s.ToString());
	UE_LOG(YourLog,Warning,TEXT("attraction:	dir: %s,	force: %s"), *dir_a.ToString(), *force_a.ToString());
	UE_LOG(YourLog,Warning,TEXT("cohesion:	dir: %s,	force: %s"), *dir_c.ToString(), *force_c.ToString());

	UE_LOG(YourLog,Warning,TEXT("speed: %s"), *force_speed.ToString());
	UE_LOG(YourLog,Warning,TEXT("pitch control: %s"), *force_pc.ToString());
	UE_LOG(YourLog,Warning,TEXT("roll control: %s"), *force_rc.ToString());
	UE_LOG(YourLog,Warning,TEXT("random: %s"), *force_rand.ToString());


	force_net = force_s + force_a + force_c + force_speed + force_pc + force_rc + force_rand;

	UE_LOG(YourLog,Warning,TEXT("force net is %s"), *force_net.ToString());
	if (force_net.Size() > force_max * FMath::Square(delta_time)) {
		force_net = force_net.GetUnsafeNormal() * force_max * FMath::Square(delta_time);
	}

	UE_LOG(YourLog,Warning,TEXT("offset is %s"), *force_net.ToString());

	UE_LOG(YourLog,Warning,TEXT("delta time is %f"), delta_time);
	UE_LOG(YourLog,Warning,TEXT("-------------------------------------------"));
	AddActorWorldOffset(force_net);
}
