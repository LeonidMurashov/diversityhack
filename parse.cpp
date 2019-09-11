#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <cmath>

using namespace std;

const int DAY_START_TIME = 360;
const int MCTS_STEPS = 1000;
const int coefficent = 100;

struct Courier {
    int courier_id;
    int location_x = 0, location_y = 0;

    bool day_finished = false;
    int current_time = DAY_START_TIME;
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

    bool order_picked = false;
    bool finished = false;
    Courier *courier;
};

struct Storage {
    int point_id;
    int location_x, location_y;
};

struct Data {
    vector<Courier *> couriers;
    vector<Order *> orders;
    vector<Storage *> storages;
};


struct Node {
    Node *parent;
    vector<Node *> children;

    vector<double> weights;
    double weights_argmax = 0;

    int visited = 0;
    bool fully_explored = false;
    int total_reward = 0;

    Order *target;
    vector<Order *> backpack;

    int orders_iterator = 0;

    Node(Node *parent, Order *target) : parent(parent), target(target) {};
};

int Distance(Courier *a, Node *b) {
    return 10 + abs(a->location_x - b->target->pickup_location_x) +
           abs(a->location_y - b->target->pickup_location_y);
}

int Distance(Courier *a, Order *b) {
    return 10 + abs(a->location_x - b->pickup_location_x) +
           abs(a->location_y - b->pickup_location_y);
}

int Distance(int x1, int y1, int x2, int y2) {
    return 10 + abs(x1 - x2) + abs(y1 - y2);
}

struct TargetValueCourier {
    Order *target;
    Courier *courier;
    double value = 0;

    TargetValueCourier(Order *target, double value, Courier *courier)
            : target(target), value(value), courier(courier) {}
};


// MCTS and environment
class CourierTree {
public:

    CourierTree(const vector<Order *> &orders, Courier *courier)
            : root(new Node(nullptr, nullptr)),
              nodes({root}),
              orders(orders),
              courier(courier) {
    }

    vector<TargetValueCourier> EstimateBestOptions() {
        vector<TargetValueCourier> estimates;
        Node *current = root;

        for (int i = 0; i < MCTS_STEPS; ++i) {
            // Case already explored
            if (current->fully_explored) {
                // Check for children
                if (current->children.empty()) {
                    if (current != root) {
                        BackPropagate(current, 0);
                        continue;
                    } else {
                        break;
                    }
                }
                // Descending to leaves or unexplored
                current = current->children[current->weights_argmax];
            }
                // Explore
            else {
                // TODO: what if explore more than one


                Order *target;
                Node *new_node;
                // Yeild actions wrt constraints
                // First delivery
                if (current->children.size() < current->backpack.size()) {
                    target = current->backpack[current->children.size()];
                    new_node = new Node(current, target);
                    new_node->backpack = current->backpack;
                    remove(new_node->backpack.begin(), new_node->backpack.end(), target);
                }
                    // Second orders
                else if (current->orders_iterator < orders.size()) {
                    bool success = false;
                    while (current->orders_iterator < orders.size()) {
                        Order *candidate =
                                orders[(current->orders_iterator) % orders.size()];
                        current->orders_iterator++;
                        if (CheckConstraints(current, candidate)) {
                            target = candidate;

                            new_node = new Node(current, target);
                            new_node->backpack = current->backpack;
                            new_node->backpack.push_back(target);

                            success = true;
                            break;
                        }
                    }
                    if (!success) {
                        current->fully_explored = true;

                        // TODO: check argmax
                        // Check for size 0 ???
                        current->weights_argmax =
                                distance(current->weights.begin(),
                                         max_element(current->weights.begin(),
                                                     current->weights.end()));
                        continue;
                    }
                } else {
                    current->fully_explored = true;

                    // TODO: check argmax
                    // Check for size 0 ???
                    current->weights_argmax =
                            distance(current->weights.begin(),
                                     max_element(current->weights.begin(),
                                                 current->weights.end()));
                    continue;
                }

                current->children.push_back(new_node);
                current->weights.push_back(0);

                nodes.push_back(new_node);

                double reward = Simulate(new_node);
                BackPropagate(new_node, reward);
                current = root;
            }
        }

        if (root->children.empty()) {
            estimates.push_back({nullptr,
                                 0,
                                 courier});
        } else {
            for (int i = 0; i < root->children.size(); ++i) {
                estimates.push_back({root->children[i]->target,
                                     root->weights[i],
                                     courier});
            }
        }

        return estimates;
    }

    ~CourierTree() {
        for (int i = 0; i < nodes.size(); ++i) {
            delete nodes[i];
        }
    }

private:
    Node *root;
    vector<Node *> nodes;
    vector<Order *> active_orders;
    vector<Order *> orders;
    Courier *courier;

    bool CheckConstraints(Node *node, Order *order) {
        // TODO: here
        bool time_acceptable = (courier->current_time + Distance(courier, order) +
                                Distance(order->pickup_location_x,
                                         order->pickup_location_y,
                                         courier->location_x,
                                         courier->location_y)) < 1440;



        return node->backpack.empty() && time_acceptable && !order->finished &&
               (order->courier == nullptr || order->courier == courier);
    }

