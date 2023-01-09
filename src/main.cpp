#include <iostream>
#include <string>
#include "myObject.hpp"
using namespace  std;
#include <vector>
#include <unordered_map>
//  define global parameters
int CS_WIDTH = 7100;
int CS_HEIGHT = 6600;
int M3_WIDTH =  440;
int M3_SPACING = 310;
int M4_WIDTH =  1000;
int M4_SPACING = 490;

int CS_X1_TO_DRAIN = 1260;
int CS_Y1_TO_DRAIN = 4100;
string CS_LIB_NAME = "MSBCS";
string VIA34_LIB_NAME = "Via34";




// ##### Step 2: create CS array #####
int Dx = CS_WIDTH + M3_SPACING *3 + M3_WIDTH * 2;
int Dy = CS_HEIGHT + M4_SPACING * 2 + M4_WIDTH;
int offy = M4_SPACING + M4_WIDTH;


int main(int argc, char **argv)
{
    // ##### Step 1: create die boundary #####
    string design_name = "CS_APR";
    int die_x1 = 0;
    int die_y1 = 0;
    int die_x2 = CS_WIDTH * 4 + M3_SPACING * (3*4-1) + M3_WIDTH *2 *4;
    int die_y2 = CS_HEIGHT *4 + M4_SPACING * (2*4-1) + M4_WIDTH *1*4;
    auto die = Die(design_name, die_x1, die_y1, die_x2, die_y2);
    // ##### Step 2: create CS array #####
    int Dx = CS_WIDTH + M3_SPACING *3 + M3_WIDTH * 2; 
    int Px = CS_HEIGHT + M4_SPACING * 2 + M4_WIDTH; 
    int offy = M4_SPACING + M4_WIDTH;
    // unordered_map <int, <Component>> cs_array ;
    vector <vector<Component>> cs_array;
    // cs_array.resize(4, vector<Component>(4));
    for (int i = 0 ; i<4 ; i++)
    {
        vector<Component> tmp_cs_array;
        for(int j =0; j<4 ; j++)
        {
            string cs_lib_name = CS_LIB_NAME;
            string cs_instance_name = "Transistor" + to_string(i * 4 + j);
            int x = i*Dx;
            int y = j*Dy + offy;
            // cs_array[i][j] = Component(cs_lib_name, cs_instance_name, x, y);
            tmp_cs_array.emplace_back(Component(cs_lib_name, cs_instance_name, x, y)); 
        }
        cs_array.emplace_back(tmp_cs_array);       
    }

    // ##### Step 3: create vertical ME3 #####
    // # ME3 nets
    Dx = CS_WIDTH + M3_SPACING;
    Px = M3_WIDTH + M3_SPACING;
    vector<vector<SpecialNet>> ME3_specialnet;
    for (int i = 0 ; i<4; i++)
    {
        vector<SpecialNet> tmp_ME3_specialnet;
        for(int j = 0 ; j<2 ; j++)
        {
            string inst_name = "Metal3_" + to_string(i * 2 + j);
            string layer = "ME3";
            int x1 = cs_array[i][0]._x + Dx + j * Px;
            int x2 = x1 + M3_WIDTH;
            int y1 = 0;
            int y2 = die_y2;
            // ME3_specialnet[i][j] = SpecialNet(inst_name, layer, x1, y1, x2, y2);
            tmp_ME3_specialnet.emplace_back(SpecialNet(inst_name, layer, x1, y1, x2, y2)); 
        }
        ME3_specialnet.emplace_back(tmp_ME3_specialnet);
    }
    // ##### Step 4: create ME4 drain #####
    // # ME4 drains
    vector<vector<SpecialNet>>ME4_specialnet_drain;
    for(int i = 0 ; i<2 ; i++)
    {
        vector<SpecialNet>tmp_ME4_specialnet_drain;
        for(int j = 0 ; j<2 ; j++)
        {
            string layer = "ME4";
            // # left bottom corner units
            string inst_name = "Metal4_drain_" + to_string(i * 2 + j + 0 * 4);
            int x1 = cs_array[i][j]._x + CS_X1_TO_DRAIN;
            int x2 = ME3_specialnet[i][j]._x2;
            int y1 = cs_array[i][j]._y + CS_Y1_TO_DRAIN;
            int y2 = y1 + M4_WIDTH;
            // ME4_specialnet_drain[i][j] = SpecialNet(inst_name, layer, x1, y1, x2, y2);
            tmp_ME4_specialnet_drain.emplace_back(SpecialNet(inst_name, layer, x1, y1, x2, y2));
        }
        ME4_specialnet_drain.emplace_back(tmp_ME4_specialnet_drain);
    }
    // ##### Step 5: create ME4 port #####
    // # ME4 ports
    vector<SpecialNet>ME4_specialnet_port;
    for(int i = 0 ; i<4 ; i++)
    {
        string inst_name = "Metal4_port_" + to_string(i);
        string layer = "ME4";
        int x1 = 0;
        int x2 = die_x2;
        int y1 = i * Dy;
        int y2 = y1 + M4_WIDTH;
        ME4_specialnet_port.emplace_back(SpecialNet(inst_name, layer, x1, y1, x2, y2));
    }

    // ##### Step 6: create Via34 from ME4 drain #####
    // # drain to ME3
    // Via34_drain2ME3 = [[Component for j in range(4)] for i in range(4)]
    // vector<vector<Component>> Via34_drain2ME3;
    // Via34_drain2ME3.resize(4, vector<Component>(4));

    // vector<int> v(N, default_value);
    // vector<vector<int>> matrix(M, v);


    vector<vector<Component*>> Via34_drain2ME3(4, vector<Component*>(4));

    for(int i = 0 ; i<2 ; i++)
    {
        for(int j = 0 ; j<2 ; j++)
        {
            string lib_name = VIA34_LIB_NAME;
            // # left bottom corner units
            string inst_name = "Via34_drain2ME3_" + to_string(i * 2 + j + 0 * 4);
            auto tmp = Via34_drain2ME3[i][j];
            int x = ME3_specialnet[i][j]._x1;
            int y = cs_array[i][j]._y + CS_Y1_TO_DRAIN;
            tmp->_x = x;tmp->_y = y;
            tmp->lib_name = lib_name;tmp->inst_name = inst_name;

            // Via34_drain2ME3[i][j] = Component(lib_name, inst_name, x, y);


            inst_name = "Via34_drain2ME3_" + to_string(i * 2 + j + 1 * 4);
            tmp = Via34_drain2ME3[3-i][j];
            x = ME3_specialnet[3-i][j]._x1;
            y = cs_array[3-i][j]._y + CS_Y1_TO_DRAIN;
            tmp->_x = x;tmp->_y = y;
            tmp->lib_name = lib_name;tmp->inst_name = inst_name;
            // Via34_drain2ME3[3-i][j] = Component(lib_name, inst_name, x, y);


            inst_name = "Via34_drain2ME3_" + to_string(i * 2 + j + 2 * 4);
            tmp = Via34_drain2ME3[i][3-j];
            x = ME3_specialnet[i][j]._x1;
            y = cs_array[i][3-j]._y + CS_Y1_TO_DRAIN;
            tmp->_x = x;tmp->_y = y;
            tmp->lib_name = lib_name;tmp->inst_name = inst_name;
            // Via34_drain2ME3[i][3-j] = Component(lib_name, inst_name, x, y);


            inst_name = "Via34_drain2ME3_" + to_string(i * 2 + j + 3 * 4);
            tmp = Via34_drain2ME3[3-i][3-j];
            x = ME3_specialnet[3-i][j]._x1;
            y = cs_array[3-i][3-j]._y + CS_Y1_TO_DRAIN;
            tmp->_x = x;tmp->_y = y;
            tmp->lib_name = lib_name;tmp->inst_name = inst_name;
            // Via34_drain2ME3[3-i][3-j] = Component(lib_name, inst_name, x, y);



        }
    }

    // // ##### Step 7: create Via34 to ME4 port #####
    // // # port to ME4
    // // # Precaution:
    // // # 1. create a port list 'Via34_port2ME3' to contain port 'Component'
    // // # 2. lib_name = VIA34_LIB_NAME
    // // # 3. inst_name = 'Via34_port2ME3_'
    // // # TODO
    // // Via34_port2ME3 = [[Component for j in range(2)] for i in range(4)]
    // string lib_name = VIA34_LIB_NAME;

    // auto _ = new Component("tmp", "tmp", 0, 0);
    // vector<Component> row(4, _);

    // vector<vector<Component>> Via34_port2ME3(2, row);

    // // std::vector<std::vector<int>> matrix;
    // // matrix.resize(M, std::vector<int>(N, default_value))

    // for(int i = 0 ; i<2 ; i++)
    //     for(int j = 0 ; j<2 ; j++)
    //     {
    //         string inst_name = "Via34_port2ME3_" + to_string(4*i + 2*j );
    //         int x = Via34_drain2ME3[j][i]->_x;
    //         int y = ME4_specialnet_port[2*i + j]._y1;
    //         // auto tmp_ = Via34_port2ME3[i*2+j][0];
    //         // tmp_._x = x; tmp_._y = y;
    //         // tmp_.lib_name = lib_name;tmp_.inst_name = inst_name;
    //         Via34_port2ME3[i*2+j][0] = Component(lib_name, inst_name, x, y);
            
    //         inst_name = "Via34_port2ME3_" + to_string(4*i + 2*j +1);
    //         x = Via34_drain2ME3[3-j][i]->_x;
    //         y = ME4_specialnet_port[2*i + j]._y1;
    //         // tmp_ = Via34_port2ME3[i*2+j][0];
    //         // tmp_._x = x; tmp_._y = y;
    //         // tmp_.lib_name = lib_name;tmp_.inst_name = inst_name;
    //         Via34_port2ME3[i*2+j][1] = Component(lib_name, inst_name, x, y);
    //     }

    
    // # write info to def file
    vector<Component>component_list;
    for(int i = 0 ; i<4 ; i++)
        for(int j = 0 ; j<4 ; j++)
            component_list.emplace_back(cs_array[i][j]);

    // # 4. add 'Via34_port2ME3' component to 'component_list'
    // # TODO
    // for(int i = 0 ; i <4 ; i++)
    //     for(int j = 0 ; j<4 ; j++)
    //         component_list.emplace_back(Via34_port2ME3[i][j]);

   
}

