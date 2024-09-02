#include <bits/stdc++.h>

const long inf = 1e10;

class Router
{
private:
    int router_id;
    std::vector<std::pair<Router *, long>> neighbours;
    std::unordered_map<Router *, Router *> routing_table;

    std::unordered_map<Router *, Router *> dijkstra_utility(Router *start)
    {
        std::unordered_map<Router *, long> distances;
        std::priority_queue<std::pair<long, Router *>> pq;
        std::unordered_map<Router *, bool> vis;
        std::unordered_map<Router *, Router *> parent;

        pq.push(std::make_pair(0, start));
        distances[start] = 0;

        while (not pq.empty())
        {
            auto tp = pq.top();
            pq.pop();
            if (vis[tp.second])
                continue;
            vis[tp.second] = true;
            auto adj = tp.second->get_neighbours();
            for (auto neighbour : adj)
            {
                long dist = (distances.find(neighbour.first) == distances.end()) ? inf : distances[neighbour.first];
                if (dist > distances[tp.second] + neighbour.second)
                {
                    distances[neighbour.first] = distances[tp.second] + neighbour.second;
                    parent[neighbour.first] = tp.second;
                    pq.push({distances[neighbour.first], neighbour.first});
                }
            }
        }
        return parent;
    }

public:
    Router(int id) : router_id(id){};

    void add_neighbour(Router *neighbour, long link_cost)
    {
        neighbours.push_back(std::make_pair(neighbour, link_cost));
    }

    void update_routing_table()
    {
        auto parents = dijkstra_utility(this);
        routing_table.clear();
        for (auto pr : parents)
        {
            auto curr = pr.first;
            while (parents[curr] != this)
                curr = parents[curr];
            routing_table[pr.first] = curr;
        }
    }

    void print_routing_table()
    {
        std::cout << "Here is the Routing info for Router" << this->get_id() << "\n";
        for (auto pr : routing_table)
        {
            std::cout << "To go to Router" << pr.first->get_id() << " go to Router" << pr.second->get_id() << "\n";
        }
    }
    int get_id() { return router_id; }
    std::vector<std::pair<Router *, long>> get_neighbours() { return neighbours; }
};

int main()
{
    Router r1(1);
    Router r2(2);
    Router r3(3);
    Router r4(4);
    r1.add_neighbour(&r2, 10);
    r2.add_neighbour(&r1, 10);
    r1.add_neighbour(&r3, 100);
    r3.add_neighbour(&r1, 100);
    r4.add_neighbour(&r2, 10);
    r2.add_neighbour(&r4, 10);
    r2.add_neighbour(&r3, 5);
    r3.add_neighbour(&r2, 5);
    r3.add_neighbour(&r4, 5);
    r4.add_neighbour(&r3, 5);
    r1.add_neighbour(&r4, 100);
    r4.add_neighbour(&r1, 100);

    r1.update_routing_table();
    r2.update_routing_table();
    r3.update_routing_table();
    r4.update_routing_table();

    r1.print_routing_table();
    r2.print_routing_table();
}