    void BackPropagate(Node *node, double reward) {
        node->total_reward += reward;
        node->visited++;
        do {
            node = node->parent;

            node->total_reward += reward;
            node->visited++;

            // Recalc weights
            for (int i = 0; i < node->children.size(); ++i) {
                node->weights[i] = node->children[i]->total_reward / node->visited +
                                   coefficent * sqrt(log(node->visited)
                                                     / (double) node->children[i]->visited);
            }
            node->weights_argmax = distance(node->weights.begin(),
                                            max_element(node->weights.begin(),
                                                        node->weights.end()));
        } while (node != root);
    }

    double Simulate(Node *node) {
        return 0;
    }
};

int GetSolution(Data &data) {
    vector<Courier *> couriers = data.couriers;
    vector<Order *> orders = data.orders;
    vector<Storage *> storages = data.storages;

    int finished_number = 0;
    int income = 0;
    int outcome = 0;

    while (finished_number != couriers.size()) {
        // Collect MCTS results
        vector<TargetValueCourier> all_estimates; // Target id, value(reward)
        for (int i = 0; i < couriers.size(); ++i) {
            if (!couriers[i]->day_finished) {
                CourierTree tree(orders, couriers[i]);

                vector<TargetValueCourier> estimates = tree.EstimateBestOptions();
                all_estimates.reserve(estimates.size());
                all_estimates.insert(all_estimates.end(),
                                     estimates.begin(),
                                     estimates.end());
            }
        }

        // Sort by value
        auto cmp = [](const TargetValueCourier &a, const TargetValueCourier &b) {
            return a.value > b.value;
        };
        sort(all_estimates.begin(), all_estimates.end(), cmp);
        // TODO: check sort

        int busy_number = finished_number;
        int i = 0;
        int skipped = 0;
        while (busy_number != couriers.size() && i < all_estimates.size()) {
            // Code for finish day
            if (all_estimates[i].target == nullptr) {
                all_estimates[i].courier->day_finished = true;
                outcome += all_estimates[i].courier->current_time - DAY_START_TIME;

                finished_number++;

                cout << "finished " << all_estimates[i].courier->courier_id << endl;
            }
                // Picking order
            else if (!all_estimates[i].target->order_picked &&
                     !all_estimates[i].target->finished) {
                // Modify order
                all_estimates[i].target->order_picked = true;
                all_estimates[i].target->courier = all_estimates[i].courier;

                // Modify courier
                all_estimates[i].courier->location_x =
                        all_estimates[i].target->pickup_location_x;
                all_estimates[i].courier->location_y =
                        all_estimates[i].target->pickup_location_y;
                all_estimates[i].courier->current_time +=
                        Distance(all_estimates[i].courier, all_estimates[i].target);

                cout << "picked   " << all_estimates[i].target->order_id << " " <<
                                    all_estimates[i].courier->courier_id << endl;
            }
                // Dropping order off
            else if (all_estimates[i].target->courier == all_estimates[i].courier &&
                     !all_estimates[i].target->finished) {
                all_estimates[i].target->finished = true;
                income += all_estimates[i].target->payment;
                cout << "dropped  " << all_estimates[i].target->order_id << " " <<
                        all_estimates[i].courier->courier_id << endl;
            } else {
                i++;
                skipped++;
                continue;
            }
            busy_number++;
            i++;
        }
        cout << "Skipped " << skipped << " of " << all_estimates.size() << endl;
    }
    cout << "i: " << income << " o: " << outcome << endl;
    return income - 2 * outcome;
}

Data ReadData() {
    ifstream fin;
    fin.open("/home/ubuntu/hack/diversityhack/data.txt");

    int n = 0;
    vector<Courier *> couriers;
    vector<Order *> orders;
    vector<Storage *> storages;

    // Couriers
    fin >> n;
    for (int i = 0; i < n; ++i) {
        Courier *courier = new Courier();
        fin >> courier->courier_id >> courier->location_x >> courier->location_y;
        couriers.push_back(courier);
    }

    // Storages
    fin >> n;
    for (int i = 0; i < n; ++i) {
        Storage *storage = new Storage();
        fin >> storage->location_x >> storage->location_y >> storage->point_id;
        storages.push_back(storage);
    }

    // Orders
    fin >> n;
    for (int i = 0; i < n; ++i) {
        Order *order = new Order();
        fin >> order->dropoff_from >> order->dropoff_location_x
            >> order->dropoff_location_y >> order->dropoff_point_id
            >> order->dropoff_to >> order->order_id >> order->payment
            >> order->pickup_from >> order->pickup_location_x
            >> order->pickup_location_y >> order->pickup_point_id
            >> order->pickup_to;
        orders.push_back(order);
    }

    fin.close();

    return {couriers, orders, storages};
}

int main() {
    Data data = ReadData();
    cout << "Orders " << data.orders.size() << endl;
    double reward = GetSolution(data);
    cout << "reward " << reward << endl;
    return 0;
}