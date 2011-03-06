/*
 * vision_position_kalman.c
 *
 *  Created on: 21.12.2010
 *      Author: Laurens Mackay
 */
#include "vision_position_kalman.h"
#include "kalman.h"

#include "debug.h"
#include "sensors.h"
#include "math.h"
#include "altitude_speed.h"
#include "transformation.h"
#include "gps_transformations.h"

#define VELOCITY_HOLD 0.999f
#define ACCELERATION_HOLD 0.99f

kalman_t vision_position_kalman_x;
kalman_t vision_position_kalman_y;
kalman_t vision_position_kalman_z;

void vision_position_kalman_init(void)
{
	//X Kalmanfilter
	//initalize matrices
#define TIME_STEP_X (1.0f / 200.0f)

	static m_elem kal_x_a[4 * 4] =
	{ 1.0f, TIME_STEP_X, TIME_STEP_X * TIME_STEP_X / 2.0f, 0,
	 0, VELOCITY_HOLD, TIME_STEP_X, 0 ,
	 0, 0, ACCELERATION_HOLD, 0 ,
	 0, 0, 0, 1.0f  };

	static m_elem kal_x_c[2*4] =
	{
	 1.0f, 0, 0, 0 ,
	 0, 0, 1.0f, 1.0f };

	static m_elem kal_x_gain[4 * 2] =
	{
			0.120089421679731,	4.35722593937886e-06,
			0.213207108111745,	0.00113300623965082,
			0.177947308239338,	0.772331751820938,
			-0.177890740554481,	0.000772735355118873};

	static m_elem kal_x_gain_start[4*2] =
	{
			0.184954631641736,	3.83974662133946e-14,
			0.0273267826361096,	1.76786246429369e-11,
			0.00201871900830449,	9.64639763760847e-08,
			-0.00201871900823153,	0.964639764723918};

	static m_elem kal_x_x_apriori[4*1] =
	{
	 0 ,
	 0 ,
	 0 ,
	 0  };

	static m_elem kal_x_x_aposteriori[4*1] =
	{
	 0 ,
	 0 ,
	 0 ,
	 0 };

	kalman_init(&vision_position_kalman_x, 4, 2, kal_x_a, kal_x_c,
			kal_x_gain_start, kal_x_gain, kal_x_x_apriori, kal_x_x_aposteriori,
			1000);




	//Y Kalmanfilter
	//initalize matrices
#define TIME_STEP_Y (1.0f / 200.0f)

	static m_elem kal_y_a[4 * 4] =
	{ 1.0f, TIME_STEP_Y, TIME_STEP_Y * TIME_STEP_Y / 2.0f, 0,
	 0, VELOCITY_HOLD, TIME_STEP_Y, 0 ,
	 0, 0, ACCELERATION_HOLD, 0 ,
	 0, 0, 0, 1.0f  };

	static m_elem kal_y_c[2*4] =
	{
	 1.0f, 0, 0, 0 ,
	 0, 0, 1.0f, 1.0f };

	static m_elem kal_y_gain[4 * 2] =
	{
			0.120089421679731,	4.35722593937886e-06,
			0.213207108111745,	0.00113300623965082,
			0.177947308239338,	0.772331751820938,
			-0.177890740554481,	0.000772735355118873};

	static m_elem kal_y_gain_start[4*2] =
	{
			0.184954631641736,	3.83974662133946e-14,
			0.0273267826361096,	1.76786246429369e-11,
			0.00201871900830449,	9.64639763760847e-08,
			-0.00201871900823153,	0.964639764723918};

	static m_elem kal_y_x_apriori[4*1] =
	{
	 0 ,
	 0 ,
	 0 ,
	 0  };

	static m_elem kal_y_x_aposteriori[4*1] =
	{
	 0 ,
	 0 ,
	 0 ,
	 0 };

	kalman_init(&vision_position_kalman_y, 4, 2, kal_y_a, kal_y_c,
			kal_y_gain_start, kal_y_gain, kal_y_x_apriori, kal_y_x_aposteriori,
			1000);




	//Altitude Kalmanfilter
	//initalize matrices
#define TIME_STEP_Z (1.0f / 200.0f)

	static m_elem kal_z_a[4 * 4] =
	{ 1.0f, TIME_STEP_Z, TIME_STEP_Z * TIME_STEP_Z / 2.0f, 0,
	 0, VELOCITY_HOLD, TIME_STEP_Z, 0 ,
	 0, 0, ACCELERATION_HOLD, 0 ,
	 0, 0, 0, 1.0f  };

	static m_elem kal_z_c[2*4] =
	{
	 1.0f, 0, 0, 0 ,
	 0, 0, 1.0f, 1.0f };


//	static m_elem kal_z_gain[4 * 2] =
//	{
//			0.120089421679731,	4.35722593937886e-06,
//			0.213207108111745,	0.00113300623965082,
//			0.177947308239338,	0.772331751820938,
//			-0.177890740554481,	0.000772735355118873};
	static m_elem kal_z_gain[4 * 2] =
	{
			0.120089421679731,	3.73444963864759e-08,
			0.213207108111745,	1.49106022715582e-05,
			0.177947308239338,	0.997017766710577,
			-0.177890740554481,	9.97097528182815e-08};

	static m_elem kal_z_gain_start[4*2] =
	{
			0.184954631641736,	3.83974662133946e-14,
			0.0273267826361096,	1.76786246429369e-11,
			0.00201871900830449,	9.64639763760847e-08,
			-0.00201871900823153,	0.964639764723918};


	static m_elem kal_z_x_apriori[4*1] =
	{
	 0 ,
	 0 ,
	 0 ,
	 -9.81  };

	static m_elem kal_z_x_aposteriori[4*1] =
	{
	 0 ,
	 0 ,
	 0 ,
	 -9.81 };

	kalman_init(&vision_position_kalman_z, 4, 2, kal_z_a, kal_z_c,
			kal_z_gain_start, kal_z_gain, kal_z_x_apriori, kal_z_x_aposteriori,
			1000);
}

