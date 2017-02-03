/*
 * callBackClassAPI.cpp
 *
 *  Created on: Dec 13, 2016
 *      Author: nasa
 */


#include "callBackClassAPI.hpp"

CallBackClassAPI::CallBackClassAPI(int NoParameterHMP1,int NoOutputHMP1) {

	HMP_cognition_flag			=true;
	HMP_ack_flag				=true;
	HMP_initial_command_flag	=true;

	HMP_parameter_cognition_flag=true;
	sub_HMPAck	=nh.subscribe("HMPAck",80, &CallBackClassAPI::HMPAckCallBack, this);
	sub_HMPOut	=nh.subscribe("HMPOutput",1, &CallBackClassAPI::HMPOutputCallBack, this);
	NoParameterHMP=NoParameterHMP1;
	parameterHMP=new string [NoParameterHMP];

	for (int ii=0;ii<NoParameterHMP;ii++)
	{	parameterHMP[ii]="0";
	}

	NoOutputHMP=NoOutputHMP1;//NoOutputHMP
	HMPOutput=new float[NoOutputHMP];

	for (int ii=0;ii<NoOutputHMP;ii++)
	{	HMPOutput[ii]=0.0;
	}

}

//	HMP
void CallBackClassAPI::HMPAckCallBack(const std_msgs::String::ConstPtr& msg) {
	ROS_INFO("I heard HMP acknowledge ...");

	if(HMP_initial_command_flag==false) {
		HMP_initial_command_flag=true;
	}
	else {

		string MSG=msg-> data.c_str();
		istringstream ss (MSG);
		copy(
				istream_iterator <string>(ss),
				istream_iterator <string> (),
				parameterHMP);
		HMP_ack_flag=true;
		HMP_parameter_cognition_flag=false;// related to HMP-cognition flag;

	}
}

void CallBackClassAPI::HMPOutputCallBack(const std_msgs::String::ConstPtr& msg) {
	string MSG=msg-> data.c_str();
	for (int ii=0;ii<NoOutputHMP;ii++)
	{
		HMPOutput[ii]=0.0;
	}
	istringstream ss (MSG);
	copy(
			istream_iterator <float>(ss),
			istream_iterator <float> (),
			HMPOutput);
	HMP_cognition_flag=false;
}
