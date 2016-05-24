#include "stdafx.h"

#include "WalkingController.h"
#include "RagDollApplication.h"
#include "State.h"
#include "Gains.h"

#include "dirent.h"
#include <fstream>

#include <limits.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include <iostream>
#include <fstream>


WalkingController::WalkingController()
{
}

WalkingController::WalkingController(RagDollApplication *app) {
	m_app = app;
	m_ragDollState = STATE_0;
	m_currentState = RESET;	
}

std::vector<State *> WalkingController::ReadStateFile() {

	DIR *dir;
	struct dirent *ent;
	std::string state_dir = "..\\..\\State Configurations";
	if ((dir = opendir(state_dir.c_str())) != NULL) {
		std::string state_ext = "cfg";
		
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_REG) {
				//printf("%s\n", ent->d_name);
				std::string fname = ent->d_name;
				if (fname.find(state_ext, (fname.length() - state_ext.length())) != std::string::npos) {
					std::stringstream ss;
					ss << state_dir << "\\" << ent->d_name;
					std::ifstream infile(ss.str());
					float torso, ull, url, lll, lrl, lf, rf;
					char c;
					//printf("States: \n");
					int state = 0;
					while ((infile >> torso >> c >> ull >> c >> url >> c >> lll >> c >> lrl >> c >> lf >> c >> rf) && (c == ',')) {
						//printf("%f, %f, %f, %f, %f, %f, %f \n", torso, ull, url, lll, lrl, lf, rf);
						// Set GLUI to read parameters
						switch (state)
						{
						case 0:
							m_state0 = new State(torso, ull, url, lll, lrl, lf, rf);
							break;
						case 1:
							m_state1 = new State(torso, ull, url, lll, lrl, lf, rf);
							break;
						case 2:
							m_state2 = new State(torso, ull, url, lll, lrl, lf, rf);
							break;
						case 3:
							m_state3 = new State(torso, ull, url, lll, lrl, lf, rf);
							break;
						case 4:
							m_state4 = new State(torso, ull, url, lll, lrl, lf, rf);
							break;
						default:
							break;
						}

						state++;
					}
				}
			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		// Initialize state to be zeros
		m_state0 = new State(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		m_state1 = new State(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		m_state2 = new State(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		m_state3 = new State(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		m_state4 = new State(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	}

	std::vector<State *> states = {m_state0, m_state1, m_state2, m_state3, m_state4};
	return states;

}

std::vector<Gains *>WalkingController::ReadGainsFile() {

	DIR *dir;
	struct dirent *ent;
	std::string state_dir = "..\\..\\State Configurations";
	if ((dir = opendir(state_dir.c_str())) != NULL) {
		std::string gains_ext = "gns";
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_REG) {
				//printf("%s\n", ent->d_name);
				std::string fname = ent->d_name;
				if (fname.find(gains_ext, (fname.length() - gains_ext.length())) != std::string::npos) {
					std::stringstream ss;
					ss << state_dir << "\\" << ent->d_name;
					std::ifstream infile(ss.str());
					float kp, kd;
					char c;
					int body = 0;
					printf("Gains: \n");
					while ((infile >> kp >> c >> kd) && (c == ',')) {
						//printf("%f, %f \n", kp, kd);
						// Set GLUI to read parameters
						switch (body)
						{
						case 0:
							// Torso
							m_torso_gains = new Gains(kp, kd, TORSO);
							break;
						case 1:
							// Upper Left Leg
							m_ull_gains = new Gains(kp, kd, UPPER_LEFT_LEG);
							break;
						case 2:
							// Upper right leg
							m_url_gains = new Gains(kp, kd, UPPER_RIGHT_LEG);
							break;
						case 3:
							// Lower Left leg
							m_lll_gains = new Gains(kp, kd, LOWER_LEFT_LEG);
							break;
						case 4:
							// Lower right leg
							m_lrl_gains = new Gains(kp, kd, LOWER_RIGHT_LEG);
							break;
						case 5:
							// Left foot
							m_lf_gains = new Gains(kp, kd, LEFT_FOOT);
							break;
						case 6:
							// Right foot
							m_rf_gains = new Gains(kp, kd, RIGHT_FOOT);
							break;
						default:
							break;
						}
						body++;
					}
				}
			}

		}
	}
	else {
		m_torso_gains = new Gains(0, 0, TORSO);
		m_ull_gains = new Gains(0, 0, UPPER_LEFT_LEG);
		m_url_gains = new Gains(0, 0, UPPER_RIGHT_LEG);
		m_lll_gains = new Gains(0, 0, LOWER_LEFT_LEG);
		m_lrl_gains = new Gains(0, 0, LOWER_RIGHT_LEG);
		m_lf_gains = new Gains(0, 0, LEFT_FOOT);
		m_rf_gains = new Gains(0, 0, RIGHT_FOOT);
	}

	std::vector<Gains *> gains = { m_torso_gains, m_ull_gains, m_url_gains, m_lll_gains, m_lrl_gains, m_lf_gains, m_rf_gains };
	return gains;

}

void WalkingController::SaveStates() {
	std::ofstream states_file;
	states_file.open("..\\..\\State Configurations\\states.cfg");
	std::vector<State *> states = { m_state1, m_state2, m_state3, m_state4 };
	for (std::vector<State *>::iterator it = states.begin(); it != states.end(); it++) {
		char buffer[100];
		sprintf_s(buffer, "%f, %f, %f, %f, %f, %f, %f\n", (*it)->m_torsoAngle, (*it)->m_upperLeftLegAngle, (*it)->m_upperRightLegAngle, (*it)->m_lowerLeftLegAngle, (*it)->m_lowerRightLegAngle, (*it)->m_leftFootAngle, (*it)->m_rightFootAngle);
		std::cout << buffer;
		states_file << buffer;
	}
	states_file.close();
}

void WalkingController::SaveGains() {

	std::ofstream gains_file;
	gains_file.open("..\\..\\State Configurations\\gains.gns");
	std::vector<Gains *> gains = { m_torso_gains, m_ull_gains, m_url_gains, m_lll_gains, m_lrl_gains, m_lf_gains, m_rf_gains };
	for (std::vector<Gains *>::iterator it = gains.begin(); it != gains.end(); it++) {
		char buffer[100];
		sprintf_s(buffer, "%f, %f \n", (*it)->m_kp, (*it)->m_kd);
		std::cout << buffer;
		gains_file << buffer;
	}
	gains_file.close();
}


WalkingController::~WalkingController()
{
}

#pragma region WALKER_INTERACTION

void WalkingController::Walk() {

	m_currentState = WALKING;

	switch (m_currentState)
	{
	case STATE_0:
		break;
	case STATE_1:
		break;
	case STATE_2:
		break;
	case STATE_3:
		break;
	default:
		break;
	}

}

void WalkingController::PauseWalking(){

	m_currentState = PAUSE;

}

void WalkingController::Reset(){
	m_ragDollState = STATE_0;
	m_currentState = RESET;
}

#pragma endregion WALKER_INTERACTION

#pragma region GAINS

void WalkingController::SetTorsoGains(float kp, float kd){}
void WalkingController::SetUpperLeftLegGains(float kp, float kd){}
void WalkingController::SetUpperRightLegGains(float kp, float kd){}
void WalkingController::SetLowerLeftLegGains(float kp, float kd){}
void WalkingController::SetLowerRightLegGains(float kp, float kd){}
void WalkingController::SetLeftFootGains(float kp, float kd){}
void WalkingController::SetRightFootGains(float kp, float kd){}

#pragma endregion GAINS

#pragma region STATES

void WalkingController::SetState1(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot){

	m_state1 = new State(torso, upperLeftLeg, upperRightLeg, lowerLeftLeg, lowerRightLeg, leftFoot, rightFoot);
	
}

void WalkingController::SetState2(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot){
	m_state2 = new State(torso, upperLeftLeg, upperRightLeg, lowerLeftLeg, lowerRightLeg, leftFoot, rightFoot);
}

void WalkingController::SetState3(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot){

	m_state3 = new State(torso, upperLeftLeg, upperRightLeg, lowerLeftLeg, lowerRightLeg, leftFoot, rightFoot);

}

void WalkingController::SetState4(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot){
	
	m_state4 = new State(torso, upperLeftLeg, upperRightLeg, lowerLeftLeg, lowerRightLeg, leftFoot, rightFoot);

}

#pragma endregion STATES