void vision_position_kalman(void)
{
	//Transform accelerometer used in all directions
	float_vect3 acc_nav;
	body2navi(&global_data.accel_si, &global_data.attitude, &acc_nav);

	//X &Y Kalman Filter
	kalman_predict(&vision_position_kalman_x);
	kalman_predict(&vision_position_kalman_y);
	kalman_predict(&vision_position_kalman_z);

	m_elem x_measurement[2] =
	{ };
	m_elem x_mask[2] =
	{ 0, 1 };//only acceleromenters normaly
	m_elem y_measurement[2] =
	{ };
	m_elem y_mask[2] =
	{ 0, 1 };//only acceleromenters normaly
	m_elem z_measurement[2] =
	{ };
	m_elem z_mask[2] =
	{ 0, 1 };//only acceleromenters normaly

//	static int i = 0;
//	if (i++ == 200)
//	{
//		i = 0;
//		x_measurement[0]=0;
//		x_mask[0]=1;//simulate GPS position 0  at 1 Hz
	//	}	static int i = 0;
	if (global_data.state.vision_ok && global_data.vision_data.new_data)
	{
		global_data.vision_data.new_data=0;
		x_measurement[0] = global_data.vision_data.pos.x;
		x_mask[0] = 1;
		y_measurement[0] = global_data.vision_data.pos.y;
		y_mask[0] = 1;
		z_measurement[0] = global_data.vision_data.pos.z;
		z_mask[0] = 1;

		if (global_data.vision_data.new_data)
		{
			uint32_t vision_delay = (uint32_t) (global_data.vision_data.comp_end
					- global_data.vision_data.time_captured);
			// Debug Time for Vision Processing
			mavlink_msg_debug_send(global_data.param[PARAM_SEND_DEBUGCHAN], 100,
					(float) vision_delay);
		}
	}


	x_measurement[1] = acc_nav.x;
	y_measurement[1] = acc_nav.y;
	z_measurement[1] = acc_nav.z;

	//Put measurements into filter
	kalman_correct(&vision_position_kalman_x, x_measurement, x_mask);
	kalman_correct(&vision_position_kalman_y, y_measurement, y_mask);
	kalman_correct(&vision_position_kalman_z, z_measurement, z_mask);

		static int i=2;
		if(i++==4){
			i=0;
	//debug

	mavlink_msg_debug_send(global_data.param[PARAM_SEND_DEBUGCHAN], 50,
			z_measurement[1]);
	float_vect3 out_kal_z;
	out_kal_z.x = kalman_get_state(&vision_position_kalman_z,1);
	out_kal_z.y = kalman_get_state(&vision_position_kalman_z,2);
	out_kal_z.z = kalman_get_state(&vision_position_kalman_z,3);
	debug_vect("out_kal_z", out_kal_z);
		}

// save outputs
	global_data.position.x = kalman_get_state(&vision_position_kalman_x,0);
	global_data.position.y = kalman_get_state(&vision_position_kalman_y,0);
	global_data.position.z = kalman_get_state(&vision_position_kalman_z,0);

	global_data.velocity.x = kalman_get_state(&vision_position_kalman_x,1);
	global_data.velocity.y = kalman_get_state(&vision_position_kalman_y,1);
	global_data.velocity.z = kalman_get_state(&vision_position_kalman_z,1);



}
