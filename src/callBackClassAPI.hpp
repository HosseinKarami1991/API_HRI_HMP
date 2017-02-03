/*
 * callBackClassAPI.hpp
 *
 *  Created on: Dec 13, 2016
 *      Author: nasa
 */

#include <iostream>
#include <stdio.h>
#include <ros/ros.h>
#include <std_msgs/Char.h>
#include <std_msgs/String.h>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>


using namespace std;

class CallBackClassAPI {
	public:
		bool HMP_ack_flag;
		bool HMP_cognition_flag;
		bool HMP_initial_command_flag;
		bool HMP_parameter_cognition_flag;
		int NoParameterHMP;
		int NoOutputHMP;// Number of outputs
		float* HMPOutput;
		string* parameterHMP;

		CallBackClassAPI(int,int);
		void HMPAckCallBack(const std_msgs::String::ConstPtr& msg1);
		void HMPOutputCallBack(const std_msgs::String::ConstPtr& msg2);

	private:
		ros::NodeHandle nh;
		ros::Subscriber sub_HMPAck;
		ros::Subscriber sub_HMPOut;

};
