#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "myObject.hpp"
using namespace  std;

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

void write_def(string file_name, Die die,  vector<Component*> component_list,  vector<SpecialNet*> sepcialnet_list);


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
    int Dy = CS_HEIGHT + M4_SPACING * 2 + M4_WIDTH;
    int offy = M4_SPACING + M4_WIDTH;

 
    vector <vector<Component*>> cs_array;
    cs_array.resize(4, vector<Component*>(4));
    for (int i = 0 ; i<4 ; i++)
    {
        for(int j =0; j<4 ; j++)
        {
            string cs_lib_name = CS_LIB_NAME;
            string cs_instance_name = "Transistor" + to_string(i * 4 + j);
            int x = i*Dx;
            int y = j*Dy + offy;
            Component *tmp = new Component(cs_lib_name, cs_instance_name, x, y);
            cs_array[i][j] = tmp;
        }
    }


    // ##### Step 3: create vertical ME3 #####
    // # ME3 nets
    Dx = CS_WIDTH + M3_SPACING;
    int Px = M3_WIDTH + M3_SPACING;
    vector<vector<SpecialNet*>> ME3_specialnet;
    ME3_specialnet.resize(4, vector<SpecialNet*>(2));
    for (int i = 0 ; i<4; i++)
    {
        for(int j = 0 ; j<2 ; j++)
        {
            string inst_name = "Metal3_" + to_string(i * 2 + j);
            string layer = "ME3";
            int x1 = cs_array[i][0]->_x + Dx + j * Px;
            // cout<<"Dx : "<<Dx <<" Px : "<<Px<<endl;
            int x2 = x1 + M3_WIDTH;
            int y1 = 0;
            int y2 = die_y2;
            SpecialNet *tmp = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            ME3_specialnet[i][j] = tmp;
        }
    }
    // ##### Step 4: create ME4 drain #####
    // # ME4 drains
    vector<vector<SpecialNet*>> ME4_specialnet_drain;
    // SpecialNet *ME4_specialnet_drain_empty = new SpecialNet(" ", " ", 0, 0, 0, 0);
    ME4_specialnet_drain.resize(4, vector<SpecialNet*>(4));

    for(int i = 0 ; i<2 ; i++)
        for(int j = 0 ; j<2 ; j++)
        {
            string layer = "ME4";
            string inst_name = "Metal4_drain_" + to_string(i * 2 + j + 0 * 4);
            int x1 = cs_array[i][j]->_x + CS_X1_TO_DRAIN;
            int x2 = ME3_specialnet[i][j]->_x2;
            int y1 = cs_array[i][j]->_y + CS_Y1_TO_DRAIN;
            int y2 = y1 + M4_WIDTH;
            SpecialNet *drain_1 = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            ME4_specialnet_drain[i][j] = drain_1;

            inst_name = "Metal4_drain_" + to_string(i * 2 + j + 1 * 4);
            x1 = cs_array[3-i][j]->_x + CS_X1_TO_DRAIN;
            x2 = ME3_specialnet[3-i][j]->_x2;
            y1 = cs_array[3-i][j]->_y + CS_Y1_TO_DRAIN;
            y2 = y1 + M4_WIDTH;
            SpecialNet *drain_2 = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            ME4_specialnet_drain[3-i][j] = drain_2;

            inst_name = "Metal4_drain_" + to_string(i * 2 + j + 2 * 4);
            x1 = cs_array[i][3-j]->_x + CS_X1_TO_DRAIN;
            x2 = ME3_specialnet[i][j]->_x2;
            y1 = cs_array[i][3-j]->_y + CS_Y1_TO_DRAIN;
            y2 = y1 + M4_WIDTH;
            SpecialNet *drain_3 = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            ME4_specialnet_drain[i][3-j] = drain_3;

            inst_name = "Metal4_drain_" + to_string(i * 2 + j + 3 * 4);
            x1 = cs_array[3-i][3-j]->_x + CS_X1_TO_DRAIN;
            x2 = ME3_specialnet[3-i][j]->_x2;
            y1 = cs_array[3-i][3-j]->_y + CS_Y1_TO_DRAIN;
            y2 = y1 + M4_WIDTH;
            SpecialNet *drain_4 = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
            ME4_specialnet_drain[3-i][3-j] = drain_4;
        }
    
    
   

    // ##### Step 5: create ME4 port #####
    // # ME4 ports
    vector<SpecialNet*>ME4_specialnet_port;

    // SpecialNet *ME4_specialnet_port_empty = new SpecialNet(" ", " ", 0, 0, 0, 0);
    // ME4_specialnet_port.resize(4, vector<SpecialNet*>(1, ME4_specialnet_port_empty));
    
    for(int i = 0 ; i<4 ; i++)
    {
        string inst_name = "Metal4_port_" + to_string(i);
        string layer = "ME4";
        int x1 = 0;
        int x2 = die_x2;
        int y1 = i * Dy;
        int y2 = y1 + M4_WIDTH;
        auto *ME4_tmp = new SpecialNet(inst_name, layer, x1, y1, x2, y2);
        ME4_specialnet_port.emplace_back(ME4_tmp);
        // auto ME4_tmp = ME4_specialnet_port[i];
        // ME4_tmp->inst_name = inst_name;ME4_tmp->layer = layer;
        // ME4_tmp->_x1 = x1; ME4_tmp->_y1 = y1;
        // ME4_tmp->_x2 = x2; ME4_tmp->_y2 = y2;
        // ME4_specialnet_port.emplace_back(SpecialNet(inst_name, layer, x1, y1, x2, y2));
    }

    // ##### Step 6: create Via34 from ME4 drain #####
    // # drain to ME3
    // Via34_drain2ME3 = [[Component for j in range(4)] for i in range(4)]
    vector<vector<Component*>> Via34_drain2ME3;
    // Component *empty = new Component(" ", " ", 0, 0);
    Via34_drain2ME3.resize(4, vector<Component*>(4));
  
    for(int i = 0 ; i<2 ; i++)
    {
        for(int j = 0 ; j<2 ; j++)
        {
            string lib_name = VIA34_LIB_NAME;
            // # left bottom corner units
            string inst_name = "Via34_drain2ME3_" + to_string(i * 2 + j + 0 * 4);
            int x = ME3_specialnet[i][j]->_x1;
            int y = cs_array[i][j]->_y + CS_Y1_TO_DRAIN;
            Component *Via34_1 = new Component(lib_name, inst_name, x, y);
            Via34_drain2ME3[i][j] = Via34_1;


            inst_name = "Via34_drain2ME3_" + to_string(i * 2 + j + 1 * 4);
            x = ME3_specialnet[3-i][j]->_x1;
            y = cs_array[3-i][j]->_y + CS_Y1_TO_DRAIN;
            Component *Via34_2 = new Component(lib_name, inst_name, x, y);
            Via34_drain2ME3[3-i][j] = Via34_2;
      
            inst_name = "Via34_drain2ME3_" + to_string(i * 2 + j + 2 * 4);
            x = ME3_specialnet[i][j]->_x1;
            y = cs_array[i][3-j]->_y + CS_Y1_TO_DRAIN;
            Component *Via34_3 = new Component(lib_name, inst_name, x, y);
            Via34_drain2ME3[i][3-j] = Via34_3;

            inst_name = "Via34_drain2ME3_" + to_string(i * 2 + j + 3 * 4);
            x = ME3_specialnet[3-i][j]->_x1;
            y = cs_array[3-i][3-j]->_y + CS_Y1_TO_DRAIN;
            Component *Via34_4 = new Component(lib_name, inst_name, x, y);
            Via34_drain2ME3[3-i][3-j] = Via34_4;

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
    string lib_name = VIA34_LIB_NAME;

    vector<vector<Component*> > Via34_port2ME3;
    Via34_port2ME3.resize(4, vector<Component*>(2));


    for(int i = 0 ; i<2 ; i++)
        for(int j = 0 ; j<2 ; j++)
        {
            string inst_name = "Via34_port2ME3_" + to_string(4*i + 2*j );
            int x = Via34_drain2ME3[j][i]->_x;
            int y = ME4_specialnet_port[2*i + j]->_y1;
            Component *Via34_drain_1 = new Component(lib_name, inst_name, x, y);
            Via34_port2ME3[i*2+j][0] = Via34_drain_1;

            inst_name = "Via34_port2ME3_" + to_string(4*i + 2*j +1);
            x = Via34_drain2ME3[3-j][i]->_x;
            y = ME4_specialnet_port[2*i + j]->_y1;;
            Component *Via34_drain_2 = new Component(lib_name, inst_name, x, y);
            Via34_port2ME3[i*2+j][1] = Via34_drain_2;
        }

    
    // # write info to def file
    vector<Component*>component_list;
    for(int i = 0 ; i<4 ; i++)
        for(int j = 0 ; j<4 ; j++)
            component_list.emplace_back(cs_array[i][j]);


    // # 4. add 'Via34_port2ME3' component to 'component_list'
    // # TODO
    for(int i = 0 ; i <4 ; i++)
        for(int j = 0 ; j<2 ; j++)
            component_list.emplace_back(Via34_port2ME3[i][j]);



    vector<SpecialNet*> specialnet_list;
    for(int i = 0 ; i <4 ; i++)
        for(int j = 0 ; j<2 ; j++)
            specialnet_list.emplace_back(ME3_specialnet[i][j]);


    for(int i = 0 ; i <4 ; i++)
        for(int j = 0 ; j<4 ; j++)
            specialnet_list.emplace_back(ME4_specialnet_drain[i][j]);

    for(int i = 0 ; i <4 ; i++)
        specialnet_list.emplace_back(ME4_specialnet_port[i]);

    for(int i = 0 ; i <4 ; i++)
        for(int j = 0 ; j<4 ; j++)
            component_list.emplace_back(Via34_drain2ME3[i][j]);


    string file_name = "../output/CS_4.def";
    write_def(file_name, die, component_list, specialnet_list);

}

void write_def(string file_name, Die die,  vector<Component*> component_list,  vector<SpecialNet*> sepcialnet_list)
{
    ofstream fout;

    fout.open(file_name);
    fout<<"VERSION 5.6 ;\n";
    fout<<"DIVIDERCHAR \"/\" ;\n";
    fout<<"BUSBITCHARS \"[]\" ;\n";
    fout<<"DESIGN "<<die.design_name<<" ;\n\n";
    fout<<"UNITS DISTANCE MICRONS 1000 ;\n\n";
    fout<<"PROPERTYDEFINITIONS\n";
    fout<<"  COMPONENTPIN text STRING ;\n";
    fout<<"END PROPERTYDEFINITIONS\n\n";
    
 
    fout<<"DIEAREA ( " << die._x1 <<" "<<die._y1<< " ) "<< "( "<<die._x2<<" "<<die._y2<<" )"<<" ;\n\n";
    fout<<"COMPONENTS "<<component_list.size()<<" ;\n";
    for(auto component : component_list)
    {
        fout<<"- "<<component->inst_name<<" "<<component->lib_name<<"\n";
        fout<<"  + PLACED ( "<<component->_x<<" "<<component->_y<<" ) N ;\n";
    }
    fout<<"END COMPONENTS\n\n";

    int x, x1, x2, y, y1, y2;
    fout<<"SPECIALNETS "<<sepcialnet_list.size()<<" ;\n";
    for(auto specialnet : sepcialnet_list)
    {
        if(specialnet->layer =="ME3")
        {
            string name = specialnet->inst_name;
            // cout<<"_x1 : "<<specialnet->_x1<<" _x2 : "<<specialnet->_x2<<endl;
            x = (specialnet->_x1 + specialnet->_x2) / 2;
            y1 = specialnet->_y1;
            y2 = specialnet->_y2;
            fout<<" - "<<name<<"\n";
            fout<<"  + ROUTED ME3 440 "<<"( "<<x<<" "<<y1<< " ) "<<"( * "<<y2<<" ) ; \n";
        }

        else if(specialnet->layer =="ME4")
        {
            string name = specialnet->inst_name;
            x1 = specialnet->_x1;
            x2 = specialnet->_x2;
            y = (specialnet->_y1 + specialnet->_y2) / 2;
            fout<<" - "<<name<<"\n";
            fout<<"  + ROUTED ME4 1000 "<<"( "<<x1<<" "<<y<< " ) "<<"( "<<x2<<" * ) ; \n";
        }
  
    }
    fout<<"END SPECIALNETS\n\n";
    fout<<"END DESIGN\n";


}



