#include <iostream>
#include <fstream>
#include <vector>

using std::vector;
using std::ifstream;
using std::cin;
using std::cout;
using std::endl;

#include "data_types.h"

namespace dostavista {
    Data ReadData() {
        ifstream fin;
        fin.open("../data.txt");

        int n = 0;
        vector<Courier_data> couriers;
        vector<Order> orders;
        vector<Storage> storages;

        // Couriers
        fin >> n;
        for (int i = 0; i < n; ++i) {
            Courier_data courier;
            fin >> courier.courier_id >> courier.location_x >> courier.location_y;
            couriers.push_back(courier);
        }

        // Storages
        fin >> n;
        for (int i = 0; i < n; ++i) {
            Storage storage;
            fin >> storage.location_x >> storage.location_y >> storage.point_id;
            storages.push_back(storage);
        }

        // Orders
        fin >> n;
        for (int i = 0; i < n; ++i) {
            Order order;
            fin >> order.dropoff_from >> order.dropoff_location_x >> order.dropoff_location_y >> order.dropoff_point_id
                >> order.dropoff_to >> order.order_id >> order.payment >> order.pickup_from >> order.pickup_location_x
                >> order.pickup_location_y >> order.pickup_point_id >> order.pickup_to;
            orders.push_back(order);
        }

        fin.close();

        return {orders, couriers, storages};
    }
}
int main() {

    dostavista::Data data = dostavista::ReadData();
    cout << data.orders.size() << endl;

    return 0;
}