#include "COMVelocityConstraint.hpp"
#include <base-logging/Logging.hpp>
#include <base/samples/RigidBodyStateSE3.hpp>

namespace wbc{

COMVelocityConstraint::COMVelocityConstraint(ConstraintConfig config, uint n_robot_joints)
    : CartesianConstraint(config, n_robot_joints){
}

COMVelocityConstraint::~COMVelocityConstraint(){
}

void COMVelocityConstraint::setReference(const base::samples::RigidBodyStateSE3& ref){

    if(!ref.hasValidTwist()){
        LOG_ERROR("Constraint %s has invalid velocity and/or angular velocity", config.name.c_str())
        throw std::invalid_argument("Invalid constraint reference value");
    }

    if(ref.time.isNull())
        this->time = base::Time::now();
    else
        this->time = ref.time;
    this->y_ref.segment(0,3) = ref.twist.linear;
}

} // namespace wbc
