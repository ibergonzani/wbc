#include <boost/test/unit_test.hpp> 
#include "../src/HierarchicalLSSolver.hpp"
#include <stdlib.h>

using namespace std;


BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
   srand (time(NULL));

   const int NO_JOINTS = 11;
   const int NO_CONSTRAINTS = 2;

   HierarchicalLSSolver solver;
   std::vector<unsigned int> ny_per_prio(2,NO_CONSTRAINTS);
   solver.configure(ny_per_prio, NO_JOINTS);

   Eigen::MatrixXd A(NO_CONSTRAINTS,NO_JOINTS);
   for(uint i = 0; i < NO_CONSTRAINTS*NO_JOINTS; i++ ) A.data()[i] = (rand()%1000)/1000.0;
   Eigen::VectorXd y(NO_CONSTRAINTS);
   y.setConstant(1);

   std::vector<Eigen::MatrixXd> A_prio;
   A_prio.push_back(A);
   for(uint i = 0; i < NO_CONSTRAINTS*NO_JOINTS; i++ ) A.data()[i] = (rand()%1000)/1000.0;
   A_prio.push_back(A);

   std::vector<Eigen::VectorXd> y_prio;
   for(uint i = 0; i < NO_CONSTRAINTS; i++ ) y.data()[i] = (rand()%1000)/1000.0;
   y_prio.push_back(y);
   for(uint i = 0; i < NO_CONSTRAINTS; i++ ) y.data()[i] = (rand()%1000)/1000.0;
   y_prio.push_back(y);

   cout<<"............Testing Hierarchical Solver "<<endl<<endl;
   cout<<"Number of priorities: "<<ny_per_prio.size()<<endl;
   cout<<"Constraints per priority: "; for(uint i = 0; i < ny_per_prio.size(); i++) cout<<ny_per_prio[i]<<" "; cout<<endl;
   cout<<"No of joints: "<<NO_JOINTS<<endl;
   cout<<"Solver Input: "<<endl;
   for(uint i = 0; i < ny_per_prio.size(); i++){
       cout<<"Priority: "<<i<<endl;
       cout<<"A: "<<endl; cout<<A_prio[i]<<endl;
       cout<<"y: "<<endl; cout<<y_prio[i]<<endl;
       cout<<endl;
   }

   Eigen::VectorXd solver_output;
   solver.solve(A_prio, y_prio, solver_output);

   cout<<"Output: "<<solver_output<<endl;
   cout<<"Test: "<<endl;
   for(uint i = 0; i < ny_per_prio.size(); i++){
       cout<<"Priority: "<<i<<endl;
       Eigen::VectorXd test = A_prio[i]*solver_output;
       cout<<"A*q: "<<test<<endl; cout<<endl;
       for(uint j = 0; j < NO_CONSTRAINTS; j++)
           BOOST_CHECK_EQUAL(fabs(test(j) - y_prio[i](j)) < 1e-9, true);
   }

   cout<<endl;
}
