/*
 * interfacehmp.cPP
 *
 *  Created on: Dec 2, 2016
 *      Author: nasa
 */



//CPP
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <vector>
#include <ros/ros.h>
#include <string>
#include <algorithm>
#include <iterator>
#include <std_msgs/Char.h>
#include <std_msgs/String.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fstream>
#include <iostream>

#include "cognitionClassAPI.hpp"
#include "callBackClassAPI.hpp"


#define RST  "\x1B[0m"
#define KBLU  "\x1B[34m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define FBLU(x) KBLU x RST
#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define BOLD(x) "\x1B[1m" x RST
using namespace std;

int main(int argc, char **argv)
{
	ros::init(argc, argv, "API_HRI_HMP");
	ros::NodeHandle nh; //! nh:nodehandle variable of api_hri_hmp node
/*!
 *
 *  * write the commands to hmp detector from the hri
 *
 */
	ros::Publisher pub_HMP_cmnd	=nh.advertise<std_msgs::String>("HMPCommand",1);// SA_arrow No:5
	ros::Publisher pub_HRecAction	=nh.advertise<std_msgs::String>("HRecAction",1);//Human Recognized Action

	int noParamHMPInit=1;	//! Initial HMP Parameters: (method we use for recognizing human action)
	int noParamHMP=1+4;		//! Execution HMP Parameters		[0: set/get;	1-noOutputHMP:ModelsName; 1-4 Actions Parameters,]
	int noOutputHMP=4; //! Number of output from HMP node (No of models that HMP can recognize)

	cognitionClass obj_cognition(noOutputHMP);
	CallBackClassAPI obj_callback(noParamHMP,noOutputHMP);// callback class of subscribers

	float paramHMPInit [noParamHMPInit];//! Initial Parameters of HMP
	float paramHMP [noParamHMP];//!  Execution Parameters of HMP, first input: set Data: 1, or get Data:0

	int hri_ros_freq=80;//! ROS LOOP freq: hz
	ros::Rate loop_rate(hri_ros_freq);
	long int count=0;
	usleep(1e6);//! microsec, make the initialization.


	bool HMP_command_flag=true; //!
	bool Human_Gesture_Flag=true; // false from gesture recognized, until check completely gesture
	bool Gesture_Flag_Resolved=true;// false only if R should do sth, instead human invade Robot action, this is false until robot stop

	int HMP_count=0;// save count number for checking and changing flag
	char HMP_init_param_change;// check yes/no for changing initial questions of hmp/ctrl
	int HMP_type_no; // choosing type and mode of ctrl, hmp methods.(we currently have just one case)

	const char* DataLogPath	="/home/nasa/Datalog/AIIA";
	string DataLogPath2		="/home/nasa/Datalog/AIIA/0";
	mkdir(DataLogPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	struct timeval tp;
	unsigned long int ms0 ;
	ofstream Myfile1;
	string recogActionName;
	Myfile1.open ((DataLogPath2+"/38_Gestures.txt").c_str(),ios::app);

	cout<<"\n\n"<<BOLD(FGRN("Default HMP Type is GMM. If it is OK Press <y>,"
			" Otherwise Press <n>: y"));
//	cin>>HMP_init_param_change;
	// if HMP initial question:yes-> initial HMP flag to false.
	if (HMP_init_param_change=='n') {
		obj_callback.HMP_initial_command_flag=false;
		cout<<"Which HMP method do you want to use:"<<endl;
		cout<<"GMM: press 1:"<<endl<<"KF: press 2:"<<endl<<"NN: press 3 \t";

	//	cin>>HMP_type_no;
		paramHMPInit[0]=HMP_type_no;
	}

	while (ros::ok() ) {

/*!
 *
 * API Cognition:
 * 		Get name of the actions from HMP-detector
 * 		Get possibilities from HMP-detector
 *
 *
 */
		//!  Get the Action names from HMP-detector
		if (obj_callback.HMP_parameter_cognition_flag==false){
			for(int k1=0;k1<noOutputHMP;k1++)
				obj_cognition.hmp_ActionsName[k1]=obj_callback.parameterHMP[1+k1];
				obj_callback.HMP_parameter_cognition_flag=true;
			}

		/*!
		 *  Get the possibilities from HMP-detector to API
		 *	Call cognition function based on the input possibilities
		 */
		if (obj_callback.HMP_cognition_flag==false){
			obj_cognition.cognitionHMP_set(obj_callback.HMPOutput);
			obj_cognition.cognitionHMP();
			obj_callback.HMP_cognition_flag=true;
		}
		//! make reasoning on the output of pattern recognition:
		if (obj_cognition.cognitionHMP_flag==false){
			obj_cognition.reasoningHMP();
		}

		//! when an action recognized publish the action to hri
		if (obj_cognition.reasoningHMP_flag==false ){
			std_msgs::String msg_HRecAction;
			msg_HRecAction.data=obj_cognition.cognitionHMP_get();
			ROS_INFO("Publish Recognized Action:  %s",msg_HRecAction.data.c_str());
			pub_HRecAction.publish(msg_HRecAction);
			gettimeofday(&tp, NULL);
			ms0 = tp.tv_sec * 1000000 + tp.tv_usec ;
			recogActionName=msg_HRecAction.data.c_str();
			Myfile1 <<ms0<<" "<<recogActionName<<"\n";

		}


/*!
	 *
	 * Communication between API & HMP
	 *
*/

		/*!
		 * HMP Initial Parameters, we don't use this option now!
		 * if we want to another method instead of GMM/GMR we send command from here to HMP
		*/
		if (obj_callback.HMP_initial_command_flag==false) {

			obj_callback.NoParameterHMP=noParamHMPInit;
			std_msgs::String msg_HMPPar;
			std::stringstream ss_HMPPar;
			for (int ii=1; ii<= noParamHMPInit; ii++) {
					ss_HMPPar<<paramHMPInit[ii-1];
			}
			msg_HMPPar.data=ss_HMPPar.str();
			ROS_INFO("I publish HMP Initial command ...");
			pub_HMP_cmnd.publish(msg_HMPPar);


		}

		/*!
		 * 	Higher level HMP, here publish some numbers to HMP-detector,
		 * 	 later maybe use for sending some commands to HMP!
		 */
		if (count <1 && obj_callback.HMP_initial_command_flag==true) {
			HMP_command_flag=false;
			obj_callback.HMP_ack_flag=false;
			HMP_count=count;
			paramHMP[0]=0.0;
			for (int ii=2; ii<= noParamHMP; ii++) {
				 paramHMP[ii-1]=1.0/(1+ii);
			}
			obj_callback.NoParameterHMP=noParamHMP;
		}

		//! HMP-API, to be sure messages have been received.
		if (count>=HMP_count+5 && obj_callback.HMP_ack_flag==false) {
			HMP_command_flag=false;
			HMP_count=count;
		}

		 //!	API-HMP,Not Important, Publish HMP Parameter to HMP package
		if ( HMP_command_flag==false ) {
			std_msgs::String msg_HMPPar;
			std::stringstream ss_HMPPar;
			for (int ii=1; ii<= noParamHMP; ii++) {
				ss_HMPPar<<paramHMP[ii-1]<<" ";
			}
			cout<<endl;
			msg_HMPPar.data=ss_HMPPar.str();
			ROS_INFO("I publish HMP Parameters ...");
			pub_HMP_cmnd.publish(msg_HMPPar);
			HMP_command_flag=true;
		}

		if (count==0)	usleep(0.5e6);
		loop_rate.sleep();
		ros::spinOnce();
		count++;
}
	Myfile1.close();
return 0;
}

