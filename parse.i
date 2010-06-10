%module osm4routing_xml
%include "std_vector.i"
%include "std_string.i"
%{
    #include "parse.h"
%}
 
%template(Nodes) std::vector<Node>;
%template(Edges) std::vector<Edge>;

class Edge_property
{
    public:
    int car_direct;
    int car_reverse;
    int bike_direct;
    int bike_reverse;
    int foot;

    Edge_property();

    // Can at least one mean use that edge
    bool accessible();
    bool direct_accessible();
    bool reverse_accessible();

    // Update the properties given new information
    bool update(const std::string & tag, const std::string & val);

    // Infer unknown data
    void normalize();

    void reset();
};

struct Edge
{ 
    int edge_id;
    int source;
    int target;
    float length;
    char car;
    char car_d;
    char bike;
    char bike_d;
    char foot;
    std::string geom;
};

struct Node
{
    int id;
    double lon;
    double lat;
    char uses;
};


struct Parser
{
    Parser();
    void read(char *, int, bool);
    std::vector<Node> get_nodes() const;
    std::vector<Edge> get_edges() const;
    int get_osm_nodes() const;
    int get_osm_ways() const;
};
