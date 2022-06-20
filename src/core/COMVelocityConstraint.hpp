#ifndef COMVELOCITYCONSTRAINT_HPP
#define COMVELOCITYCONSTRAINT_HPP

#include "CartesianConstraint.hpp"

namespace wbc{

/**
 * @brief Implementation of a Cartesian COM velocity constraint.
 */
class COMVelocityConstraint : public CartesianConstraint{
public:
    COMVelocityConstraint(ConstraintConfig config, uint n_robot_joints);
    virtual ~COMVelocityConstraint();

    /**
     * @brief Update the Cartesian reference input for this constraint.
     * @param ref Reference input for this constraint. Only the linear velocity part is relevant.
     */
    virtual void setReference(const base::samples::RigidBodyStateSE3& ref);
};

typedef std::shared_ptr<COMVelocityConstraint> COMVelocityConstraintPtr;

} // namespace wbc

#endif
