//
// Created by Kirill on 2019-09-07.
//

#ifndef DOSTAVISTA_DATA_TYPES_H
#define DOSTAVISTA_DATA_TYPES_H

#include <vector>

using std::vector;

namespace dostavista {

    struct Order {
        int order_id;
        int pickup_point_id;
        int pickup_location_x, pickup_location_y;
        int pickup_from, pickup_to;
        int dropoff_point_id;
        int dropoff_location_x, dropoff_location_y;
        int dropoff_from, dropoff_to;
        int payment;
    };

    struct Courier_data {
        int courier_id;
        int location_x, location_y;
    };


    struct Storage {
        int point_id;
        int location_x, location_y;
    };

    struct Data {
        vector<Order> orders;
        vector<Courier_data> couriers;
        vector<Storage> storages;
    };
}


#endif //DOSTAVISTA_DATA_TYPES_H
