//
// Created by Kirill on 2019-09-07.
//


#ifndef DOSTAVISTA_MCT_H
#define DOSTAVISTA_MCT_H

#include "data_types.h"

using std::vector;

namespace dostavista {
    struct Courier : public Courier_data {
        vector<Order> taken_orders; //orders which are taken by this courier

        //checks if courier is able to take order and still be able to deliver the rest
        bool is_able_to_take(const Order &order) const;

        void take(const Order &order);//wrapper for taken_orders.append(order)

        //returns the payment of the delivered order, removes order from taken orders, executes some side logic
        int deliver(size_t idx);
    };

    class MCT {
        Courier cur; //current courier who's route is going to be calculated by this Monte-Carlo Tree
        vector<Order> orders; //all orders available now

        class Tree {
            enum Action {
                dropoff, pickup
            };

            struct node { //node of the Monte-Carlo Tree
                Courier cur; //courier state after this node's action
                bool is_root; //if this is a root action and order idx are exc
                Action action; //which action to perform while going to this node
                int order_idx; //which order to perform this action to
                double value; //The criteria for choosing optimal action for courier
                vector<node *> children; //next possible actions
                size_t sim_n; //How many times the simulation is ran from this node

                node *best_child(); //returns child with maximum value

                double simulate();


            } *root; //root of Monte-Carlo Tree

        } tree; //Monte-Carlo Tree itself

        //constructs MCT from courier struct and order vector. Initializing Monte-Carlo tree.
        MCT(const Courier &cour, const vector<Order> &orders);


    };
}

#endif //DOSTAVISTA_MCT_H
