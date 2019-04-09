#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>

#include "controllers/CartesianPotentialFieldsController.hpp"
#include "controllers/RadialPotentialField.hpp"
#include "controllers/PlanarPotentialField.hpp"

using namespace std;
using namespace ctrl_lib;

void runPotentialFieldController(std::string filename,
                                 CartesianPotentialFieldsController* ctrl,
                                 wbc::CartesianState start_pos,
                                 double cycleTime){

    FILE* fp = fopen(filename.c_str(), "w");

    cout << "Running potential field controller: " << endl << endl;
    cout << "Maximum Ctrl. Out. is " << ctrl->getMaxControlOutput().transpose()<< endl;
    cout << "Maximum influence distance is ";
    for(size_t i = 0; i < ctrl->getFields().size(); i++)
        cout << ctrl->getFields()[i]->influence_distance << " ";
    cout<<endl;
    cout << "Prop. Gain is " << ctrl->getPGain().transpose() << endl;
    sleep(1);

    wbc::CartesianState feedback = start_pos, control_output;
    std::vector<PotentialFieldPtr> fields = ctrl->getFields();

    for(uint i = 0; i < 10000; i++)
    {
        control_output = ctrl->update(feedback);

        for(uint j = 0; j < ctrl->getDimension(); j++) fprintf(fp, "%f ", feedback.pose.position(j));
        for(uint j = 0; j < fields.size(); j++)
            for(uint k = 0; k < fields[j]->dimension; k++)
                fprintf(fp, "%f ", fields[j]->pot_field_center(k));
        for(uint j = 0; j < ctrl->getDimension(); j++) fprintf(fp, "%f ", control_output.twist.linear(j));
        fprintf(fp, "\n");

        feedback.pose.position += control_output.twist.linear * cycleTime;

    }
    fclose(fp);
    cout << "Results have been saved in " << filename << "  in following order: x x0 y" << endl;
    cout << "  where: x      = Current position" << endl;
    cout << "         x0     = Potential field origin" << endl;
    cout << "         y      = Control output" << endl << endl;
}

int plot(std::string gnuplot_command){
    FILE* fp = fopen("gnuplotFile", "w");
    fprintf(fp,"%s\n", gnuplot_command.c_str());
    fclose(fp);
    return system("gnuplot -persist gnuplotFile");
}

BOOST_AUTO_TEST_CASE(radial_field)
{
    const double dim = 3;
    const double cycleTime = 0.01;
    const double propGain = 10;
    const double influence_distance = 10;

    PotentialFieldPtr field = std::make_shared<RadialPotentialField>(dim);
    field->influence_distance = influence_distance;
    field->pot_field_center << 0,0,0;

    std::vector<PotentialFieldPtr> fields;
    fields.push_back(field);

    base::VectorXd p_gain;
    p_gain.setConstant(dim, propGain);

    CartesianPotentialFieldsController controller;
    controller.setFields(fields);
    controller.setPGain(p_gain);

    wbc::CartesianState start_pos;
    start_pos.pose.position << 1,0,0;
    runPotentialFieldController("tmp.txt", &controller, start_pos, cycleTime);

    // Install gnuplot and uncomment to plot right away
    std::string cmd = "plot 'tmp.txt' using 1 with lines title 'Current position (x)', "
                      "'tmp.txt' using 4 with lines title 'Pot. Field center (x0)', "
                      "'tmp.txt' using 7 with lines title 'ctrl. out (y)'";
    plot(cmd);
}

