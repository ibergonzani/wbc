#ifndef JOINT_LIMIT_VELOCITY_HARD_CONSTRAINT_HPP
#define JOINT_LIMIT_VELOCITY_HARD_CONSTRAINT_HPP

#include "../core/HardConstraint.hpp"

#include <base/Eigen.hpp>
#include <base/Time.hpp>
#include <base/NamedVector.hpp>
#include <memory>

namespace wbc{

/**
 * @brief Abstract class to represent a generic hard constraint for a WBC optimization problem.
 */
class JointLimitsVelocityHardConstraint : public HardConstraint {
public:

    /** @brief Default constructor */
    JointLimitsVelocityHardConstraint() = default;

    JointLimitsVelocityHardConstraint(double dt);

    virtual ~JointLimitsVelocityHardConstraint() = default;

    virtual void update(RobotModelPtr robot_model) override;

protected:

    /** Control timestep: used to integrate and differentiate velocities */
    double dt;

};
typedef std::shared_ptr<JointLimitsVelocityHardConstraint> JointLimitsVelocityHardConstraintPtr;

} // namespace wbc
#endif