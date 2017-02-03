/*
 * cognitionClassAPI.hpp
 *
 *  Created on: Dec 13, 2016
 *      Author: Kourosh Darvish
 */


#include <iostream>
#include <stdio.h>
#include <ros/ros.h>
#include <std_msgs/Char.h>
#include <std_msgs/String.h>
#include <string>
#include <iterator>
#include <sstream>
#include <algorithm>


using namespace std;

class cognitionClass {
	public:
//variables:
		bool 		cognitionHMP_flag;
		bool 		reasoningHMP_flag;
		bool		firstAction_flag;
		string*		hmp_ActionsName;	//Actions name of output of HMP detector.

//class functions:

		cognitionClass(int);						//constructor
		void cognitionHMP_set (float *);		//! Set the possibilities coming from HMP to a variable.
		void cognitionHMP (void);				//HMP & cognition
		void reasoningHMP (void);				//HMP & cognition
		string cognitionHMP_get (void);		//HMP & cognition get function
		//void get(float []);

	private:

		int 		cog_hmp_ActionNumber;
		int 		reas_hmp_ActionNumber;
		int 		reas_hmp_previous_ActionNumber;

		int 		NO_OUTPUT_HMP;
		float* 		hmpOutput;
		float*		HMP_posib_max;				// value of max possibility of each action.
		int 		HMP_max_element;			// the element number of the highest possibility until now from last check point
		float	 	percentage_check_min;		//when to check for update the action
		float* 		percentage_check_max;		//when to check for update the action
		int 		prev_max_element;			// save previous max_element we publish, to not publish two times!
		float 		prev_max_value; 			// save previous max value
		float		zero_threshold;				//threshold of considering zero value for possibilities
		bool 		min_flag;					// flag for checking min;
		bool* 		max_flag;					// flag for checking new HMP_posib_max[j] is bigger than current possibility of the previous max action.
		long int	time_counter;			// counter of time to know where the max happened
		long int* 	HMP_posib_max_time;		//location of each max.
		long int	last_check_point;			// to know when before checkpoint happened.
		int 		HMP_moment_zero_no;			// Number of actions that their probabilities are equal to zero in each moment
		int 		zero_counter;

};

