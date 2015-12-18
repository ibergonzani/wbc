#include <kdl/frames_io.hpp>
#include <boost/test/unit_test.hpp>
#include <kdl_parser/kdl_parser.hpp>
#include "../src/WbcVelocity.hpp"
#include "../src/HierarchicalWDLSSolver.hpp"
#include "../src/KinematicModel.hpp"

using namespace std;
using namespace wbc;

/**
 * Test hierarchical solver with random input data
 */

BOOST_AUTO_TEST_CASE(solver)
{
    srand (time(NULL));

    const uint NO_JOINTS = 3;
    const uint NO_CONSTRAINTS = 2;
    const double NORM_MAX = 5.75;

    HierarchicalWDLSSolver solver;
    std::vector<int> ny_per_prio(1,NO_CONSTRAINTS);
    BOOST_CHECK(solver.configure(ny_per_prio, NO_JOINTS) == true);
    solver.setNormMax(NORM_MAX);

    std::vector<LinearEquationSystem> input(1);
    input[0].resize(NO_CONSTRAINTS, NO_JOINTS);

    for(uint i = 0; i < NO_CONSTRAINTS*NO_JOINTS; i++ )
        input[0].A.data()[i] = (rand()%1000)/1000.0;

    for(uint i = 0; i < NO_CONSTRAINTS; i++ )
        input[0].y_ref.data()[i] = (rand()%1000)/1000.0;

    input[0].W_row.setConstant(1);
    input[0].W_col.setConstant(1);

    cout<<"............Testing Hierarchical Solver "<<endl<<endl;
    cout<<"Number of priorities: "<<ny_per_prio.size()<<endl;
    cout<<"Constraints per priority: "; for(uint i = 0; i < ny_per_prio.size(); i++) cout<<ny_per_prio[i]<<" "; cout<<endl;
    cout<<"No of joints: "<<NO_JOINTS<<endl;
    cout<<"\n----------------------- Solver Input ----------------------"<<endl<<endl;
    for(uint i = 0; i < ny_per_prio.size(); i++){
        cout<<"Priority: "<<i<<endl;
        cout<<"A: "<<endl; cout<<input[i].A<<endl;
        cout<<"y_ref: "<<endl; cout<<input[i].y_ref<<endl;
        cout<<endl;
    }

    Eigen::VectorXd solver_output;
    try{
        solver.solve(input,  solver_output);
    }
    catch(std::exception e){
        BOOST_ERROR("Solver.solve threw an exception");
    }

    cout<<"----------------- Solver Output: ------------------------"<<endl;
    cout<<"q_ref = "<<endl;
    cout<<solver_output<<endl;
    cout<<"\nTest: "<<endl;
    for(uint i = 0; i < ny_per_prio.size(); i++){
        cout<<"----------------- Priority: "<<i<<" ------------------"<<endl;
        Eigen::VectorXd test = input[i].A*solver_output;
        cout<<"A * q_ref: "<<endl;
        cout<<test<<endl; cout<<endl;
        for(uint j = 0; j < NO_CONSTRAINTS; j++)
            BOOST_CHECK(fabs(test(j) - input[i].y_ref(j)) < 1e-9);
    }

    cout<<"\n............................."<<endl;
}

BOOST_AUTO_TEST_CASE(kinematic_model){

    KDL::Tree robot_tree;
    BOOST_CHECK(kdl_parser::treeFromFile("/home/dfki.uni-bremen.de/dmronga/rock/bundles/kuka_lbr/data/urdf/kuka_lbr.urdf", robot_tree) == true);

    KDL::Tree obj_tree;
    BOOST_CHECK(kdl_parser::treeFromFile("/home/dfki.uni-bremen.de/dmronga/rock_aila/bundles/iss_demo/data/urdf/iss_drawer.urdf", obj_tree) == true);

    KinematicModel model;
    BOOST_CHECK(model.addTree(robot_tree, base::samples::RigidBodyState()) == true);
    BOOST_CHECK(model.addTree(obj_tree, base::samples::RigidBodyState(), "kuka_lbr_top_right_camera") == true);

    BOOST_CHECK(model.addTaskFrame("iss_drawer_base") == true);
    const TaskFrame &tf = model.getTaskFrame("iss_drawer_base");

    base::samples::Joints joint_state;
    joint_state.resize(tf.joint_names.size());
    joint_state.names = tf.joint_names;
    for(uint i = 0; i < tf.joint_names.size(); i++)
        joint_state[i].position = 0;
    model.updateJoints(joint_state);

    std::cout<<"Initial Object Pose: "<<std::endl;
    std::cout<<tf.pose.p<<std::endl;
    double qx,qy,qz,qw;
    tf.pose.M.GetQuaternion(qx,qy,qz,qw);
    std::cout<<"qx: "<<qx<<" qy: "<<qy<<" qz: "<<qz<<" qw: "<<qw<<std::endl;

    std::cout<<"Initial Jacobian: "<<std::endl;
    std::cout<<tf.jacobian.data<<std::endl;

    base::samples::RigidBodyState segment_pose;
    segment_pose.position.setZero();
    segment_pose.position(2) = 0.5;
    segment_pose.orientation.setIdentity();
    model.updateLink(segment_pose);
    joint_state.elements.pop_back();
    joint_state.names.pop_back();
    model.updateJoints(joint_state);

    std::cout<<"Modified Object Pose: "<<std::endl;
    std::cout<<tf.pose.p<<std::endl;
    tf.pose.M.GetQuaternion(qx,qy,qz,qw);
    std::cout<<"qx: "<<qx<<" qy: "<<qy<<" qz: "<<qz<<" qw: "<<qw<<std::endl;

    std::cout<<"Modified Jacobian: "<<std::endl;
    std::cout<<tf.jacobian.data<<std::endl;
}
