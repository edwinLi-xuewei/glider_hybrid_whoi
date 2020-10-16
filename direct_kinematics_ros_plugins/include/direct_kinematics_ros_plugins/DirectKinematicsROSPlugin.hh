// Copyright (c) 2020 The Dave Simulator Authors.
// All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file Direct kinematics ROS plugin for a ROS node

#ifndef __DIRECT_KINEMATICS_ROS_PLUGIN_HH__
#define __DIRECT_KINEMATICS_ROS_PLUGIN_HH__

#include <ros/ros.h>
#include "ros/callback_queue.h"
#include "ros/subscribe_options.h"
#include <gazebo_msgs/ModelState.h>
#include <gazebo_msgs/SetModelState.h>
#include <gazebo_msgs/GetModelState.h>
#include <gazebo_msgs/LinkState.h>
#include <gazebo_msgs/SetLinkState.h>
#include <geometry_msgs/TransformStamped.h>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2/LinearMath/Quaternion.h>

#include <std_msgs/Bool.h>
#include <uuv_sensor_ros_plugins_msgs/DVL.h>
#include <sensor_msgs/NavSatFix.h>

#include <frl_vehicle_msgs/UwGliderCommand.h>
#include <frl_vehicle_msgs/UwGliderStatus.h>

#include <boost/scoped_ptr.hpp>
#include <gazebo/gazebo.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/common/Plugin.hh>

#include <map>
#include <string>
#include <thread>

namespace direct_kinematics_ros
{

  class DirectKinematicsROSPlugin : public gazebo::ModelPlugin
  {
    /// \brief Constructor
    public: DirectKinematicsROSPlugin();

    /// \brief Destructor
    public: virtual ~DirectKinematicsROSPlugin();

    /// \brief Load module and read parameters from SDF.
    public: void Load(gazebo::physics::ModelPtr _model,
                      sdf::ElementPtr _sdf);

    /// \brief Initialize Module.
    public: virtual void Init();

    /// \brief Reset Module.
    public: virtual void Reset();

    /// \brief Update the simulation state.
    /// \param[in] _info Information used in the update event.
    public: virtual void Update(const gazebo::common::UpdateInfo &);

    /// \brief Connects the update event callback
    protected: virtual void Connect();

    /// \brief Update event
    protected: gazebo::event::ConnectionPtr updateConnection;

    /// \brief Convey model state from gazebo topic to outside
    protected: virtual void ConveyModelState();

    /// \brief Convey model state from gazebo topic to outside
    protected: virtual void ConveyCommands(const frl_vehicle_msgs::
                                UwGliderCommand::ConstPtr &_msg);

    /// \brief Convey model state from gazebo topic to outside (model)
    protected: virtual void ConveyModelCommand(const frl_vehicle_msgs::
                                    UwGliderCommand::ConstPtr &_msg);

    // /// \brief Convey link state from gazebo topic to outside (rudder)
    // protected: virtual void ConveyRudderVisualCommand
    //             (const frl_vehicle_msgs::UwGliderCommand::ConstPtr &_msg);

    /// \brief Pointer to the model structure
    protected: gazebo::physics::ModelPtr model;

    /// \brief Pointer to this ROS node's handle.
    private: boost::scoped_ptr<ros::NodeHandle> rosNode;

    /// \brief ROS Subscribers from outside
    private: ros::Subscriber commandSubscriber;

    /// \brief A ROS callbackqueue that helps process messages
    private: ros::CallbackQueue commandSubQueue;

    /// \brief A thread the keeps running the rosQueue
    private: std::thread commandSubQueueThread;

    /// \brief Subscribers for sensor data
    private: std::map<std::string, ros::Subscriber> sensorSubscribers;
    
    /// \brief update DVL sensor state
    protected: virtual void UpdateDVLSensorOnOff
                      (const std_msgs::Bool::ConstPtr &_msg);

    /// \brief flag for DVL OnOff status
    protected: bool DVLOnOff;

    /// \brief DVL sensor data
    protected: double sensorAltitude;

    /// \brief update DVL sensor data
    protected: virtual void UpdateDVLSensorData(
                          const uuv_sensor_ros_plugins_msgs::DVL::ConstPtr &_msg);
    
    /// \brief update GPS sensor state
    protected: virtual void UpdateGPSSensorOnOff
                      (const std_msgs::Bool::ConstPtr &_msg);

    /// \brief flag for GPS OnOff status
    protected: bool GPSOnOff;

    /// \brief GPS sensor data
    protected: double sensorLatitude;
    protected: double sensorLongitude;

    /// \brief update GPS sensor data
    protected: virtual void UpdateGPSSensorData(
                          const sensor_msgs::NavSatFix::ConstPtr &_msg);

    /// \brief ROS helper function that processes messages
    private: void commandSubThread();

    /// \brief ROS Publishers to gazebo_msgs topic
    private: std::map<std::string, ros::ServiceClient> commandPublisher;

    /// \brief ROS Subscriber from gazebo topic
    private: std::map<std::string, ros::ServiceClient> stateSubscriber;

    /// \brief update model state
    protected: virtual void updateModelState();

    /// \brief update model state
    protected: virtual ignition::math::Vector3<double> calculateRPY(
                              double x, double y, double z, double w);

    /// \brief ROS Publishers to outside
    private: ros::Publisher statePublisher;

    /// \brief Time at gazebo simulation
    protected: gazebo::common::Time time;

    // /// \brief Command starting time recorder
    // protected: gazebo::common::Time cmd_start_time;

    /// \brief Base link name
    protected: std::string base_link_name;

    // /// \brief Rudder link name
    // protected: std::string rudder_link_name;

    // /// \brief Rudder link exist bool
    // protected: bool rudderExist;

    /// \brief Model State 
    protected: gazebo_msgs::ModelState modelState;

    /// \brief Model Pose
    protected: ignition::math::Vector3<double> modelXYZ;
    protected: ignition::math::Vector3<double> modelRPY;

    /// \brief Motor power
    protected: double motorPower;

    /// \brief Rudder angle
    protected: double rudderAngle;

    // /// \brief Model State (in world reference frame)
    // protected: gazebo_msgs::ModelState modelState_AtWorldFrame;

    /// \brief CSV log writing stream for verifications
    protected: std::ofstream writeLog;
    protected: u_int64_t writeCounter;
    protected: bool writeLogFlag;
    protected: virtual void writeCSVLog();

    // private: geometry_msgs::TransformStamped nedTransform;

    // private: tf2_ros::TransformBroadcaster tfBroadcaster;

    private: std::map<std::string, geometry_msgs::TransformStamped> nedTransform;

    private: std::map<std::string, tf2_ros::TransformBroadcaster> tfBroadcaster;
  };
}

#endif  // __DIRECT_KINEMATICS_ROS_PLUGIN_HH__