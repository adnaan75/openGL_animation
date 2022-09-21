/*
Author: Mohammad Adnaan
Class: ECE6122
Last Date Modified: 07 Dec, 2021
Description:
Header file for UAV class and thread function
*/

#pragma once
#include<thread>
class ECE_UAV
{
public:
	ECE_UAV();
	void start(ECE_UAV* pUAV);
	void accelerationCalc();
	float mass = 1;
	float initialVelocity[3] = {0,0,0 };
	float velocity[3] = { 1,1,1 };
	float acceleration[3] = { 0,0,0 };
	float initialPos[3] = { 0,0,0 };
	float currentPosChange[3] = { 0,0,0 };
	float currentPos[3] = { 0,0,0 };
	float targetPos[3] = { 0,0,50 };
	float translation[3] = { 0,0,0 };
	float rotation[3] = { 0,0,0 };
	float surfacePosChange[3] = { 0,0,0 };
	
	float targetTime = 25.0;
	float gravityForce = 10.0;
	float gravityAcceleration = gravityForce / mass;

	std::thread uav;
	int stopThread = 0;


};

void threadFunction(ECE_UAV* pUAV);