#include <iostream>
#include <map>
#include <fstream>

#define Graph std::multimap<std::string, Vertex>

struct Vertex {
    std::multimap<std::string, Vertex*> next;
};

struct Bridge {
    std::string in;
    std::string out;
};

Graph getInput(const std::string &graphFileDirectory) {
    using namespace std;

    multimap<string, Vertex> graph;
    ifstream GraphFile(graphFileDirectory, std::ios::binary);
    if(GraphFile.is_open()) {
        string curr_line;
        while(getline(GraphFile, curr_line)) {
            string vertexName = curr_line.substr(0, curr_line.find('['));
            if(graph.find(vertexName) == graph.end()) {
                Vertex vertex;
                graph.emplace(vertexName, vertex);
            }
            size_t pos = curr_line.find('[') + 1;
            string current_sign;
            current_sign = curr_line.at(pos);
            while(current_sign != "]") {
                if(current_sign != ",") {
                    if(graph.find(current_sign) == graph.end()) {
                        Vertex vertex;
                        graph.emplace(current_sign, vertex);
                    }
                    graph.find(vertexName)->second.next.emplace(current_sign, &graph.find(current_sign)->second);
                }
                pos++;
                current_sign = curr_line.at(pos);
            }
        }
    }
    else {
        std::cout << "Unable to open file";
    }
    return graph;
}

bool isAdjoint(const Graph& graph) {
    std::multimap<std::string, Vertex> checked;
    for (const auto& vertex : graph) {
        for(const auto& other_vertex : graph) {
            if(checked.find(vertex.first) == checked.end() && (vertex.first != other_vertex.first)) {
                int counter = 0;
                for(const auto &edge : vertex.second.next) {
                    if(other_vertex.second.next.find(edge.first) != other_vertex.second.next.end()) {
                        counter++;
                    }
                }
                if(!(counter == 0 || counter == vertex.second.next.size())) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool isLinear(const Graph& graph) {
    for(const auto& vertex : graph) {
        if(vertex.second.next.size() > 1) {
            //PIERWSZA STRUKTURA X->X, X->a->X
            if(vertex.second.next.contains(vertex.first)) {
                for(const auto &edge : vertex.second.next) {
                    if(edge.first != vertex.first && edge.second->next.contains(vertex.first)) {
                        return false;
                    }
                }
            }
            //DRUGA STRUKTURA X-a->X, X->b->X
            for(const auto &a : vertex.second.next) {
                for(const auto &b : vertex.second.next) {
                    if(a.first != b.first && a.second->next.contains(vertex.first) && b.second->next.contains(vertex.first)) {
                        return false;
                    }
                }
            }
            //TRZECIA STRUKTURA X->a->Y, X->b->Y
            for(const auto &a : vertex.second.next) {
                for(const auto &b : vertex.second.next) {
                    if(a.first != b.first) {
                        for(const auto &Y : a.second->next) {
                            if(b.second->next.contains(Y.first)) {
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

Graph transform(const Graph& graph) {
    using namespace std;
    Graph origin;
    multimap<string, Bridge> bridges;
    for(const auto &vertex : graph) {
        Bridge empty;
        empty.in = "_null";
        empty.out = "_null";
        bridges.emplace(vertex.first, empty);
    }
    int names = 0;
    for(const auto &vertex : graph) {
        if(bridges.find(vertex.first)->second.in == "_null") {
            bridges.find(vertex.first)->second.in = std::to_string(names);
            names++;
        }
        for(const auto &edge : vertex.second.next) {
            if(bridges.find(edge.first)->second.in != "_null") {
                bridges.find(vertex.first)->second.out = bridges.find(edge.first)->second.in;
            }
        }
        if(bridges.find(vertex.first)->second.out == "_null") {
            bridges.find(vertex.first)->second.out = std::to_string(names);
            names++;
        }
        for(const auto &edge : vertex.second.next) {
            if(bridges.find(edge.first)->second.in == "_null") {
                bridges.find(edge.first)->second.in = bridges.find(vertex.first)->second.out;
            }
        }
    }
    for(const auto &bridge : bridges) {
        if(origin.find(bridge.second.in) == origin.end()) {
            Vertex vertex;
            origin.emplace(bridge.second.in, vertex);
        }
        if(origin.find(bridge.second.out) == origin.end()) {
            Vertex vertex;
            origin.emplace(bridge.second.out, vertex);
        }
        origin.find(bridge.second.in)->second.next.emplace(bridge.second.out, &origin.find(bridge.second.out)->second);
    }
    return origin;
}

void getOutput(const Graph& graph) {
    using namespace std;
    for (const auto& pair : graph) {
        cout << pair.first << "[";
        const auto& vertex = pair.second;
        bool first = true;

        for (const auto& next_pair : vertex.next) {
            if (!first) {
                cout << ",";
            }
            cout << next_pair.first;
            first = false;
        }
        cout <<"]"<< endl;
    }
}

int main()
{
    using namespace std;
    Graph graph1 = getInput("C:/Users/micha/CLionProjects/Algorytmy Grafowe/graph");
    //getOutput(graph1);
    if(isAdjoint(graph1)) {
        if(isLinear(graph1)) {
            cout<<"Graf jest sprzezony i liniowy"<<endl;
        } else {
            cout<<"Graf jest sprzezony ale nie jest liniowy"<<endl;
        }
        getOutput(transform(graph1));
    } else {
        cout<< "Graf nie jest sprzezony"<<endl;
    }
    return 0;
}
