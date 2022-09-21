/*
Author: Mohammad Adnaan
Class: ECE6122
Last Date Modified: 07 Dec, 2021
Description:
UAV class functions and thread function
*/
#include<ECE_UAV.h>
#include<thread>
#include<chrono>
#include <iostream>
#include<mutex>
#include<cmath>

std::mutex mtx;

ECE_UAV::ECE_UAV()
{
	
}

void ECE_UAV::start(ECE_UAV* pUAV)
{
	//initiating threads for each uav

	uav = std::thread(threadFunction, pUAV);
}

void ECE_UAV::accelerationCalc()
{
	//calculates the acclearation required for each uav to reach (0,0,50) location

	acceleration[0] = 2*(targetPos[0]-initialPos[0]- initialVelocity[0]* targetTime)/(targetTime* targetTime);
	acceleration[1] = 2 * (targetPos[1] - initialPos[1] - initialVelocity[1] * targetTime) / (targetTime * targetTime);
	acceleration[2] = gravityAcceleration+(2 * (1.2874*targetPos[2] - initialPos[2] - initialVelocity[2] * targetTime) / (targetTime * targetTime));
}



void threadFunction(ECE_UAV* pUAV)
{
	//function provided to each thread to control the kinematics of uavs

	int i = 0;
	int count = 0;
	int id = std::hash<std::thread::id>{}(std::this_thread::get_id());
	srand(id);
	float hookForce = 0.0;
	float rotationVelocity = rand()%7+3.0;
	float distance = 100.0;
	float hookForceUnitVector[3] = {0,0,0};
	float rotationNormalVector[3] = { 0,0,0 };
	float rotationDirectionVector[3] = { rand() % 2,rand() % 2,0 };
	float rotationDirectionVectorNormalized[3] = { 0,0,0 };
	float rotationUnit = 100;
	bool hookActivated = false;
	bool rotationActivated = false;

	float hooks = 55.0;
	float k = 0.018;
	float factor = 1.2874;
	float threshold = 10;

	std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point prevTime = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> currentTimeSec = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - beginTime);
	std::chrono::duration<double> prevTimeSec = std::chrono::duration_cast<std::chrono::duration<double>>(prevTime - beginTime);

	pUAV->currentPos[0] = pUAV->initialPos[0];
	pUAV->currentPos[1] = pUAV->initialPos[1];
	pUAV->currentPos[2] = pUAV->initialPos[2];


	while (1 && pUAV->stopThread==0)
	{
		//kinematic calculation of uav
		// calculates the position of uav every 10 millisec

		currentTime = std::chrono::steady_clock::now();
		std::chrono::duration<double> time_diff = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - prevTime);
		currentTimeSec = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - beginTime);


		pUAV->currentPosChange[0] = pUAV->initialVelocity[0] * time_diff.count() + 0.5 * pUAV->acceleration[0] * ((currentTimeSec.count() * currentTimeSec.count()) - (prevTimeSec.count() * prevTimeSec.count())) + pUAV->rotation[0]* time_diff.count();
		pUAV->currentPosChange[1] = pUAV->initialVelocity[1] * time_diff.count() + 0.5 * pUAV->acceleration[1] * ((currentTimeSec.count() * currentTimeSec.count()) - (prevTimeSec.count() * prevTimeSec.count())) + pUAV->rotation[1] * time_diff.count();
		pUAV->currentPosChange[2] = pUAV->initialVelocity[2] * time_diff.count() + 0.5 * (pUAV->acceleration[2] - pUAV->gravityAcceleration) * ((currentTimeSec.count() * currentTimeSec.count()) - (prevTimeSec.count() * prevTimeSec.count())) + pUAV->rotation[2] * time_diff.count();

		pUAV->velocity[0] = pUAV->initialVelocity[0] + pUAV->acceleration[0] * currentTimeSec.count();
		pUAV->velocity[1] = pUAV->initialVelocity[1] - pUAV->acceleration[1] * currentTimeSec.count();
		pUAV->velocity[2] = pUAV->initialVelocity[2] + pUAV->acceleration[2] * currentTimeSec.count();
		

		pUAV->translation[0] = pUAV->translation[0] + factor * pUAV->currentPosChange[0];
		pUAV->translation[1] = pUAV->translation[1] - factor * pUAV->currentPosChange[1];
		pUAV->translation[2] = pUAV->translation[2] + factor * pUAV->currentPosChange[2];

		pUAV->currentPos[0] = pUAV->currentPos[0] +  pUAV->currentPosChange[0];
		pUAV->currentPos[1] = pUAV->currentPos[1] +  pUAV->currentPosChange[1];
		pUAV->currentPos[2] = pUAV->currentPos[2] + pUAV->currentPosChange[2];

		distance = std::sqrt(std::pow(pUAV->currentPos[0] - pUAV->targetPos[0], 2) + std::pow(pUAV->currentPos[1] - pUAV->targetPos[1], 2) + std::pow(pUAV->currentPos[2] - factor * pUAV->targetPos[2], 2));


		if (pUAV->translation[2]> hooks )
		{

			hookForce = k* (distance- threshold);

			if (abs(hookForce) < 0.02)
			{
				rotationActivated = true;

			}


			hookForceUnitVector[0] = (pUAV->targetPos[0] - pUAV->currentPos[0]) / distance;
			hookForceUnitVector[1] = (pUAV->targetPos[1] - pUAV->currentPos[1]) / distance;
			hookForceUnitVector[2] = (factor * pUAV->targetPos[2] - pUAV->currentPos[2]) / distance;
			hookActivated = true;


			pUAV->acceleration[0] = hookForce* hookForceUnitVector[0];
			pUAV->acceleration[1] = hookForce * hookForceUnitVector[1];
			pUAV->acceleration[2] = hookForce * hookForceUnitVector[2]+ pUAV->gravityAcceleration;

		}

		if (rotationActivated)
		{
			rotationNormalVector[0] = -( pUAV->targetPos[0] -  pUAV->currentPos[0] ) ;
			rotationNormalVector[1] = -( pUAV->targetPos[1] -  pUAV->currentPos[1] ) ;
			rotationNormalVector[2] = -(pUAV->targetPos[2]*factor -  pUAV->currentPos[2]) ;

			float number = 80;

			if (count<= number)
			{
				rotationDirectionVector[1] = rand() % 300 + 100;
				rotationDirectionVector[2] = rand() % 300 + 100;
				rotationDirectionVector[0] = -(rotationDirectionVector[1] * rotationNormalVector[1] + rotationDirectionVector[2] * rotationNormalVector[2]) / (rotationNormalVector[0] + 0.0001);
				rotationUnit = std::sqrt(std::pow(rotationDirectionVector[0], 2) + std::pow(rotationDirectionVector[1], 2) + std::pow(rotationDirectionVector[2], 2)) + 0.0001;

				rotationDirectionVectorNormalized[0] = rotationDirectionVector[0] / rotationUnit;
				rotationDirectionVectorNormalized[1] = rotationDirectionVector[1] / rotationUnit;
				rotationDirectionVectorNormalized[2] = rotationDirectionVector[2] / rotationUnit;
			}
			if(count > number && count<= 2*number)
			{
				rotationDirectionVector[0] = rand() % 300 + 100;
				rotationDirectionVector[2] = -(rand() % 300 + 100);
				rotationDirectionVector[1] = -(rotationDirectionVector[0] * rotationNormalVector[0] + rotationDirectionVector[2] * rotationNormalVector[2]) / (rotationNormalVector[1] + 0.0001);
				rotationUnit = std::sqrt(std::pow(rotationDirectionVector[0], 2) + std::pow(rotationDirectionVector[1], 2) + std::pow(rotationDirectionVector[2], 2)) + 0.0001;

				rotationDirectionVectorNormalized[0] = rotationDirectionVector[0] / rotationUnit;
				rotationDirectionVectorNormalized[1] = rotationDirectionVector[1] / rotationUnit;
				rotationDirectionVectorNormalized[2] = rotationDirectionVector[2] / rotationUnit;
				
			}

			if (count> 2 * number && count <= 3 * number)
			{
				rotationDirectionVector[0] = -(rand() % 300 + 100);
				rotationDirectionVector[1] = -(rand() % 300 + 100);
				rotationDirectionVector[2] = -(rotationDirectionVector[0] * rotationNormalVector[0] + rotationDirectionVector[1] * rotationNormalVector[1]) / (rotationNormalVector[2] + 0.0001);
				rotationUnit = std::sqrt(std::pow(rotationDirectionVector[0], 2) + std::pow(rotationDirectionVector[1], 2) + std::pow(rotationDirectionVector[2], 2)) + 0.0001;

				rotationDirectionVectorNormalized[0] = rotationDirectionVector[0] / rotationUnit;
				rotationDirectionVectorNormalized[1] = rotationDirectionVector[1] / rotationUnit;
				rotationDirectionVectorNormalized[2] = rotationDirectionVector[2] / rotationUnit;
			}

			if (count > 3 * number)
			{
				count = 0;
			}





			pUAV->rotation[0] = rotationVelocity * rotationDirectionVectorNormalized[0];
			pUAV->rotation[1] = rotationVelocity * rotationDirectionVectorNormalized[1];
			pUAV->rotation[2] = rotationVelocity * rotationDirectionVectorNormalized[2] ;
		}
		prevTime = currentTime;
		prevTimeSec = currentTimeSec;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		count = count + 1;
	}
}