BOOST_AUTO_TEST_CASE(constrained_radial_field)
{
    const uint dim = 3;
    const double cycleTime = 0.01;
    const double propGain = 10;
    const double influence_distance = 10;
    const double max_control_output = 0.2;

    PotentialFieldPtr field = std::make_shared<RadialPotentialField>(dim);
    field->influence_distance = influence_distance;
    field->pot_field_center<< 0,0,0;

    std::vector<PotentialFieldPtr> fields;
    fields.push_back(field);

    base::VectorXd prop_gain, max_ctrl_out;
    prop_gain.setConstant(dim, propGain);
    max_ctrl_out.setConstant(dim, max_control_output);

    CartesianPotentialFieldsController controller;
    controller.setFields(fields);
    controller.setPGain(prop_gain);
    controller.setMaxControlOutput(max_ctrl_out);

    wbc::CartesianState start_pos;
    start_pos.pose.position << 1,0,0;
    runPotentialFieldController("tmp.txt", &controller, start_pos, cycleTime);

    // Install gnuplot and uncomment to plot right away
    std::string cmd = "plot 'tmp.txt' using 1 with lines title 'Current position (x)',"
                      "'tmp.txt' using 4 with lines title 'Pot. Field center (x0)', "
                      "'tmp.txt' using 7 with lines title 'ctrl. out (y)'";
    plot(cmd);
}

BOOST_AUTO_TEST_CASE(planar_field){

    const int dim = 3;
    const double cycleTime = 0.01;
    const double influence_distance = 3.0;
    const double propGain = 0.1;
    const double max_control_output = 0.5;

    PlanarPotentialFieldPtr field = std::make_shared<PlanarPotentialField>();
    field->influence_distance = influence_distance;
    field->n.resize(3);
    field->pot_field_center << 0,0,0;
    field->n << 0,1,0;

    std::vector<PotentialFieldPtr> fields;
    fields.push_back(field);

    base::VectorXd prop_gain, max_ctrl_out;
    prop_gain.setConstant(dim, propGain);
    max_ctrl_out.setConstant(dim, max_control_output);


    CartesianPotentialFieldsController controller;
    controller.setFields(fields);
    controller.setPGain(prop_gain);
    controller.setMaxControlOutput(max_ctrl_out);

    wbc::CartesianState start_pos;
    start_pos.pose.position << 0,0.2,0;

    runPotentialFieldController("tmp.txt", &controller, start_pos, cycleTime);

    // Install gnuplot and uncomment to plot right away
    std::string cmd = "plot 'tmp.txt' using 1 with lines title 'Current x-position',"
                      "'tmp.txt' using 2 with lines title 'Current y-Position'";
    plot(cmd);
}

BOOST_AUTO_TEST_CASE(multi_radial_field)
{
    const uint dim = 3;
    const double cycleTime = 0.01;
    const double propGain = 0.1;
    const double maxCtrlOut = 0.1;

    PotentialFieldPtr field1 = std::make_shared<RadialPotentialField>(dim);
    field1->influence_distance = 0.3;
    field1->pot_field_center << 0, 0, 0;
    PotentialFieldPtr field2 = std::make_shared<RadialPotentialField>(dim);
    field2->influence_distance = 3;
    field2->pot_field_center << 2, 0.2, 0;

    std::vector<PotentialFieldPtr> fields;
    fields.push_back(field1);
    fields.push_back(field2);

    base::VectorXd prop_gain, max_ctrl_out;
    prop_gain.setConstant(dim, propGain);
    max_ctrl_out.setConstant(dim, maxCtrlOut);

    CartesianPotentialFieldsController controller;
    controller.setFields(fields);
    controller.setPGain(prop_gain);
    controller.setMaxControlOutput(max_ctrl_out);

    wbc::CartesianState start_pos;
    start_pos.pose.position << 0.0, 0.1, 0.0;
    runPotentialFieldController("tmp.txt", &controller, start_pos, cycleTime);

    std::string cmd = "plot 'tmp.txt' using 1:2 with lines title 'Current position',"
                      "'tmp.txt' using 4:5 title 'Pot. Field 1 center', "
                      "'tmp.txt' using 7:8 title 'Pot. Field 2 center'";

    // Install gnuplot and uncomment to plot right away
    plot(cmd);
}
