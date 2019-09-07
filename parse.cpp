#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


struct Courier {
    int courier_id;
    int location_x, location_y;
};

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

struct Storage {
    int point_id;
    int location_x, location_y;
};

struct Data{
	vector<Courier> couriers;
	vector<Order> orders;
	vector<Storage> storages;
};

Data ReadData(){
	ifstream fin;
	fin.open("data.txt");

	int n = 0;
	vector<Courier> couriers;	
	vector<Order> orders;	
	vector<Storage> storages;	
	
	// Couriers
	fin >> n;
	for (int i = 0; i < n; ++i){
		Courier courier;
		fin >> courier.courier_id >> courier.location_x >> courier.location_y;
		couriers.push_back(courier);
	}

	// Storages
	fin >> n;
	for (int i = 0; i < n; ++i){
		Storage storage;
		fin >> storage.location_x >> storage.location_y >> storage.point_id;
		storages.push_back(storage);
	}

	// Orders
	fin >> n;
	for (int i = 0; i < n; ++i){
		Order order;
		fin >> order.dropoff_from >> order.dropoff_location_x >> order.dropoff_location_y >> order.dropoff_point_id >> order.dropoff_to >> order.order_id >> order.payment >> order.pickup_from >> order.pickup_location_x >> order.pickup_location_y >> order.pickup_point_id >> order.pickup_to;
		orders.push_back(order);
	}
	
	fin.close();

	return {couriers, orders, storages};
}

int main(){

	Data data = ReadData();
	cout << data.orders.size() << endl;

	return 0;
}